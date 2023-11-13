#include <HX711.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Definições de pinos
#define pinDT 14  // Pino de dados do HX711 conectado a IO02
#define pinSCK 27 // Pino de clock do HX711 conectado a IO04

// Definições
#define pesoMin 0.010
#define pesoMax 30.0
#define escala 0.0f

HX711 scale;

const String uuid = "83b37187-ecf6-45a7-95b3-860792b1ed96";
const String ssid = "matheus";
const String password = "12345678";
const String apiHost = "http://fulla-api.vercel.app/";  // Host da API
const int apiPort = 443;  // Porta padrão para conexões https
const String weightPath = "api/v1/device_weight"; //enviar peso 
const String deviceIdPath = "api/v1/device"; //enviar device id



float medidaAnterior = 0;

void setup() {
  Serial.begin(57600);

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
  float medida = 15.5;
  enviarMedidaParaAPI(medida);
}

void loop() {
  scale.power_up(); // LIGANDO O SENSOR
  float medida = scale.get_units(5);
  Serial.println(medida,3);
  scale.power_down(); // DESLIGANDO O SENSOR

  //if (abs(medida - medidaAnterior) > 0.1) {
    // Se a diferença entre a medida atual e a anterior for maior que 0.1, envie para a API
    //enviarMedidaParaAPI(medida);
    //Serial.println("--------------------------------------------------------------------------");
    //Serial.print("--------------------------------- ");
    //Serial.print(medida);
    //Serial.print(" ---------------------------------");
    //Serial.println("--------------------------------------------------------------------------");
   // medidaAnterior = medida;
  //}

  delay(1000);
}

void enviarMedidaParaAPI(float medida) {
  DynamicJsonDocument payload(1024);
  payload["device_uuid"] = uuid;
  payload["weight"] = medida;
  HTTPClient http;
  http.begin("https://fulla-api.vercel.app/api/v1/device_weight/"); 
  http.addHeader("Content-Type", "application/json");
  String payloadHtppFormat;
  serializeJson(payload, payloadHtppFormat);
  Serial.println(payloadHtppFormat);
  http.POST(payloadHtppFormat);
  Serial.println(http.getString());
  http.end();
}

void cadastraDevice(String uuid) {
  DynamicJsonDocument payload(1024);
  payload["device_uuid"] = uuid;
  HTTPClient http;
  http.begin("https://fulla-api.vercel.app/api/v1/device/");
  http.addHeader("Content-Type", "application/json");
  String payloadHtppFormat;
  serializeJson(payload, payloadHtppFormat);
  Serial.println(payloadHtppFormat);
  http.POST(payloadHtppFormat);
  Serial.println(http.getString());
  http.end();
}
