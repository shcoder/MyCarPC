#define SDA_PORT PORTD
#define SDA_PIN 1
#define SCL_PORT PORTD
#define SCL_PIN 0

#define I2C_TIMEOUT 100
#define I2C_NOINTERRUPT 0
#define I2C_SLOWMODE 0
#include <SoftI2CMaster.h>
#include "I2CTools.h"

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}

byte ReadByte() {
  while (!Serial.available())
    ;
  return Serial.read();
}

char ReadChar() {
  return char(ReadByte());
}

void WriteUShort(short u) {
  Serial.write(u >> 8);
  Serial.write(u & 0xFF);
}

void PCMessage(String s) {
  Serial.print("$M");
  WriteUShort(s.length());
  Serial.print(s);
  Serial.print("#");
}

uint8_t buf[128];

void loop() {
  int i;
  char cmd;
  char cmd2;
  uint8_t a_addr;
  uint8_t r_reg;
  uint8_t r_len;
  uint8_t rr_res;
  uint8_t rw_val;

  // ждём начала команды
  while (ReadChar() != '$') ;
  
  cmd = ReadChar();
  
  switch (cmd) {
    case 'V': // версия прошивки Arduino
      Serial.print("$V1.0.0.0.#");    
      break;
    case 'T': // Test
      PCMessage("Test Message for back message!!!");
      break;
    case 'A': // I2C Arddess
      a_addr = ReadByte();
      ADDR = a_addr;
      //PCMessage("Assigned I2C address");
      break;
    case 'R': // I2C: ReadReg / WriteReg  
      cmd2 = ReadChar();
      r_reg = ReadByte();
      switch (cmd2) {
        case 'R': // I2C: ReadReg
          rr_res = ReadReg(r_reg);
          Serial.print("$R");
          Serial.write(rr_res);
          Serial.print("#");
          break;
        case 'W': // I2C: WriteReg
          rw_val = ReadByte();
          WriteReg(r_reg, rw_val);
          //PCMessage("Recieved WriteReg");
          break;
      }
      break;
    case 'B': // I2C: ReadBytes / WriteBytes
      cmd2 = ReadChar();
      r_reg = ReadByte();
      r_len = ReadByte();
      switch (cmd2) {
        case 'R': // I2C: ReadReg
          memset(buf, 0, sizeof buf);
          ReadBytesFromAddr(r_reg, buf, r_len);
          rr_res = ReadReg(r_reg);
          Serial.print("$B");
          Serial.write(r_len);
          for (i = 0; i < r_len; i++) {
            Serial.write(buf[i]);
          }
          Serial.print("#");
          break;
        case 'W': // I2C: WriteReg
          if (r_len > 64)
            r_len = 64;
          memset(buf, 0, sizeof buf);
          for (i = 0; i < r_len; i++) {
            buf[i] = ReadByte();
          }
          WriteBytesToAddr(r_reg, buf, r_len);

            //Serial.print("$M");
            //WriteUShort(64+4+4);
            //Serial.print("BW: ");
            //for (i = 0; i < r_len; i++) {
            //  Serial.write(buf[i]);
            //}
            //Serial.print("<<<<");         
            //Serial.print("#");
      
          break;
      }
      break;
    default:
      PCMessage("Unknown command: " + cmd);
      return;
  }
  
  char fin = ReadChar();
  if (fin ==  '@') { // маркер контрольной суммы. Проверять и отвечать на это мы пока не умеем. Просто жуём
    ReadChar();
    fin = ReadChar(); // новый маркер
  }
  if (fin != '#') { // маркер конца отсутствует. Опять ошибка. Пока игнорим
    
  }  
}

