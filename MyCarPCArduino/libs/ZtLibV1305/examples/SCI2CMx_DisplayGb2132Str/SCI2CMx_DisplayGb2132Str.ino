/*
汉字为16*16点阵
要显示汉字则调用16x16点阵显示函数ScM1DisplayDot16x16()
ZT.ScM1DisplayDot16x16(uint8_t, uint8_t, uint8_t, const char *);
函数功能 ：在指定的地方显示16x16点阵
第1个参数：为模块的地址
第2个参数：所要显示点阵的起始页
第3个参数：所要显示点阵的起始列
第4个参数：所要先点阵的点阵数据
** Kozig 将在一个版本中增加中文字库（在模块内），直接调用内部显示函数
*/
#include <ZtLib.h>
#include <Wire.h>
#define OLED_ADDRESS    0x27
const char HelloWorld[10]= {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0xCA, 0xC0, 0xBD, 0xE7, 0x21};
const char HelloChina[10]= {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0xD6, 0xD0, 0xB9, 0xFA, 0x21};
const char HelloKozig[10]= {0x4B, 0x6F, 0x7A, 0x69, 0x67, 0xC4, 0xE3, 0xBA, 0xC3, 0x21};

void setup()
{
  ZT.I2cInit();
  ZT.ScI2cMxReset(OLED_ADDRESS);
  while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
  ZT.ScI2cMxDisplayGb2132Str(OLED_ADDRESS,0,0,&HelloWorld[0],10);
  while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_UP,0,64,1,250);
  
  while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
  ZT.ScI2cMxDisplayGb2132Str(OLED_ADDRESS,2,0,HelloChina,10);
  while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_UP,16,64,1,200);
  
  while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
  ZT.ScI2cMxDisplayGb2132Str(OLED_ADDRESS,4,0,HelloKozig,10);
  while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_UP,32,64,1,150);
  while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
}
void loop()
{
}
