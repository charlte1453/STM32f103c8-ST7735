/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st7735.h"
#include <stdlib.h>
#include "sprites.h"

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
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;

PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE BEGIN PV */

#define MAXBULLETS 128
#define MAXENEMYBULLETS 50

struct enemy {
	uint8_t x;
	uint8_t y;
	uint8_t isAlive;
	uint8_t isDying;
};

 struct bullet {
	uint8_t x;
	uint8_t y;
	uint8_t speed;
	uint8_t isActive;
};

 struct player {
	uint8_t x;
	uint8_t y;
	int8_t dx;
	int8_t dy;
	uint8_t sprite; // 0 is normal , 1 is left orientated , 2 is right orientated
};

struct enemyBullets {
	 uint8_t x;
	 uint8_t y;
	 uint8_t dx;
	 uint8_t dy;
	 uint8_t isActive;
 };

struct  bullet Bullets[MAXBULLETS];
struct enemyBullets evilBullets[MAXENEMYBULLETS];

void init_Bullets(){
	for(int i = 0 ; i < MAXBULLETS ; i++){
		Bullets[i].x = 0;
		Bullets[i].y = 0;
		Bullets[i].speed = 0;
		Bullets[i].isActive = 0;
	};
}
void init_enemyBullets(){
	for(int i = 0 ; i < MAXENEMYBULLETS ; i++){
		evilBullets[i].x = 0;
		evilBullets[i].y = 0;
		evilBullets[i].dx= 0;
		evilBullets[i].dy = 0;
		evilBullets[i].isActive= 0;
	}
}








struct enemy enemyList[20] = {
    {124, 42 , 1 , 0 },
    {59, 15 , 1 , 0},
    {44, 1 , 1 , 0 },
    {102, 4 , 1 , 0},
    {8, 16 , 1 , 0},
    {44, 28 , 1 , 0},
    {24, 22 , 1 , 0},
    {111, 30 , 1 , 0 },
    {87, 13 , 1 , 0},
    {81, 37 , 1 , 0},
    {6, 33 , 1 , 0 },
    {35, 43 , 1 , 0},
    {105, 49  , 1 , 0},
    {69, 48 , 1 , 0 },
    {119, 15 , 1 , 0},
    {71, 1 , 1 , 0},
    {24, 1 , 1 , 0},
    {127, 1 ,1 , 0},
    {19, 48 , 1 , 0},
    {52, 46 , 1 , 0}
};
int attackPattern[10][5] = {
    {13,  2, 18,  7, 11},
    { 0,  9,  4,  1, 17},
    { 6, 14, 19, 12,  3},
    {15,  8,  5,  2, 20},
    {16,  7, 13,  0,  6},
    {10,  4,  1, 18, 11},
    { 3,  8, 17, 12,  5},
    {14,  9,  6, 19,  2},
    { 7, 15, 20,  0, 10},
    {16,  1,  3, 13,  9}
};
struct player playerObj = {64 , 110 ,  0 , 0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
static void MX_USB_PCD_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//void EXTI4_IRQHandler(void){
//
//	/*if(~gameStarted){
//		gameStarted = 1;
//		EXTI->PR |= EXTI_PR_PR4;
//
//		return;
//
//	}*/
//	ST7735_FillRectangleFast(playerObj.x , playerObj.y , 9 , 9 , 0x0000);
//	playerObj.x++;
//	ST7735_DrawImage(playerObj.x , playerObj.y , 9 , 9 , player_ship_flat);
//
//	EXTI->PR |= EXTI_PR_PR4;
//
//}

void killEnemy(struct enemy* enemy){
	enemy->isDying = 1;
}
void calculateCollisions(){
	for(int i = 0 ; i < MAXBULLETS ; i++){
		if(Bullets[i].isActive == 0){
			return;
		}

		for(int j = 0 ; j <20 ; j++){
			if(enemyList[j].isAlive == 0){
				continue;
			}

			int8_t disp_x = enemyList[j].x-Bullets[i].x;
			int8_t disp_y = enemyList[j].y-Bullets[i].y;
			disp_x = abs(disp_x);
			disp_y = abs(disp_y);

			if(disp_x < 8 && disp_y < 8){
				killEnemy(&enemyList[j]);
				Bullets[i].isActive = 0;
				ST7735_DrawPixel(Bullets[i].x , Bullets[i].y , 0x0000);
				break;
			}
		}

	}
}

void update_Bullets(){
	for(int i = 0 ; i < MAXBULLETS ; i++){
		if(!Bullets[i].isActive){
			continue;
		}

		if(Bullets[i].y == 127){
			Bullets[i].isActive = 0;
		}

		ST7735_DrawPixel(Bullets[i].x , Bullets[i].y , 0x0000);
		Bullets[i].y -= Bullets[i].speed;
		ST7735_DrawPixel(Bullets[i].x , Bullets[i].y , 0xFFFF);
	}
}
//void EXTI3_IRQHandler(void){
//
//	/*if(~gameStarted){
//		gameStarted = 1;
//		EXTI->PR |= EXTI_PR_PR4;
//
//		return;
//
//	}*/
//
//	for (int i = 0; i < MAXBULLETS; i++) {
//	            if (!Bullets[i].isActive) {
//	                Bullets[i].x = playerObj.x+5;
//	                Bullets[i].y = playerObj.y+1;
//	                Bullets[i].speed = 4;
//	                Bullets[i].isActive = 1;
//	                break;
//	            }
//	        }
//
//	EXTI->PR |= EXTI_PR_PR3;
//
//}
//void EXTI1_IRQHandler(void){
//
//	/*if(~gameStarted){
//		gameStarted = 1;
//		EXTI->PR |= EXTI_PR_PR4;
//
//		return;
//
//	}*/
//
//	ST7735_FillRectangleFast(playerObj.x , playerObj.y , 9 , 9 , 0x0000);
//	playerObj.x--;
//	ST7735_DrawImage(playerObj.x , playerObj.y , 9 , 9 , player_ship_flat);
//	EXTI->PR |= EXTI_PR_PR1;
//
//}

void updateEnemyStates(){
	for(int i = 0 ; i < 20 ; i++){
		if(enemyList[i].isAlive == 0){
			continue;
		}else if(enemyList[i].isDying == 1){
			enemyList[i].isDying++ ;
			ST7735_DrawImage(enemyList[i].x , enemyList[i].y , 8 , 8 , enemy_ship_dying1);
		}else if(enemyList[i].isDying == 2){
			enemyList[i].isDying++ ;
			ST7735_DrawImage(enemyList[i].x , enemyList[i].y , 8 , 8 , enemy_ship_dying2);
		}else if(enemyList[i].isDying == 3){
			enemyList[i].isAlive = 0;
			enemyList[i].isDying = 0;
			ST7735_FillRectangleFast(enemyList[i].x , enemyList[i].y , 8 , 8 , 0x0000);
		}
	}
}

void updatePlayerSpeed(){
	int8_t speed = GPIOA->IDR;
	if((speed & 0x1B )== 0){
		playerObj.dx = 0;
		playerObj.dy = 0;
		return;
	}

	if(playerObj.dx == 0x3) speed &= ~(0x09);
	if(playerObj.dy == 0x3) speed &= ~(0x11);


	if(speed & DOWN_Pin) playerObj.dy++;
	if(speed & UP_Pin) playerObj.dy--;
	if(speed & LEFT_Pin) playerObj.dx--;
	if(speed & RIGHT_Pin) playerObj.dx++;
}

void updatePlayerPosition(){
	ST7735_FillRectangleFast(playerObj.x , playerObj.y , 12 , 11 , 0x0000);
	playerObj.x += playerObj.dx ;
	playerObj.y += playerObj.dy ;

	ST7735_DrawImage(playerObj.x , playerObj.y , 12 , 11 , player_ship_flat);
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
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_USB_PCD_Init();
  /* USER CODE BEGIN 2 */
  ST7735_Init();
  ST7735_FillScreenFast(0x0000);

  init_Bullets();

  for(int i = 0 ; i <20 ; i++){
	  ST7735_DrawImage(enemyList[i].x , enemyList[i].y , 8 , 8 , enemy_ship_flat1 );
  }

  ST7735_DrawImage(playerObj.x , playerObj.y , 12 , 11 , player_ship_flat);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
	  updatePlayerSpeed();
	  updatePlayerPosition();
	  update_Bullets();
	  calculateCollisions();
	  for(int i = 0 ; i < 20 ; i++){
		  if(enemyList[i].isAlive == 0 || enemyList[i].isDying != 0){
			  continue;
		  }else{
			  ST7735_DrawImage(enemyList[i].x , enemyList[i].y , 8 , 8 , enemy_ship_flat1);
		  }
	  }
	  updateEnemyStates();
	  HAL_Delay(100);
	  for(int i = 0 ; i < 20 ; i++){
		  if(enemyList[i].isAlive == 0 || enemyList[i].isDying != 0){
			  continue;
		  }else{
			  ST7735_DrawImage(enemyList[i].x , enemyList[i].y , 8 , 8 , enemy_ship_flat2);
		  }
	  }
	  HAL_Delay(100);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_1LINE;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ST7735_BL_Pin|ST7735_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ST7735_CS_Pin|ST7735_RES_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : DOWN_Pin RIGHT_Pin FIRE_Pin LEFT_Pin
                           UP_Pin */
  GPIO_InitStruct.Pin = DOWN_Pin|RIGHT_Pin|FIRE_Pin|LEFT_Pin
                          |UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ST7735_BL_Pin ST7735_DC_Pin */
  GPIO_InitStruct.Pin = ST7735_BL_Pin|ST7735_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ST7735_CS_Pin ST7735_RES_Pin */
  GPIO_InitStruct.Pin = ST7735_CS_Pin|ST7735_RES_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
