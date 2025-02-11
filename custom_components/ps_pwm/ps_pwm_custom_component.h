#include "esphome.h"
#include "ps_pwm.h"

static const char * TAG = "PSPWM";

using namespace esphome;

class PSpwmCustomOutput : public Component, public FloatOutput {
  public:
  float g_frequency = 20.0e3f;
  float g_psDuty = 0.5f;
  bool g_onoff = false;
  bool g_error_occured = false;

  bool g_write_state_float_changed = false;
  bool g_write_state_binary_changed = false;
  bool g_update_frequency_changed = false;

  void setup() override {
    ESP_LOGI(TAG,"PSpwmCustomOutput setup() ... \n");

    ///////////////////////////// Configuration ////////////////////////////////
    // MCPWM unit can be [0,1]
    mcpwm_unit_t mcpwm_num = MCPWM_UNIT_0;

    #ifdef CONFIG_IDF_TARGET_ESP32
    // GPIO config for PWM output
    gpio_num_t gpio_pwm0a_out = GPIO_NUM_27; // PWM0A := LEAD leg, Low Side
    gpio_num_t gpio_pwm0b_out = GPIO_NUM_26; // PWM0B := LEAD leg, High Side
    gpio_num_t gpio_pwm1a_out = GPIO_NUM_25; // PWM1A := LAG leg, Low Side
    gpio_num_t gpio_pwm1b_out = GPIO_NUM_33; // PWM1B := LAG leg, High Side
    // gpio_num_t gpio_pwm1b_out = GPIO_NUM_14; // PWM1B := LAG leg, High Side
    // Shutdown/fault input for PWM outputs
    gpio_num_t gpio_fault_shutdown = GPIO_NUM_4;
    #elif CONFIG_IDF_TARGET_ESP32S3
    // GPIO config for PWM output
    gpio_num_t gpio_pwm0a_out = GPIO_NUM_5; // DRV_B, PWM0A := LEAD leg, Low Side
    gpio_num_t gpio_pwm0b_out = GPIO_NUM_4; // DRV_A, PWM0B := LEAD leg, High Side
    gpio_num_t gpio_pwm1a_out = GPIO_NUM_7; // DRV_D, PWM1A := LAG leg, Low Side
    gpio_num_t gpio_pwm1b_out = GPIO_NUM_6; // DRV_C, PWM1B := LAG leg, High Side
    // Shutdown/fault input for PWM outputs
    gpio_num_t gpio_fault_shutdown = GPIO_NUM_8;    // OCP_PULSE, Pulse by Pulse protect
    #endif

    // Active low / active high selection for fault input pin
    mcpwm_fault_input_level_t fault_pin_active_level = MCPWM_LOW_LEVEL_TGR;
    // Define here if the output pins shall be forced low or high
    // or high-impedance when a fault condition is triggered.
    // PWMxA and PWMxB have the same type of action, see declaration in mcpwm.h
    mcpwm_action_on_pwmxa_t disable_action_lag_leg = MCPWM_FORCE_MCPWMXA_LOW;
    // Lead leg might have a different configuration, e.g. stay at last output level
    mcpwm_action_on_pwmxa_t disable_action_lead_leg = MCPWM_FORCE_MCPWMXA_LOW;

    float init_frequency = 100e3f;
    // Initial phase-shift setpoint
    float init_ps_duty = 0.45f;
    // Initial leading leg dead-time value in ns
    float init_lead_dt = 125e-9f;
    // Initial lagging leg dead-time value in ns
    float init_lag_dt = 125e-9f;
    // Initial output state is "true" representing "on"
    bool init_power_pwm_active = true;
    ////////////////////////////////////////////////////////////////////////////

    printf("Configuring Phase-Shift-PWM...\n");
    esp_err_t errors = pspwm_init_symmetrical(mcpwm_num,
                                              gpio_pwm0a_out,
                                              gpio_pwm0b_out,
                                              gpio_pwm1a_out,
                                              gpio_pwm1b_out,
                                              init_frequency,
                                              init_ps_duty,
                                              init_lead_dt,
                                              init_lag_dt,
                                              init_power_pwm_active,
                                              disable_action_lead_leg,
                                              disable_action_lag_leg);
    // Enable fault shutdown input, low level disables output.
    // Must then be reset manually by removing fault condition and then calling:
    // "pspwm_clear_hw_fault_shutdown_occurred(mcpwm_unit_t mcpwm_num);"
    // followed by:
    // "pspwm_resync_enable_output(mcpwm_unit_t mcpwm_num);"
    errors |= pspwm_enable_hw_fault_shutdown(mcpwm_num,
                                            gpio_fault_shutdown,
                                            fault_pin_active_level);
    // Pull-up enabled for avoiding shutdown on start
    errors |= gpio_pullup_en(gpio_fault_shutdown);

    if (errors != ESP_OK) {
        printf("Error initializing the PS-PWM module!\n");
        abort();
    }
  }

  void loop() override {
    bool error_occurred = pspwm_get_hw_fault_shutdown_occurred(MCPWM_UNIT_0);
    if (error_occurred) {
      if (!g_error_occured) {
        g_error_occured = true;
        ESP_LOGI(TAG, "g_error_occurred: %d", g_error_occured);
        // publish_state(g_error_occured);
        // pspwm_fault->publish_state(g_error_occured);
      }
    } else {
      if (g_error_occured) {
        g_error_occured = false;
        ESP_LOGI(TAG, "g_error_occurred: %d", g_error_occured);
        // publish_state(g_error_occured);
        // pspwm_fault->publish_state(g_error_occured);
      }
    }

    if (g_write_state_float_changed || g_update_frequency_changed) {    
      pspwm_set_frequency(MCPWM_UNIT_0, (float)g_frequency);
      pspwm_set_ps_duty(MCPWM_UNIT_0, (float)g_psDuty);
      ESP_LOGI(TAG,"g_frequency: %f, g_psDuty: %f ... ", g_frequency, g_psDuty);
      g_write_state_float_changed = false;
      g_update_frequency_changed = false;
    }

    if (g_psDuty <= 0.0f) {
      pspwm_disable_output(MCPWM_UNIT_0);
      g_onoff = false;
    }

    if (g_write_state_binary_changed) {
      if (g_onoff) {
        pspwm_resync_enable_output(MCPWM_UNIT_0);
      } else {
        pspwm_disable_output(MCPWM_UNIT_0);    
        if (error_occurred) { 
          // push off-button again to clear the flault state
          pspwm_clear_hw_fault_shutdown_occurred(MCPWM_UNIT_0); 
        }
      }  
      g_write_state_binary_changed = false;
    }
  }

  void update_frequency(float frequency) override {
    ESP_LOGI(TAG,"update_frequency(): %f ... \n", frequency);
    g_frequency = frequency * 1.0f;
    g_update_frequency_changed = true;
  }

  void write_state(float state) override {
    ESP_LOGI(TAG,"FloatOutput write_state(): %f ... \n", state);
    g_psDuty = state * 1.0f;
    g_write_state_float_changed = true;
  }

  void write_state(bool state) override {
    // ESP_LOGI(TAG,"BinaryOutput write_state(): %s ... \n", state ? "true" : "false");
    g_onoff = state ? true : false;
    g_write_state_binary_changed = true;
  }
};

class PSpwmCustomBinarySensor : public PollingComponent, public BinarySensor {
  public:
  
  PSpwmCustomBinarySensor() : PollingComponent(1000) {}

  void update() override {
    bool error_occurred = pspwm_get_hw_fault_shutdown_occurred(MCPWM_UNIT_0);
    publish_state(error_occurred);
  }
};