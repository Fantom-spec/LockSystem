#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

#define SS_PIN 10
#define RST_PIN A0 
#define GREEN_LED A1
#define BLUE_LED A2
#define RED_LED A3   
MFRC522 mfrc522(SS_PIN, RST_PIN);

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String uidString = "";
String pinString = "";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    char trigger = Serial.read();
    if (trigger == '1') { 
      digitalWrite(RED_LED, HIGH);
      delay(500);
      digitalWrite(RED_LED, LOW);
    }
    else if (trigger == '0') { 
      digitalWrite(GREEN_LED, HIGH);
      delay(500);
      digitalWrite(GREEN_LED, LOW);
    }
  }

  if (uidString == "") {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    uidString = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] < 0x10) uidString += "0";
      uidString += String(mfrc522.uid.uidByte[i], HEX);
    }
    uidString.toUpperCase();
    digitalWrite(GREEN_LED, HIGH);
    delay(500);
    digitalWrite(GREEN_LED, LOW);

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }

  if (uidString != "" && pinString.length() < 4) {
    char key = keypad.getKey();
    if (key) {
      digitalWrite(BLUE_LED, HIGH);
      delay(500);
      digitalWrite(BLUE_LED, LOW);

      if (key >= '0' && key <= '9') {
        pinString += key;
      }
      if (key == '#') { // Clear PIN
        pinString = "";
      }
    }
  }

  if (uidString != "" && pinString.length() == 4) {
    delay(100);
    digitalWrite(GREEN_LED, HIGH);
    delay(500);
    digitalWrite(GREEN_LED, LOW);

    String encryptedData = encryptAndReturn(uidString, pinString);
    Serial.println(encryptedData);

    uidString = "";
    pinString = "";
    delay(1000);
  }
}
