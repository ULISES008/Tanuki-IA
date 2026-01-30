#include <WiFi.h>
#include <PubSubClient.h>

// Configuración WiFi
const char* ssid = "sumida_01_2.4GHz";
const char* password = "sumida_dri_5611";

// Configuración MQTT
const char* mqtt_server = "10.51.103.214"; // Cambia por la IP de tu PC
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void scanNetworks() {
  Serial.println("\nEscaneando redes WiFi disponibles...");
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
    Serial.println("No se encontraron redes WiFi");
  } else {
    Serial.printf("Encontradas %d redes:\n", n);
    for (int i = 0; i < n; i++) {
      Serial.printf("%d: %s (Canal: %d) RSSI: %d dBm %s\n", 
        i+1, 
        WiFi.SSID(i).c_str(), 
        WiFi.channel(i), 
        WiFi.RSSI(i), 
        (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Abierta" : "Protegida");
      delay(10);
    }
  }
}

void setup_wifi() {
  delay(10);
  Serial.begin(115200);
  Serial.println("\n\n=== INICIANDO ESP32-S3 ===");
  Serial.println("Versión: " + String(ARDUINO));
  
  // Escanear redes primero
  scanNetworks();
  
  Serial.println("\n=== INTENTANDO CONEXIÓN ===");
  Serial.print("Conectando a: ");
  Serial.println(ssid);
  Serial.print("SSID ingresado: ");
  Serial.println(ssid);
  
  // Configurar modo WiFi
  WiFi.mode(WIFI_STA);
  
  // Opcional: Configurar potencia (si tienes problemas de señal)
  // WiFi.setTxPower(WIFI_POWER_19_5dBm); // Máxima potencia
  
  // Intentar conexión
  WiFi.begin(ssid, password);
  
  int intentos = 0;
  const int maxIntentos = 30; // 15 segundos máximo
  
  while (WiFi.status() != WL_CONNECTED && intentos < maxIntentos) {
    delay(500);
    Serial.print(".");
    intentos++;
    
    // Mostrar estado cada 10 intentos
    if (intentos % 10 == 0) {
      Serial.printf("\nIntento %d - Estado: ", intentos);
      switch(WiFi.status()) {
        case WL_IDLE_STATUS: Serial.println("IDLE"); break;
        case WL_NO_SSID_AVAIL: Serial.println("SSID no disponible"); break;
        case WL_SCAN_COMPLETED: Serial.println("Escaneo completado"); break;
        case WL_CONNECTED: Serial.println("Conectado"); break;
        case WL_CONNECT_FAILED: Serial.println("Conexión fallida"); break;
        case WL_CONNECTION_LOST: Serial.println("Conexión perdida"); break;
        case WL_DISCONNECTED: Serial.println("Desconectado"); break;
        default: Serial.println("Estado desconocido"); break;
      }
    }
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi CONECTADO");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC address: ");
    Serial.println(WiFi.macAddress());
    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("Canal: ");
    Serial.println(WiFi.channel());
  } else {
    Serial.println("\n❌ ERROR DE CONEXIÓN WiFi");
    Serial.print("Estado final: ");
    
    switch(WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("Red no encontrada. Verifica:");
        Serial.println("1. Nombre del WiFi (SSID)");
        Serial.println("2. Que el WiFi esté encendido");
        Serial.println("3. Que estés en el rango");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Contraseña incorrecta");
        Serial.println("Verifica la contraseña del WiFi");
        break;
      case WL_DISCONNECTED:
        Serial.println("Desconectado durante el intento");
        break;
      default:
        Serial.printf("Código de error: %d\n", WiFi.status());
    }
    
    Serial.println("\n⚠️  El ESP32 continuará sin WiFi");
  }
}

void setup() {
  // Inicializar LED integrado (si existe)
  pinMode(48, OUTPUT); // LED integrado en ESP32-S3 (generalmente GPIO 48 o 21)
  digitalWrite(48, HIGH);
  
  setup_wifi();
  
  // Configurar MQTT solo si WiFi está conectado
  if (WiFi.status() == WL_CONNECTED) {
    client.setServer(mqtt_server, mqtt_port);
    Serial.print("Servidor MQTT configurado: ");
    Serial.println(mqtt_server);
  }
  
  // Parpadear LED para indicar inicio
  for(int i = 0; i < 3; i++) {
    digitalWrite(48, LOW);
    delay(200);
    digitalWrite(48, HIGH);
    delay(200);
  }
}

void loop() {
  // LED parpadea según estado WiFi
  if (WiFi.status() == WL_CONNECTED) {
    // WiFi OK - Parpadeo rápido
    digitalWrite(48, !digitalRead(48));
    delay(1000);
    
    // Intentar MQTT si está disponible
    if (!client.connected()) {
      Serial.println("Intentando conectar MQTT...");
      if (client.connect("ESP32S3Client")) {
        Serial.println("✅ MQTT conectado");
      }
    }
  } else {
    // Sin WiFi - Parpadeo lento
    digitalWrite(48, !digitalRead(48));
    delay(3000);
    
    // Intentar reconectar WiFi cada 30 segundos
    static unsigned long lastReconnect = 0;
    if (millis() - lastReconnect > 30000) {
      lastReconnect = millis();
      Serial.println("Reintentando conexión WiFi...");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
    }
  }
  
  // Imprimir estado periódicamente
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 10000) {
    lastStatus = millis();
    Serial.printf("\n[Estado] WiFi: %s, MQTT: %s\n",
      WiFi.status() == WL_CONNECTED ? "Conectado" : "Desconectado",
      client.connected() ? "Conectado" : "Desconectado"
    );
  }
}