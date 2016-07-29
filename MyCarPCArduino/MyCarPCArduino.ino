float temperature = 0.0;
float voltage = 0.0;
uint8_t powerStatus;

#include <OneWire.h>
#include <DallasTemperature.h>
#include "TempControl.h"

#include "Keyboard.h"

#include "PowerControl.h"

#include <Wire.h>
#include "ZtLib.h"
#include "Display.h"

void setup(void) {
  delay(500);
  //Serial.println("Press any key to run...");
  //while (!Serial.available()) ;
  //while (Serial.available())
  //  Serial.read();
    
  setupTemp();
  //setupKeyboard();
  setupPowerControl();
  setupDisplay();
}

void loop(void){
  loopTemp();
  //loopKeyboard();
  loopPowerControl();
  loopDisplay();
}
