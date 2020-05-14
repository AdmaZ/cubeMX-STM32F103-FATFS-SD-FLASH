#include "bsp_led.h"
#include "main.h"

void Light_Flash()
{   
		digitalLo(GPIOB,LED_R_Pin);
    digitalHi(GPIOB,LED_G_Pin);
    digitalLo(GPIOB,LED_B_Pin);
    HAL_Delay(2000);
    digitalLo(GPIOB,LED_G_Pin);
    digitalHi(GPIOB,LED_R_Pin);
    HAL_Delay(2000);
}
/************************ (C) COPYRIGHT Adma ׯ *****END OF FILE****/
