/*
显示6x8字体的十进制变量数据调用内部库函数ScI2cMxDisplay6x8Str
函数功能 ：在指定的地方开始显示十进制变量数字
第1个参数：为模块的地址
第2个参数：所要显示内容的起始页
第3个参数：所要显示内容的起始列
第4个参数：所要显示的变量（范围为0~65535,0~0xFF）
第5个参数：显示类型，
           DECNUM_5BIT总显示5位十进制数，高位没有的用0填补
           DECNUM_AUTO自动显示实际的十进制位数，高位为0的不显示

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
   ZT.ScI2cMxDisplay6x8Str(OLED_ADDRESS,0,20,"Dec Number Test!");
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay6x8DecNum(OLED_ADDRESS,2,0,65432,DECNUM_5BIT);
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay6x8DecNum(OLED_ADDRESS,2,44,5432,DECNUM_5BIT);
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay6x8DecNum(OLED_ADDRESS,2,88,432,DECNUM_5BIT);
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay6x8DecNum(OLED_ADDRESS,4,0,32,DECNUM_5BIT);
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay6x8DecNum(OLED_ADDRESS,4,44,2,DECNUM_5BIT);
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   
   ZT.ScI2cMxDisplay6x8DecNum(OLED_ADDRESS,4,88,65432,DECNUM_AUTO);
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay6x8DecNum(OLED_ADDRESS,6,0,5432,DECNUM_AUTO);
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay6x8DecNum(OLED_ADDRESS,6,36,432,DECNUM_AUTO);
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay6x8DecNum(OLED_ADDRESS,6,64,32,DECNUM_AUTO);
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
   ZT.ScI2cMxDisplay6x8DecNum(OLED_ADDRESS,6,84,2,DECNUM_AUTO);
   while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);  
}
void loop()
{
}
