#include "beep.h"

#include "stdio.h"
#include "stdint.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

const static char *TAG = "BEEP";

static uint8_t beep_state = BEEP_STATE_OFF;
static uint16_t beep_on_time = 0;

// configure GPIO with the buzzer
static void Beep_GPIO_Init(void)
{
    // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    // disable interrupt 
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // set as output mode 
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask of the pins that you want to set 
    io_conf.pin_bit_mask = (1ULL<<BEEP_GPIO);
    // disable pull-down mode 
    io_conf.pull_down_en = 0;
    // disable pull-up mode 
    io_conf.pull_up_en = 0;
    // configure GPIO with the given settings 
    gpio_config(&io_conf);
    // turn off the buzzer
    Beep_Off();
}

// Buzzer background task
static void Beep_Task(void *arg)
{
    ESP_LOGI(TAG, "Start Beep_Task with core:%d", xPortGetCoreID());
    while (1)
    {
        Beep_Handle();

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}


// Initialize the buzzer
void Beep_Init(void)
{
    Beep_GPIO_Init();

    // start buzzer task
    xTaskCreatePinnedToCore(Beep_Task, "Beep_Task", 3*1024, NULL, 5, NULL, 1);
}

// turn on buzzer
void Beep_On(void)
{
    beep_state = BEEP_STATE_ON_ALWAYS;
    beep_on_time = 0;
    gpio_set_level(BEEP_GPIO, BEEP_ACTIVE_LEVEL);
}

// turn off buzzer
void Beep_Off(void)
{
    beep_state = BEEP_STATE_OFF;
    beep_on_time = 0;
    gpio_set_level(BEEP_GPIO, !BEEP_ACTIVE_LEVEL);
}

// Set the buzzer start time. When time=0, the buzzer is turned off. 
// When time=1, the buzzer keeps ringing. 
// When time>=10, the buzzer is turned off automatically after xx milliseconds
void Beep_On_Time(uint16_t time)
{
	if (time == 1)
	{
		beep_state = BEEP_STATE_ON_ALWAYS;
		beep_on_time = 0;
		Beep_On();
	}
	else if (time == 0)
	{
		beep_state = BEEP_STATE_OFF;
		beep_on_time = 0;
		Beep_Off();
	}
	else
	{
		if (time < 10) time = 10;
        if (time > 10000) time = 10000;
        beep_state = BEEP_STATE_ON_DELAY;
        beep_on_time = (time / 10);
        gpio_set_level(BEEP_GPIO, BEEP_ACTIVE_LEVEL);
	}
}

// The buzzer automatically stops the controller and is called every 10 milliseconds.
void Beep_Handle(void)
{
    if (beep_state == BEEP_STATE_ON_DELAY)
    {
        if (beep_on_time > 0)
        {
            beep_on_time--;
        }
        else
        {
            Beep_Off();
            beep_state = BEEP_STATE_OFF;
        }
    }
}