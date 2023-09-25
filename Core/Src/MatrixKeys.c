/*
 * MatrixKeyboardLib.c
 *
 *  Created on: Jun 23, 2023
 *      Author: Shilin Alexander
 */

#include "stm32f4xx_hal.h"
#include "MatrixKeys.h"

GPIO_TypeDef* columns_port[KEYBOARD_COLUMNS] = {GPIOA, GPIOA, GPIOA, GPIOA};
uint16_t columns_pin[KEYBOARD_COLUMNS] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4};

GPIO_TypeDef* lines_port[KEYBOARD_LINES] = {GPIOA, GPIOA, GPIOA, GPIOB};
uint16_t lines_pin[KEYBOARD_LINES] = {GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_0};

void KeyboardInit(keyboard_t* keyboard)
{
	// Set ports for matrix keyboard
	//-----------------------------------------------------------------
	for (uint8_t i = 0; i < KEYBOARD_COLUMNS; i++)
	{
		keyboard->columns_port[i] = columns_port[i];
		keyboard->columns_pin[i] = columns_pin[i];
	}

	for (uint8_t i = 0; i < KEYBOARD_LINES; i++)
	{
		keyboard->lines_port[i] = lines_port[i];
		keyboard->lines_pin[i] = lines_pin[i];
	}

	// Set default function for each buttons of matrix keyboard
	//-----------------------------------------------------------------
	for (uint8_t i = 0; i < KEYBOARD_LINES*KEYBOARD_COLUMNS; i++)
	{
	    keyboard->button_status[i] = FREE;
	    keyboard->button_counter[i] = 0;
	}
}

void KeyboardScan(keyboard_t* keyboard)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	uint8_t index = 0;

	// Первый опрос матричной клавиатуры
	// Всем нажатым кнопкам присваивается статус PRESS
	//=========================================================================================================================================================================
	for (uint8_t line = 0; line < KEYBOARD_LINES; line++)
	{
		GPIO_InitStruct.Pin =  keyboard->lines_pin[line];					// конфигурируем порт строки как выход
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

		HAL_GPIO_Init(keyboard->lines_port[line], &GPIO_InitStruct);

		HAL_GPIO_WritePin(keyboard->lines_port[line], keyboard->lines_pin[line], GPIO_PIN_RESET);

		for (uint8_t col = 0; col < KEYBOARD_COLUMNS; col++)
		{
			index = line*KEYBOARD_COLUMNS + col;
			if (HAL_GPIO_ReadPin(keyboard->columns_port[col], keyboard->columns_pin[col]) == GPIO_PIN_RESET)
			{
				if (keyboard->button_status[index] == FREE)
					keyboard->button_status[index] = PRESS;
			}

			else
			{
				if (keyboard->button_status[index] == PRESS)
				{
						keyboard->button_status[index] = FREE;
				}
			}
		}
		HAL_GPIO_WritePin(keyboard->lines_port[line], keyboard->lines_pin[line], GPIO_PIN_SET);

		GPIO_InitStruct.Pin = keyboard->lines_pin[line];					// конфигурируем порт строки обратно как вход
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(keyboard->lines_port[line], &GPIO_InitStruct);
	}
}

uint8_t GetKeyStatus(keyboard_t* keyboard, uint8_t key_number)
{
	return keyboard->button_status[key_number];
}

