#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"

WebServer server(80);

void setup() {
  Serial.begin(115200);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  File configFile = SPIFFS.open("/config.json");

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, configFile);

  const char* ssid = doc["wifi_client"]["ssid"];
  const char* password = doc["wifi_client"]["password"];

  //its not work
  const char* www_username = doc["web_server"]["www_username"];
  const char* www_password = doc["web_server"]["www_password"];
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }
  ArduinoOTA.begin();

  server.on("/", [www_username, www_password]() {
    if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
    }
    server.send(200, "text/plain", "Login OK\n");
  });
  server.begin();

  Serial.print("Open http://");
  Serial.print(WiFi.localIP());
  Serial.println("/ in your browser to see it working");
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
}
