#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ (D3)
#define PN532_RESET (D4)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

ESP8266WebServer WebServer(80);

uint8_t last_uid[] = { 0, 0, 0, 0, 0, 0, 0 };

void defaultPage() {
  char buffer[40];
  sprintf(buffer, "%02x:%02x:%02x:%02x", last_uid[0], last_uid[1], last_uid[2], last_uid[3]);
  WebServer.send(200, "text/plain", buffer);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP("iPad", "password");
//  WiFi.mode(WIFI_STA);
//  WiFi.begin("LPHS_BYOD", "kapukataumahaka"); 
//  while(WiFi.status() != WL_CONNECTED) {
//    Serial.print('.');
//    delay(500);
//  }
  
  if (MDNS.begin("info")){
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
  
  WebServer.onNotFound(defaultPage);
  WebServer.begin();
  
  MDNS.addService("http", "tcp", 80);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1)
      ;  // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  // nfc.setPassiveActivationRetries(0xFF);

  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);

  if (success) {
    memcpy(last_uid, uid, sizeof(uid[0])*7);
  }
  MDNS.update();
  WebServer.handleClient();
}
