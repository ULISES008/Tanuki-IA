#include <Arduino.h>

#define RELE 12
// Pines para cada color (usa pines con PWM)
#define RED_PIN    42
#define GREEN_PIN  41
#define BLUE_PIN   40

// Función para establecer color (0-255 para cada canal)
void setColor(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}
void rainbowEffect(int cycles, int speedDelay);

void setup() {
  pinMode(RELE, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  setColor(0, 0, 0);
  Serial.begin(115200);
  Serial.println("Control de tira RGB lista");
}

void loop() {

  // Ciclo de colores básicos
  /*Serial.println("Rojo");
  digitalWrite(RELE, LOW);  // Apagar
  setColor(255, 0, 0);
  delay(1000);
  
  Serial.println("Verde");
  digitalWrite(RELE, HIGH);   // Encender
  setColor(0, 255, 0);
  delay(1000);
  
  Serial.println("Azul");
  setColor(0, 0, 255);
  delay(1000);*/

  
  digitalWrite(RELE, LOW);  // Apagar
  delay(1000);
  
  digitalWrite(RELE, HIGH);   // Encender
  delay(1000);
  
  rainbowEffect(1, 10);

  
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