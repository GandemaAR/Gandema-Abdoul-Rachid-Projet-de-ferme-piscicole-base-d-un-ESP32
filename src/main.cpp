#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <FS.h>
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
WebServer server(80);

void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "Fichier index.html non trouvé");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

void handleCss() {
  File file = SPIFFS.open("/style.css", "r");
  if (!file) {
    server.send(404, "text/plain", "Fichier style.css non trouvé");
    return;
  }
  server.streamFile(file, "text/css");
  file.close();
}

void handleJs() {
  File file = SPIFFS.open("/script.js", "r");
  if (!file) {
    server.send(404, "text/plain", "Fichier script.js non trouvé");
    return;
  }
  server.streamFile(file, "application/javascript");
  file.close();
}

void handleLedOn() {
  digitalWrite(ledPin, HIGH);
  server.send(200, "text/plain", "OK");
}

void handleLedOff() {
  digitalWrite(ledPin, LOW);
  server.send(200, "text/plain", "OK");
}

void handleApiStatus() {
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
  server.send(200, "application/json", json);
}

void handleNotFound() {
  server.send(404, "text/plain", "Page non trouvée");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  dht.begin();

  // Initialiser SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Échec de l'initialisation de SPIFFS");
    return;
  }

  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("Échec de la configuration AP");
  }
  WiFi.softAP(ssid, password);
  Serial.println("Point d'accès démarré");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.softAPIP());

  // Définir les routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/style.css", HTTP_GET, handleCss);
  server.on("/script.js", HTTP_GET, handleJs);
  server.on("/led/on", HTTP_GET, handleLedOn);
  server.on("/led/off", HTTP_GET, handleLedOff);
  server.on("/api/status", HTTP_GET, handleApiStatus);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Serveur web démarré");
}

void loop() {
  server.handleClient();
  delay(10);
}