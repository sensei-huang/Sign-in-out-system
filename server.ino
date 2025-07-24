// Use 160MHz cpu

//Software Serial library
#include <SoftwareSerial.h>

//Wifi+Webserver+mDNS libraries
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

//Software Serial defines
#define rxPin D6
#define txPin D7
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
String address = "none";

//Webserver defines and functions
ESP8266WebServer WebServer(80);
String pass_code = R"===(
<!DOCTYPE html>
<html>
<body>
<p>Loading...</p>
<script>
async function load(){
  try {
    let response = await fetch("https://lphs.github.io/pass");
    document.querySelector('html').innerHTML = await response.text();
    setTimeout(function(){
      var scripts = document.getElementsByTagName("script");
      var ii = scripts.length;
      for(var i = 0; i < ii; i++) {
        var script = document.createElement('script');
        script.innerHTML = scripts[i].innerHTML;
        document.body.appendChild(script);
      }
    }, 50);
  } catch (err) {
    console.log('Fetch error:' + err); // Error handling
  }
}
load();
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
async function load(){
  try {
    let response = await fetch("https://lphs.github.io/master");
    document.querySelector('html').innerHTML = await response.text();
    setTimeout(function(){
      var scripts = document.getElementsByTagName("script");
      var ii = scripts.length;
      for(var i = 0; i < ii; i++) {
        var script = document.createElement('script');
        script.innerHTML = scripts[i].innerHTML;
        document.body.appendChild(script);
      }
    }, 50);
  } catch (err) {
    console.log('Fetch error:' + err); // Error handling
  }
}
if(getCookie("pass") == ""){
  window.location.pathname = "/pass";
}
load();
</script>
</body>
</html>
)===";

void setup() {
  Serial.begin(115200);

  //LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  //Software Serial init
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);

  //Wifi init
  WiFi.mode(WIFI_STA);
  WiFi.begin("LPHS_BYOD"); 
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
  WebServer.onNotFound([](){
    WebServer.send(200, "text/html", default_code);
  });
  WebServer.on("/address", [](){
    WebServer.send(200, "text/plain", address);
  });
  WebServer.on("/pass", [](){
    WebServer.send(200, "text/html", pass_code);
  });
  WebServer.begin();

  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  unsigned long last = millis();
  while(millis()-last < 50){
    MDNS.update();
    WebServer.handleClient();
  }
  if(mySerial.available() > 0){
    while(mySerial.available() > 0){
      address = mySerial.readStringUntil('\n');
      address = address.substring(0, address.length()-1);
    }
  }
}
