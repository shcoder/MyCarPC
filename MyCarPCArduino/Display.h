//Адрес дисплея
#define OLED_ADDRESS 0x27
 
ZtLib led;

unsigned char ct[] = 
{
   0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F, 0x77, 0x7C,0x39,0x5E,0x79,0x71,0x00
};


void setupDisplay() {
  //delay(1000);
  led.I2cInit();
  //включаем дисплей
  led.Seg8b4a036aUnSleep(OLED_ADDRESS);
  //Пример управления яркостью
  led.Seg8b4a036aSetBrightness(OLED_ADDRESS, 0x20, 0x20);
}

const uint8_t char_U = 0x3E;
const uint8_t char_t = 0x78;
const uint8_t char_P = 0x73;
const uint8_t char_O = 0x3F;
const uint8_t char_n = 0x54;
const uint8_t char_F = 0x71;

void loopDisplay() {
  uint8_t buf[4] = {0x00, 0x00, 0x00, 0x00};
  
  uint8_t page = millis()/2000%3;
  // page = 2;
  if (page == 0) { // U xx
    buf[3] = char_U;
    buf[2] = ct[uint8_t(voltage) / 10];
    buf[1] = ct[uint8_t(voltage) % 10] | 0x80;
    buf[0] = ct[uint16_t(voltage * 10) % 10];
    //         led.Seg8b4a036aDisplayDec(OLED_ADDRESS, voltage, 5, 0);
  } else if (page == 1) { // temperature
    buf[3] = char_t;
    buf[2] = ct[uint8_t(temperature) / 10];
    buf[1] = ct[uint8_t(temperature) % 10] | 0x80;
    buf[0] = ct[uint16_t(temperature * 10) % 10];
  } else if (page == 2) { // power status
    buf[3] = char_P;
    buf[2] = 0;
    if (powerStatus == POWER_STATUS_ON) {
      buf[1] = char_O;
      buf[0] = char_n;
    } else if (powerStatus == POWER_STATUS_OFF) {
      buf[2] = char_O;
      buf[1] = char_F;
      buf[0] = char_F;
    } else {
      buf[1] = 0x40;
      buf[0] = 0x40;
    }
  } else {
    buf[0] = 0x40;
    buf[1] = 0x40;
    buf[2] = 0x40;
    buf[3] = 0x40;
  }
  
  led.Seg8b4a036aDisplayBuff(OLED_ADDRESS, buf);
}
