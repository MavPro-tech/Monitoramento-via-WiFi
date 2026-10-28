// =====================================================
// ESP32-C3
// DHT22 + OLED INTEGRADO + LED + BUZZER
//
// DHT22 DATA -> GPIO 4
// LED        -> GPIO 3
// BUZZER     -> GPIO 2
//
// OLED integrado:
// SDA -> GPIO 5
// SCL -> GPIO 6
// Endereco -> 0x3C
// =====================================================

#include <Wire.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// =====================================================
// PINOS
// =====================================================

#define DHT_PIN 4
#define DHT_TYPE DHT22

#define LED_PIN 3
#define BUZZER_PIN 2

#define OLED_SDA 5
#define OLED_SCL 6


// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);


// =====================================================
// DHT22
// =====================================================

DHT dht(DHT_PIN, DHT_TYPE);


// =====================================================
// VARIAVEIS
// =====================================================

float temperatura = 0.0;
float umidade = 0.0;

bool sensorOK = false;

unsigned long ultimaLeitura = 0;


// =====================================================
// LER DHT22
// =====================================================

void lerSensor() {

  // DHT22 atualizado a cada 2 segundos

  if (millis() - ultimaLeitura >= 2000 || ultimaLeitura == 0) {

    ultimaLeitura = millis();

    float t = dht.readTemperature();
    float u = dht.readHumidity();


    if (!isnan(t) && !isnan(u)) {

      temperatura = t;
      umidade = u;

      sensorOK = true;


      Serial.print("Temperatura: ");
      Serial.print(temperatura, 1);
      Serial.println(" C");


      Serial.print("Umidade: ");
      Serial.print(umidade, 1);
      Serial.println(" %");


      Serial.println("----------------");

    }

    else {

      sensorOK = false;

      Serial.println("ERRO NO DHT22");
    }
  }
}


// =====================================================
// ATUALIZAR OLED
// =====================================================

void atualizarOLED() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);


  // ===================================================
  // TITULO
  // ===================================================

  display.setTextSize(1);

  display.setCursor(37, 1);
  display.print("SISTEMA");

  display.drawLine(
    8,
    11,
    119,
    11,
    SSD1306_WHITE
  );


  // ===================================================
  // TEMPERATURA
  // ===================================================

  display.setTextSize(1);

  display.setCursor(14, 17);
  display.print("TEMP");


  if (sensorOK) {

    display.setTextSize(2);

    display.setCursor(48, 15);

    display.print(
      temperatura,
      1
    );


    display.setTextSize(1);

    display.print(" C");

  }

  else {

    display.setTextSize(1);

    display.setCursor(55, 17);

    display.print("ERRO");
  }


  // ===================================================
  // UMIDADE
  // ===================================================

  display.setTextSize(1);

  display.setCursor(14, 36);
  display.print("UMID");


  if (sensorOK) {

    display.setTextSize(2);

    display.setCursor(48, 34);

    display.print(
      umidade,
      1
    );


    display.setTextSize(1);

    display.print(" %");

  }

  else {

    display.setTextSize(1);

    display.setCursor(55, 36);

    display.print("ERRO");
  }


  // ===================================================
  // LINHA INFERIOR
  // ===================================================

  display.drawLine(
    8,
    52,
    119,
    52,
    SSD1306_WHITE
  );


  // ===================================================
  // STATUS
  // ===================================================

  display.setTextSize(1);


  if (sensorOK) {

    display.setCursor(28, 55);
    display.print("DHT22: OK");

  }

  else {

    display.setCursor(22, 55);
    display.print("DHT22: ERRO");
  }


  // Mostra tudo na tela
  display.display();
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(1000);


  Serial.println();
  Serial.println("====================");
  Serial.println(" ESP32-C3");
  Serial.println("====================");


  // ===================================================
  // LED
  // ===================================================

  pinMode(
    LED_PIN,
    OUTPUT
  );


  // LED fica aceso
  digitalWrite(
    LED_PIN,
    HIGH
  );


  Serial.println(
    "LED ligado"
  );


  // ===================================================
  // BUZZER
  // ===================================================

  pinMode(
    BUZZER_PIN,
    OUTPUT
  );


  // Comeca desligado
  digitalWrite(
    BUZZER_PIN,
    LOW
  );


  // ===================================================
  // DHT22
  // ===================================================

  dht.begin();

  Serial.println(
    "DHT22 iniciado"
  );


  // ===================================================
  // OLED
  // ===================================================

  // IMPORTANTE:
  // OLED integrado da sua placa:
  //
  // SDA = GPIO 5
  // SCL = GPIO 6

  Wire.begin(
    OLED_SDA,
    OLED_SCL
  );


  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS
      )) {

    Serial.println(
      "ERRO NO OLED"
    );

    while (true) {

      delay(1000);
    }
  }


  Serial.println(
    "OLED iniciado"
  );


  // ===================================================
  // TELA DE INICIALIZACAO
  // ===================================================

  display.clearDisplay();

  display.setTextColor(
    SSD1306_WHITE
  );


  display.setTextSize(1);

  display.setCursor(
    34,
    18
  );

  display.print(
    "ESP32-C3"
  );


  display.setCursor(
    25,
    35
  );

  display.print(
    "Iniciando..."
  );


  display.display();


  // ===================================================
  // TESTE DO BUZZER
  // ===================================================

  Serial.println(
    "Buzzer ligado"
  );


  digitalWrite(
    BUZZER_PIN,
    HIGH
  );


  // Buzzer fica ligado por 3 segundos
  delay(3000);


  digitalWrite(
    BUZZER_PIN,
    LOW
  );


  Serial.println(
    "Buzzer desligado"
  );


  // ===================================================
  // PRIMEIRA LEITURA
  // ===================================================

  lerSensor();

  atualizarOLED();


  Serial.println();
  Serial.println(
    "Sistema funcionando!"
  );
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  // Faz leitura do sensor
  lerSensor();


  // Atualiza OLED
  atualizarOLED();


  // Mantem LED sempre aceso
  digitalWrite(
    LED_PIN,
    HIGH
  );


  delay(100);
}