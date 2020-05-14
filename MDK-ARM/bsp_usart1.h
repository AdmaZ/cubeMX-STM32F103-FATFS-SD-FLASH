#ifndef __BSP_USART1_H_
#define __BSP_USART1_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"
#include <stdio.h>
/* Private function prototypes -----------------------------------------------*/
void Usart_SendString(uint8_t *str);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);
extern UART_HandleTypeDef huart1;
#endif

/************************ (C) COPYRIGHT Adma ׯ *****END OF FILE****/
