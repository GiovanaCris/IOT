//código Giovana Cristina

#include <WiFi.h>
#include <WiFiClientSecure.h>

// === Rede Wi-Fi ===
const char* ssid     = "WIFI_EDUC_CFP501";
const char* password = "SENAICAMPINAS501";

// === Google Apps Script ===
const char* host = "script.google.com";
const int httpsPort = 443;
const char* GScriptId = "AKfycbyLI_qG4klwnaDV26NhvTkLxJRYOcGHNbVshWO8jmiADuf1jV9VamgV_0U80pkVCQ1B";
String url = String("/macros/s/") + GScriptId + "/exec";

// === Pinos ===
const int ledPin = 14;
const int buttonPin = 15;

// === Cliente HTTPS ===
WiFiClientSecure client;

// === Estado ===
bool ledState = false;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Conecta Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando-se à rede ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  client.setInsecure(); // Ignora certificado SSL
}

void sendToGoogleSheets(String status) {
  String payload = "{\"command\": \"append_row\", \"sheet_name\": \"Sheet1\", \"values\": \"" + status + "\"}";

  Serial.println("Enviando: " + payload);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Falha na conexão HTTPS");
    return;
  }

  String request = String("POST ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Content-Type: application/json\r\n" +
                   "Content-Length: " + payload.length() + "\r\n\r\n" +
                   payload;

  client.print(request);

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  String response = client.readString();
  Serial.println("Resposta: " + response);

  client.stop();
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) { // Botão pressionado
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);

    sendToGoogleSheets(ledState ? "Ativado" : "Desativado");
    Serial.println(ledState ? "LED Ativado" : "LED Desativado");

    delay(500); // debounce
  }
}