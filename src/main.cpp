#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

// Identifiants WiFi (Station)
const char* ssidStation = "VOTRE_SSID";
const char* motDePasseStation = "VOTRE_MOT_DE_PASSE";

// Identifiants Point d'Accès
const char* ssidAP = "ESP32_AP";
const char* motDePasseAP = "12345678";

// Paramètres IP statiques pour le mode AP
IPAddress localIP(192, 168, 4, 1);      // Adresse IP fixe pour l'AP
IPAddress gateway(192, 168, 4, 1);      // Passerelle (même que l'IP)
IPAddress subnet(255, 255, 255, 0);     // Masque de sous-réseau

// Création du serveur web asynchrone
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Broches
const int pinCapteur = 5;   // GPIO5 pour capteur (simulé)
const int pinBouton = 19;   // GPIO19 pour bouton poussoir
const int pinLED = 18;      // GPIO18 pour LED

// États
bool etatLED = false;
bool etatCapteur = false;
bool etatBouton = false;
bool modeAutomatique = true;
unsigned long dernierControleManuel = 0;
const unsigned long delaiRetourAuto = 10000; // 10s avant retour auto

// Envoyer les données aux clients WebSocket
void envoyerDonnees() {
  DynamicJsonDocument doc(200);
  doc["type"] = "etat";
  doc["led"] = etatLED;
  doc["capteur"] = etatCapteur;
  doc["mode"] = modeAutomatique ? "automatique" : "manuel";
  String json;
  serializeJson(doc, json);
  ws.textAll(json);
}

// Gestion des messages WebSocket
void gererMessageWebSocket(void *arg, uint8_t *data, size_t len) {
  String message = "";
  for (size_t i = 0; i < len; i++) {
    message += (char)data[i];
  }

  DynamicJsonDocument doc(200);
  deserializeJson(doc, message);
  if (doc["type"] == "commande") {
    etatLED = doc["led"];
    modeAutomatique = false;
    dernierControleManuel = millis();
    digitalWrite(pinLED, etatLED ? HIGH : LOW);
    envoyerDonnees();
  } else if (doc["type"] == "mode") {
    modeAutomatique = doc["mode"] == "automatique";
    envoyerDonnees();
  }
}

// Gestion des événements WebSocket
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("Client WebSocket connecté");
    envoyerDonnees();
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Client WebSocket déconnecté");
  } else if (type == WS_EVT_DATA) {
    gererMessageWebSocket(arg, data, len);
  }
}

void setup() {
  Serial.begin(115200);

  // Initialisation des broches
  pinMode(pinCapteur, INPUT_PULLUP);
  pinMode(pinBouton, INPUT_PULLUP);
  pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, LOW);

  // Initialisation de SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Erreur lors du montage de SPIFFS");
    return;
  }

  // Configuration WiFi en mode hybride
  WiFi.mode(WIFI_AP_STA);

  // Configurer le point d'accès avec IP statique
  if (!WiFi.softAPConfig(localIP, gateway, subnet)) {
    Serial.println("Échec de la configuration IP statique de l'AP");
    return;
  }
  WiFi.softAP(ssidAP, motDePasseAP);
  Serial.println("Point d'accès démarré");
  Serial.print("IP AP: ");
  Serial.println(WiFi.softAPIP());

  // Connexion au WiFi (Station)
  WiFi.begin(ssidStation, motDePasseStation);
  Serial.println("Connexion au WiFi en mode Station...");
  // Ne pas bloquer l'exécution, tenter la connexion en arrière-plan
  // La connexion Station est gérée dans loop() pour vérifier l'état

  // Configuration du WebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Servir les fichiers statiques
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  // Démarrer le serveur
  server.begin();
}

void loop() {
  static unsigned long dernierTemps = 0;
  static unsigned long dernierEssaiWifi = 0;
  unsigned long maintenant = millis();

  // Vérifier l'état de la connexion WiFi (Station) toutes les 10 secondes
  if (maintenant - dernierEssaiWifi > 10000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Tentative de connexion WiFi...");
      WiFi.reconnect(); // Tenter de se reconnecter si déconnecté
    } else {
      Serial.print("Connecté au WiFi, IP Station: ");
      Serial.println(WiFi.localIP());
    }
    dernierEssaiWifi = maintenant;
  }

  // Vérifier capteur et bouton toutes les 100ms
  if (maintenant - dernierTemps > 100) {
    // Lire le capteur
    bool nouvelEtatCapteur = digitalRead(pinCapteur) == LOW;
    if (nouvelEtatCapteur != etatCapteur) {
      etatCapteur = nouvelEtatCapteur;
      envoyerDonnees();
    }

    // Contrôle automatique
    if (modeAutomatique && etatCapteur != etatLED) {
      etatLED = etatCapteur;
      digitalWrite(pinLED, etatLED ? HIGH : LOW);
      envoyerDonnees();
    }

    // Vérifier le bouton poussoir
    bool nouvelEtatBouton = digitalRead(pinBouton) == LOW;
    if (nouvelEtatBouton && !etatBouton) {
      etatLED = !etatLED;
      modeAutomatique = false;
      dernierControleManuel = millis();
      digitalWrite(pinLED, etatLED ? HIGH : LOW);
      envoyerDonnees();
    }
    etatBouton = nouvelEtatBouton;

    // Revenir au mode automatique
    if (!modeAutomatique && maintenant - dernierControleManuel > delaiRetourAuto) {
      modeAutomatique = true;
      envoyerDonnees();
    }

    dernierTemps = maintenant;
  }

  // Nettoyage des clients WebSocket
  ws.cleanupClients();
}