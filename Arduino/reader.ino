// Use 160MHz cpu

//Software Serial library
#include <SoftwareSerial.h>

//PN532 libraries
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

//Software Serial defines
#define rxPin D6
#define txPin D7
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

//PN532 defines+functions
#define MSCHECK 500
#define MSREAD 125
#define PN532_IRQ (D3)
#define PN532_RESET (D4)
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
uint8_t last_uid[] = { 0, 0, 0, 0, 0, 0, 0 };
unsigned long last_detect = millis();
void readCard(){ // Read card information
  bool success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, MSREAD);
  String read;
  if(success){
    memcpy(last_uid, uid, sizeof(uid[0])*7);
    Serial.println("Card detected!");
    last_detect = millis();
  }
}

void setup() {
  Serial.begin(115200);

  //LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  //Software Serial init
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);

  //PN532 init
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1);
  }
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);
  Serial.println("Waiting for an ISO14443A Card ...");
  
  digitalWrite(LED_BUILTIN, LOW);
}

unsigned long last_read = millis();

void loop() {
  readCard();
  if(millis()-last_read >= MSREAD){ // Send card info to server
    if(millis()-last_detect >= MSCHECK){ // No card present in last MSCHECK ms
      mySerial.println("none");
    }else{ // Found card
      char buffer[40];
      sprintf(buffer, "%02x:%02x:%02x:%02x", last_uid[0], last_uid[1], last_uid[2], last_uid[3]);
      mySerial.println(buffer);
    }
  }
}
