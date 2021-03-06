/*
显示6x8 Ascii字符串调用内部库函数ScI2cMxDisplay6x8Str
函数功能 ：在指定的地方6x8 Ascii字符串
第1个参数：为模块的地址
第2个参数：所要显示内容的起始页
第3个参数：所要显示内容的起始列
第4个参数：所要显示的ascii 字符串

** 本功能函数尽在固件版本为ZT.SC-I2CMx-V1.2或以上版本可以实现
** 目前已经在固件版本为ZT.SC-I2CMx-V2.0中增加了中文字库
*/
#include <ZtLib.h>
#include <Wire.h>
#define OLED_ADDRESS    0x27
void setup()
{
   ZT.I2cInit();
   ZT.ScI2cMxReset(OLED_ADDRESS);
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay6x8Str(OLED_ADDRESS,0,0,"Hello World! ^_^");
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay6x8Str(OLED_ADDRESS,1,0,"ZT.SC-I2CMx V1.2");
  
}
void loop()
{
}
