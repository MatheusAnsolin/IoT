#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

//atualize SSID e senha WiFi
const char* ssid = "HackaTruckIoT";
const char* password = "iothacka";

//D4 only for Lolin board
#define LED_BUILTIN D4

//Atualize os valores de Org, device type, device id e token
#define ORG ""
#define DEVICE_TYPE ""
#define DEVICE_ID ""
#define TOKEN ""

//broker messagesight server
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/status/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

#define pinoAnalogico A0

char percentualStr[32];

WiFiClient wifiClient;
PubSubClient client(server, 1883, NULL, wifiClient);

void setup() {
  Serial.begin(9600);

  Serial.print("Conectando na rede WiFi "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("[INFO] Conectado WiFi IP: ");
  Serial.println(WiFi.localIP());

  pinMode(LED_BUILTIN, OUTPUT);


  pinMode(pinoAnalogico, INPUT);

}

void loop() {

  if (!!!client.connected()) {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    Serial.println();
  }

  Serial.print("Gas = ");
  int leitura_analogica = analogRead(pinoAnalogico);
  float percentualGas = 100 * ((978-(float)leitura_analogica)/978);
  dtostrf(percentualGas, 2, 1, percentualStr);

    Serial.println(percentualGas);

  digitalWrite(LED_BUILTIN, LOW);   
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);

  // Prepara JSON para IOT Platform
  int length = 0;

  String payload = "{\"d\":{\"porcentagemGas\": \"" + String(percentualGas) + "\"}}";

  length = payload.length();
  Serial.print(F("\nData length"));
  Serial.println(length);

  Serial.print("Sending payload: ");
  Serial.println(payload);

  if (client.publish(topic, (char*) payload.c_str())) {
    Serial.println("Publish ok");
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
  } else {
    Serial.println("Publish failed");
  }
  
  delay(10);
}

