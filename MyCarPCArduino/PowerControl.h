#define VOLTAGE_PIN A3
#define VOLTAGE_RATE_MS 100

#define VOLTAGE_LOPOWER 11.5
#define VOLTAGE_UPPOWER 12

#define POWER_STATUS_OFF 0
#define POWER_STATUS_ON 1
#define POWER_ALLOW_STATUS_CHANGE 2000
#define POWER_CONTROL_PIN 5

const float power_conver_coeff = 886 / 15;

long voltageLastRefresh;
long lastPowerStatusON;
long lastPowerStatusOFF;

void setupPowerControl() {
  voltageLastRefresh = 0;
  powerStatus = POWER_STATUS_OFF;
  lastPowerStatusON = 0;
  lastPowerStatusOFF = 0;
  pinMode(POWER_CONTROL_PIN, OUTPUT);
}

void readVoltage() {
  if (millis() - voltageLastRefresh < VOLTAGE_RATE_MS)
    return;
    
  float pinVal = analogRead(VOLTAGE_PIN);
  //voltage = voltage * 0.7  + (pinVal / 1024 * 5) * 0.3;
  voltage = voltage * 0.5  + pinVal / power_conver_coeff * 0.5;
  //Serial.print("U=");
  //Serial.println(voltage);
  voltageLastRefresh = millis();
}

void loopPowerControl() {
  readVoltage();
  Serial.print("U=");
  Serial.println(voltage);
  
  uint8_t newPowerStatus = powerStatus;
  if (powerStatus == POWER_STATUS_OFF && (voltage >= VOLTAGE_UPPOWER))
    newPowerStatus = POWER_STATUS_ON;
  if (powerStatus == POWER_STATUS_ON && (voltage <= VOLTAGE_LOPOWER))
    newPowerStatus = POWER_STATUS_OFF;
  // = (voltage >= VOLTAGE_LOPOWER)?POWER_STATUS_ON:POWER_STATUS_OFF;
  
    
  if (powerStatus == POWER_STATUS_OFF) {
    Serial.println("powerStatus = OFF");
    if (newPowerStatus == POWER_STATUS_ON && (millis() - lastPowerStatusOFF > POWER_ALLOW_STATUS_CHANGE)) {
      Serial.println("set new powerstatus = ON");
      powerStatus = POWER_STATUS_ON;
    }
  } else if (powerStatus == POWER_STATUS_ON) {
    Serial.println("powerStatus = ON");
    if (newPowerStatus == POWER_STATUS_OFF && (millis() - lastPowerStatusON > POWER_ALLOW_STATUS_CHANGE)) {
      Serial.println("set new powerstatus = OFF");
      powerStatus = POWER_STATUS_OFF;
    }
  }
  
  if (newPowerStatus == POWER_STATUS_OFF)
    lastPowerStatusOFF = millis();
  if (newPowerStatus == POWER_STATUS_ON)
    lastPowerStatusON = millis();
    
  digitalWrite(POWER_CONTROL_PIN, !powerStatus);
}
