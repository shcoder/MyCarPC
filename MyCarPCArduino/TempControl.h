#define ONE_WIRE_BUS   18
#define COOLING_PIN    10


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

int  resolution = 9;
unsigned long lastTempRequest = 0;
int  delayInMillis = 0;

void setupTemp() {
  sensors.begin();
  sensors.getAddress(tempDeviceAddress, 0);
  sensors.setResolution(tempDeviceAddress, resolution);
  
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  pinMode(COOLING_PIN, OUTPUT);
  delayInMillis = 790 / (1 << (12 - resolution)); 
  lastTempRequest = millis();
}

bool coolingStatus = false;

void processTemperature(float t) {
  //Serial.println(t);
  if (coolingStatus) { // охлаждение включено
    if (t <= 40)
      coolingStatus = false;
  } else {             // охлаждение выключено
    if (t >= 50)
      coolingStatus = true;
  }
  digitalWrite(COOLING_PIN, coolingStatus);
  Serial.print("c=");
  Serial.println(coolingStatus);
}

void loopTemp() {
  if (millis() - lastTempRequest >= delayInMillis) // waited long enough??
  {
    temperature = sensors.getTempCByIndex(0); 
    //if (temperature >= 0 && temperature <= 80)
      processTemperature(temperature);
    
    //sensors.setResolution(tempDeviceAddress, resolution);
    sensors.requestTemperatures(); 
    lastTempRequest = millis(); 
  }
}
