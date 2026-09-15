#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         5          // Configurable, see typical pin layout above
#define SS_PIN          4        // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
MFRC522::MIFARE_Key key;
// Input words here:
const char english[16] = "Test";
const char german[16] = "Testen";
// doesnt have to be german
byte blockeng = 4; // safe block to store data
byte blockdeu = 5; // safe block to store data

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println(F("Writing with the LLH"));
  Serial.println(F("English Word:"));
  Serial.print(english);
  Serial.println(F("German Word:"));
  Serial.print(german);
 

  for (byte i =0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
 
  Serial.println(F("Scan a NFC sticker"));

}

void loop() {

  if (!mfrc522.PICC_IsNewCardPresent()){
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()){
    return;
  }
 
  Serial.println();
  Serial.println(F("card detected!(noice)"));

  MFRC522::StatusCode status;

  // Authenticate
  status = mfrc522.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A,
    blockeng,
    &key,
    &(mfrc522.uid)
  );

  if (status != MFRC522::STATUS_OK) {
    Serial.println(F("Authentication failed"));
    Serial.println(mfrc522.
    GetStatusCodeName(status));
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }

  Serial.println(F("authentication succesful"));

  //writing for english(4)


  status = mfrc522.MIFARE_Write(blockeng, (byte*)english, 16);

  if (status == MFRC522::STATUS_OK) {
    Serial.println(F("english writing was succesful"));
  } else {
    Serial.println(F("english write failed"));
    Serial.println(mfrc522.
    GetStatusCodeName(status));
  }

  //writing for german(5)


  status = mfrc522.MIFARE_Write(blockdeu, (byte*)german, 16);

  if (status == MFRC522::STATUS_OK) {
    Serial.println(F("german writing was succesful"));
  } else {
    Serial.println(F("german write failed"));
    Serial.println(mfrc522.
    GetStatusCodeName(status));
  }

  // verifying what was written in english

  byte buffer[18];
  byte size = sizeof(buffer);


  status = mfrc522.MIFARE_Read(blockeng, buffer, &size);

  if (status == MFRC522::STATUS_OK) {
    Serial.println(F("verifying english:"));
    Serial.println(F("block 4 contains:\""));

    for (byte i=0; i<16; i++){
      if(buffer[i] !=0){
        Serial.write(buffer[i]);
      }
    }
    Serial.println(F("\""));

  }else{
    Serial.print(F("block 4 read failed:"));
    Serial.print(mfrc522.
    GetStatusCodeName(status));
  }

// verifying what was written in german





  status = mfrc522.MIFARE_Read(blockdeu, buffer, &size);

  if (status == MFRC522::STATUS_OK) {
    Serial.println(F("verifying german:"));
    Serial.println(F("block 5 contains:\""));

    for (byte i=0; i<16; i++){
      if(buffer[i] !=0){
        Serial.write(buffer[i]);
      }
    }
    Serial.println(F("\""));

  }else{
    Serial.print(F("block 5 read failed:"));
    Serial.print(mfrc522.
    GetStatusCodeName(status));
  }


 

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(1500); // prevent multiple scans

}