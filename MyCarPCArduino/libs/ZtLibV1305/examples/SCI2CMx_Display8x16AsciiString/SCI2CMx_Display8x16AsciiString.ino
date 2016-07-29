/*
显示8x16 Ascii字符串调用内部库函数ScI2cMxDisplay8x16Str
函数功能 ：在指定的地方开始显示指定点阵数据图英文字符串
第1个参数：为模块的地址
第2个参数：所要显示英文字符串的起始页
第3个参数：所要显示英文字符串的起始列
第4个参数：所要显示的Ascii字符串

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
   ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,0,0,"Hello World!");
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,2,0,"ZT.SCI2CMx!");
}
void loop()
{
}
