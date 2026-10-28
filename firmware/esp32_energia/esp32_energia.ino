#include <Wire.h>
#include <Adafruit_INA219.h>
#include <WiFi.h>
#include <PubSubClient.h>

Adafruit_INA219 ina219;

// ---- Wi-Fi ----
// ssid, password e mqtt_server ficam em secrets.h (nao versionado)
#include "secrets.h"

// ---- MQTT (broker próprio, rodando no seu PC) ----
const int   mqtt_port   = 1883;
const char* client_id   = "esp32_energia_01";

// ---- Tópicos ----
const char* TOPICO_TENSAO       = "energia/esp32_caio/telemetria/tensao";
const char* TOPICO_CORRENTE     = "energia/esp32_caio/telemetria/corrente";
const char* TOPICO_POTENCIA     = "energia/esp32_caio/telemetria/potencia";
const char* TOPICO_COMANDO      = "energia/esp32_caio/comando/carga";
const char* TOPICO_CONFIRMACAO  = "energia/esp32_caio/confirmacao/carga";
const char* TOPICO_ALERTA       = "energia/esp32_caio/alerta";
const char* TOPICO_STATUS       = "energia/esp32_caio/status";

// ---- Pinos ----
#define LED_PIN 26
#define POT_PIN 34   // mantido como entrada auxiliar, se quiser usar

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long ultimoEnvio = 0;
const long intervaloTelemetria = 2000;
String estadoAtual = "NORMAL";
bool cargaLigada = false;

// ---- Limites de exemplo — ajuste conforme a fonte/carga real ----
const float LIMITE_ATENCAO = 4.5;
const float LIMITE_CRITICO = 5.5;

void conectarWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado: " + WiFi.localIP().toString());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();

  Serial.print("Recebido em [");
  Serial.print(topic);
  Serial.print("]: [");
  Serial.print(msg);
  Serial.println("]");

  if (String(topic) == TOPICO_COMANDO) {
    if (msg == "ON") {
      cargaLigada = true;
      digitalWrite(LED_PIN, HIGH);
      client.publish(TOPICO_CONFIRMACAO, "ON");
    } else if (msg == "OFF") {
      cargaLigada = false;
      digitalWrite(LED_PIN, LOW);
      client.publish(TOPICO_CONFIRMACAO, "OFF");
    }
  }
}

void conectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect(client_id)) {
      Serial.println("conectado");
      client.subscribe(TOPICO_COMANDO);
      client.publish(TOPICO_STATUS, "online");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void verificarEstado(float tensao) {
  String novoEstado;
  if (tensao > LIMITE_CRITICO) novoEstado = "CRITICO";
  else if (tensao >= LIMITE_ATENCAO) novoEstado = "ATENCAO";
  else novoEstado = "NORMAL";

  if (novoEstado != estadoAtual) {
    estadoAtual = novoEstado;
    client.publish(TOPICO_ALERTA, estadoAtual.c_str());

    if (estadoAtual == "CRITICO" && cargaLigada) {
      cargaLigada = false;
      digitalWrite(LED_PIN, LOW);
      client.publish(TOPICO_CONFIRMACAO, "OFF");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  Wire.begin(21, 22);
  if (!ina219.begin()) {
    Serial.println("Falha ao encontrar o INA219 — confira a fiacao I2C (SDA=21, SCL=22)");
  }

  conectarWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) conectarWiFi();
  if (!client.connected()) conectarMQTT();
  client.loop();

  if (millis() - ultimoEnvio > intervaloTelemetria) {
    ultimoEnvio = millis();

    float tensao = ina219.getBusVoltage_V();
    float corrente = ina219.getCurrent_mA() / 1000.0;   // A
    float potencia = tensao * corrente;                  // W

    char bufV[8], bufI[8], bufP[8];
    dtostrf(tensao, 4, 2, bufV);
    dtostrf(corrente, 4, 3, bufI);
    dtostrf(potencia, 4, 3, bufP);

    client.publish(TOPICO_TENSAO, bufV);
    client.publish(TOPICO_CORRENTE, bufI);
    client.publish(TOPICO_POTENCIA, bufP);

    Serial.print("V="); Serial.print(bufV);
    Serial.print(" I="); Serial.print(bufI);
    Serial.print(" P="); Serial.println(bufP);

    verificarEstado(tensao);
  }
}
