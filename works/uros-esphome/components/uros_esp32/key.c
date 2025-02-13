#include "key.h"

#include "stdio.h"
#include "stdint.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/gpio.h"

// Check whether the key0 is pressed. If pressed, KEY_STATE_PRESS is returned. If released, KEY_STATE_RELEASE is returned
static uint8_t Key0_is_Pressed(void)
{
    uint8_t key_state = KEY_STATE_RELEASE;
    if (!gpio_get_level(KEY_GPIO_BOOT0))
    {
        key_state = KEY_STATE_PRESS;
    }
    return key_state;
}

// Check whether the key1 is pressed. If pressed, KEY_STATE_PRESS is returned. If released, KEY_STATE_RELEASE is returned
static uint8_t Key1_is_Pressed(void)
{
    uint8_t key_state = KEY_STATE_RELEASE;
    if (!gpio_get_level(KEY_GPIO_USER1))
    {
        key_state = KEY_STATE_PRESS;
    }
    return key_state;
}

// Non-blocking read key0 status (software anti-shake), Returns the KEY_STATE_PRESS once per key press
uint8_t Key0_Read_State(void)
{
	static uint16_t key_state = 0;

	if (Key0_is_Pressed() == KEY_STATE_PRESS)
	{
		if (key_state < 3)
		{
			key_state++;
		}
	}
	else
	{
		key_state = 0;
	}
	if (key_state == 2)
	{
		return KEY_STATE_PRESS;
	}
	return KEY_STATE_RELEASE;
}

// Non-blocking read key1 status (software anti-shake), Returns the KEY_STATE_PRESS once per key press
uint8_t Key1_Read_State(void)
{
	static uint16_t key_state = 0;

	if (Key1_is_Pressed() == KEY_STATE_PRESS)
	{
		if (key_state < 3)
		{
			key_state++;
		}
	}
	else
	{
		key_state = 0;
	}
    if (key_state == 2)
	{
		return KEY_STATE_PRESS;
	}
	return KEY_STATE_RELEASE;
}

void Key_Init(void)
{
    // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt 
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as input mode 
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set 
    io_conf.pin_bit_mask = ((1ULL<<KEY_GPIO_BOOT0)|(1ULL<<KEY_GPIO_USER1));
    //disable pull-down mode 
    io_conf.pull_down_en = 0;
    //enable pull-up mode 
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings 
    gpio_config(&io_conf);
}