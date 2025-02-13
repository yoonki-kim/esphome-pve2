#include "led.h"

#include "stdio.h"
#include "stdint.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

// The LED blinks and needs to be called every 10 milliseconds. interval Indicates the interval (unit: 10ms).
void Led_Flash(uint16_t interval)
{
    static uint16_t state = 0;
    static uint16_t count = 0;
    count++;
    if (count >= interval)
    {
        count = 0;
        state = (state + 1) % 2;
        Led_State(state);
    }
}


// Initial LED
void Led_Init(void)
{
    // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt 
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode 
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set 
    io_conf.pin_bit_mask = (1ULL<<LED_GPIO);
    //disable pull-down mode 
    io_conf.pull_down_en = 0;
    //disable pull-up mode 
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings 
    gpio_config(&io_conf);
    // turn off led
    Led_Off();
}

// turn on led
void Led_On(void)
{
    gpio_set_level(LED_GPIO, LED_ACTIVE_LEVEL);
}

// turn off led
void Led_Off(void)
{
    gpio_set_level(LED_GPIO, !LED_ACTIVE_LEVEL);
}

// Change the LED status. state=1 indicates on, and state=0 indicates off.
void Led_State(uint8_t state)
{
    if (state == 0)
    {
        gpio_set_level(LED_GPIO, !LED_ACTIVE_LEVEL);
    }
    else
    {
        gpio_set_level(LED_GPIO, LED_ACTIVE_LEVEL);
    }
}