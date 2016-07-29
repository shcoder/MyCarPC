/*
读取模块的版本号，并将版本号显示在OLED上和从UART PRINTF出来
ZT.ScI2cMxReadVersion(uint8_t, const char *);
函数功能 ：读取ZT.SC-I2CMx的版本号
第1个参数：为模块的地址
第2个参数：所要存储模块版本号的缓存，版本号为16BYTE字节
返回值：   实际读到的版本号字节数
*/
#include <ZtLib.h>
#include <Wire.h>
#define OLED_ADDRESS    0x27

void setup()
{
  uint8_t versbuff[16];
  uint8_t temp;
  Serial.begin(9600);
  ZT.I2cInit();
  
  ZT.ScI2cMxReset(OLED_ADDRESS);
  
  while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
  ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,0,0,"Read Version");
  temp = ZT.ScI2cMxReadVersion(OLED_ADDRESS,versbuff);
  
  while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
  ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,2,0,"Version:");
  
  while (ZT.ScI2cMxReadState(OLED_ADDRESS)==STATUS_BUSY);
  ZT.ScI2cMxDisplay8x16Str(OLED_ADDRESS,4,0,(char *)versbuff);
  
  Serial.print("Version:\n");
  for (int i=0; i<temp; i++)
  {
      Serial.print((char)versbuff[i]);
  }
   Serial.print('\n');
  
}
void loop()
{
}
