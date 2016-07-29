/*
/*
垂直翻滚，向上翻或向下翻
ZT.ScI2cM1ScrollingHorizontal(uint8_t, uint8_t, uint8_t, uint8_t,uint8_t);
函数功能 ：指定区域显示的内容向左还是向右翻滚
第1个参数：为模块的地址
第2个参数：指定向左还是向右翻滚 SCROLL_LEFT SCROLL_RIGHT
第3个参数：要翻滚的起始页
第4个参数：要翻滚的结束页
第3个参数：翻滚帧
#define FRAMS_2         0x07
#define FRAMS_3         0x04
#define FRAMS_4         0x05
#define FRAMS_5         0x00
#define FRAMS_25        0x06
#define FRAMS_64        0x01
#define FRAMS_128       0x02
#define FRAMS_256       0x03
*/
#include <ZtLib.h>
#include <Wire.h>
#define OLED_ADDRESS    0x27
void setup()
{
   ZT.I2cInit();
   ZT.ScI2cMxReset(OLED_ADDRESS);
   delay(5);//复位，重新初始化，清理缓存，需要稍稍等待
   
   ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,0,0,"Hello World!");//显示Logo，满屏，共8页，每页128列
   delay(1);
   ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,2,0,"ZT.SCI2CMx!");//显示Logo，满屏，共8页，每页128列
   delay(1);
   ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,4,0,"Hi Guy @__@");//显示Logo，满屏，共8页，每页128列
   delay(1);
   ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,6,0,"My OLED! *^_^*");//显示Logo，满屏，共8页，每页128列
   delay(1000);
}
void loop()
{
   
  ZT.ScI2cMxDeactivateScroll(OLED_ADDRESS);
  ZT.ScI2cMxScrollingHorizontal(OLED_ADDRESS,SCROLL_LEFT,0,1,FRAMS_2);
  delay(1000);
  
  ZT.ScI2cMxDeactivateScroll(OLED_ADDRESS);
  ZT.ScI2cMxScrollingHorizontal(OLED_ADDRESS,SCROLL_RIGHT,2,3,FRAMS_3);
  delay(1000);

  ZT.ScI2cMxDeactivateScroll(OLED_ADDRESS);
  ZT.ScI2cMxScrollingHorizontal(OLED_ADDRESS,SCROLL_LEFT,4,5,FRAMS_4);
  delay(1000);
  
  ZT.ScI2cMxDeactivateScroll(OLED_ADDRESS);
  ZT.ScI2cMxScrollingHorizontal(OLED_ADDRESS,SCROLL_RIGHT,6,7,FRAMS_5);
  delay(1000);

  ZT.ScI2cMxDeactivateScroll(OLED_ADDRESS);
  ZT.ScI2cMxScrollingHorizontal(OLED_ADDRESS,SCROLL_LEFT,0,7,FRAMS_2);
  delay(1000);
  
  ZT.ScI2cMxDeactivateScroll(OLED_ADDRESS);
  ZT.ScI2cMxScrollingHorizontal(OLED_ADDRESS,SCROLL_RIGHT,0,7,FRAMS_4);
  delay(1000);
}
