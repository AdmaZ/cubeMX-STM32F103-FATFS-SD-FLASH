#ifndef __FATFS_USER_FUN_
#define __FATFS_USER_FUN_

/* ----------------- includes ---------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "sdio.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

#include "string.h"
#include "../MDK-ARM/bsp_led.h"
#include "../MDK-ARM/bsp_usart1.h"
#include "../MDK-ARM/bsp_SPI_Flash.h"

/* ----------------- Prototypes ---------------------------------------*/

FRESULT FlashfMount(void);//flash挂载文件系统
FRESULT SDfMount(void);//SD 卡 挂载文件系统
FRESULT FlashDeviceInfoGet(void);//flash 信息获取
FRESULT SDDeviceInfoGet(void);//SD卡 信息获取
FRESULT scan_files(char *path);//遍历文件
void FlashTraverFiles(void);//遍历flash文件
void SDTraverFiles(void);//遍历SD卡文件
void FlashWriteFile(char *string, int num, char WriteBuffer[num]);//向 Flash 中写文件
void SDWriteFile(char *string, int num, char WriteBuffer[num]);//向 SD 卡中写文件
void FlashReadFile(char *string);
void SDReadFile(char *string);

#endif
