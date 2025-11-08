#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

// --- RC522 Pins ---
#define SS_PIN 10
#define RST_PIN A0 
MFRC522 mfrc522(SS_PIN, RST_PIN);

// --- Keypad Setup (Pins 2–9) ---
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
  //Serial.println("Place RFID card and then enter PIN...");
}

void loop() {
  // --- Step 1: Wait for RFID card ---
  if (uidString == "") {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    uidString = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      // Ensure 2-digit hex (e.g., "04" instead of "4")
      if (mfrc522.uid.uidByte[i] < 0x10) uidString += "0";
      uidString += String(mfrc522.uid.uidByte[i], HEX);
    }
    uidString.toUpperCase();

    //Serial.print("Card UID: ");
    //Serial.println(uidString);

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    //Serial.println("Now enter 4-digit PIN:");
  }

  // --- Step 2: Read 4-digit PIN from keypad ---
  if (uidString != "" && pinString.length() < 4) {
    char key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        pinString += key;
        //Serial.print('*'); // Mask input
      }
      if (key == '#') { // Clear PIN
        pinString = "";
        //Serial.println("\nPIN cleared.");
      }
    }
  }

  // --- Step 3: When both ready, send UID|PIN ---
  if (uidString != "" && pinString.length() == 4) {
    //Serial.println();
    //String output = uidString + "|" + pinString;
    //Serial.println(output);
    //Serial.println("Data sent.");
    String encryptedData=encryptAndReturn(uidString,pinString);
    Serial.println(encryptedData);
    // Reset for next read
    uidString = "";
    pinString = "";
    delay(1000);
    //Serial.println("\nPlace next card and enter PIN...");
  }
}
