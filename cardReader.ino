// Use 160MHz CPU

// WiFi/WebServer libraries
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>

//Time libraries
#include <TZ.h>
#include <time.h>
#include <sys/time.h>

//PN532 libraries
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

//PN532 init
#define PN532_IRQ (D3)
#define PN532_RESET (D4)
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
uint8_t last_uid[] = { 0, 0, 0, 0, 0, 0, 0 };
void readCard(int del){
  bool success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, del);
  if(success){
    memcpy(last_uid, uid, sizeof(uid[0])*7);
    Serial.println("Card Detected!");
  }
}

//WiFi init
#define ssid "iPad"
#define password "password"
#define dname "info"

//WebServer init
BearSSL::ESP8266WebServerSecure server(443);

//Certificates for https
static const char serverCert[] PROGMEM = R"=(
-----BEGIN CERTIFICATE-----
MIIC6zCCAlSgAwIBAgIUEo3K0OaRcHA08tZMWRZRAXELZs4wDQYJKoZIhvcNAQEL
BQAwfDELMAkGA1UEBhMCTloxDjAMBgNVBAgMBU90YWdvMRAwDgYDVQQHDAdEdW5l
ZGluMR8wHQYDVQQKDBZMb2dhbiBQYXJrIEhpZ2ggU2Nob29sMRUwEwYDVQQLDAxT
YW11ZWwgSHVhbmcxEzARBgNVBAMMCmluZm8ubG9jYWwwHhcNMjUwNjEwMDQ0OTMy
WhcNMzUwNjA4MDQ0OTMyWjB8MQswCQYDVQQGEwJOWjEOMAwGA1UECAwFT3RhZ28x
EDAOBgNVBAcMB0R1bmVkaW4xHzAdBgNVBAoMFkxvZ2FuIFBhcmsgSGlnaCBTY2hv
b2wxFTATBgNVBAsMDFNhbXVlbCBIdWFuZzETMBEGA1UEAwwKaW5mby5sb2NhbDCB
nzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAsVQ/MSYtvsvyIyfYassDzs9JyFM8
8tFcJmt0XV5YH6Wf0gelIRv9g3lIpvWcokLjksTVB8dmR4dhWMB6VLX7xRgRRtSD
F1F9mQVvpBOWduZj6FhiirBZSWAYRJiDzvd0AnwVy4Za7IvlLOi+ZzvukSf+/D5f
Szqwsfj9LyNVtJkCAwEAAaNqMGgwHQYDVR0OBBYEFCS5j+D2eaIhiPfOwVvdlmPx
aHuqMB8GA1UdIwQYMBaAFCS5j+D2eaIhiPfOwVvdlmPxaHuqMA8GA1UdEwEB/wQF
MAMBAf8wFQYDVR0RBA4wDIIKaW5mby5sb2NhbDANBgkqhkiG9w0BAQsFAAOBgQCi
xrRPXMUt+srGgeALJEynNmMLPuLRXHTVaQKJ2ixU1oa6qJp0MpmtS2lj0G2jPaty
amRUYP8OzEHjz44dT4tc2Nys4KCTCojItm5IqqFXYStZKLIhRlTjHVBPFQpscg08
1en3tnOFeac3Rb9m2J3dHEH7i32gUU4DZ0xSlQbxCA==
-----END CERTIFICATE-----
)=";
static const char serverKey[] PROGMEM =  R"=(
-----BEGIN PRIVATE KEY-----
MIICeAIBADANBgkqhkiG9w0BAQEFAASCAmIwggJeAgEAAoGBALFUPzEmLb7L8iMn
2GrLA87PSchTPPLRXCZrdF1eWB+ln9IHpSEb/YN5SKb1nKJC45LE1QfHZkeHYVjA
elS1+8UYEUbUgxdRfZkFb6QTlnbmY+hYYoqwWUlgGESYg873dAJ8FcuGWuyL5Szo
vmc77pEn/vw+X0s6sLH4/S8jVbSZAgMBAAECgYEApv7lu81uix3ZXA4sGvYxEdPY
chUhcrKDC5NqNHuilbsN9WdJtRJ8/hF7v+/RoGVJl3BSBk9cHMR7gm9RRPFUtUiB
Du6CH1t7bcGqlKgsYiqc7GwiMY28Mb8qq1UGvZJZ07CroTY97Z/p/rRE9LTFKdhE
I8QMmpdOKghFuxIZL3kCQQDZXjmgNN+TDDeo/Rg7/PCeEzDYHyNdzcPF4c5C6mLc
KM3iIUW+exSaqlygETJtH0a/pfUxFm4mxRuvdS0OaJc3AkEA0NhV0bjiuvBVhLr9
+x40w4t63NeCf/DOswYUPwJx0WF2gmodsaUtG4xVdqlenIvJAxsdqlsP2TUwq5Ju
LTtarwJAKO9bvtX2PMUSPlrbk4CA9+P5UNocWPAFLGDn7SYi0/9DNpC5h7O6H55x
rxtN+croEfN5B3OH7vkzy4kvoQUFZQJBAJEI/ChmVobuauV/muxvIMOANPXzcHx7
doCbQyoy/2oiXlPZb/+zryzHe36pxru+0/0lMqfhD5OPvdoZGgytgJ8CQQCFO5nm
2GZ5xmjyTeOTmxwmBdQv5VorrYH7qKbhlo1PF9jBOflKWVTA20txdu4bj7IcllM1
VnG5x5QK0w9ah3YJ
-----END PRIVATE KEY-----
)=";

//HTML for WebServer
String default_code = R"===(
<!DOCTYPE html>
<html>
<body>
<p>Last mac address: <span id="address">Loading...</span></p>
<script>
function readAdd(){
  xhr = new XMLHttpRequest();
    xhr.open("GET", "/address", true);
    xhr.onreadystatechange = function() {
      if(xhr.readyState == 4 && xhr.status == 200){
        document.getElementById("address").innerText = xhr.responseText;
        setTimeout(readAdd, 100);
      }
    };
    xhr.send(null);
}
readAdd();
</script>
</body>
</html>
)===";

//WebServer functions
void address() {
  char buffer[40];
  sprintf(buffer, "%02x:%02x:%02x:%02x", last_uid[0], last_uid[1], last_uid[2], last_uid[3]);
  server.send(200, "text/plain", buffer);
}

void defaultPage() {
  server.send(200, "text/html", default_code);
}

void setup() {
  Serial.begin(115200);
  
  //WiFi init
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
//  WiFi.mode(WIFI_STA);
//  WiFi.begin("LPHS_BYOD", "kapukataumahaka"); 
//  while(WiFi.status() != WL_CONNECTED) {
//    Serial.print('.');
//    delay(500);
//  }

  //Time init
  configTime(TZ_Pacific_Auckland, "pool.ntp.org");

  //HTTPS init
  server.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
  server.onNotFound(defaultPage);
  server.on("/address", address);
  server.enableCORS(true);
  server.begin();
  Serial.println("Server is ready");

  //mDNS init
  if (MDNS.begin(dname)){
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
  MDNS.addService("https", "tcp", 443);
  
  //PN532 init
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
  }
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);
  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop() {
  readCard(30);
  unsigned long before = millis()+200;
  while(millis() < before){
    MDNS.update();
    server.handleClient();
  }
}
