//Giovana Cristina

// Inclui as bibliotecas necessárias para conectar o ESP32 ao Wi-Fi e enviar dados via HTTPS
#include <WiFi.h>
#include <WiFiClientSecure.h>

// === CONFIGURAÇÃO DE REDE ===
// Nome da rede Wi-Fi
const char* ssid     = "WIFI_EDUC_CFP501";
// Senha da rede Wi-Fi
const char* password = "SENAICAMPINAS501";

// === CONFIGURAÇÃO DO GOOGLE APPS SCRIPT ===
// ID único do seu script Google Apps (que processará os dados e enviará para a planilha)
const char *GScriptId = "AKfycbwi1q4OUH6G_IMfgWCYhMvuV3pGRUoxYNa_0os96aYJFdHI-iNYZyjYJ1WbMcfXzD-y";

// Texto base que será enviado (formato JSON) para indicar que vamos inserir uma linha na planilha
String payload_base = "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
String payload = ""; // Variável que vai guardar o texto final antes do envio

// Endereço do servidor (Google Scripts) e porta segura (HTTPS usa porta 443)
const char* host = "script.google.com";
const int httpsPort = 443;
// Caminho final da URL para acessar seu script
String url = String("/macros/s/") + GScriptId + "/exec";

// === VARIÁVEIS QUE SERÃO ENVIADAS ===
int value0 = 0; // Contador que começa em 0
int value1 = 0; // Número aleatório entre 0 e 999
int value2 = 0; // Número aleatório entre 0 e 99.999

// Cria um cliente HTTPS (conexão segura)
WiFiClientSecure client;

void setup() {
  Serial.begin(9600); // Inicia comunicação serial para depuração (velocidade: 9600 bps)
  delay(1000); // Pequena pausa para inicializar

  // Conecta à rede Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando-se à rede: ");
  Serial.println(ssid);

  // Espera até conectar
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Quando conectar, exibe o IP na tela
  Serial.println("\nConectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Desativa a verificação de certificado SSL (não recomendado em produção)
  client.setInsecure();
}

void loop() {
  // Atualiza os valores
  value0++; // Contador aumenta a cada envio
  value1 = random(0, 1000);     // Gera valor aleatório entre 0 e 999
  value2 = random(0, 100000);   // Gera valor aleatório entre 0 e 99.999

  // Monta o texto no formato que o Google Script espera
  payload = payload_base + "\"" + value0 + "," + value1 + "," + value2 + "\"}";

  // Exibe no monitor serial o que vai ser enviado
  Serial.println("Publicando dados...");
  Serial.println(payload);

  // Tenta conectar ao servidor do Google Scripts
  if (!client.connect(host, httpsPort)) {
    Serial.println("Falha na conexão");
    delay(5000); // Espera 5 segundos e tenta de novo
    return;
  }

  // Monta a requisição HTTP POST
  String request = String("POST ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Content-Type: application/json\r\n" +
                   "Content-Length: " + payload.length() + "\r\n\r\n" +
                   payload;

  // Envia a requisição ao servidor
  client.print(request);

  // Aguarda e lê a resposta do servidor
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break; // Sai quando terminar o cabeçalho HTTP
  }

  // Lê o corpo da resposta (o conteúdo enviado pelo servidor)
  String response = client.readString();
  Serial.println("Resposta:");
  Serial.println(response);

  // Fecha a conexão
  client.stop();

  // Aguarda 5 segundos antes de enviar novamente
  delay(5000);
}

