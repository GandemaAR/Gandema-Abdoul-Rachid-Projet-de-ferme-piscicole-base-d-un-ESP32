#include <Arduino.h>
#include <DHT.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>


const char* ssid = "ESP32-AP";
const char* password = "12345678";
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
const int ledPin = 2;
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
AsyncWebServer server(80);

void setup()
{
  //----------------------------------------------------Serial
  Serial.begin(115200);
  Serial.println("\n");

  //----------------------------------------------------GPIO
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  //----------------------------------------------------SPIFFS
  if(!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while(file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }

  //----------------------------------------------------WIFI
  WiFi.softAPConfig(local_IP, gateway, subnet);
  Serial.println("Configuration du point d'accès WiFi...");
  WiFi.softAP(ssid, password);
	
	

	
	Serial.println("\n");
	Serial.println("Connexion etablie!");
	Serial.print("Adresse IP: ");
	Serial.println(WiFi.localIP());

  //----------------------------------------------------SERVER
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });
  
  server.on("/led/on", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(ledPin, HIGH);
    request->send(200);
  });

  server.on("/led/off", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(ledPin, LOW);
    request->send(200);
  });

  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    String json = "{";
    json += "\"led\": \"" + String(digitalRead(ledPin) ? "ALLUMÉE" : "ÉTEINTE") + "\",";
    if (isnan(temperature) || isnan(humidity)) {
      json += "\"temperature\": \"Erreur\",";
      json += "\"humidity\": \"Erreur\"";
    } else {
      json += "\"temperature\": " + String(temperature) + ",";
      json += "\"humidity\": " + String(humidity);
    }
    json += "}";
      request->send(200, "application/json", json);
  });


  server.begin();
  Serial.println("Serveur actif!");
}

void loop()
{

}