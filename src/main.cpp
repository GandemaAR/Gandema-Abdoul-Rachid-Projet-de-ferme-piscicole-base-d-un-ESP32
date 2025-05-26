#include <Arduino.h>
#include <WiFi.h>                       // Include the WiFi library
#include <WebServer.h>
#include <DHT.h>

//GPIO
#define ledPin 2
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

const char* ssid = "ESP32-AP ";          // SSID of the access point
const char* password = "123456789";     // Password of the access point

// Static IP configuration
IPAddress local_ip  (192,168,4,1);
IPAddress getway    (192,168,4,1);
IPAddress subnet    (255,255,255,0);

// Create an instance of the WebServer class
WebServer server(80);

//Page web 
const char* style = "<style>"
                    "body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; padding: 20px; }"
                    "h1 { color: #333; }"
                    ".button { padding: 12px 24px; margin: 10px; background-color: #007bff; color: white; text-decoration: none; border-radius: 5px; }"
                    ".button:hover { background-color: #0056b3; }"
                    "</style>";
String getHtml() {
  String html = "<!DOCTYPE html><html><head>"
                "<meta charset=\"UTF-8\">"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                "<title>ESP32 Web Server</title>"
                + String(style) +
                "</head><body>"
                "<h1>Contrôle de la LED</h1>"
                "<p>État de la LED : " + String(digitalRead(ledPin) ? "ALLUMÉE" : "ÉTEINTE") + "</p>"
                "<p><a href=\"/led/on\" class=\"button\">Allumer</a></p>"
                "<p><a href=\"/led/off\" class=\"button\">Éteindre</a></p>"
                "<h2>Données du capteur</h2>" // Nouveau lien
                "<p><a href=\"/sensor\" class=\"button\">Voir température et humidité</a></p>"
                "</body></html>";
  return html;
}

//Gestionnaire associer aux routes
void page_principal();
void allumeled();
void eteindreled();
void capteur();


void setup() {

Serial.begin(115200);
Serial.println("Starting...");

//Configuration des IO
pinMode(ledPin,OUTPUT);

// Set up the access point
WiFi.softAP(ssid,password);
WiFi.softAPConfig(local_ip,getway,subnet);

//Racine web page
server.on("/",HTTP_GET, page_principal);
server.on("/led/on",HTTP_GET, allumeled);
server.on("/led/off",HTTP_GET, eteindreled);
server.on("/sensor",HTTP_GET, capteur);


//Set up the server
server.begin();
Serial.println("Server started");
}

void loop() {
    server.handleClient();  
}

void page_principal(){
server.send(200,"text/html",getHtml());
}
void allumeled(){
    digitalWrite(ledPin,1);
    server.send(200,"text/html",getHtml());
}
void eteindreled(){
    digitalWrite(ledPin,0);
    server.send(200,"text/html",getHtml());
}

void capteur() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  String response = "<!DOCTYPE html><html><head>"
                   "<meta charset=\"UTF-8\">"
                   "<title>Données du capteur</title>"
                   + String(style) +
                   "</head><body>"
                   "<h1>Données du capteur DHT11</h1>";
  if (isnan(temperature) || isnan(humidity)) {
    response += "<p>Erreur de lecture du capteur !</p>";
  } else {
    response += "<p>Température : " + String(temperature) + " °C</p>";
    response += "<p>Humidité : " + String(humidity) + " %</p>";
  }
  response += "<p><a href=\"/\" class=\"button\">Retour</a></p>"
             "</body></html>";
  server.send(200, "text/html", response);
}