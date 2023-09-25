/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_hid_custom.h"
#include "ssd1306.h"
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NUM_LEDS 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
  extern USBD_HandleTypeDef hUsbDevice;
  static uint16_t ws2812b_frame[24 * (WS2812B_NUM_LEDS+2)] = {0};
  extern volatile uint8_t rxflag;
  extern char rxdata[16];
  volatile uint8_t blink_flag = 0;
  uint8_t warning_on = 0;

  uint16_t int_rpms = 0;
  uint16_t int_max_rpms = 500;
  char speed[3] = {0x30, 0x30, 0x30};
  char gear[1] = {0x30};
  char rpms[4];
  char max_rpms[4];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t USBD_CUSTOM_HID_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  MX_USB_DEVICE_Init();
  uint8_t report[5] = {0x01, 0x00, 0x00, 0x00, 0x00};
  keyboard_t Keyboard;

  KeyboardInit(&Keyboard);
  uint32_t temp = 0;
  color_hsv_t blue_color = {200,100,15};
  color_hsv_t red_color = {0,100,15};
  color_hsv_t black_color = {0,100,0};

  for (uint8_t ybright = 0; ybright<15; ybright++)
  {
	  HAL_Delay(40);
	  blue_color.value = ybright;
	  WS2812B_SetFullColor(ws2812b_frame, WS2812B_HSVtoRGB(blue_color));
	  WS2812B_UpdateLeds(ws2812b_frame);

  }

  for (uint8_t ybright = 15; ybright> 0 ; ybright--)
  {
	  HAL_Delay(40);
	  blue_color.value = ybright;
	  WS2812B_SetFullColor(ws2812b_frame, WS2812B_HSVtoRGB(blue_color));
	  WS2812B_UpdateLeds(ws2812b_frame);
  }

  blue_color.value = 15;
  WS2812B_SetFullColor(ws2812b_frame, WS2812B_HSVtoRGB(black_color));
  WS2812B_UpdateLeds(ws2812b_frame);

  ssd1306_Init();
  HAL_Delay(100);
  ssd1306_SetDisplayOn(1);


  HAL_TIM_Base_Start_IT(&htim3);

  // rounds

   ssd1306_DrawCircle(63,31,56,White);
   ssd1306_DrawCircle(63,31,60,White);
   ssd1306_UpdateScreen();

   HAL_Delay(50);

   // -   - speed
   ssd1306_DrawRectangle(14,13,24,17,White);
   ssd1306_DrawRectangle(102,13,112,17,White);
   ssd1306_UpdateScreen();

   HAL_Delay(50);

   // ( ) speed
   ssd1306_Line(38,2,30,15,White);
   ssd1306_Line(88,2,96,15,White);
   ssd1306_Line(30,15,38,28,White);
   ssd1306_Line(96,15,88,28,White);

   ssd1306_Line(34,2,26,15,White);
   ssd1306_Line(92,2,100,15,White);
   ssd1306_Line(26,15,34,28,White);
   ssd1306_Line(100,15,92,28,White);

   ssd1306_Line(34,2,38,2,White);
   ssd1306_Line(34,28,38,28,White);
   ssd1306_Line(88,2,92,2,White);
   ssd1306_Line(88,28,92,28,White);
   ssd1306_UpdateScreen();
   //------------------

   HAL_Delay(50);

   // (  ) gear---------
   ssd1306_Line(47,32,51,32,White);
   ssd1306_Line(47,58,51,58,White);
   ssd1306_Line(74,32,78,32,White);
   ssd1306_Line(74,58,78,58,White);

   ssd1306_Line(47,32,41,40,White);
   ssd1306_Line(41,40,41,50,White);
   ssd1306_Line(41,50,47,58,White);

   ssd1306_Line(51,32,45,40,White);
   ssd1306_Line(45,40,45,50,White);
   ssd1306_Line(45,50,51,58,White);

   ssd1306_Line(74,32,80,40,White);
   ssd1306_Line(80,40,80,50,White);
   ssd1306_Line(80,50,74,58,White);

   ssd1306_Line(78,32,84,40,White);
   ssd1306_Line(84,40,84,50,White);
   ssd1306_Line(84,50,78,58,White);

   ssd1306_UpdateScreen();
   //------------------

   HAL_Delay(50);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   while (1)
   {
		  // keyboard and HID --------------------------------------------
		  KeyboardScan(&Keyboard);
		  for(uint8_t counter = 0; counter<16; counter++)
		  {
			  if (Keyboard.button_status[counter] == PRESS)
				  temp += (1<<counter);
		  }
		  report[2] = (uint8_t) temp;
		  report[1] = (uint8_t)(temp>>8);
		  USBD_CUSTOM_HID_SendReport(&hUsbDevice, report, 5);
		  temp = 0;
		  //--------------------------------------------------------------
	  // leds and virtual-COM ----------------------------------------

	  if (rxflag == 1)
	  {
		  uint8_t byte_counter = 0;
		  uint8_t counter = 0;

		  for (counter = 0; counter<3; counter++)
			  speed[counter] = 0x30;

		  gear[0] = 0x30;

		  for (counter = 0; counter<4; counter++)
		  {
			  rpms[counter] = 0x30;
			  max_rpms[counter] = 0x30;
		  }

		  while(rxdata[byte_counter] != 0x3b)
		  {
			  switch (byte_counter)
			  {
			  case 0:
				  speed[2]=(char)rxdata[0];
				  break;
			  case 1:
				  speed[2]=(char)rxdata[1];
				  speed[1]=(char)rxdata[0];
				  break;
			  case 2:
				  speed[2]=(char)rxdata[2];
				  speed[1]=(char)rxdata[1];
				  speed[0]=(char)rxdata[0];
				  break;
			  default:
				  break;
			  }
			  byte_counter++;

		  }

		  counter = 0;
		  byte_counter++;
		  while(rxdata[byte_counter] != 0x3b)
		  {
			  gear[counter]=(char)rxdata[byte_counter];
			  byte_counter++;
		  }

		  counter = 0;
		  byte_counter++;
		  int_rpms = 0;
		  while(rxdata[byte_counter] != 0x3b)
		  {
			  rpms[counter]=(char)rxdata[byte_counter];
			  int_rpms *= 10;
			  int_rpms += (uint16_t)(rpms[counter]-0x30);

			  byte_counter++;
			  counter++;
		  }

		  counter = 0;
		  byte_counter++;
		  int_max_rpms = 0;
		  while(rxdata[byte_counter] != 0x3b)
		  {
			  max_rpms[counter]=(char)rxdata[byte_counter];
			  int_max_rpms *= 10;
			  int_max_rpms += (uint16_t)(max_rpms[counter]-0x30);

			  byte_counter++;
			  counter++;
		  }
		  for (uint8_t cnt = 0; cnt<16; cnt++)
			  rxdata[cnt] = 0;
		  rxflag = 0;
	  }

	ssd1306_SetCursor(40,5);
	ssd1306_WriteString("   ", Font_16x24, White);
	ssd1306_SetCursor(40,5);
	ssd1306_WriteString(speed, Font_16x24, White);

	ssd1306_SetCursor(56,35);
	ssd1306_WriteString(" ", Font_16x24, White);
	ssd1306_SetCursor(56,35);
	ssd1306_WriteString(gear, Font_16x24, White);

	ssd1306_UpdateScreen();

	if (int_rpms+300 < int_max_rpms)
	{
		WS2812B_SetProgressBar(ws2812b_frame, blue_color, int_max_rpms, int_rpms);
		WS2812B_UpdateLeds(ws2812b_frame);
	}
	else
	{
		if (blink_flag == 1)
		{
			if (warning_on == 0)
			{
				WS2812B_SetFullColor(ws2812b_frame, WS2812B_HSVtoRGB(red_color));
				warning_on = 1;
			}
			else
			{
				WS2812B_SetFullColor(ws2812b_frame, WS2812B_HSVtoRGB(black_color));
				warning_on = 0;
			}
			WS2812B_UpdateLeds(ws2812b_frame);
			blink_flag = 0;
		}
	}



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
