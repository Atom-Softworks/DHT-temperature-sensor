#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include "DHT.h"

#define DHTTYPE DHT11 

const char* ssid="changeme";
const char* password = "changeme";
bool isOn = false;
float h = 0;
float t = 0;
float hi = 0;
AsyncWebServer server(80);

DHT dht(D7, DHTTYPE);

String sensorData() {  
  return "{\"temp\":" + String(t) + ",\"humidity\":" + String(h) + ",\"htemp\":" + String(hi) + "}";
}

void setup() {
  pinMode(D4, OUTPUT);
    
  Serial.begin(115200);
  Serial.println();
  Serial.print("Wifi connecting to ");
  Serial.println( ssid );

  WiFi.begin(ssid,password);

  Serial.println();
  Serial.print("Connecting");

  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      Serial.print(".");        
  }

  Serial.println();

  Serial.println("Wifi Connected Success!");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP() );

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "application/json", sensorData().c_str());
  });

  server.on("/echo", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "application/json", "Hello from board!");
  });
  
  server.begin();
  dht.begin();
}

// the loop function runs over and over again forever
void loop() { 
    isOn = !isOn;
    digitalWrite(D4, isOn ? LOW : HIGH);

  h = dht.readHumidity();
  t = dht.readTemperature();

  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  hi = (5.0 / 9.0) * (dht.computeHeatIndex(f, h) - 32.0);      
}
