// main program for the language lerning helper (llh)
// dvtsellios 2026

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 20000; // wait time in milliseconds
const int buttonPin = 15; // pin that the button is conected to
int buttonState = 0;
bool buttonPressed = false;

#define RST_PIN 5
#define SS_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte blockeng = 4;
byte blockdeu = 5;

void setup(){
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  // x, y
  lcd.setCursor(3, 0);
  pinMode(15, INPUT_PULLUP);

  for (byte i=0; i<6; i++){
    key.keyByte[i] = 0xFF;
  }
}

void loop(){
  if(!mfrc522.PICC_IsNewCardPresent()){
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()){
    return;
  }
  MFRC522::StatusCode status;


  // authentication
  status = mfrc522.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A,
    blockeng,
    &key,
    &(mfrc522.uid)
  );

  if (status != MFRC522::STATUS_OK){
    Serial.println(F("authantication failed"));
    Serial.println(mfrc522.
    GetStatusCodeName(status));
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }

  Serial.println(F("authentication succeful"));

  byte buffer[18];
  byte size = sizeof(buffer);

  status = mfrc522.MIFARE_Read(blockeng, buffer, &size);

  if(status == MFRC522::STATUS_OK){
    Serial.println(F("reading english:"));
    Serial.println(F("block 4 contains:\""));

    for (byte i=0; i<16; i++){
      if(buffer[i] !=0){
        lcd.print((char)buffer[i]);
      }
    }
    Serial.println(F("\""));
    unsigned long startWait = millis();
    buttonPressed = false;
    while (millis() - startWait <= period){
      if (digitalRead(buttonPin) == LOW){
        buttonPressed = true;
        break;
      }
       
    }
    if (buttonPressed ==  true){
      lcd.clear();
       status = mfrc522.MIFARE_Read(blockdeu, buffer, &size);

       if(status == MFRC522::STATUS_OK){
        Serial.println(F("reading german:"));
        Serial.println(F("block 5 contains:\""));

       for(byte i=0; i<16; i++){
        if(buffer[i] !=0){
        lcd.print((char)buffer[i]);
        }
       }  
       Serial.println(F("\""));
      }else{
       Serial.print(F("block 5 read failed:"));
       Serial.print(mfrc522.
       GetStatusCodeName(status));
       return;
      }


    }else{
      lcd.print("Didnt click button");
      delay(2000);
      lcd.clear();
      return;
    }
   




  }else{
    Serial.print(F("block 4 read failed:"));
    Serial.print(mfrc522.
    GetStatusCodeName(status));
    return;
  }

  delay(4000);
  lcd.clear();
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(1500); // prevent multiple scans

}
