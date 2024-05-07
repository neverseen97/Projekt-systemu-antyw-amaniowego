#include <Adafruit_PCD8544.h>
#include <Adafruit_GFX.h>

#include <Keypad.h>
#include <EEPROM.h> 

const int PIR = A5;
const int BUZZER = A3;
const int LED = A4;
#define DRZWI_1_KONTAKTRON A2
#define OKNO_1_KONTAKTRON A1
#define OKNO_2_KONTAKTRON A0
#define DRZWI_2_KONTAKTRON A3
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
char currentCode[20];
char enteredCode[20];
bool armed = false;
byte index = 0;

void setup() {
  
  EEPROM.get(0, currentCode);

  if (strlen(currentCode) == 0) {
    strcpy(currentCode, "1234");
    EEPROM.put(0, currentCode);
  }

  pinMode(DRZWI_1_KONTAKTRON, INPUT_PULLUP);
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
  Serial.print("Aktualny kod: ");
  Serial.println(currentCode);
  digitalWrite(BUZZER, HIGH);
  digitalWrite(LED, LOW);
}

void loop() {
  int DRZWI = digitalRead(DRZWI_1_KONTAKTRON);
  delay(100);
  int DRZWI_2 = digitalRead(DRZWI_2_KONTAKTRON);
  delay(100);
  int OKNO_1 = digitalRead(OKNO_1_KONTAKTRON);
  delay(100);
  int OKNO_2 = digitalRead(OKNO_2_KONTAKTRON);
  delay(100);

  char key = keypad.getKey();

  if (!armed) {
    display.setCursor(0, 0);
    display.println("kliknij # aby uzbroic alarm");
    display.display();
    display.setCursor(0, 20);
    display.println("kliknij 1 aby zmienic kod");
    display.display();
  }

  if (armed) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("kliknij * aby rozbroic alarm");
    display.display();
  }

  if  (key == '1' && !armed ) {
    changeCode();
  }

  if (key == '#' && !armed) {
    enterCodeAndArmAlarm();
  }

  if (key == '*' && armed) {
    enterCodeAndDisarmAlarm();
  }

  if (armed && digitalRead(PIR) == HIGH) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Pan złodzieij!");
    display.display();
    Serial.println("Pan złodzieij!");
    Serial.println("Czujnik PIR wykryl ruch");
    activateAlarm();
  }

  if (armed && (DRZWI || OKNO_1 || OKNO_2)) {
    
    if (armed && DRZWI) {
      Serial.println("Otwarte DRZWI");
    } else if (armed && OKNO_1) {
      Serial.println("Otwarte OKNO 1");
    } else if (armed && OKNO_2) {
      Serial.println("Otwarte OKNO 2");
    } 
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Pan zlodziej!");
    display.display();
    Serial.println("Pan zlodziej!");
    activateAlarm();
  }
}
// Funkcja do zmiany kodu
void changeCode() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Wpisz nowy kod i naciśnij # ");
  display.display();
  byte index = 0;
  char newCode[20];
  char key;

  while (index < sizeof(newCode) - 1) {
    key = keypad.getKey();
    if (key != NO_KEY && key != '#' && key != '*') {
      newCode[index++] = key;
      display.print("*");
      display.display();
    }
    if (key == '#') {
      newCode[index] = '\0';
      break;
    }
  }

  if (index > 0) {
    strcpy(currentCode, newCode);
    EEPROM.put(0, currentCode);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Kod zmieniony!");
    display.display();
    Serial.println("Kod zostal zmieniony\n");
    delay(2000);
    display.clearDisplay();
    display.display();
  }
}

void enterCodeAndArmAlarm() {
  enterCode();
  if (checkEnteredCode()) {
    armAlarm();
  } else {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Bledny kod!");
    Serial.println("Wprowadzono bledny kod\n");
    display.display();
    delay(2000);
    display.clearDisplay();
    display.display();
  }
}

void enterCodeAndDisarmAlarm() {
  enterCode();
  if (checkEnteredCode()) {
    disarmAlarm();
  } else {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Bledny kod!");
    Serial.println("Wprowadzono bledny kod\n");
    display.display();
    delay(2000);
    display.clearDisplay();
    display.display();
  }
}


// Funkcja do wpisywania kodu
void enterCode() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Wpisz kod i naciśnij # ");
  display.display();
  index = 0;
  while (index < 20) {
    char key = keypad.getKey();
    if (key != NO_KEY && key != '#' && key != '*') {
      enteredCode[index++] = key;
      display.print("*");
      display.display();
    }
    if (key == '#') {
      enteredCode[index] = '\0';
      break;
    }
  }
  delay(300);
}


bool checkEnteredCode() {
  return strcmp(enteredCode, currentCode) == 0; // strcmp sprawdza czy 2 zmienne są sobie równe
}


// funkcja do rozbrojenia alarmu
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
  Serial.println("Alarm zostal rozbrojony\n");
  delay(2000);
  display.clearDisplay();
  display.display();
}


// funkcja do uzbrojenia alarmu
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
  Serial.println("Alarm zostal uzbrojony\n");
  delay(2000);
  display.clearDisplay();
  display.display();
}


// jesli czujnik ruchu / kontaktrony się aktywują to wywoła się ta funkcja która powoduje że buzzer zaczyna drzeć pizde 
void activateAlarm() {
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED, HIGH);
  delay(200);
}
