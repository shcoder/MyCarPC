/*
垂直翻滚，向上翻或向下翻
ZT.ScI2cM1ScrollingVertical(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
函数功能 ：指定区域显示的内容向上翻或向下翻
第1个参数：为模块的地址
第2个参数：指定向上或者向下翻滚  SCROLL_UP SCROLL_DOWN
第3个参数：最上方翻滚起始行
第4个参数：最下方翻滚结束行
第3个参数：翻滚步数大小
第4个参数：翻滚快慢延时

*/
#include <ZtLib.h>
#include <Wire.h>
#define OLED_ADDRESS    0x27
void setup()
{
   ZT.I2cInit();
   ZT.ScI2cMxReset(OLED_ADDRESS);
   delay(5);//复位，重新初始化，清理缓存，需要稍稍等待
   
   ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,0,0,"Hello World!");
   delay(1);
   ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,2,0,"ZT.SC-I2CMx!");
   delay(1);
   ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,4,0,"Hi Guy @__@");
   delay(1);
   ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,6,0,"My OLED! *^_^*");
}
void loop()
{
  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_UP,0,32,1,250);
  delay(1000);
  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_DOWN,32,64,1,50);
  delay(1000);
  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_UP,0,64,1,50);
  delay(1000);
  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_DOWN,0,64,1,250);
  delay(1000);
  
  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_UP,0,32,5,250);
  delay(1000);
  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_DOWN,32,64,5,50);
  delay(1000);
  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_UP,0,64,5,50);
  delay(1000);
  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_DOWN,0,64,5,250);
  delay(1000);
}
