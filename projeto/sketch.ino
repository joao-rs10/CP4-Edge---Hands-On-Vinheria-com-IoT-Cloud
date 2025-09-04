#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// --- CONFIGURAÇÕES DO DHT22 ---
#define DHTPIN 4        // Pino onde o DHT22 está conectado
#define DHTTYPE DHT22   // Definindo o modelo do sensor
DHT dht(DHTPIN, DHTTYPE);

// --- CONFIGURAÇÕES DO WIFI ---
const char* ssid = "FIAP-IOT";       // Coloque aqui o nome da sua rede Wi-Fi
const char* password = "F!@p25.IOT";  // Coloque aqui a senha da sua rede

// --- CONFIGURAÇÕES DO THINGSPEAK ---
String server = "http://api.thingspeak.com/update?";
String apiKey = "CEJB3M51WPKHRHAN"; // Substitua pela sua chave de escrita do ThingSpeak

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Conexão com Wi-Fi
  Serial.print("Conectando ao WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();

    if (isnan(temperatura) || isnan(umidade)) {
      Serial.println("Falha ao ler o DHT22!");
      delay(2000);
      return;
    }

    // Mostrando no Serial Monitor
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C  |  Umidade: ");
    Serial.print(umidade);
    Serial.println(" %");

    // Montando URL para envio
    String url = server + "api_key=" + apiKey + "&field1=" + String(temperatura) + "&field2=" + String(umidade);

    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("Dados enviados com sucesso para ThingSpeak!");
      Serial.println("Resposta: " + http.getString());
    } else {
      Serial.println("Erro no envio HTTP!");
    }

    http.end();
  } else {
    Serial.println("WiFi desconectado, tentando reconectar...");
    WiFi.begin(ssid, password);
  }

  delay(20000); // ThingSpeak aceita mínimo de 15 segundos entre envios
}