/*
 * ws2812b.h
 *
 *  Created on: Sep 19, 2023
 *      Author: Alexander
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#define WS2812B_NUM_LEDS 16
#define WS2812B_BIT_0 40
#define WS2812B_BIT_1 80

typedef struct
{
	uint8_t green;		// 0..255
	uint8_t red;		// 0..255
	uint8_t blue;		// 0..255
} color_rgb_t;

typedef struct
{
	uint16_t hue; 		// 0..360
	uint8_t saturate;	// 0..100
	uint8_t value;		// 0..100
} color_hsv_t;

color_rgb_t WS2812B_HSVtoRGB(color_hsv_t color_hsv);
void WS2812B_SetPixelColor(uint16_t* frame, uint8_t pixel, color_rgb_t color);
void WS2812B_SetFullColor(uint16_t* frame, color_rgb_t color);
void WS2812B_SetProgressBar(uint16_t* frame, color_hsv_t base_color, uint16_t max_value, uint16_t value);
void WS2812B_UpdateLeds(uint16_t* frame);
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);

#endif /* INC_WS2812B_H_ */
