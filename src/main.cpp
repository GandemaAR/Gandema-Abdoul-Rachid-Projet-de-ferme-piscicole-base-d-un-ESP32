#include <Arduino.h>
#include <WiFi.h>                       // Include the WiFi library
#include <WebServer.h>

const char* ssid = "ESP32-AP";          // SSID of the access point
const char* password = "123456789";     // Password of the access point

// Static IP configuration
IPAddress local_ip  (192,168,4,1);
IPAddress getway    (192,168,4,1);
IPAddress subnet    (255,255,255,0);

// Create an instance of the WebServer class
WebServer server(80);

//Page web 
String page_web = "<!DOCTYPE html>"
                    "<html>"
                        "<head>"
                            "<title> ESP32 </title>"
                            "<meta charset='utf-8'>"
                        "</head>"
                        "<body>"
                        "<h1>Bienvenu sur notre site IOT</h1>"
                        "<a href ='/led/on'>allumé la led</a>"
                        "<br>"
                        "<a href ='/led/off'>éteindre la led</a>"
                        "</body>"
                    "</html>";
String page_web1 = "<!DOCTYPE html>"
                    "<html>"
                        "<head>"
                            "<title> ESP32 </title>"
                            "<meta charset='utf-8'>"
                        "</head>"
                        "<body>"
                        "<h1>LED allumée</h1>"
                        "</body>"
                    "</html>";
String page_web2 = "<!DOCTYPE html>"
                    "<html>"
                        "<head>"
                            "<title> ESP32 </title>"
                            "<meta charset='utf-8'>"
                        "</head>"
                        "<body>"
                        "<h1>LED éteinte</h1>"
                        "</body>"
                    "</html>";


//Gestionnaire associer aux routes
void page_principal();
void allumeled();
void eteindreled();


void setup() {

Serial.begin(115200);
Serial.println("Starting...");

// Set up the access point
WiFi.softAP(ssid,password);
WiFi.softAPConfig(local_ip,getway,subnet);

//Racine web page
server.on("/",HTTP_GET, page_principal);
server.on("/led/on",HTTP_GET, allumeled);
server.on("/led/off",HTTP_GET, eteindreled);


//Set up the server
server.begin();
Serial.println("Server started");
}

void loop() {
    server.handleClient();
}

void page_principal(){
server.send(200,"text/html",page_web);
}
void allumeled(){
    server.send(200,"text/html",page_web1);
}
void eteindreled(){
    server.send(200,"text/html",page_web2);
}