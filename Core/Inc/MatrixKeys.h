/*
 * MatrixKeyboardLib.h
 *
 *  Created on: Jun 23, 2023
 *      Author: Shilin Alexander
 */

#ifndef INC_MATRIXKEYS_H_
#define INC_MATRIXKEYS_H_

#define KEYBOARD_LINES 4
#define KEYBOARD_COLUMNS 4

#define HOLD_TIME 15 // сколько опросов должна быть нажата кнопка для распознавания удержания

typedef enum
{
	FREE = 0,
	PRESS = 1,
	RELEASED = 3
} button_status_e;

typedef struct
{
	GPIO_TypeDef* lines_port[KEYBOARD_LINES];
	uint16_t lines_pin[KEYBOARD_LINES];
	GPIO_TypeDef* columns_port[KEYBOARD_COLUMNS];
	uint16_t columns_pin[KEYBOARD_COLUMNS];

	button_status_e button_status[KEYBOARD_LINES * KEYBOARD_COLUMNS];
	uint16_t button_counter[KEYBOARD_LINES * KEYBOARD_COLUMNS];
} keyboard_t;

void KeyboardInit(keyboard_t* keyboard);
void KeyboardScan(keyboard_t* keyboard);
uint8_t GetKeyStatus(keyboard_t* keyboard, uint8_t key_number);

#endif /* INC_MATRIXKEYS_H_ */
