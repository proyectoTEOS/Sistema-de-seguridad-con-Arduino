/*
  Created by TEOS
  Domotic with Arduino https://goo.gl/btZpjs
  YouTube https://goo.gl/k7TUSZ
  Instagram https://goo.gl/zEIjia
  Facebook https://goo.gl/eivraR
  Twitter https://goo.gl/du5Wgn
  Github https://goo.gl/Xl5IiS
  WEB https://www.proyecto-teos.com
*/

#include <Keypad.h> //http://playground.arduino.cc/Code/Keypad
#include <Password.h> //http://playground.arduino.cc/Code/Password
#include <LiquidCrystal_I2C.h> //https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <Wire.h>

//Declaration of waiting time variables between functions.
const uint16_t delayFunctionT = 1000, intervalT = 400, delayLedT = 50;
unsigned long previousMillisT;

//They start in a state under the variables that control the system.
uint8_t passPositionT = 0, speakerStatusT = LOW, ledStatusT = LOW;
bool alarmStatusT = false, alarmActiveT = false, motionDetectionT = false;

//Declaration of the keypad pins, LEDs, motion sensor and buzzer.
const uint8_t colsT = 4, rowsT = 4;
const uint8_t colPinsT[colsT] = {3, 2, 1, 0}, rowPinsT[rowsT] = {7, 6, 5, 4}, pirPinT = 8, speakerPinT = 9, ledRedPinT = 10, ledBluePinT = 11;
const char keysT[rowsT][colsT] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

//Instances of the lcd, password and keypad objects. The password is "1111", you can
//change it by any combination of 4 digits.
LiquidCrystal_I2C lcd(0x27, 20, 4);
Password passwordT = Password("1105");
Keypad keypad = Keypad( makeKeymap(keysT), rowPinsT, colPinsT, rowsT, colsT );

void setup() {
  lcd.begin();
  //Pin configuration.
  pinMode(speakerPinT, OUTPUT);
  pinMode(ledRedPinT, OUTPUT);
  pinMode(ledBluePinT, OUTPUT);
  pinMode(pirPinT, INPUT);
  digitalWrite(ledBluePinT, HIGH);

  //Start message with 3 seconds wait.
  lcd.setCursor(0, 0);
  lcd.print("Sistema de seguridad");
  lcd.setCursor(0, 1);
  lcd.print(" proyecto-teos.com");
  delay(3000);
  previousMillisT = millis();
  keypad.addEventListener(keypadEventT);
  blockingMessageT();
}

void loop() {
  //If the alarmActiveT variable is in a high state and the pirPinT detects movement,
  //it will call the motionDetectedT function. The motionDetectionT function triggers
  //the buzzer's sonar.
  keypad.getKey();
  if (alarmActiveT == true && digitalRead(pirPinT) == HIGH)
    motionDetectedT();
  if (motionDetectionT)
    digitalWrite(speakerPinT, speakerAlarmT());
}

//keypadEventT registers the events of the keyboard.
void keypadEventT(KeypadEvent keyT) {
  switch (keypad.getState()) {
    //When typing "#" or entering more than 4 characters the function will be
    //called checkPasswordT.
    case PRESSED:
      if (passPositionT >= 5)
        checkPasswordT();
      lcd.setCursor((passPositionT++), 2);
      switch (keyT) {
        case '#':
          checkPasswordT();
          break;
        default:
          passwordT.append(keyT);
          lcd.print("*");
      }
  }
}

//If the password is correct and the alarms are deactivated, then it will call the
//blockedSystemT function, if the password is correct and the alarms are activated,
//then it will call the unlockSystemT function, contrary to all this it will be called
//the incorrectPasswordT function, which is when introduced an incorrect password.
void checkPasswordT() {
  if (passwordT.evaluate()) {
    if (alarmActiveT == false && alarmStatusT == false)
      blockedSystemT();
    else if (alarmActiveT == true || alarmStatusT == true)
      unlockSystemT();
  }
  else
    incorrectPasswordT();
}

//Function that indicates that an incorrect password has been entered.
void incorrectPasswordT() {
  passwordT.reset();
  passPositionT = 0;
  digitalWrite(ledBluePinT, LOW);
  //An incorrect password message is sent.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Clave incorrecta");
  //Using the ternary operator will turn on or off a led.
  for (uint8_t iT = 1; iT <= 8; iT++) {
    ledStatusT = (ledStatusT) ? LOW : HIGH;
    digitalWrite(ledRedPinT, ledStatusT);
    delay(delayLedT);
  }
  //The following conditions will serve to return the state of the LED.
  if (alarmActiveT == false && alarmStatusT == false) {
    digitalWrite(ledRedPinT, LOW);
    digitalWrite(ledBluePinT, HIGH);
    delay(delayFunctionT);
    blockingMessageT();
  }
  else if (alarmActiveT == true || alarmStatusT == true) {
    digitalWrite(ledRedPinT, HIGH);
    digitalWrite(ledBluePinT, LOW);
    delay(delayFunctionT);
    unlockMessageT();
  }
}

//Function that shows motion detection.
void motionDetectedT() {
  //Activate the alarms and restart the password.
  motionDetectionT = true;
  alarmStatusT = true;
  passwordT.reset();
  passPositionT = 0;
  //Send a message that movement has been detected, also indicated by LEDs.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Movimiento detectado");
  for (uint8_t iT = 1; iT <= 8; iT++) {
    ledStatusT = (ledStatusT) ? LOW : HIGH;
    digitalWrite(ledRedPinT, ledStatusT);
    delay(delayLedT);
  }
  for (uint8_t iT = 1; iT <= 8; iT++) {
    ledStatusT = (ledStatusT) ? LOW : HIGH;
    digitalWrite(ledBluePinT, ledStatusT);
    delay(delayLedT);
  }
  digitalWrite(ledRedPinT, HIGH);
  digitalWrite(ledBluePinT, LOW);
  delay(delayFunctionT);
  unlockMessageT();
}

//Function that blocks the system.
void blockedSystemT() {
  passwordT.reset();
  passPositionT = 0;
  digitalWrite(ledRedPinT, HIGH);
  digitalWrite(ledBluePinT, LOW);
  //10 seconds of tolerance are given for the system to activate.
  for (uint8_t iT = 10; iT > 0; iT--) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Quedan ");
    lcd.setCursor(7, 0);
    lcd.print(iT);
    lcd.setCursor(9, 0);
    lcd.print(" segundos");
    lcd.setCursor(0, 1);
    lcd.print("para que se active");
    lcd.setCursor(0, 2);
    lcd.print("el sistema");
    delay(delayFunctionT);
  }

  alarmActiveT = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Activo");
  delay(delayFunctionT);
  unlockMessageT();
}

//Function that unlocks the system.
void unlockSystemT() {
  //The alarm states change to a low state.
  alarmStatusT = false;
  alarmActiveT = false;
  motionDetectionT = false;
  passwordT.reset();
  passPositionT = 0;
  digitalWrite(ledRedPinT, LOW);
  digitalWrite(ledBluePinT, HIGH);
  digitalWrite(speakerPinT, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema desactivado");
  delay(delayFunctionT);
  blockingMessageT();
}

//The next two functions show messages on the screen.
void blockingMessageT() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Digite su clave para");
  lcd.setCursor(0, 1);
  lcd.print("activar el sistema");
}

void unlockMessageT() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Digite su clave para");
  lcd.setCursor(0, 1);
  lcd.print("desactivar la alarma");
}

//This function returns a high or low state that is assigned to the buzzer.
uint8_t speakerAlarmT() {
  //Using the millis function we can execute functions in "background".
  unsigned long currentMillisT = millis();
  if ((unsigned long)(currentMillisT - previousMillisT) >= intervalT) {
    previousMillisT = millis();
    return speakerStatusT = (speakerStatusT) ? LOW : HIGH;
  }
}
