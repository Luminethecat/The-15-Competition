
#include "key.h"

#define KB1  HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define KB2  HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define KB3  HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define KB4  HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)
#define KEYPORT  KB1 | (KB2<<1) | (KB3<<2) | (KB4<<3) | 0xf0

u8 Trg;	 	 
u8 Cont; 	 
void Key_Read(void)
{
    u8 ReadData = (KEYPORT)^0xff;   		   // 1
    Trg = ReadData & (ReadData ^ Cont);        // 2 
    Cont = ReadData;                           
}

