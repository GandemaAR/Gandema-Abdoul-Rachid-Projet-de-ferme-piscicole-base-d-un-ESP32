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
                    ".button { padding: 12px 24px; margin: 10px; background-color: #007bff; color: white; text-decoration: none; border-radius: 5px; cursor: pointer; }"
                    ".button:hover { background-color: #0056b3; }"
                    "#status { font-size: 18px; color: #444; }"
                    "</style>";

String getHtml() {
  String html = "<!DOCTYPE html><html><head>"
                "<meta charset=\"UTF-8\">"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                "<title>ESP32 Web Server</title>"
                + String(style) +
                "<script>"
                // Script AJAX pour mises à jour en temps réel
                "function updateStatus() {"
                "  var xhr = new XMLHttpRequest();"
                "  xhr.open('GET', '/sensor', true);"
                "  xhr.onreadystatechange = function() {"
                "    if (xhr.readyState == 4 && xhr.status == 200) {"
                "      var data = JSON.parse(xhr.responseText);"
                "      document.getElementById('status').innerHTML = "
                "        'LED : ' + data.led + '<br>' + "
                "        'Température : ' + data.temperature + ' °C<br>' + "
                "        'Humidité : ' + data.humidity + ' %';"
                "    }"
                "  };"
                "  xhr.send();"
                "}"
                "function sendCommand(cmd) {"
                "  var xhr = new XMLHttpRequest();"
                "  xhr.open('GET', '/led/' + cmd, true);"
                "  xhr.onreadystatechange = function() {"
                "    if (xhr.readyState == 4 && xhr.status == 200) { updateStatus(); }"
                "  };"
                "  xhr.send();"
                "}"
                "setInterval(updateStatus, 5000);" // Mise à jour toutes les 5 secondes
                "window.onload = updateStatus;"
                "</script>"
                "</head><body>"
                "<h1>Contrôle de la LED</h1>"
                "<p id=\"status\">Chargement...</p>"
                "<p><a href=\"javascript:sendCommand('on')\" class=\"button\">Allumer</a></p>"
                "<p><a href=\"javascript:sendCommand('off')\" class=\"button\">Éteindre</a></p>"
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