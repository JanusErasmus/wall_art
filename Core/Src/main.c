/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Utils/terminal.h"
#include "ws2812.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void visInit()
{
	ws2812b_init();
}

void visHandle()
{
	ws2812b_handle();
}

void paint(uint8_t argc, char **argv)
{
	printf("Painting...\n");

	if(argc > 3)
	{
		int red = atoi(argv[1]);
		int g = atoi(argv[2]);
		int b = atoi(argv[3]);

//		printf("Set %c = %d\n", color, val);
//		if(color == 'r')
//		  {
			for (int r = 0; r < 16; ++r) {
				for (int c = 0; c < 16; ++c) {
					ws2812b_set_pixel(r,c,red,g,b);
				}
			}
//		  }
//		if(color == 'g')
//		  {
//			for (int r = 0; r < 16; ++r) {
//				for (int c = 0; c < 16; ++c) {
//					ws2812b_set_pixel(r,c,0,val,0);
//				}
//			}
//		  }
//		if(color == 'b')
//		  {
//			for (int r = 0; r < 16; ++r) {
//				for (int c = 0; c < 16; ++c) {
//					ws2812b_set_pixel(r,c,0,0,val);
//				}
//			}
//		  }
	}

	visHandle();
}


int mix_state = 0;
int mix_delta = 8;
int main_delta = 8;
void animate_mix(int *r, int *g, int *b)
{
	 switch(mix_state)
	 {
	 //mix green
	 case 0:
		 *g += mix_delta;
		 if((*g > 255) || (*g <= 0))
		 {
			 if(*g <= 0)
				 *g = 0;
			 if(*g > 255)
				 *g = 255;

			 mix_delta *= -1;

			 *r = *r + main_delta;
			 if((*r > 255) || (*r <= 0))
			 {
				 if(*r <= 0)
					 *r = 0;
				 if(*r > 255)
					 *r = 255;

				 main_delta *= -1;
				 mix_state = 1;
			 }
		 }
		 break;
		 //mix blue
	 case 1:
		 *b += mix_delta;
		 if((*b > 255) || (*b <= 0))
		 {
			 if(*b <= 0)
				 *b = 0;
			 if(*b > 255)
				 *b = 255;

			 mix_delta *= -1;

			 *r = *r + main_delta;
			 if((*r > 255) || (*r <= 0))
			 {
				 if(*r <= 0)
					 *r = 0;
				 if(*r > 255)
					 *r = 255;

				 main_delta *= -1;
				 mix_state = 2;
			 }
		 }
		 break;
		 //mix red and blue
	 case 2:
		 *g += mix_delta;
		 *b += mix_delta;
		 if((*g > 255) || (*g <= 0))
		 {
			 if(*b <= 0)
				 *b = 0;
			 if(*b > 255)
				 *b = 255;

			 if(*g <= 0)
				 *g = 0;
			 if(*g > 255)
				 *g = 255;

			 mix_delta *= -1;

			 *r = *r + main_delta;
			 if((*r > 255) || (*r < 0))
			 {
				 if(*r <= 0)
					 *r = 0;
				 if(*r > 255)
					 *r = 255;

				 main_delta *= -1;
				 mix_state = 0;
			 }
		 }
		 break;
	 }
}

int row_delta = 1;
void animate_column_run(int *row)
{
	*row += row_delta;

	if(*row > 15)
	{
		row_delta *= -1;
		*row = 14;
	}
	if(*row < 0)
	{
		row_delta *= -1;
		*row = 1;
	}
}

double radius = 100;
int red_row = 15;
int red_col = 0;
int green_row = 0;
int green_col = 7;
int blue_row = 15;
int blue_col = 15;
double scale = 1;//0.9;

void get_scale(int row, int col, int *r, int *g, int *b)
{
	//red distance (15, 0)
	int delta_r = red_row - row;
	int delta_c = red_col - col;
	int distance = (delta_r *delta_r) + (delta_c * delta_c);
	double factor = 1.0 - ((double)distance / radius);
	if(factor < 0)
		factor = 0;
	//printf("R (%d, %d), dr: %d, dc %d, d %d, f %f\n", row, col, delta_r, delta_c, distance, factor);
	*r = factor * 255.0 * scale;

	//green distance (0, 7)
	delta_r = green_row - row;
	delta_c = green_col - col;
	distance = (delta_r *delta_r) + (delta_c * delta_c);
	factor = 1.0 - ((double)distance / radius);
	if(factor < 0)
		factor = 0;
	//printf("G (%d, %d), dr: %d, dc %d, d %d, f %f\n", row, col, delta_r, delta_c, distance, factor);
	*g = factor * 255.0 * scale;

	//blue distance (15, 15)
	delta_r = blue_row - row;
	delta_c = blue_col - col;
	distance = (delta_r *delta_r) + (delta_c * delta_c);
	factor = 1.0 - ((double)distance / radius);
	if(factor < 0)
		factor = 0;
	//printf("B (%d, %d), dr: %d, dc %d, d %d, f %f\n", row, col, delta_r, delta_c, distance, factor);
	*b = factor * 255.0 * scale;
}

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
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  setbuf(stdout, 0);
  printf("LED Wall art\n");
  printf("SYSCLOCK: %lu Hz\n", HAL_RCC_GetSysClockFreq());
  printf("HCLOCK  : %lu Hz\n", HAL_RCC_GetHCLKFreq());
  printf("P1CLOCK : %lu Hz\n", HAL_RCC_GetPCLK1Freq());
  printf("P2CLOCK : %lu Hz\n", HAL_RCC_GetPCLK2Freq());

  //SystemCorecClockUpdate();

  visInit();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  terminal_init("art $");

  uint32_t tick = 0;
  //int row = 0;
  //int column = 0;
  int red_delta = 1;
  int green_delta = 1;
  int blue_delta = 1;
  int r = 0,g = 0,b = 0;
  ws2812b_handle();

  while (1)
  {
	  terminal_run();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if(tick < HAL_GetTick())
	  {
		  tick = HAL_GetTick() + 100;
		  ws2812b_clear();

//		  animate_mix(&r, &g, &b);
//
//		  //printf("(%3d, %3d, %3d)\n", r, g, b);
//
//		  for (int row = 0; row < 16; ++row)
//		  {
//			  for (int col = 0; col < 16; ++col)
//			  {
//				  ws2812b_set_pixel(col, row, r, g, b);
//			  }
//		  }
//
		  red_row += red_delta;
		  if(red_row > 15)
		  {
			  red_row = 14;
			  red_delta *= -1;
		  }
		  if(red_row < 0)
		  {
			  red_row = 1;
			  red_delta *= -1;
		  }

		  green_row += green_delta;
		  if(green_row > 15)
		  {
			  green_row = 14;
			  green_delta *= -1;
		  }
		  if(green_row < 0)
		  {
			  green_row = 1;
			  green_delta *= -1;
		  }

		  blue_row += blue_delta;
		  if(blue_row > 15)
		  {
			  blue_row = 14;
			  blue_delta *= -1;
		  }
		  if(blue_row < 0)
		  {
			  blue_row = 1;
			  blue_delta *= -1;
		  }

		  for (int row = 0; row < 16; ++row)
		  {
			  for (int col = 0; col < 16; ++col)
			  {
				  get_scale(row, col, &r, &g, &b);
				  ws2812b_set_pixel(row, col, r, g, b);
			  }
		  }
//		  ws2812b_set_pixel(red_row, red_col, 255, 0, 0);
//		  ws2812b_set_pixel(green_row, green_col, 0, 255, 0);
//		  ws2812b_set_pixel(blue_row, blue_col, 0, 0, 255);
		  ws2812b_handle();
	  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
int __io_putchar(int ch)
{
	if(ch == '\r')
	{
		uint8_t byte = '\n';
		HAL_UART_Transmit(&huart2, &byte, 1, 100);
	}
	if(ch == '\n')
	{
		uint8_t byte = '\r';
		HAL_UART_Transmit(&huart2, &byte, 1, 100);
	}

	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 100);

	return 0;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
