#include <HX711.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// Definições de pinos
#define pinDT 2  // Pino de dados do HX711 conectado a IO02
#define pinSCK 4 // Pino de clock do HX711 conectado a IO04

// Definições
#define pesoMin 0.010
#define pesoMax 30.0
#define escala 0.0f

HX711 scale;

const char* uuid = "83b37187-ecf6-45a7-95b3-860792b1ed91"
const char* ssid = "matheus";
const char* password = "12345678";
const char* apiHost = "https://fulla-api.vercel.app";  // Host da API
const int apiPort = 443;  // Porta padrão para conexões https
const char* weightPath = "/api/v1/device_weight";
const char* deviceIdPath = "/api/v1/device";

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
  WiFiClientSecure client;
  if (client.connect(apiHost, apiPort)) {
    Serial.println("Conected to the server");

    String payload = "POST " + weightPath + " HTTP/1.1\r\n" +
                     "Host: " + apiHost + "\r\n" +
                     "Connection: close\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + String(measure.length()) + "\r\n\r\n" +
                     "{\"device_uuid\":\"seu_uuid\":" +uuid, \"weight\":" + String(medida) + "}\r\n";

    client.print(payload);

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }

    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
  } else {
    Serial.println("Error while connecting to the server");
  }

  client.stop();
}

void cadastraDevice(String uuid){
  WiFiClientSecure client;
  if (client.connect(apiHost, apiPort)) {
    Serial.println("Conected to the server");
    
    String payload = "POST" + deviceIdPath +  " HTTP/1.1\r\n" +
                     "Host: " + apiHost + "\r\n" +
                     "Connection: close\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + String(measure.length()) + "\r\n\r\n" +
                     "{\"device_uuid\":\"seu_uuid\":" +uuid + "}\r\n";

    client.print(payload);

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }

    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
  } else {
    Serial.println("Error while connecting to the server");
  }

  client.stop();
  }
}
