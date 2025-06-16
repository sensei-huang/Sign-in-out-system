// Use 160MHz cpu

//Wifi+Webserver+mDNS libraries
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

//PN532 libraries
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

//PN532 defines+functions
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

//Webserver defines and functions
AsyncWebServer WebServer(80);
String pass_code = R"===(
<!DOCTYPE html>
<html>
<body>
<p>Loading...</p>
<script>
function setCookie(cname,cvalue,exdays) {
  const d = new Date();
  d.setTime(d.getTime() + (exdays*24*60*60*1000));
  let expires = "expires=" + d.toUTCString();
  document.cookie = cname + "=" + cvalue + ";" + expires + ";path=/";
}
pass = prompt("Please enter the password:","");
if (pass != "" && pass != null) {
  setCookie("pass", pass, 365);
}
window.location.pathname = "/";
</script>
</body>
</html>
)===";
String default_code = R"===(
<!DOCTYPE html>
<html>
<body>
<p>Loading...</p>
<script>
function getCookie(cname) {
  let name = cname + "=";
  let decodedCookie = decodeURIComponent(document.cookie);
  let ca = decodedCookie.split(';');
  for(let i = 0; i < ca.length; i++) {
    let c = ca[i];
    while (c.charAt(0) == ' ') {
      c = c.substring(1);
    }
    if (c.indexOf(name) == 0) {
      return c.substring(name.length, c.length);
    }
  }
  return "";
}
function setCookie(cname,cvalue,exdays) {
  const d = new Date();
  d.setTime(d.getTime() + (exdays*24*60*60*1000));
  let expires = "expires=" + d.toUTCString();
  document.cookie = cname + "=" + cvalue + ";" + expires + ";path=/";
}
if(getCookie("pass") == ""){
  pass = prompt("Please enter the password:","");
  if (pass != "" && pass != null) {
    setCookie("pass", pass, 365);
  }
}
var xhr = new XMLHttpRequest();
xhr.open("GET", "https://lphs.github.io/master", true);
xhr.onreadystatechange = function() {
  if(xhr.readyState == 4 && xhr.status == 200){
    document.querySelector('html').innerHTML = xhr.responseText;
    setTimeout(function(){
      var scripts = document.getElementsByTagName("script");
      var ii = scripts.length;
      for(var i = 0; i < ii; i++) {
        var script = document.createElement('script');
        script.innerHTML = scripts[i].innerHTML;
        document.body.appendChild(script);
      }
    }, 100);
  }
};
xhr.send(null);
</script>
</body>
</html>
)===";

void setup() {
  Serial.begin(115200);

  //Wifi init
  // WiFi.mode(WIFI_AP_STA);
  // WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  // WiFi.softAP("iPad", "password");
  WiFi.mode(WIFI_STA);
  WiFi.begin("LPHS_BYOD", "kapukataumahaka"); 
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  //mDNS init
  if(MDNS.begin("info")){
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
  MDNS.addService("http", "tcp", 80);

  //Webserver init
  WebServer.onNotFound([](AsyncWebServerRequest *request){
    request->send(200, "text/html", default_code);
  });
  WebServer.on("/address", HTTP_GET, [](AsyncWebServerRequest *request){
    char buffer[40];
    sprintf(buffer, "%02x:%02x:%02x:%02x", last_uid[0], last_uid[1], last_uid[2], last_uid[3]);
    request->send(200, "text/plain", buffer);
  });
  WebServer.on("/pass", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", pass_code);
  });
  WebServer.begin();
  
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
}

void loop() {
  readCard(50);
  MDNS.update();
}
