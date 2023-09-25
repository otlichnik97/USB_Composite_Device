/*
 * ws2812b.c
 *
 *  Created on: Sep 19, 2023
 *      Author: Alexander
 */

#include "stm32f4xx_hal.h"
#include "ws2812b.h"
#include "math.h"

extern TIM_HandleTypeDef htim1;

color_rgb_t WS2812B_HSVtoRGB(color_hsv_t color_hsv)
{
	color_rgb_t color_rgb = {0,0,0};

	double M = color_hsv.value * 2.55;
	double m = M * (100 - color_hsv.saturate) / 100.0;
	double z = (M - m)*(1 - fabs(fmod((color_hsv.hue/60.0), 2.0) - 1));

	if (color_hsv.hue >= 0  && color_hsv.hue < 60)
	{
		color_rgb.red = (uint8_t) M;
		color_rgb.green = (uint8_t) (z + m);
		color_rgb.blue = (uint8_t) m;
	}
	else if (color_hsv.hue >= 60  && color_hsv.hue < 120)
	{
		color_rgb.red = (uint8_t) (z + m);
		color_rgb.green = (uint8_t) M;
		color_rgb.blue = (uint8_t) m;
	}
	else if (color_hsv.hue >= 120  && color_hsv.hue < 180)
	{
		color_rgb.red = (uint8_t) m;
		color_rgb.green = (uint8_t) M;
		color_rgb.blue = (uint8_t) (z + m);
	}
	else if (color_hsv.hue >= 180  && color_hsv.hue < 240)
	{
		color_rgb.red = (uint8_t) m;
		color_rgb.green = (uint8_t) (z + m);
		color_rgb.blue = (uint8_t) M;
	}
	else if (color_hsv.hue >= 240  && color_hsv.hue < 300)
	{
		color_rgb.red = (uint8_t) (z + m);
		color_rgb.green = (uint8_t) m;
		color_rgb.blue = (uint8_t) M;
	}
	else if (color_hsv.hue >= 300  && color_hsv.hue < 360)
	{
		color_rgb.red = (uint8_t) M;
		color_rgb.green = (uint8_t) m;
		color_rgb.blue = (uint8_t) (z + m);
	}
	return color_rgb;
}

void WS2812B_SetPixelColor(uint16_t* frame, uint8_t pixel, color_rgb_t color)
{
	if (pixel < WS2812B_NUM_LEDS)
	{
		uint32_t temp = (color.green<<16) | (color.red<<8) | (color.blue);
		for (uint8_t counter = 0; counter<24; counter++)
		{
			if (temp & (1<<counter))
				frame[(23-counter) + (pixel+2) * 24] = WS2812B_BIT_1;
			else
				frame[(23-counter) + (pixel+2) * 24] = WS2812B_BIT_0;
		}
	}
}

void WS2812B_SetFullColor(uint16_t* frame, color_rgb_t color)
{
	for (uint8_t pixel = 0; pixel < WS2812B_NUM_LEDS; pixel++)
		WS2812B_SetPixelColor(frame, pixel, color);
}

void WS2812B_SetProgressBar(uint16_t* frame, color_hsv_t base_color, uint16_t max_value, uint16_t value)
{
	uint8_t temp_brightness = base_color.value;
	uint16_t rpm_to_led = max_value / WS2812B_NUM_LEDS;
	uint8_t num_full_leds = value / rpm_to_led;
	uint8_t last_led_brightness = (uint8_t)(((double)value / (double)rpm_to_led - (double)num_full_leds) * temp_brightness);

	for (uint8_t counter = 0; counter < num_full_leds; counter++)
	{
		switch (counter)
		{
		case 8:
			base_color.hue = 270;
			break;
		case 12:
			base_color.hue = 359;
			break;
		default:
			break;
		}
		WS2812B_SetPixelColor(frame, counter, WS2812B_HSVtoRGB(base_color));
	}

	base_color.value = last_led_brightness;
	WS2812B_SetPixelColor(frame, num_full_leds, WS2812B_HSVtoRGB(base_color));

	base_color.value = 0;
	for (uint8_t counter = num_full_leds+1; counter < WS2812B_NUM_LEDS; counter++)
	{
		WS2812B_SetPixelColor(frame, counter, WS2812B_HSVtoRGB(base_color));
	}
}

void WS2812B_UpdateLeds(uint16_t* frame)
{
	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, frame, 24*(WS2812B_NUM_LEDS+2));
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
}
