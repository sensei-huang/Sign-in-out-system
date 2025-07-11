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
String address = "00:00:00:00";

//Webserver defines and functions
ESP8266WebServer WebServer(80);
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
  pass = prompt("Please enter the password:","");
  if (pass != "" && pass != null) {
    setCookie("pass", pass, 365);
  }
}
load();
</script>
</body>
</html>
)===";

void setup() {
  Serial.begin(115200);

  //Software Serial init
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);

  //Wifi init
  WiFi.mode(WIFI_STA);
  WiFi.begin("vodafoneDC98", "UC9T2JLKLP"); 
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
}

void loop() {
  MDNS.update();
  WebServer.handleClient();
  if(mySerial.available() > 0){
    while(mySerial.available() > 0){
      address = mySerial.readStringUntil('\n');
      address = address.substring(0, address.length()-1);
    }
  }
}
