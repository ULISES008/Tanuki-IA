#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>


//Pin RELE
#define RELE 12
// Pines para cada color (usa pines con PWM)
#define RED_PIN    42
#define GREEN_PIN  41
#define BLUE_PIN   40

// WiFi SSID and Password
const char* ssid = "sumida_01_2.4GHz";
const char* password = "sumida_dri_5611";
//MQTT Broker IP address
const char* mqtt_server = "10.51.103.214";
// Setup WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);
// Contadores para mensajes
long lastMsg = 0;
char msg[50];
int value = 0;


// FUNCIONES //

// WiFi and MQTT setup functions
void setup_wifi();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();

// Funciones Tira Led RGB
void setColor(int red, int green, int blue);
void rainbowEffect(int cycles, int speedDelay);

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  // Conectar a la red WiFi
  setup_wifi();
  // Configurar el servidor MQTT y la función callback
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Configurar pines
  pinMode(RELE, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  // Apagar todos los colores al inicio
  setColor(0, 0, 0);
}

void loop() {
  // Asegurarse de estar conectado al broker MQTT y procesar mensajes entrantes
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}


// Función para conectar a la red WiFi
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
// Función callback que se ejecuta cuando llega un mensaje MQTT
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  //if statements to control more GPIOs with MQTT
  
  if (String(topic) == "esp32/output/light") {
    int lightValue = messageTemp.toInt();
    digitalWrite(RELE, lightValue);
  }
  if (String(topic) == "esp32/output/red") {
    int redValue = messageTemp.toInt();
    analogWrite(RED_PIN, redValue);
  }
  if (String(topic) == "esp32/output/green") {
    int greenValue = messageTemp.toInt();
    analogWrite(GREEN_PIN, greenValue);
  }
  if (String(topic) == "esp32/output/blue") {
    int blueValue = messageTemp.toInt();
    analogWrite(BLUE_PIN, blueValue);
  }
}
// Función para reconectar al broker MQTT y suscribirse al tópico
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output/light");
      client.subscribe("esp32/output/red");
      client.subscribe("esp32/output/green");
      client.subscribe("esp32/output/blue");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Función para establecer color (0-255 para cada canal)
void setColor(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}
// Efecto arcoíris
void rainbowEffect(int cycles = 3, int speedDelay = 20) {
  for (int cycle = 0; cycle < cycles; cycle++) {
    // Rojo -> Amarillo
    for (int green = 0; green <= 255; green++) {
      setColor(255, green, 0);
      delay(speedDelay);
    }
    
    // Amarillo -> Verde
    for (int red = 255; red >= 0; red--) {
      setColor(red, 255, 0);
      delay(speedDelay);
    }
    
    // Verde -> Cyan
    for (int blue = 0; blue <= 255; blue++) {
      setColor(0, 255, blue);
      delay(speedDelay);
    }
    
    // Cyan -> Azul
    for (int green = 255; green >= 0; green--) {
      setColor(0, green, 255);
      delay(speedDelay);
    }
    
    // Azul -> Magenta
    for (int red = 0; red <= 255; red++) {
      setColor(red, 0, 255);
      delay(speedDelay);
    }
    
    // Magenta -> Rojo
    for (int blue = 255; blue >= 0; blue--) {
      setColor(255, 0, blue);
      delay(speedDelay);
    }
  }
}