#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>


// =====================================================
// PINOS
// =====================================================

// DHT22
#define DHT_PIN 4
#define DHT_TYPE DHT22

// LED
#define LED_PIN 3

// BUZZER
#define BUZZER_PIN 2

// OLED integrado
#define OLED_SDA 5
#define OLED_SCL 6


// =====================================================
// DHT22
// =====================================================

DHT dht(DHT_PIN, DHT_TYPE);


// =====================================================
// OLED
// =====================================================

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
    U8G2_R0,
    U8X8_PIN_NONE,
    OLED_SCL,
    OLED_SDA
);


// =====================================================
// WI-FI
// =====================================================

const char* ssid = "DIP_4";
const char* senha = "12345678";

WebServer servidor(80);


// =====================================================
// VARIAVEIS
// =====================================================

float temperatura = 0.0;
float umidade = 0.0;

bool sensorOK = false;
bool wifiOK = false;

// Estado do alerta
bool buzzerLigado = false;

// Controle da leitura do DHT22
unsigned long ultimaLeitura = 0;

// Leitura local a cada 2 segundos
const unsigned long intervaloLeitura = 2000;


// =====================================================
// OLED
// =====================================================

void atualizarOLED()
{
    u8g2.clearBuffer();

    u8g2.setFont(
        u8g2_font_9x15B_tf
    );

    u8g2.drawStr(
        39,
        50,
        "MAVPRO"
    );

    u8g2.sendBuffer();
}


// =====================================================
// LIGAR ALERTA
//
// GPIO 2 = Buzzer
// GPIO 3 = LED
// =====================================================

void ligarBuzzer()
{
    buzzerLigado = true;

    digitalWrite(
        BUZZER_PIN,
        HIGH
    );

    digitalWrite(
        LED_PIN,
        HIGH
    );

    Serial.println(
        "ALERTA: BUZZER + LED LIGADOS"
    );
}


// =====================================================
// DESLIGAR ALERTA
// =====================================================

void desligarBuzzer()
{
    buzzerLigado = false;

    digitalWrite(
        BUZZER_PIN,
        LOW
    );

    digitalWrite(
        LED_PIN,
        LOW
    );

    Serial.println(
        "ALERTA: BUZZER + LED DESLIGADOS"
    );
}


// =====================================================
// ROTA /dados
//
// Exemplo:
//
// http://192.168.4.1/dados
//
// Resposta:
//
// T=26.5;U=64.1
// =====================================================

void enviarDados()
{
    if (!sensorOK)
    {
        servidor.send(
            503,
            "text/plain",
            "ERRO_SENSOR"
        );

        return;
    }


    String resposta =
        "T=" +
        String(temperatura, 1) +
        ";U=" +
        String(umidade, 1);


    servidor.send(
        200,
        "text/plain",
        resposta
    );
}


// =====================================================
// ROTA /buzzer/on
//
// Liga buzzer + LED
// =====================================================

void rotaBuzzerOn()
{
    ligarBuzzer();

    servidor.send(
        200,
        "text/plain",
        "ALERTA_ON"
    );
}


// =====================================================
// ROTA /buzzer/off
//
// Desliga buzzer + LED
// =====================================================

void rotaBuzzerOff()
{
    desligarBuzzer();

    servidor.send(
        200,
        "text/plain",
        "ALERTA_OFF"
    );
}


// =====================================================
// ROTA /buzzer/status
// =====================================================

void rotaBuzzerStatus()
{
    if (buzzerLigado)
    {
        servidor.send(
            200,
            "text/plain",
            "ON"
        );
    }
    else
    {
        servidor.send(
            200,
            "text/plain",
            "OFF"
        );
    }
}


// =====================================================
// SETUP
// =====================================================

void setup()
{
    Serial.begin(115200);

    delay(2000);


    Serial.println();

    Serial.println(
        "========================"
    );

    Serial.println(
        "         MAVPRO"
    );

    Serial.println(
        "========================"
    );


    // =================================================
    // CONFIGURAR LED
    // =================================================

    pinMode(
        LED_PIN,
        OUTPUT
    );

    digitalWrite(
        LED_PIN,
        LOW
    );


    // =================================================
    // CONFIGURAR BUZZER
    // =================================================

    pinMode(
        BUZZER_PIN,
        OUTPUT
    );

    digitalWrite(
        BUZZER_PIN,
        LOW
    );


    // =================================================
    // TESTE INICIAL
    //
    // LED + BUZZER ligados por 3 segundos
    // =================================================

    Serial.println(
        "Teste inicial do alerta"
    );


    digitalWrite(
        LED_PIN,
        HIGH
    );

    digitalWrite(
        BUZZER_PIN,
        HIGH
    );

    buzzerLigado = true;


    Serial.println(
        "LED + BUZZER ligados"
    );


    delay(3000);


    digitalWrite(
        LED_PIN,
        LOW
    );

    digitalWrite(
        BUZZER_PIN,
        LOW
    );

    buzzerLigado = false;


    Serial.println(
        "LED + BUZZER desligados"
    );


    // =================================================
    // OLED
    // =================================================

    Wire.begin(
        OLED_SDA,
        OLED_SCL
    );

    u8g2.begin();

    u8g2.setContrast(
        255
    );

    atualizarOLED();


    Serial.println(
        "OLED iniciado - MAVPRO"
    );


    // =================================================
    // DHT22
    // =================================================

    dht.begin();

    // Tempo para estabilizar
    delay(2000);


    Serial.println(
        "DHT22 iniciado"
    );


    // =================================================
    // WI-FI
    // =================================================

    WiFi.mode(
        WIFI_AP
    );


    wifiOK =
        WiFi.softAP(
            ssid,
            senha
        );


    if (wifiOK)
    {
        Serial.println(
            "Wi-Fi criado"
        );
    }
    else
    {
        Serial.println(
            "Erro no Wi-Fi"
        );
    }


    Serial.print(
        "Rede: "
    );

    Serial.println(
        ssid
    );


    Serial.print(
        "IP: "
    );

    Serial.println(
        WiFi.softAPIP()
    );


    // =================================================
    // ROTAS DO SERVIDOR
    // =================================================

    servidor.on(
        "/dados",
        enviarDados
    );


    servidor.on(
        "/buzzer/on",
        rotaBuzzerOn
    );


    servidor.on(
        "/buzzer/off",
        rotaBuzzerOff
    );


    servidor.on(
        "/buzzer/status",
        rotaBuzzerStatus
    );


    // =================================================
    // INICIAR SERVIDOR
    // =================================================

    servidor.begin();


    Serial.println();

    Serial.println(
        "Servidor HTTP iniciado"
    );


    Serial.println(
        "Rotas:"
    );


    Serial.println(
        "/dados"
    );


    Serial.println(
        "/buzzer/on"
    );


    Serial.println(
        "/buzzer/off"
    );


    Serial.println(
        "/buzzer/status"
    );


    Serial.println();

    Serial.println(
        "Sistema MAVPRO funcionando"
    );
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
    // =================================================
    // ATENDER WAVESHARE
    // =================================================

    servidor.handleClient();


    // =================================================
    // LEITURA DO DHT22
    //
    // A cada 2 segundos no próprio C3.
    // =================================================

    if (
        millis() - ultimaLeitura >=
        intervaloLeitura
    )
    {
        ultimaLeitura =
            millis();


        float novaTemperatura =
            dht.readTemperature();


        float novaUmidade =
            dht.readHumidity();


        // =================================================
        // LEITURA CORRETA
        // =================================================

        if (
            !isnan(novaTemperatura) &&
            !isnan(novaUmidade)
        )
        {
            temperatura =
                novaTemperatura;


            umidade =
                novaUmidade;


            sensorOK =
                true;


            // =============================================
            // MONITOR SERIAL
            // =============================================

            Serial.print(
                "Temperatura: "
            );


            Serial.print(
                temperatura,
                1
            );


            Serial.print(
                " C   "
            );


            Serial.print(
                "Umidade: "
            );


            Serial.print(
                umidade,
                1
            );


            Serial.print(
                " %   "
            );


            Serial.print(
                "Alerta: "
            );


            if (buzzerLigado)
            {
                Serial.println(
                    "LIGADO"
                );
            }
            else
            {
                Serial.println(
                    "DESLIGADO"
                );
            }
        }


        // =================================================
        // ERRO DHT22
        // =================================================

        else
        {
            sensorOK =
                false;


            Serial.println(
                "Erro ao ler DHT22"
            );
        }


        // OLED continua mostrando MAVPRO

        atualizarOLED();
    }


    // =================================================
    // IMPORTANTE
    // =================================================
    //
    // NÃO desligamos LED nem buzzer aqui.
    //
    // Eles permanecem no último estado recebido
    // pela Waveshare.
    // =================================================
}