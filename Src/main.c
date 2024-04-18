/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "key.h"
#include "led.h"
#include "string.h"
#include "stdio.h"

#define PH_MAX 10000
#define PH_MIN 100

#define PD_MAX 1000
#define PD_MIN 100

#define PX_MAX 1000
#define PX_MIN -1000
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void DATA_ZHOUQI(void) ;
void DATA_PINLV(void) ;
void PARA(void) ;
void RECD(void) ;
void Clear(void)  ;
void Get_F(void) ;


u32 tim2_cnt1 = 0, tim2_cnt2 = 0;
u32 f40 = 0;
float d40 = 0;

u32 tim3_cnt1 = 0, tim3_cnt2 = 0;
u32 f39 = 0;
float d39 = 0;
u8 tim2_state = 0;	
u8 tim3_state = 0;	
u32 time,time1 ;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
	{
		if(tim2_state == 0)						
		{
			__HAL_TIM_SetCounter(&htim2,0);			
			TIM2->CCER |= 0x02;						
			tim2_state = 1;							
		}
		else if(tim2_state == 1)					
		{
			tim2_cnt1 = __HAL_TIM_GetCounter(&htim2);
			TIM2->CCER &= ~0x02;					
			tim2_state = 2;
		}
		else if(tim2_state == 2)			
		{
			tim2_cnt2 = __HAL_TIM_GetCounter(&htim2);
			f40 = 1000000/tim2_cnt2;				

			tim2_state = 0;
		}
		HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
	}
	
	if(htim == &htim3)
	{
		if(tim3_state == 0)			
		{
			__HAL_TIM_SetCounter(&htim3,0);			
			TIM3->CCER |= 0x02;					
			tim3_state = 1;							
		}
		else if(tim3_state == 1)				
		{
			tim3_cnt1 = __HAL_TIM_GetCounter(&htim3);
			TIM3->CCER &= ~0x02;			
			tim3_state = 2;
		}
		else if(tim3_state == 2)					
		{
			tim3_cnt2 = __HAL_TIM_GetCounter(&htim3);
			f39 = 1000000/tim3_cnt2;				

			tim3_state = 0;
		}
		HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
	}
}


s32 PX = 0;
u32 PD = 1000 ;
u32 PH = 5000 ;
s32 A = 0 ,B  = 0;
s32 A_z = 0,B_Z = 0 ;
u8 NDA = 0,NDB = 0 ,NHA = 0,NHB = 0 ;
u8 flag_A = 0 ,flag_B = 0;

s32 Old_A = 0,Old_B = 0 ;
s32 A_MAX = 0,A_MIN = 0, B_MAX = 0,B_MIN = 0;
__IO uint32_t Juge1Tick = 0 ;
u8 FLAG = 0;
void Juge_PINLV(void)
{
	if(uwTick - Juge1Tick <100) return ;
	Juge1Tick = uwTick ;
	
	A = f40 + PX ;
	B = f39 + PX ;
	if(FLAG == 0)
	{
		A_MAX = A_MIN = A ;
		B_MAX = B_MIN = B ;
		FLAG = 1 ;
	}
		
	Get_F() ;
	
	Old_A = A ;
	Old_B = B ;
	A_z = 1000000/f40 ;
	B_Z = 1000000/f39 ;
	
	
	if(A > PH & flag_A == 0)
	{
		NHA +=1 ;
		flag_A = 1 ;

	}
	else if(A <= PH)  flag_A = 0 ;
	if(B > PH & flag_B == 0)
	{
		NHB +=1 ;
		flag_B = 1 ;
	}
	else if(B <=PH)  flag_B = 0 ;
}

void Get_F(void)
{
	if(A > A_MAX)
	{
		A_MAX = A ;
	}
	else if(A < A_MIN)
	{
		A_MIN = A ;		
	}
		
	if(B > B_MAX)
	{
		B_MAX = B ;
	}
	else if(B <B_MIN)
	{
		B_MIN = B ;		
	}
}
__IO uint32_t JugeTick = 0;
u8 Flag_A = 0,Flag_B = 0;
void Juge_F(void) 
{

	Flag_A = 0 ;
	Flag_B = 0 ;
	if(uwTick - JugeTick <3000) return ;
	JugeTick = uwTick ;
	
	if((A_MAX - A_MIN) > PD & Flag_A == 0) 
	{
		NDA += 1 ;
		Flag_A = 1 ;
	}

	if((B_MAX - B_MIN) > PD & Flag_B == 0) 
	{
		NDB += 1 ;
		Flag_B = 1 ;
	}
		FLAG = 0 ;
}
__IO uint32_t ContTick = 0;
__IO uint32_t keyTick = 0;
u8 Key_Mode = 0;
u8 Key_Select = 0;
u8 Key_P = 0;
void Key_Process(void)
{
	if(uwTick - keyTick < 10) return ; 
	keyTick = uwTick; 
	
	Key_Read();
	
	if(Trg & 0x01)	//B1
	{
		if(Key_Mode == 1)
		{
			if(Key_P == 0)
			{
				if(PD <= 900)
				PD +=100 ;
			}
			else if(Key_P == 1)
			{
				if(PH <= 9900)
				PH +=100 ;
			}
			else if(Key_P == 2)
			{
				if(PX <= 900)
				PX+= 100 ;
			}
		}
	}
	if(Trg & 0x02)	//B2
	{
		if(Key_Mode == 1)
		{
			if(Key_P == 0)
			{
				if(PD >= 200)
				PD -=100 ;
			}
			else if(Key_P == 1)
			{
				if(PH >= 1100)
				PH -=100 ;
			}
			else if(Key_P == 2)
			{
				if(PX >= -900)
				PX-= 100 ;
			}
		}
	}
	if(Trg & 0x04)	//B3
	{
		if(Key_Mode == 0)
			Key_Select = !Key_Select ;
		if(Key_Mode == 1)
		{
			Key_P +=1 ;
			if(Key_P == 3)
				Key_P = 0;
		}
		if(Key_Mode == 2)
			ContTick = uwTick ;
	}
	
	if(Cont & 0x04)
	{
		if(Key_Mode == 2)
		{
			if(uwTick - ContTick <= 1000) return ;
			{
				 NDA = 0 ;
			   NDB = 0 ;
         NHA = 0 ;
				 NHB = 0 ;
				ContTick = uwTick ;
			}
	  }
	}
	if(Trg & 0x08)	//B4
	{
		Key_Mode+=1 ;
		if(Key_Mode == 3)
			Key_Mode = 0;
	}
	
	if(Key_Mode == 0)
	{
//		LED_Control(0x01) ;
		if(Key_Select == 0)
			DATA_PINLV() ;
		if(Key_Select == 1)
			DATA_ZHOUQI() ;
	}
	else if(Key_Mode == 1)
	{
//		LED_Control(0x00) ;
		PARA() ;
	}
	else if(Key_Mode == 2)
	{
//		LED_Control(0x00) ;
		RECD() ;
	}

}


__IO uint32_t lcdTick = 0;
u8 display_buf[20] ;
void DATA_ZHOUQI(void)
{
	if(uwTick - lcdTick < 100) return ; 
	lcdTick = uwTick; 

  LCD_DisplayStringLine(Line1, (unsigned char *)"        DATA        ");
	
	if(A_z<=1000 & A>0)
	{
	sprintf((char*)display_buf,"     A=%duS    ",A_z);
	LCD_DisplayStringLine(Line3,display_buf);
	}
	else if(A_z>1000 & A>0)
	{
	sprintf((char*)display_buf,"     A=%0.2fmS    ",A_z/1000.0f);
	LCD_DisplayStringLine(Line3,display_buf);		
	}
	else if(A < 0)
	{
  LCD_DisplayStringLine(Line3, (unsigned char *)"     A=NULL        ");
	}
	
	if(B_Z<=1000 & B>0)
	{
	sprintf((char*)display_buf,"     B=%duS    ",B_Z);
	LCD_DisplayStringLine(Line4,display_buf);
	}
	else if(B_Z>1000 & B>0)
	{
	sprintf((char*)display_buf,"     B=%0.2fmS    ",B_Z/1000.0f);
	LCD_DisplayStringLine(Line4,display_buf);	
	}
	else if(B < 0)
	{
  LCD_DisplayStringLine(Line4, (unsigned char *)"     B=NULL        ");
	}
  LCD_DisplayStringLine(Line5, (unsigned char *)"                ");	
  LCD_DisplayStringLine(Line6, (unsigned char *)"                ");	
}

void DATA_PINLV(void)
{
	if(uwTick - lcdTick < 100) return ; 
	lcdTick = uwTick; 

  LCD_DisplayStringLine(Line1, (unsigned char *)"        DATA        ");
	
	if(A<=1000 & A>0)
	{
	sprintf((char*)display_buf,"     A=%dHz   ",A);
	LCD_DisplayStringLine(Line3,display_buf);
	}
	else if(A>1000)
	{
	sprintf((char*)display_buf,"     A=%0.2fKHz   ",A/1000.0f);
	LCD_DisplayStringLine(Line3,display_buf);		
	}
	else if(A < 0)
	{
  LCD_DisplayStringLine(Line3, (unsigned char *)"     A=NULL        ");
	}
	
	if(B<=1000 & B>0)
	{
	sprintf((char*)display_buf,"     B=%dHz   ",B);
	LCD_DisplayStringLine(Line4,display_buf);
	}
	else if(B>1000)
	{
	sprintf((char*)display_buf,"     B=%0.2fKHz   ",B/1000.0f);
	LCD_DisplayStringLine(Line4,display_buf);	
	}
	else if(B < 0)
	{
  LCD_DisplayStringLine(Line4, (unsigned char *)"     B=NULL        ");
	}
	
  LCD_DisplayStringLine(Line5, (unsigned char *)"                ");	
  LCD_DisplayStringLine(Line6, (unsigned char *)"                ");	
}

void PARA(void)
{
	if(uwTick - lcdTick < 100) return ; 
	lcdTick = uwTick; 

  LCD_DisplayStringLine(Line1, (unsigned char *)"        PARA        ");
	
	sprintf((char*)display_buf,"     PD=%dHz   ",PD);
	LCD_DisplayStringLine(Line3,display_buf);

	sprintf((char*)display_buf,"     PH=%dHz   ",PH);
	LCD_DisplayStringLine(Line4,display_buf);		

	sprintf((char*)display_buf,"     PX=%dHz   ",PX);
	LCD_DisplayStringLine(Line5,display_buf);		
	
  LCD_DisplayStringLine(Line6, (unsigned char *)"                ");	

}

void RECD(void) 
{
	if(uwTick - lcdTick < 100) return ; 
	lcdTick = uwTick; 

  LCD_DisplayStringLine(Line1, (unsigned char *)"        RECD        ");
	
	sprintf((char*)display_buf,"     NDA=%d     ",NDA);
	LCD_DisplayStringLine(Line3,display_buf);

	sprintf((char*)display_buf,"     NDB=%d     ",NDB);
	LCD_DisplayStringLine(Line4,display_buf);		

	sprintf((char*)display_buf,"     NHA=%d     ",NHA);
	LCD_DisplayStringLine(Line5,display_buf);		
	
	sprintf((char*)display_buf,"     NHB=%d     ",NHB);
	LCD_DisplayStringLine(Line6,display_buf);		
}

void Clear(void) 
{
  LCD_DisplayStringLine(Line3, (unsigned char *)"                ");	
  LCD_DisplayStringLine(Line4, (unsigned char *)"                ");	
  LCD_DisplayStringLine(Line5, (unsigned char *)"                ");	
  LCD_DisplayStringLine(Line6, (unsigned char *)"                ");	
}

void LED(void)
{
	if(Key_Mode == 0)
	{
		if(NDA >=3 | NDB>= 3)
		{
			if(A > PH & B > PH)
				LED_Control(0x87) ;
			else if(A > PH & B < PH)
				LED_Control(0x83) ;				
			else if(A < PH & B > PH)
				LED_Control(0x85) ;			
			else if(A < PH & B < PH)
				LED_Control(0x81) ;					
	  }
		else if(NDA <3 & NDB < 3)
		{
			if(A > PH & B > PH)
				LED_Control(0x07) ;
			else if(A > PH & B < PH)
				LED_Control(0x03) ;				
			else if(A < PH & B > PH)
				LED_Control(0x05) ;			
			else if(A < PH & B < PH)
				LED_Control(0x01) ;						
		}
  }
	else
	{
			if(NDA >=3 | NDB>= 3)
		{
			if(A > PH & B > PH)
				LED_Control(0x86) ;
			else if(A > PH & B < PH)
				LED_Control(0x82) ;				
			else if(A < PH & B > PH)
				LED_Control(0x84) ;			
			else if(A < PH & B < PH)
				LED_Control(0x80) ;					
	  }
		else if(NDA <3 & NDB < 3)
		{
			if(A > PH & B > PH)
				LED_Control(0x06) ;
			else if(A > PH & B < PH)
				LED_Control(0x02) ;				
			else if(A < PH & B > PH)
				LED_Control(0x04) ;			
			else if(A < PH & B < PH)
				LED_Control(0x00) ;						
		}	
	}
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
    LCD_Init();
  HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);

  HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
	

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
	  LCD_Clear(Black);

	LED_Control(0x00) ;
	FLAG = 0 ;


    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			LED() ;
			
			Key_Process() ;
			Juge_PINLV() ;
			Juge_F() ;

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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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

