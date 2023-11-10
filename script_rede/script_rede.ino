#include <HX711.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Definições de pinos
#define pinDT 2  // Pino de dados do HX711 conectado a IO02
#define pinSCK 4 // Pino de clock do HX711 conectado a IO04

// Definições
#define pesoMin 0.010
#define pesoMax 30.0
#define escala 0.0f

HX711 scale;

const String uuid = "83b37187-ecf6-45a7-95b3-860792b1ed91";
const String ssid = "matheus";
const String password = "12345678";
const String apiHost = "https://fulla-api.vercel.app";  // Host da API
const int apiPort = 443;  // Porta padrão para conexões https
const String weightPath = "/api/v1/device_weight";
const String deviceIdPath = "/api/v1/device";


float medidaAnterior = 0;

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  cadastraDevice(uuid);
  scale.begin(pinDT, pinSCK);
  scale.set_scale(escala);

  delay(2000);
  scale.tare();
  Serial.println("Setup Done!");
}

void loop() {
  float medida = scale.get_units(5);

  if (abs(medida - medidaAnterior) > 0.1) {
    // Se a diferença entre a medida atual e a anterior for maior que 0.1, envie para a API
    enviarMedidaParaAPI(medida);
    medidaAnterior = medida;
  }

  delay(1000);
}

void enviarMedidaParaAPI(float medida) {
  HTTPClient http;
  if (http.begin(apiHost, apiPort, weightPath)) {
    Serial.println("Connected to the server");

    String payload = "{\"device_uuid\":\"" + String(uuid) + "\",\"weight\":" + String(medida) + "}";
    int httpCode = http.POST(payload);

    if (httpCode > 0) {
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.println("Error sending data to the server");
    }

    http.end();
  } else {
    Serial.println("Error connecting to the server");
  }
}

void cadastraDevice(String uuid) {
  HTTPClient http;
  if (http.begin(apiHost, apiPort, deviceIdPath)) {
    Serial.println("Connected to the server");

    String payload = "{\"device_uuid\":\"" + String(uuid) + "\"}";
    int httpCode = http.POST(payload);

    if (httpCode > 0) {
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.println("Error sending data to the server");
    }

    http.end();
  } else {
    Serial.println("Error connecting to the server");
  }
}
