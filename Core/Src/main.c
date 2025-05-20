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

//hiiii hiiiiii 
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
	uint8_t health;
	uint8_t sprite;
	uint8_t ticksUntilSpriteChange;
	uint8_t ticksSinceLastChange;
	int8_t dx;
	int8_t dy;
};

 struct bullet {
	uint8_t x;
	uint8_t y;
	int8_t speed;
	uint8_t isActive;
};

 struct player {
	uint8_t x;
	uint8_t y;
	int8_t dx;
	int8_t dy;
	uint8_t sprite; // 0 is normal , 1 is left orientated , 2 is right orientated
	uint8_t health;
};



struct enemyBullets {
	 uint8_t x;
	 uint8_t y;
	 int8_t dx;
	 int8_t dy;
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
    {124, 42 , 1 , 0 , 1 , 0 , 22 , 0},
    {59, 15 , 1 , 0 , 1  , 1 , 25 , 0 },
    {44, 1 , 1 , 0  ,1 , 1 , 18 , 0},
    {102, 4 , 1 , 0, 1 , 0 , 23 , 0},
    {8, 16 , 1 , 0 , 1 , 1 , 21 , 0},
    {44, 28 , 1 , 0 ,1 ,  1 , 15 , 0},
    {24, 22 , 1 , 0 ,1 , 0 , 20 , 0},
    {111, 30 , 1 , 0 , 1 , 1 , 21 , 0},
    {87, 13 , 1 , 0 , 1 , 0 , 20 , 0},
    {81, 37 , 1 , 0 , 1, 1 , 21 , 0},
    {6, 33 , 1 , 0 , 1 , 1 , 17 , 0},
    {35, 43 , 1 , 0 , 1 , 1 , 14 , 0},
    {105, 49  , 1 , 0, 1, 1 , 25 , 0},
    {69, 48 , 1 , 0 , 1 , 1 , 20 , 0},
    {119, 15 , 1 , 0 , 1, 0 , 19 , 0},
    {71, 1 , 1 , 0 , 1 , 0 , 21 , 0},
    {24, 1 , 1 , 0 , 1 , 1 , 22 , 0},
    {127, 1 ,1 , 0 , 1 , 1 , 20 , 0},
    {19, 48 , 1 , 0, 1, 0  , 21 , 0},
    {52, 46 , 1 , 0 , 1 , 0 , 20 , 0}
};

uint8_t attackPattern[10][5] = {
    {13,  2, 18,  7, 11},
    { 0,  9,  4,  1, 17},
    { 6, 14, 19, 12,  3},
    {15,  8,  5,  2, 20},
    {16,  7, 13,  0,  6},
    {10,  4,  1, 18, 11},
    { 3,  8, 17, 12,  5},
    {14,  9,  6, 19,  2},
    { 7, 15, 19,  0, 10},
    {16,  1,  3, 13,  9}
};
struct player playerObj = {64 , 110 ,  0 , 0 , 0};

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
uint8_t enemiesAlive = 20;

void killEnemy(struct enemy* enemy){
	enemy->isDying = 1;
	enemy->ticksSinceLastChange = 0;
	ST7735_DrawImage(enemy->x , enemy->y , 8 , 8 , enemy_ship_dying2);



}
void calculateCollisions(){
	for(int i = 0 ; i < MAXBULLETS ; i++){
		if(Bullets[i].isActive == 0){
			return;
		}

		for(int j = 0 ; j <20 ; j++){
			if(enemyList[j].isAlive == 0 || enemyList[j].isDying == 1){
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
uint16_t playerBulletColour = 0xFFFF;
uint16_t EnemyBulletColour = 0xFFFF;

void update_Bullets(){
	for(int i = 0 ; i < MAXBULLETS ; i++){
		if(!Bullets[i].isActive){
			continue;
		}

		if(Bullets[i].y >= 128){
			Bullets[i].isActive = 0;
			ST7735_DrawPixel(Bullets[i].x , Bullets[i].y , 0x0000);
		}

		ST7735_DrawPixel(Bullets[i].x , Bullets[i].y , 0x0000);
		Bullets[i].y -= Bullets[i].speed;
		ST7735_DrawPixel(Bullets[i].x , Bullets[i].y , playerBulletColour);
	}

	for(int i = 0 ; i < MAXENEMYBULLETS ; i++){
		if(!evilBullets[i].isActive){
			continue;
		}
		if(evilBullets[i].y >= 128){
			evilBullets[i].isActive = 0;
			ST7735_DrawPixel(evilBullets[i].x , evilBullets[i].y , 0x0000);
		}

		ST7735_DrawPixel(evilBullets[i].x , evilBullets[i].y , 0x0000);
		evilBullets[i].y -= evilBullets[i].dy;
		ST7735_DrawPixel(evilBullets[i].x , evilBullets[i].y , EnemyBulletColour);

	}
}
void EXTI2_IRQHandler(void){

	/*if(~gameStarted){
		gameStarted = 1;
		EXTI->PR |= EXTI_PR_PR4;

		return;

	}*/

	for (int i = 0; i < MAXBULLETS; i++) {
	            if (!Bullets[i].isActive) {
	                Bullets[i].x = playerObj.x+5;
	                Bullets[i].y = playerObj.y+1;
	                Bullets[i].speed = 4;
	                Bullets[i].isActive = 1;
	                break;
	            }
	}

	EXTI->PR |= EXTI_PR_PR3;

}
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
uint8_t deathAnimationTicks = 20;
void updateEnemyStates(){
	for(int i = 0 ; i < 20 ; i++){
		if(enemyList[i].isAlive == 0){
			continue;
		}else if(enemyList[i].isDying == 1){
			if(!(enemyList[i].ticksSinceLastChange == deathAnimationTicks)) continue;
			enemyList[i].isDying++ ;
			ST7735_DrawImage(enemyList[i].x , enemyList[i].y , 8 , 8 , enemy_ship_dying2);
			enemyList[i].ticksSinceLastChange = 0;
		}else if(enemyList[i].isDying == 2){
			if(!(enemyList[i].ticksSinceLastChange == deathAnimationTicks)) continue;
			enemyList[i].isDying++ ;
			ST7735_DrawImage(enemyList[i].x , enemyList[i].y , 8 , 8 , enemy_ship_dying2);
			enemyList[i].ticksSinceLastChange = 0;
		}else if(enemyList[i].isDying == 3){
			if(!(enemyList[i].ticksSinceLastChange == deathAnimationTicks)) continue;
			enemyList[i].isAlive = 0;
			enemyList[i].isDying = 0;
			enemiesAlive--;
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

	if(playerObj.dx == 0x2) speed &= ~(0x09);
	if(playerObj.dy == 0x2) speed &= ~(0x11);


	if(speed & DOWN_Pin) playerObj.dy++;
	if(speed & UP_Pin) playerObj.dy--;
	if(speed & LEFT_Pin) playerObj.dx--;
	if(speed & RIGHT_Pin) playerObj.dx++;
}


int checkIfOutOfBounds(){
	uint8_t futureX = playerObj.x + playerObj.dx;
	uint8_t futureY = playerObj.y + playerObj.dy;

	if(futureX >= 117) {
		return 1;
	}
	else  if(futureY >= 116){
		return 1;
	}else{
		return 0;
	}

}

void updatePlayerPosition(){
	ST7735_FillRectangleFast(playerObj.x , playerObj.y , 12 , 11 , 0x0000);

	uint8_t outofBounds = checkIfOutOfBounds();

	if(!outofBounds){
		playerObj.x += playerObj.dx ;
		playerObj.y += playerObj.dy ;
	}


	ST7735_DrawImage(playerObj.x , playerObj.y , 12 , 11 , player_ship_flat);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */


uint8_t tickSpeed = 50;

void createEnemyBullets(uint8_t patternIteration){

	for(int k = 0; k < 5 ; k++){
		if(enemyList[attackPattern[patternIteration][k]].isAlive && !(enemyList[attackPattern[patternIteration][k]].isDying))
		for (int i = 0; i < MAXENEMYBULLETS; i++) {
		            if (!(evilBullets[i].isActive)) {
		                evilBullets[i].x = enemyList[attackPattern[patternIteration][k]].x+4;
		                evilBullets[i].y = enemyList[attackPattern[patternIteration][k]].y+9;
		                evilBullets[i].dy = -2;
		                evilBullets[i].isActive = 1;
		                break;
		            }
		}
	}

}
uint8_t negativeMask = 0x80;
void generateNewSpeeds(struct enemy* enemy) {
    // Random small values between -3 and +3
    int8_t delx = (rand() % 7) - 3;
    int8_t dely = (rand() % 7) - 3;

    enemy->dx = delx;
    enemy->dy = dely;
}

void updateEnemyAnimations(){ //Change enemy animations and also their speeds
	  for(int i = 0 ; i < 20 ; i++){
		  if(enemyList[i].isAlive == 0 || enemyList[i].isDying != 0){
			  continue;
		  }

		  if(enemyList[i].ticksSinceLastChange == enemyList[i].ticksUntilSpriteChange){
			  enemyList[i].sprite ^= 1;

			  generateNewSpeeds(&enemyList[i]);
			  enemyList[i].ticksSinceLastChange = 0;
		  }

	  }
}

void incrementTicksSinceLastAnimationChange(){
	for(int i = 0 ; i < 20 ; i++){
		enemyList[i].ticksSinceLastChange++;
	}
}

uint8_t threshold = 8;
void enemyCollisionAvoidance(struct enemy* enemy , int i ){ //i is the index of the enmey in the enemylist array
		uint8_t currentEnemyX = enemy->x;
		uint8_t currentEnemyY = enemy->y;
		uint8_t conflict = 0;

		uint8_t futureEnemyX = currentEnemyX + enemy->dx;
		uint8_t futureEnemyY = currentEnemyY + enemy->dy;

		for(int k = 0 ; k < 20 ; k++){
			if(k == i) continue;

			uint8_t delx = abs(futureEnemyX-enemyList[k].x);
			uint8_t dely = abs(futureEnemyY-enemyList[k].y);

			if((delx < threshold ) && (dely < threshold ) ){
				conflict = 1 ;
				break ; //return early without modification if overlap found
			}



		}

		if(!conflict){
			enemy->x = futureEnemyX;
			enemy->y = futureEnemyY;
		}


}
uint8_t movementTicks = 2;

int enemyOutOfBounds(struct enemy* enemy){
	uint8_t futureX = enemy->x + enemy->dx;
	uint8_t futureY = enemy->y + enemy->dy;

	if(futureX >= 120) {
		return 1;
	}
	else  if(futureY >= 120){
		return 1;
	}else{
		return 0;
	}
}
void updateEnemyPositions(){
	for(int i = 0 ; i < 20 ; i++){
		if(enemyList[i].isAlive == 0 || enemyList[i].isDying != 0){
			continue;
		}

		if(!enemyList[i].ticksSinceLastChange%movementTicks && !enemyList[i].ticksSinceLastChange){

			uint8_t outOfBounds = enemyOutOfBounds(&enemyList[i]);

			ST7735_FillRectangleFast(enemyList[i].x , enemyList[i].y , 8 , 8 , 0x0000);


			if(!outOfBounds) enemyCollisionAvoidance(&enemyList[i] , i);

			 if(!enemyList[i].sprite){
				  ST7735_DrawImage(enemyList[i].x , enemyList[i].y , 8 , 8 , enemy_ship_flat1);
			 }else{
				  ST7735_DrawImage(enemyList[i].x , enemyList[i].y , 8 , 8 , enemy_ship_flat2);
			 }


		}

	}
}
uint8_t gameState = 0 ;
void gameOverScreen(void){
	uint8_t y_upper = 64;
	uint8_t y_lower = 63;
	for(int i = 0; i < 20 ; i++ ){
		ST7735_FillRectangleFast(0 , y_upper , 128 , 1 , 0x0000);
		HAL_Delay(100);
		ST7735_FillRectangleFast(0 , y_lower , 128 , 1 , 0x0000);
		HAL_Delay(100);

		y_upper--;
		y_lower++;


	}

	ST7735_WriteString(22 , 52 , "GAME OVER" , Font_11x18 , 0xb000 , 0x0000);

	HAL_Delay(5000);

	ST7735_FillScreenFast(0x0000);
	ST7735_WriteString(9 , 64 , "Press any button" , Font_7x10 , 0xFFFF , 0x0000);
	ST7735_WriteString(22 , 74 , "to continue" , Font_7x10 , 0xFFFF , 0x0000);

	gameState = 0;
}

uint8_t playerCollisionLogic(uint8_t delx , uint8_t dely){
	if(delx < 5 && dely >5){
		return 1;
	}else if(delx > 5 && delx < 7){
		return 1;
	}else if(delx > 7 && dely > 5){
		return 1;
	}

	return 0;
}
void calculatePlayerCollisions(){
	for(int i = 0 ; i < MAXENEMYBULLETS ; i++){
		if(evilBullets[i].isActive == 0) continue;


		uint8_t delx = evilBullets[i].x - playerObj.x;
		uint8_t dely = evilBullets[i].y - playerObj.y;

		if(delx < 11 && dely < 12){
			uint8_t isHit = playerCollisionLogic(delx , dely);
			if(isHit){
				gameOverScreen();
			}
		}
	}
}

void checkForVictory(){
	if(enemiesAlive == 0){
		    uint8_t y_upper = 64;
			uint8_t y_lower = 63;
			for(int i = 0; i < 20 ; i++ ){
				ST7735_FillRectangleFast(0 , y_upper , 128 , 1 , 0x0000);
				HAL_Delay(100);
				ST7735_FillRectangleFast(0 , y_lower , 128 , 1 , 0x0000);
				HAL_Delay(100);

				y_upper--;
				y_lower++;


			}

			ST7735_WriteString(22 , 52 , "YOU WIN" , Font_11x18 , 0xffc0 , 0x0000);

			HAL_Delay(5000);

			ST7735_FillScreenFast(0x0000);
			ST7735_WriteString(9 , 64 , "Press any button" , Font_7x10 , 0xFFFF , 0x0000);
			ST7735_WriteString(22 , 74 , "to continue" , Font_7x10 , 0xFFFF , 0x0000);

			gameState = 0;
	}
}

void killAll(){
	for(int i = 0 ; i  < 20 ; i++){
		killEnemy(&enemyList[i]);
	}
}

void checkForCheats(void){
	if((GPIOA->IDR & 0x001F) == 0x001F){
		killAll();
	}
}

 //0 is the screen , 1 means its in play , 2 means you died
uint8_t ticksForAttack = 30;
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
  ST7735_WriteString(56 , 64 , "EDW Project" , Font_16x26 , 0x4323 , 0x0000);
  HAL_Delay(3000);
  ST7735_FillScreenFast(0x0000);
  ST7735_WriteString(2,2, "Contributers:" , Font_7x10 , 0x3245 , 0x0000);
  ST7735_WriteString(2 , 22 ,"Inderpreet Singh" , Font_7x10 , 0xAE89 , 0x0000);
  ST7735_WriteString(10 , 32 ,"Design &" , Font_7x10 , 0xAE89 , 0x0000);
  ST7735_WriteString(10 , 42 ,"Programming" , Font_7x10 , 0xAE89 , 0x0000);
  ST7735_WriteString(2 , 52 ,"Yash Dharia" , Font_7x10 , 0xAE89 , 0x0000);
  ST7735_WriteString(10 , 62 ,"Enclosure Design" , Font_7x10 , 0xAE89 , 0x0000);
  ST7735_WriteString(2 , 72 ,"Anubhav" , Font_7x10 , 0xAE89 , 0x0000);
  ST7735_WriteString(10 , 82 ,"Documentation" , Font_7x10 , 0xAE89 , 0x0000);
  HAL_Delay(4000);
  ST7735_WriteString(10 , 32 ,"2023UEC2691" , Font_7x10 , 0xAE89 , 0x0000);
  ST7735_FillRectangleFast(0 , 62 , 128 , 10 , 0x0000);
  ST7735_WriteString(10 , 62 ,"2023UEC2696" , Font_7x10 , 0xAE89 , 0x0000);
  ST7735_FillRectangleFast(0 , 82 , 128 , 10 , 0x0000);
  ST7735_WriteString(10 , 82 ,"2023UEC2679" , Font_7x10 , 0xAE89 , 0x0000);
  ST7735_FillRectangleFast(0 , 42 , 128 , 10 , 0x0000);
  HAL_Delay(3000);
  ST7735_FillScreenFast(0x0000);
  ST7735_WriteString(9 , 64 , "Press any button" , Font_7x10 , 0xFFFF , 0x0000);
  ST7735_WriteString(22 , 74 , "to continue" , Font_7x10 , 0xFFFF , 0x0000);




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t timeSinceLastAttack = 1;
  uint8_t currentPatternIteration = 0;
  while (1)
  {
	  if(gameState == 0){
		  if(GPIOA->IDR & 0x001F){
			  gameState = 1;
		  }

		  continue;

	  }

	  if(gameState == 1){
		  init_Bullets();
		  init_enemyBullets();
		  ST7735_FillScreenFast(0x0000);
		  for(int i = 0 ; i <20 ; i++){
		  	  ST7735_DrawImage(enemyList[i].x , enemyList[i].y , 8 , 8 , enemy_ship_flat1 );
		    }

		    ST7735_DrawImage(playerObj.x , playerObj.y , 12 , 11 , player_ship_flat);
		    gameState = 3;
		    HAL_Delay(1000);
		    checkForCheats();
	  }

	  if(timeSinceLastAttack%ticksForAttack == 0){
		  createEnemyBullets(currentPatternIteration);
		  if(currentPatternIteration == 10){
			  currentPatternIteration = 0;
		  }
		  currentPatternIteration++;
		  timeSinceLastAttack = 1;
	  }else{
		  timeSinceLastAttack++;
	  }

	  checkForVictory();
	  updatePlayerSpeed();
	  updatePlayerPosition();
	  update_Bullets();
	  calculateCollisions();
	  calculatePlayerCollisions();
	  updateEnemyStates();
	  updateEnemyAnimations();
	  updateEnemyPositions();

	  incrementTicksSinceLastAnimationChange();

	  HAL_Delay(tickSpeed);
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

  /*Configure GPIO pins : DOWN_Pin RIGHT_Pin LEFT_Pin UP_Pin */
  GPIO_InitStruct.Pin = DOWN_Pin|RIGHT_Pin|LEFT_Pin|UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
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

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

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
