#include <Adafruit_PCD8544.h>
#include <Adafruit_GFX.h>
#include <Keypad.h>

const int PIR = A5;
const int BUZZER = A3;
const int LED = A4;
#define DRZWI_KONTAKTRON A0
#define OKNO_1_KONTAKTRON A1
#define OKNO_2_KONTAKTRON A2

Adafruit_PCD8544 display = Adafruit_PCD8544(9, 10, 11, 12, 13);

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 2, 3, 4, 5 };
byte colPins[COLS] = { 6, 7, 8 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const char *correctCode = "1234";
char enteredCode[5];
bool armed = false;
byte index = 0;

void setup() {
  // tutaj dałem INPUT_PULLUP żeby nie dawać rezystorów tylko używać tego juz wbudowanego w Arduino
  pinMode(DRZWI_KONTAKTRON, INPUT_PULLUP);
  pinMode(OKNO_1_KONTAKTRON, INPUT_PULLUP);
  pinMode(OKNO_2_KONTAKTRON, INPUT_PULLUP);
  pinMode(PIR, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  display.begin();
  display.setContrast(50);
  display.clearDisplay();
  display.display();
  Serial.begin(9600);
  digitalWrite(BUZZER, HIGH);
  digitalWrite(LED, LOW);
}

void loop() {
  // odczytywanie stanów z kontaktronów co 100ms  
  int DRZWI = digitalRead(DRZWI_KONTAKTRON);
  delay(100);
  int OKNO_1 = digitalRead(OKNO_1_KONTAKTRON);
  delay(100);
  int OKNO_2 = digitalRead(OKNO_2_KONTAKTRON);
  delay(100);
  char key = keypad.getKey();

  if (!armed) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("kliknij # i  ubzroj alarm");
    display.display();
  }
  if (armed) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("kliknij * i  rozbroj alarm");
    display.display();
  }


  // uzcie # na klawiaturze jako ubzrajanie alarmu
  if (key == '#' && !armed) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Wpisz kod:");
    display.display();
    index = 0;
    while (index < 4) {
      key = keypad.getKey();
      if (key != NO_KEY && key != '#' && key != '*') {
        enteredCode[index++] = key;
        display.print("*"); // Pokazywanie gwiazdek zamiast cyfr podczas wpisywania kodu
        display.display();
      }
    }
    enteredCode[index] = '\0';
    index = 0;
    delay(300);
    if (strcmp(enteredCode, correctCode) == 0) {
      // Po wpisaniu kodu sprawdzamy, czy został naciśnięty '#'
      while (key != '#') {
        key = keypad.getKey();
        delay(100);
      }
      armAlarm(); 
    } else {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Bledny kod!");
      display.display();
      delay(2000);
      display.clearDisplay();
      display.display();
    }
  }
  // uzcie * na klawiaturze jako rozbrojenie alarmu
  if (key == '*' && armed) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Wpisz kod:");
    display.display();
    index = 0;
    while (index < 4) {
      key = keypad.getKey();
      if (key != NO_KEY && key != '#' && key != '*') {
        enteredCode[index++] = key;
        display.print("*"); // Pokazywanie gwiazdek zamiast cyfr podczas wpisywania kodu
        display.display();
      }
    }
    enteredCode[index] = '\0';
    index = 0;
    delay(300);
    if (strcmp(enteredCode, correctCode) == 0) {
      // Po wpisaniu kodu sprawdzamy, czy został naciśnięty '#'
      while (key != '#') {
        key = keypad.getKey();
        delay(100);
      }
      disarmAlarm();
    } else {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Bledny kod!");
      display.display();
      delay(2000);
      display.clearDisplay();
      display.display();
    }
  }
  // Wykrywanie ruchu
  if (armed && digitalRead(PIR) == HIGH) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Pan złodzieii!");
    display.display();
    activateAlarm();
  }

  // wykrywanie otwartych dzrwi / okien
  if (armed && (DRZWI == 1 || OKNO_1 == 1 || OKNO_2 == 1)) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Pan zlodziej!");
    display.display();
    activateAlarm();
  }
}

void disarmAlarm() {
  armed = false;
    for (int i = 0; i < 5; i++) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Rozbrajanie");
    for (int j = 0; j <= i; j++) {
      display.print(".");
    }
    display.display();
    delay(500);
  }
  digitalWrite(BUZZER, HIGH);
  digitalWrite(LED, LOW);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Alarm rozbrojony");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

void armAlarm() {
  armed = true;
  for (int i = 0; i < 5; i++) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Uzbrajanie");
    for (int j = 0; j <= i; j++) {
      display.print(".");
    }
    display.display();
    delay(500);
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Alarm uzbrojony");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

void activateAlarm() {
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED, HIGH);
  delay(200);
}
