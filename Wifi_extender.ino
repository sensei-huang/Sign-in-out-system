
// NAPT example released to public domain

#define HAVE_NETDUMP 0
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <lwip/napt.h>
#include <lwip/dns.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#define NAPT 1000
#define NAPT_PORT 10

ESP8266WebServer WebServer(80);

ESP8266WiFiMulti WiFiMulti;

struct wifis {
  char *ssid;
  char *pass;
};

wifis wifilist[] = {
  {"LPHS_BYOD", "kapukataumahaka"}
};

void addWifi(){
  for(int i = 0; i < sizeof(wifilist)/sizeof(wifilist[0]); i++){
    WiFiMulti.addAP(wifilist[i].ssid, wifilist[i].pass);
  }
}

void defaultPage() {
  int j = 0;
  for(int i = 0; i < sizeof(wifilist)/sizeof(wifilist[0]); i++){
    if((String)wifilist[i].ssid == (String)WiFi.SSID()){
      j = i;
    }
  }
  WebServer.send(200, "text/plain", "You are connected to '"+(String)WiFi.SSID()+"' and the password is '"+(String)wifilist[j].pass+"'");
}

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nNAPT Range extender\n");
  Serial.printf("Heap on start: %d\n", ESP.getFreeHeap());

  // first, connect to STA so we can get a proper local DNS server
  WiFi.mode(WIFI_STA);
  addWifi();
  while(WiFiMulti.run(10000) != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.printf("\nSTA: %s (dns: %s / %s)\n", WiFi.localIP().toString().c_str(), WiFi.dnsIP(0).toString().c_str(), WiFi.dnsIP(1).toString().c_str());

  // By default, DNS option will point to the interface IP
  // Instead, point it to the real DNS server.
  // Notice that:
  // - DhcpServer class only supports IPv4
  // - Only a single IP can be set
  auto& server = WiFi.softAPDhcpServer();
  server.setDns(WiFi.dnsIP(0));

  WiFi.softAPConfig(  // enable AP, with android-compatible google domain
    IPAddress(172, 217, 28, 254), IPAddress(172, 217, 28, 254), IPAddress(255, 255, 255, 0));
  WiFi.softAP("iPad", "password");
  Serial.printf("AP: %s\n", WiFi.softAPIP().toString().c_str());

  Serial.printf("Heap before: %d\n", ESP.getFreeHeap());
  err_t ret = ip_napt_init(NAPT, NAPT_PORT);
  Serial.printf("ip_napt_init(%d,%d): ret=%d (OK=%d)\n", NAPT, NAPT_PORT, (int)ret, (int)ERR_OK);
  if (ret == ERR_OK) {
    ret = ip_napt_enable_no(SOFTAP_IF, 1);
    Serial.printf("ip_napt_enable_no(SOFTAP_IF): ret=%d (OK=%d)\n", (int)ret, (int)ERR_OK);
    if (ret == ERR_OK) {
      Serial.printf("WiFi Network '%s' with same password is now NATed behind '%s'\n", "Extension", WiFi.SSID()); 
    }
  }
  Serial.printf("Heap after napt init: %d\n", ESP.getFreeHeap());
  if (ret != ERR_OK) {
    Serial.printf("NAPT initialization failed\n");
  }
  if (MDNS.begin("pass")){
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
  WebServer.onNotFound(defaultPage);
  WebServer.begin();
  MDNS.addService("http", "tcp", 80);
}

void loop() {
  MDNS.update();
  WebServer.handleClient();
}
