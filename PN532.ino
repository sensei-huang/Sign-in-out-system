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

String default_code = R"===(
<!DOCTYPE html>
<html>
<body>
<p>Last mac address: <span id="address">Loading...</span></p>
<script>
setInterval(function(){
  xhr = new XMLHttpRequest();
    xhr.open("GET", "/address", true);
    xhr.onreadystatechange = function() {
      if(xhr.readyState == 4 && xhr.status == 200){
        document.getElementById("address").innerText = xhr.responseText;
      }
    };
    xhr.send(null);
}, 400);
</script>
</body>
</html>
)===";

uint8_t last_uid[] = { 0, 0, 0, 0, 0, 0, 0 };

void address() {
  char buffer[40];
  sprintf(buffer, "%02x:%02x:%02x:%02x", last_uid[0], last_uid[1], last_uid[2], last_uid[3]);
  WebServer.send(200, "text/plain", buffer);
}

void defaultPage() {
  WebServer.send(200, "text/html", default_code);
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
  WebServer.on("/address", address);
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

bool wasSuccess = false;

void loop() {
  bool success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 30);
  if(success){
    wasSuccess = true;
    memcpy(last_uid, uid, sizeof(uid[0])*7);
    Serial.println("Card Detected!");
  }else if(wasSuccess){
    wasSuccess = false;
    Serial.println("Card Removed!");
  }else{
    Serial.println("Card Not Detected!");
  }
  unsigned long before = millis()+60;
  while(millis() < before){
    MDNS.update();
    WebServer.handleClient();
  }
}
