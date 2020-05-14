#ifndef __BSP_LED_H_
#define __BSP_LED_H_
/*
 *基本混色，后面高级用法使用 PWM 可混出全彩颜色,且效果更好
 */
#define			LED_RED		\
							digitalLo(GPIOB,LED_R_Pin)\
							digitalHi(GPIOB,LED_G_Pin)\
							digitalHi(GPIOB,LED_B_Pin)
#define			LED_GREEN		\
							digitalHi(GPIOB,LED_R_Pin)\
							digitalLo(GPIOB,LED_G_Pin)\
							digitalHi(GPIOB,LED_B_Pin)
#define			LED_BLUE		\
							digitalHi(GPIOB,LED_R_Pin)\
							digitalHi(GPIOB,LED_G_Pin)\
							digitalLo(GPIOB,LED_B_Pin)

/** 控制 LED 灯亮灭的宏，
* LED 低电平亮，设置 ON=0，OFF=1
* 若 LED 高电平亮，把宏设置成 ON=1 ，OFF=0 即可
*/
#define digitalHi(p,i) {p->BSRR=i;} //设置为高电平
#define digitalLo(p,i) {p->BSRR=(uint32_t)i << 16;}

#endif

/************************ (C) COPYRIGHT Adma 庄 *****END OF FILE****/
