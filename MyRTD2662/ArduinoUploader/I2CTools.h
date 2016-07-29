#define SDA_PORT PORTD
#define SDA_PIN 1
#define SCL_PORT PORTD
#define SCL_PIN 0

#define I2C_TIMEOUT 100
#define I2C_NOINTERRUPT 0
#define I2C_SLOWMODE 0

#include <SoftI2CMaster.h>
#include <avr/io.h> 

byte lastFails = 0;
uint8_t ADDR = 0x94;

bool WriteBytesToAddr(uint8_t reg, uint8_t* values, uint8_t len) {
  lastFails = 0;
  if (!i2c_start(ADDR | I2C_WRITE)) // transmit to device #8
    lastFails++;
  else { 
    if (!i2c_write(byte(reg)))
      lastFails++;
    else {
      byte idx;
      for (idx = 0; idx < len; idx++) {
        if (!i2c_write(byte(values[idx]))) {
          lastFails++;
          break;
        }
      }
    }    
  }
  i2c_stop();
  return lastFails == 0;
}

bool ReadBytesFromAddr(uint8_t reg, uint8_t* dest, uint8_t len) {
  lastFails = 0;
  if (!i2c_start(ADDR | I2C_WRITE)) // transmit to device #8
    lastFails++;
  else { 
    if (!i2c_write(byte(reg)))
      lastFails++;
    else {
      if (!i2c_rep_start(ADDR | I2C_READ))
        lastFails++;
      else {
        byte idx;
        byte b;
        for (idx = 0; idx < len; idx++) {
          b = i2c_read(idx == len-1);
          dest[idx] = b;
        }
      }
    }    
  }
  i2c_stop();
  return lastFails == 0;
}

bool WriteReg(uint8_t reg, uint8_t value) {
//  Serial.print("WriteReg(");
//  Serial.print(reg, HEX);
//  Serial.print(", ");
//  Serial.print(value, HEX);
//  Serial.println(");");
  return WriteBytesToAddr(reg, &value, 1);
}

uint8_t ReadReg(uint8_t reg) {
//  Serial.print("ReadReg(");
//  Serial.print(reg, HEX);
//  Serial.print(")");
  uint8_t result;
  ReadBytesFromAddr(reg, &result, 1);
//  Serial.print(" = ");
//  Serial.println(result, HEX);
  return result;
}


#define E_CC_NOOP 0
#define E_CC_WRITE 1
#define E_CC_READ 2
#define E_CC_WRITE_AFTER_WREN 3
#define E_CC_WRITE_AFTER_EWSR 4
#define E_CC_ERASE 5

uint32_t SPICommonCommand(uint8_t cmd_type, uint8_t cmd_code, uint8_t num_reads, uint8_t num_writes, uint32_t write_value) {
  num_reads &= 3;
  num_writes &= 3;
  write_value &= 0xFFFFFF;
  uint8_t reg_value = (cmd_type << 5) | 
    (num_writes << 3) |
    (num_reads << 1);

  WriteReg(0x60, reg_value);
  WriteReg(0x61, cmd_code);
  switch (num_writes) {
  case 3:
    WriteReg(0x64, write_value >> 16);
    WriteReg(0x65, write_value >> 8);
    WriteReg(0x66, write_value);
    break;
  case 2:
    WriteReg(0x64, write_value >> 8);
    WriteReg(0x65, write_value);
    break;
  case 1:
    WriteReg(0x64, write_value);
    break;
  }
  WriteReg(0x60, reg_value | 1); // Execute the command
  uint8_t b;
  do {
    b = ReadReg(0x60);
  } while (b & 1);  // TODO: add timeout and reset the controller

  uint32_t res = 0;
  uint8_t rreg = 0x67;
  for (int i = 0; i < num_reads; i++) {
    res = res << 8;
    res |= ReadReg(rreg);
    rreg++;
  }
  
  return res;
}
