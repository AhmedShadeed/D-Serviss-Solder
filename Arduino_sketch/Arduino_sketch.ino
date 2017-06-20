/*
 ПАЯЛЬНАЯ СТАНЦИЯ ver. 0.4

 Дата создания 2015
 Автор Олег Андреев
 d-serviss@inbox.lv
 http://www.d-serviss.lv
*/

#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);  // Выходы для дисплея 1602

int pinSolderOut = 5;  // Выход для паяльника
int pinSolderIn = A4;  // Потенциометр паяльника
int pinSolderTCouple = A3;  // Термопара паяльника
int pinSolderButton = 2;  // Кнопка вкл. и выкл. паяльника

int pinHotAirOut = 6;  // Выход для фена
int pinHotAirIn = A2;  // Потенциометр фена
int pinHotAirTCouple = A1;  // Термопара фена
int pinHotAirCoolerOut = 3;  // Выход для вентилятора фена ( PWM )
int pinHotAirCoolerIn = A0;  // Потенциометр вентилятора фена
int pinHotAirButton = 4;  // Кнопка вкл.и выкл. фена

uint8_t char_cel[8] = {
  B00111, B00101, B00111, B00000, B00000, B00000, B00000
};

void setup()
{
  TCCR2B = TCCR2B & 0b11111000 | 0x01; // Частота ШИМ 11 и 3
  pinMode(pinSolderOut, OUTPUT);
  pinMode(pinSolderButton, INPUT);
  pinMode(pinHotAirOut, OUTPUT);
  pinMode(pinHotAirButton, INPUT);
  lcd.begin(16, 2);
  lcd.createChar(1, char_cel);
}

void loop()
{
  // Преобразовываем значения
  int setSolderTemp = map(averaging(pinSolderIn), 0, 1023, 0, 480);
  int solderTCouple = map(averaging(pinSolderTCouple), 0, 780, 0, 480);
  int setHotAirTemp = map(averaging(pinHotAirIn), 0, 1023, 0, 480);
  int hotAirTCouple = map(averaging(pinHotAirTCouple), 0, 780, 0, 480);
  int setHotAirCooler = map(averaging(pinHotAirCoolerIn), 0, 1023, 130, 255);
  int displayHotAirCooler = map(averaging(pinHotAirCoolerIn), 0, 1023, 0, 99);

  // Поддержка установленной температуры паяльника
  if (setSolderTemp >= solderTCouple && digitalRead(pinSolderButton) == HIGH)
  {
    digitalWrite(pinSolderOut, LOW);
    digitalWrite(pinSolderOut, HIGH);
  }
  else {
    digitalWrite(pinSolderOut, LOW);
  }

  // Поддержка установленной температуры фена
  if (setHotAirTemp >= hotAirTCouple && digitalRead(pinHotAirButton) == HIGH)
  {
    digitalWrite(pinHotAirOut, HIGH);
    delay(100);
    digitalWrite(pinHotAirOut, LOW);
  }
  else {
    digitalWrite(pinHotAirOut, LOW);
  }

  // Установка оборотов вентилятора фена
  if (hotAirTCouple < 85 && digitalRead(pinHotAirButton) == LOW) {
    analogWrite(pinHotAirCoolerOut, 0);
  }
  else {
    analogWrite(pinHotAirCoolerOut, setHotAirCooler);
  }

  // Данные паяльника на дисплей
  lcd.clear();
  lcd.print("Solder:");
  lcd.setCursor(7, 0);
  if (digitalRead(pinSolderButton) == HIGH) {
    lcd.print(setSolderTemp);
    lcd.print("\1");
    lcd.setCursor(12, 0);
    lcd.print(solderTCouple);
    lcd.print("\1");
  }
  else {
    lcd.print(" Off");
  }
  // Данные фена на дисплей
  lcd.setCursor(0, 1);
  lcd.print("Air:");
  lcd.setCursor(4, 1);
  if (digitalRead(pinHotAirButton) == HIGH) {
    lcd.print(setHotAirTemp);
    lcd.print("\1");
    lcd.setCursor(9, 1);
    lcd.print(hotAirTCouple);
    lcd.print("\1");
    // Вентилятор фена на дисплей
    lcd.setCursor(14, 1);
    lcd.print(displayHotAirCooler);
  }
  else {
    lcd.print(" Off");
  }
  delay(100);
}

int averaging(int pin) {

  unsigned long int res = 0;
  for (int i = 0; i < 64; i++) res += analogRead(pin);
  return res >> 6;
}
