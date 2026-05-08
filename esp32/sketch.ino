#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// MQTT
const char* mqtt_server = "broker.hivemq.com";
const char* TOPICO_TEMP = "pet/temperatura";
const char* TOPICO_ACT = "pet/atividade";

WiFiClient espClient;
PubSubClient client(espClient);

// Controle de tempo
unsigned long lastRead = 0;
const long interval = 3000;

void setup_wifi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Conectado!");
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32-Pet-" + String(random(0xffff), HEX);
   
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) setup_wifi();
  if (!client.connected()) reconnect();
  
  client.loop();

  if (millis() - lastRead >= interval) {
    lastRead = millis();

    // Simulação de dados
    float temperatura = random(3000, 4501) / 100.0;
    float atividade = random(0, 501); 

    char tempString[8];
    dtostrf(temperatura, 1, 2, tempString);

    char actString[8];
    dtostrf(atividade, 1, 2, actString);

    client.publish(TOPICO_TEMP, tempString);
    client.publish(TOPICO_ACT, actString);
     
    Serial.println("--- Dados Enviados ---");
    Serial.printf("Temp: %s C | Atividade: %s\n", tempString, actString);
  }
}
