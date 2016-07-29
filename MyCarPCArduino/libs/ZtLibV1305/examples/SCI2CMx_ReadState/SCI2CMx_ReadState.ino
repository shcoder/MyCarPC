/*
读取模块的当前状态，并从UART PRINTF出来
ZT.ScI2cMxReadState(uint8_t);
函数功能 ：读取ZT.SC-I2CMx模块的当前状态
第1个参数：为模块的地址
返回值：   当前的状态
*/
#include <ZtLib.h>
#include <Wire.h>
#define OLED_ADDRESS    0x27

void UartPrintState(uint8_t state)
{
  if (state==STATUS_RUN)
  {
      Serial.print("ZT.SC-I2CMx->Normal State:\n");
  }
  else if (state==STATUS_BUSY)
  {
      Serial.print("ZT.SC-I2CMx->Busy State:\n");
  }
  else if (state==STATUS_SET_ADDRESS)
  {
      Serial.print("ZT.SC-I2CMx->Set Address Mode State:\n");
  }
  else
  {
      Serial.print("ZT.SC-I2CMx->erro:\n\n");
  }

}
void setup()
{
  uint8_t temp;
  Serial.begin(9600);
  ZT.I2cInit();
  ZT.ScI2cMxReset(OLED_ADDRESS);
  delay(5);//复位，重新初始化，清理缓存，需要稍稍等待
  Serial.print("OLED Reset\n\n");
  temp=ZT.ScI2cMxReadState(OLED_ADDRESS);
  UartPrintState(temp);
   
  Serial.print("Just Use ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,0,0,-Read State-)\n");
  ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,0,0,"Read State");  
  temp=ZT.ScI2cMxReadState(OLED_ADDRESS);
  UartPrintState(temp);
  
  delay(5);
  temp=ZT.ScI2cMxReadState(OLED_ADDRESS);
  Serial.print("After delay(5);\n");
  UartPrintState(temp);
  
  Serial.print("\nJust Use ScI2cMxScrollingHorizontal(OLED_ADDRESS,SCROLL_LEFT,0,1,FRAMS_2)\n");
  ZT.ScI2cMxScrollingHorizontal(OLED_ADDRESS,SCROLL_LEFT,0,1,FRAMS_2);
  temp=ZT.ScI2cMxReadState(OLED_ADDRESS);
  UartPrintState(temp);
  
  Serial.print("\nJust Use  ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_DOWN,0,64,1,250)\n");
   ZT.ScI2cMxScrollingVertical(OLED_ADDRESS,SCROLL_DOWN,0,64,1,250);
  temp=ZT.ScI2cMxReadState(OLED_ADDRESS);
  UartPrintState(temp);
  
  
  delay(1000);
  temp=ZT.ScI2cMxReadState(OLED_ADDRESS);
  Serial.print("After delay(1000);\n");
  UartPrintState(temp);
  delay(2000);
  temp=ZT.ScI2cMxReadState(OLED_ADDRESS);
  Serial.print("After delay(2000);\n");
  UartPrintState(temp);
 
}
void loop()
{
}
