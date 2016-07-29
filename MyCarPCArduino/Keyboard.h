#define POWER_KEY_PIN 5
#define LOCK_KEY_PERIOD 50
#define KEY_CODE 0x1a

void setupKeyboard() {
  pinMode(POWER_KEY_PIN, INPUT_PULLUP);
  Keyboard.begin();
}

bool keyStatus = false;
long lastModification = 0;

void loopKeyboard() {
  if (millis()-lastModification < LOCK_KEY_PERIOD) // исключаем дребезг контактов
    return;
  if (keyStatus) { // кнопка была нажата
      if (digitalRead(POWER_KEY_PIN)) { // отжали кнопку
        Serial.println("Key unpressed");
        //Keyboard.release(KEY_CODE);
        Keyboard.release(116);
        Keyboard.release(152);
        Keyboard.release(142);
        Keyboard.release(143);
        keyStatus = false;
        lastModification = millis();
      }
  } else {         // кнопка была не нажата
      if (!digitalRead(POWER_KEY_PIN)) { // нажали кнопку
        Serial.println("Key pressed");
        //Keyboard.press(KEY_CODE);
        Keyboard.press(116);
        Keyboard.press(152);
        Keyboard.press(142);
        Keyboard.press(143);
        keyStatus = true;
        lastModification = millis();
      }
  }
}
