/* ----------------- includes ---------------------------------------*/

#include "fatfs_user_fun.h"

/* ----------------- Private variables ---------------------------------------*/

char buff[6];

/* ----------------- function ---------------------------------------*/

FRESULT FlashfMount()
{
    FRESULT retUSER;
    retUSER = f_mount(&USERFatFS, "1:/", 1);//默认将flash分配到磁盘 1:/
    if (retUSER == FR_NO_FILESYSTEM)
    {
        printf("》SPI FLASH 还没有文件系统，即将进行格式化...\n\n");
        /* 格式化 */
        retUSER = f_mkfs("1:/", 0, 0);
    }
    else if (retUSER != FR_OK)
    {
        printf("！！SPI FLASH 挂载文件系统失败。(%d)\n\n", retUSER);
        printf("！！可能原因：SPI FLASH 初始化不成功。\n\n");
        while (1);
    }
    else
    {
        printf("》SPI FLASH 文件系统挂载成功，可以进行读写测试\n\n");
    }
    return retUSER;
}
FRESULT SDfMount()
{
    FRESULT retSD;
    retSD = f_mount(&SDFatFS, "0:/", 1); //默认将SD卡分配到磁盘 0:/
    if (retSD == FR_NO_FILESYSTEM)
    {
        printf("》SD卡 还没有文件系统，即将进行格式化...\n\n");
        /* 格式化 */
        retSD = f_mkfs("0:/", 0, 0);
    }
    else if (retSD != FR_OK)
    {
        printf("！！SD 卡挂载文件系统失败。(%d)\n\n", retSD);
        printf("！！可能原因：SD卡 初始化不成功。\n\n");
        while (1);
    }
    else
    {
        printf("》SD卡 文件系统挂载成功，可以进行读写测试\n\n");
    }
    return retSD;
}

FRESULT FlashDeviceInfoGet()
{
    FRESULT retUSER;
    printf("******** 设备信息获取 ********\n\n");
    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;

    /*获取驱动器1的卷信息和可用群集*/
    retUSER = f_getfree("1:/", &fre_clust, &fs);//默认驱动器为1

    /*获取总扇区数和空闲扇区数*/
    tot_sect = (fs-> n_fatent - 2) * fs-> csize;
    fre_sect = fre_clust * fs-> csize;

    /* 打印可用空间（假设每扇区4096字节） */
    printf("%10lu Kb 总驱动器空间.\n%10lu KB 可用.\n\n", tot_sect * 4, fre_sect * 4);

    return retUSER;
}

FRESULT SDDeviceInfoGet()
{
    FRESULT retSD;

    printf("******** 设备信息获取 ********\n\n");
    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;


    /*获取驱动器0的卷信息和可用群集*/
    retSD = f_getfree("0:/", &fre_clust, &fs);//默认驱动器为0

    /*获取总扇区数和空闲扇区数*/
    tot_sect = (fs-> n_fatent - 2) * fs-> csize;
    fre_sect = fre_clust * fs-> csize;

    /* 打印可用空间（假设每扇区512字节） */
    printf("%10lu MB 总驱动器空间.\n%10lu MB 可用.\n\n", tot_sect / 2 / 1024, fre_sect / 2 / 1024);

    return retSD;
}


FRESULT scan_files(char *path)
{
    FRESULT res;      //部分在递归过程被修改的变量，不用全局变量
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;        // 文件名

#if _USE_LFN
    /* 长文件名支持 */
    /* 简体中文需要2个字节保存一个“字”*/
    static char lfn[_MAX_LFN * 2 + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif
    res = f_opendir(&dir, path); //打开目录
    if (res == FR_OK)
    {
        i = strlen(path);
        for (;;)
        {
            //读取目录下的内容，再读会自动读下一个文件
            res = f_readdir(&dir, &fno);
            //为空时表示所有项目读取完毕，跳出
            if (res != FR_OK || fno.fname[0] == 0) break;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            //点表示当前目录，跳过
            if (*fn == '.') continue;
            //目录，递归读取
            if (fno.fattrib & AM_DIR)
            {
                //合成完整目录名
                sprintf(&path[i], "/%s", fn);
                //递归遍历
                res = scan_files(path);
                path[i] = 0;
                //打开失败，跳出循环
                if (res != FR_OK)
                    break;
            }
            else
            {
                printf("%s/%s\r\n", path, fn);//输出文件名
                /* 可以在这里提取特定格式的文件路径 */
            }
        }
    }
    res = f_closedir(&dir);//关闭目录
    return res;
}

void SDTraverFiles()
{
    printf("******** 遍历 SD卡 中所有文件 ********\n\n");
    strcpy(buff, "0:/");
    retSD = scan_files(buff);
    printf("\n");
}

void FlashTraverFiles()
{
    printf("******** 遍历 SPI FLASH	中所有文件 ********\n\n");
    strcpy(buff, "1:/");
    retUSER = scan_files(buff);
    printf("\n");
}

void FlashWriteFile(char *string, int num, char WriteBuffer[num] )
{
    UINT fnumfalsh;
    /* 打开文件，如果文件不存在则创建它 */
    printf("******** 即将进行文件写入测试... ********\n\n");
    retUSER = f_open(&USERFile, string,
                     FA_CREATE_ALWAYS | FA_WRITE);

    if (retUSER == FR_OK)
    {
        printf("》打开/创建 文件成功，向文件写入数据。\r\n");
        /* 将指定存储区内容写入到文件内 */
        retUSER = f_write(&USERFile, (void *)WriteBuffer, strlen((void *)WriteBuffer), &fnumfalsh);
        if (retUSER == FR_OK)
        {
            printf("》文件写入成功，写入字节数据：%d\n", fnumfalsh);
            printf("》向文件写入的数据为：\n\n%s \n\r\n", WriteBuffer);
        }
        else
        {
            printf("！！文件写入失败：(%d)\n", retUSER);
        }

        /* 不再读写，关闭文件 */
        f_close(&USERFile);
    }
    else
    {
        LED_RED;
        printf("！！打开/创建文件失败。\n\n");
    }
}

void FlashReadFile(char *string)
{
    char ReadBuffer[255];
    UINT fnum;
    printf("******** 即将进行文件读取测试... ********\n\n");
    retUSER = f_open(&USERFile, string, FA_OPEN_EXISTING | FA_READ);
    if (retUSER == FR_OK)
    {
        LED_GREEN;
        printf("》打开文件成功。\r\n");
        retUSER = f_read(&USERFile, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (retUSER == FR_OK)
        {
            printf("》文件读取成功,读到字节数据：%d\r\n", fnum);
            printf("》读取得的文件数据为：\n\n%s \n\r\n", ReadBuffer);
        }
        else
        {
            printf("！！文件读取失败：(%d)\n", retUSER);
        }
    }
    else
    {
        LED_RED;
        printf("！！打开文件失败。\r\n");
    }
    /* 不再读写，关闭文件 */
    f_close(&USERFile);
}
void SDWriteFile(char *string, int num, char WriteBuffer[num] )
{

    UINT fnum;
    /* 打开文件，如果文件不存在则创建它 */
    printf("******** 即将进行文件写入测试... ********\n\n");
    retSD = f_open(&SDFile, string,
                     FA_CREATE_ALWAYS | FA_WRITE);

    if (retSD == FR_OK)
    {
        printf("》打开/创建 文件成功，向文件写入数据。\r\n");
        /* 将指定存储区内容写入到文件内 */
        retSD = f_write(&SDFile, (void *)WriteBuffer, strlen((void *)WriteBuffer), &fnum);
				if (retSD == FR_OK)
        {
            printf("》文件写入成功，写入字节数据：%d\n", fnum);
            printf("》向文件写入的数据为：\n\n%s \n\r\n", WriteBuffer);
        }
        else
        {
            printf("！！文件写入失败：(%d)\n", retSD);
        }

        /* 不再读写，关闭文件 */
        f_close(&SDFile);
    }
    else
    {
        LED_RED;
        printf("！！打开/创建文件失败。\n\n");
    }
}
void SDReadFile(char *string)
{
    char ReadBuffer[255];
    UINT fnum;
		uint8_t res;
    printf("******** 即将进行文件读取测试... ********\n\n");
    res = f_open(&SDFile, string, FA_OPEN_EXISTING | FA_READ);
    if (res == FR_OK)
    {
        LED_GREEN;
        printf("》打开文件成功。\r\n");
        res = f_read(&SDFile, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (res == FR_OK)
        {
            printf("》文件读取成功,读到字节数据：%d\r\n", fnum);
            printf("》读取得的文件数据为：\n\n%s \n\r\n", ReadBuffer);
        }
        else
        {
            printf("！！文件读取失败：(%d)\n", res);
        }
    }
    else
    {
        LED_RED;
        printf("！！打开文件失败。\r\n");
    }
    /* 不再读写，关闭文件 */
    f_close(&SDFile);
}
/************************ (C) COPYRIGHT Adma Z *****END OF FILE****/

#if 0
    /*----------------------- FLASH 文件系统 -----------------*/
    //初始化函数调用流程如下
    //f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()
    //在SD卡挂载文件系统，文件系统挂载时会对SD卡初始化
    retSD = f_mount(&SDFatFS, "0:/", 1);
    //在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
    retUSER = f_mount(&USERFatFS, "1:/", 1);

    /*----------------------- 格式化测试 -----------------*/
    printf("******** SPI FLASH 文件系统 开始挂载 ********\n\n");
    /* 如果没有文件系统就格式化创建创建文件系统 */
    if (retUSER == FR_NO_FILESYSTEM)
    {
        printf("》FLASH还没有文件系统，即将进行格式化...\n\n");
        /* 格式化 */
        retUSER = f_mkfs("1:/", 0, 0);

        if (retUSER == FR_OK)
        {
            printf("》FLASH已成功格式化文件系统。\n\n");
            /* 格式化后，先取消挂载 */
            retUSER = f_mount(NULL, "1:/", 1);
            /* 重新挂载   */
            retUSER = f_mount(&USERFatFS, "1:/", 1);
        }
        else
        {
            LED_RED;
            printf("《《格式化失败。》》\n\n");
            while (1);
        }
    }
    else if (retUSER != FR_OK)
    {
        printf("！！外部Flash挂载文件系统失败。(%d)\n\n", retUSER);
        printf("！！可能原因：SPI Flash初始化不成功。\n\n");
        while (1);
    }
    else
    {
        printf("》文件系统挂载成功，可以进行读写测试\r\n");
    }


    /*----------------------- 存储空间获取 ---------------------------*/
    printf("******** 设备信息获取 ********\r\n");
    FATFS *fsflash;
    DWORD fre_clustflash, fre_sectflash, tot_sectflash;

    /*获取驱动器1的卷信息和可用群集*/
    retUSER = f_getfree("1:/", &fre_clustflash, &fsflash);

    /*获取总扇区数和空闲扇区数*/
    tot_sectflash = (fsflash-> n_fatent - 2) * fsflash-> csize;
    fre_sectflash = fre_clustflash * fsflash-> csize;

    /* 打印可用空间（假设每扇区4096字节） */
    printf("%10lu Kb 总驱动器空间.\n%10lu Kb 可用.\n\n", tot_sectflash * 4, fre_sectflash * 4);



    /*----------------------- 文件系统测试：写测试 -----------------------------*/
    /* 打开文件，如果文件不存在则创建它 */
    printf("******** 即将进行文件写入测试... ********\n\n");
    retUSER = f_open(&USERFile, "1:/FLASHFatFs读写测试文件.txt",
                     FA_CREATE_ALWAYS | FA_WRITE);

    if (retUSER == FR_OK)
    {
        printf("》打开/创建FLASHFatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
        /* 将指定存储区内容写入到文件内 */
        retUSER = f_write(&USERFile, WriteBuffer, sizeof(WriteBuffer), &fnumfalsh);

        if (retUSER == FR_OK)
        {
            printf("》文件写入成功，写入字节数据：%d\n", fnumfalsh);
            printf("》向文件写入的数据为：\r\n%s\r\n", WriteBuffer);
        }
        else
        {
            printf("！！文件写入失败：(%d)\n", retUSER);
        }

        /* 不再读写，关闭文件 */
        f_close(&USERFile);
    }
    else
    {
        LED_RED;
        printf("！！打开/创建文件失败。\n\n");
    }
    /*------------------- 文件系统测试：读测试 --------------------------*/
    printf("******** 即将进行文件读取测试... ********\n\n");
    retUSER = f_open(&USERFile, "1:/FLASHFatFs读写测试文件.txt", FA_OPEN_EXISTING | FA_READ);
    if (retUSER == FR_OK)
    {
        LED_GREEN;
        printf("》打开文件成功。\r\n");
        retUSER = f_read(&USERFile, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (retUSER == FR_OK)
        {
            printf("》文件读取成功,读到字节数据：%d\r\n", fnum);
            printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);
        }
        else
        {
            printf("！！文件读取失败：(%d)\n", retUSER);
        }
    }
    else
    {
        LED_RED;
        printf("！！打开文件失败。\r\n");
    }
    /* 不再读写，关闭文件 */
    f_close(&USERFile);

    /* -------------------扫描Flash里的文件-------------------- */

    printf("******** 遍历FLASH卡中所有文件 ********\n\n");
    printf("buff:%s\n", buff);
    if (retUSER == FR_OK)
    {
        strcpy(buff, "1:/");
        retUSER = scan_files(buff);
    }

    f_mount(NULL, "1:/", 1);

    printf("\r\n****** 这是一个SD卡 文件系统实验 ******\n\n");

    printf("》SD卡(%d)\r\n", retSD);
    printf("》SDPath为：%s\r\n", SDPath);

    /*----------------------- 格式化测试 ---------------------------*/
    /* 如果没有文件系统就格式化创建创建文件系统 */
    if (retSD == FR_NO_FILESYSTEM)
    {
        printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
        /* 格式化 */
        retSD = f_mkfs((TCHAR const *)SDPath, 0, 0);

        if (retSD == FR_OK)
        {
            printf("》SD卡已成功格式化文件系统。\r\n");
            /* 格式化后，先取消挂载 */
            retSD = f_mount(NULL, (TCHAR const *)SDPath, 1);
            /* 重新挂载 */
            retSD = f_mount(&SDFatFS, (TCHAR const *)SDPath, 1);
        }
        else
        {
            printf("《《格式化失败。》》\r\n");

            while (1);
        }
    }
    else if (retSD != FR_OK)
    {
        printf("！！SD卡挂载文件系统失败。(%d)\r\n", retSD);
        printf("(%d)\r\n", retSD);

        while (1);
    }
    else
    {
        printf("》文件系统挂载成功，可以进行读写测试\n\n");
    }

    /*----------------------- 存储空间获取 ---------------------------*/
    printf("******** 设备信息获取 ********\n\n");
    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;


    /*获取驱动器0的卷信息和可用群集*/
    retSD = f_getfree("0:/", &fre_clust, &fs);

    /*获取总扇区数和空闲扇区数*/
    tot_sect = (fs-> n_fatent - 2) * fs-> csize;
    fre_sect = fre_clust * fs-> csize;

    /* 打印可用空间（假设每扇区512字节） */
    printf("%10lu MB 总驱动器空间.\n%10lu MB 可用.\n\n", tot_sect / 2 / 1024, fre_sect / 2 / 1024);



    /*----------------------- 文件系统测试：写测试 -----------------------------*/
    /* 打开文件，如果文件不存在则创建它 */
    printf("******** 即将进行文件写入测试... ********\n\n");
    retSD = f_open(&SDFile, "0:/SD卡FatFs读写测试文件.txt",
                   FA_CREATE_ALWAYS | FA_WRITE);

    if (retSD == FR_OK)
    {
        printf("》打开/创建SD卡FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
        /* 将指定存储区内容写入到文件内 */
        retSD = f_write(&SDFile, WriteBuffer, sizeof(WriteBuffer), &fnum);

        if (retSD == FR_OK)
        {
            printf("》文件写入成功，写入字节数据：%d\r\n", fnum);
            printf("》向文件写入的数据为：\r\n%s\r\n", WriteBuffer);
        }
        else
        {
            printf("！！文件写入失败：(%d)\r\n", retSD);
        }

        /* 不再读写，关闭文件 */
        f_close(&SDFile);
    }
    else
    {
        printf("！！打开/创建文件失败。\r\n");
    }

    /*------------------- 文件系统测试：读测试 ------------------------------------*/
    printf("******** 即将进行文件读取测试... ********\n\n");
    retSD = f_open(&SDFile, "0:/SD卡FatFs读写测试文件.txt",
                   FA_OPEN_EXISTING | FA_READ);

    if (retSD == FR_OK)
    {
        printf("》打开文件成功。\r\n");
        retSD = f_read(&SDFile, ReadBuffer, sizeof(ReadBuffer), &fnum);

        if (retSD == FR_OK)
        {
            printf("》文件读取成功,读到字节数据：%d\r\n", fnum);
            printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);
        }
        else
        {
            printf("！！文件读取失败：(%d)\r\n", retSD);
        }
    }
    else
    {
        printf("！！打开文件失败。\r\n");
    }

    /* 不再读写，关闭文件 */
    f_close(&SDFile);

    /* -------------------扫描SD里的文件-------------------- */

    printf("\n******** 遍历SD卡中所有文件 ********\r\n");

    if (retSD == FR_OK)
    {
        strcpy(buff, "0:/");
        retSD = scan_files(buff);
    }


    printf("\n******** SD卡测试完成 ********\r\n\n");
    /* --------------注销一个SD卡工作区---------------------*/
    f_mount(NULL, (TCHAR const *)SDPath, 1);


    /* --------------------------操作完成，停机---------------------------- */
#endif
