
#include <Arduino.h>
#include <math.h>
#include <string.h>

#include <esp_display_panel.hpp>
#include <esp_err.h>

#include <WiFi.h>
#include <HTTPClient.h>

#include <lvgl.h>
#include "esp_lv_adapter_arduino.h"

extern "C" {
  #include "ui.h"
}

using namespace esp_panel::drivers;
using namespace esp_panel::board;

// =====================================================
// MAVPRO - ESP32-S3 WAVESHARE 7"
// LVGL 8.3 + SQUARELINE
// HTTP COM ESP32-C3
// =====================================================

// =====================================================
// WI-FI / HTTP
// =====================================================

const char* wifiSSID = "DIP_4";

const char* urlDados =
  "http://192.168.4.1/dados";

const char* urlBuzzerOn =
  "http://192.168.4.1/buzzer/on";

const char* urlBuzzerOff =
  "http://192.168.4.1/buzzer/off";

// =====================================================
// INTERVALOS
// =====================================================

// Temperatura e umidade: 2 segundos
const unsigned long intervaloConsulta = 2000;

// Grafico: 1 minuto
const unsigned long intervaloGrafico = 60000;

unsigned long ultimaConsulta = 0;
unsigned long ultimoRegistroGrafico = 0;

bool consultaImediata = false;

// =====================================================
// DADOS DO SENSOR
// =====================================================

int temperaturaAtual = 0;
int umidadeAtual = 0;

int temperaturaExibida = -999;
int umidadeExibida = -999;

bool dadosValidos = false;
bool primeiroPontoRegistrado = false;

// =====================================================
// TEMPERATURA MAXIMA
// =====================================================

int temperaturaMaxima = 30;

// =====================================================
// BUZZER
// =====================================================

// false = automatico
// true  = manual

bool modoManualBuzzer = false;
bool buzzerLigado = false;

bool estadoBuzzerExibido = false;
bool statusBuzzerInicializado = false;

// Comando pendente para executar fora
// dos eventos da interface LVGL.

bool comandoBuzzerPendente = false;
bool estadoBuzzerPendente = false;

// =====================================================
// CONTROLE WI-FI
// =====================================================

bool conectandoWiFi = false;
bool solicitarConexaoWiFi = false;

String senhaWiFiPendente = "";

unsigned long inicioConexaoWiFi = 0;
unsigned long momentoSolicitacaoWiFi = 0;

const unsigned long tempoLimiteWiFi = 15000;
const unsigned long atrasoAntesWiFi = 1000;

// =====================================================
// PROTOTIPOS
// =====================================================

void atualizarTemperaturaMaxima();
void atualizarStatusBuzzer();

void solicitarComandoBuzzer(bool ligar);
void processarComandoBuzzer();
void verificarBuzzerAutomatico();

void atualizarDadosNaTela();
void registrarNoGrafico(int temperatura, int umidade);

void consultarDadosC3();

void iniciarConexaoWifi();
void verificarConexaoWifi();

// =====================================================
// ATUALIZAR TEMPERATURA MAXIMA
// =====================================================

void atualizarTemperaturaMaxima()
{
  if (ui_temperaturaMax == NULL) return;

  char texto[20];

  snprintf(
    texto,
    sizeof(texto),
    "%d °C",
    temperaturaMaxima
  );

  lv_label_set_text(
    ui_temperaturaMax,
    texto
  );
}

// =====================================================
// ATUALIZAR STATUS BUZZER
// =====================================================

void atualizarStatusBuzzer()
{
  if (ui_LabelStatusBuzzer == NULL) return;

  if (
    statusBuzzerInicializado &&
    estadoBuzzerExibido == buzzerLigado
  ) {
    return;
  }

  lv_label_set_text(
    ui_LabelStatusBuzzer,
    buzzerLigado ? "LIGADO" : "DESLIGADO"
  );

  estadoBuzzerExibido = buzzerLigado;
  statusBuzzerInicializado = true;
}

// =====================================================
// SOLICITAR COMANDO DO BUZZER
//
// Apenas agenda a acao.
// O HTTP acontece no loop().
// =====================================================

void solicitarComandoBuzzer(bool ligar)
{
  if (
    !comandoBuzzerPendente &&
    buzzerLigado == ligar
  ) {
    return;
  }

  comandoBuzzerPendente = true;
  estadoBuzzerPendente = ligar;
}

// =====================================================
// EXECUTAR COMANDO HTTP DO BUZZER
// =====================================================

void processarComandoBuzzer()
{
  if (!comandoBuzzerPendente) return;

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  bool ligar = estadoBuzzerPendente;

  if (ligar == buzzerLigado) {
    comandoBuzzerPendente = false;
    return;
  }

  HTTPClient http;

  http.setTimeout(1000);

  const char* url =
    ligar ? urlBuzzerOn : urlBuzzerOff;

  if (!http.begin(url)) {
    Serial.println("Falha HTTP buzzer");
    return;
  }

  int codigo = http.GET();

  http.end();

  if (codigo == 200) {

    buzzerLigado = ligar;

    // Limpa somente se o comando pendente
    // ainda for o mesmo.
    if (estadoBuzzerPendente == ligar) {
      comandoBuzzerPendente = false;
    }

    Serial.print("Buzzer: ");
    Serial.println(
      ligar ? "LIGADO" : "DESLIGADO"
    );

    if (esp_lv_adapter_lock(1000) == ESP_OK) {

      atualizarStatusBuzzer();

      esp_lv_adapter_unlock();
    }

  } else {

    Serial.print("Erro HTTP buzzer: ");
    Serial.println(codigo);
  }
}

// =====================================================
// VERIFICAR BUZZER AUTOMATICO
// =====================================================

void verificarBuzzerAutomatico()
{
  if (!dadosValidos) return;
  if (modoManualBuzzer) return;

  bool deveLigar =
    temperaturaAtual >= temperaturaMaxima;

  solicitarComandoBuzzer(deveLigar);
}

// =====================================================
// BOTAO +
// =====================================================

void eventoTemperaturaMais(lv_event_t* e)
{
  if (temperaturaMaxima >= 50) return;

  temperaturaMaxima++;

  atualizarTemperaturaMaxima();

  Serial.print("Limite aumentado: ");
  Serial.println(temperaturaMaxima);

  // Regra solicitada:
  // ao aumentar o limite, desligar o
  // buzzer no modo automatico.

  if (!modoManualBuzzer) {

    if (
      buzzerLigado ||
      (comandoBuzzerPendente &&
       estadoBuzzerPendente)
    ) {
      solicitarComandoBuzzer(false);
    }
  }
}

// =====================================================
// BOTAO -
// =====================================================

void eventoTemperaturaMenos(lv_event_t* e)
{
  if (temperaturaMaxima <= 10) return;

  temperaturaMaxima--;

  atualizarTemperaturaMaxima();

  Serial.print("Limite reduzido: ");
  Serial.println(temperaturaMaxima);

  if (!modoManualBuzzer && dadosValidos) {
    verificarBuzzerAutomatico();
  }
}

// =====================================================
// SWITCH AUTOMATICO / MANUAL
// =====================================================

void eventoModoBuzzer(lv_event_t* e)
{
  lv_obj_t* sw = lv_event_get_target(e);

  modoManualBuzzer =
    lv_obj_has_state(
      sw,
      LV_STATE_CHECKED
    );

  Serial.println(
    modoManualBuzzer
      ? "Modo MANUAL"
      : "Modo AUTOMATICO"
  );

  if (!modoManualBuzzer && dadosValidos) {
    verificarBuzzerAutomatico();
  }
}

// =====================================================
// BOTAO ACIONAR BUZZER
// =====================================================

void eventoAcionarBuzzer(lv_event_t* e)
{
  if (!modoManualBuzzer) {
    Serial.println(
      "Botao disponivel apenas no modo manual"
    );
    return;
  }

  bool estadoAtual =
    comandoBuzzerPendente
      ? estadoBuzzerPendente
      : buzzerLigado;

  solicitarComandoBuzzer(!estadoAtual);
}

// =====================================================
// ATUALIZAR TEMPERATURA E UMIDADE
// =====================================================

void atualizarDadosNaTela()
{
  if (
    temperaturaAtual == temperaturaExibida &&
    umidadeAtual == umidadeExibida
  ) {
    return;
  }

  if (esp_lv_adapter_lock(1000) != ESP_OK) {
    return;
  }

  if (temperaturaAtual != temperaturaExibida) {

    char texto[20];

    snprintf(
      texto,
      sizeof(texto),
      "%d °C",
      temperaturaAtual
    );

    if (ui_temperatura != NULL) {
      lv_label_set_text(
        ui_temperatura,
        texto
      );
    }

    temperaturaExibida = temperaturaAtual;
  }

  if (umidadeAtual != umidadeExibida) {

    char texto[20];

    snprintf(
      texto,
      sizeof(texto),
      "%d %%",
      umidadeAtual
    );

    if (ui_umidade != NULL) {
      lv_label_set_text(
        ui_umidade,
        texto
      );
    }

    umidadeExibida = umidadeAtual;
  }

  esp_lv_adapter_unlock();
}

// =====================================================
// REGISTRAR PONTO NO GRAFICO
// =====================================================

void registrarNoGrafico(
  int temperatura,
  int umidade
)
{
  if (
    ui_ChartTemp == NULL ||
    ui_Chart2 == NULL ||
    ui_ChartTemp_series_1 == NULL ||
    ui_Chart2_series_1 == NULL
  ) {
    return;
  }

  if (esp_lv_adapter_lock(1000) != ESP_OK) {
    return;
  }

  lv_chart_set_next_value(
    ui_ChartTemp,
    ui_ChartTemp_series_1,
    temperatura
  );

  lv_chart_set_next_value(
    ui_Chart2,
    ui_Chart2_series_1,
    umidade
  );

  // Sem lv_chart_refresh() manual.

  esp_lv_adapter_unlock();

  Serial.println(
    "Novo ponto registrado no grafico"
  );
}

// =====================================================
// CONSULTAR DADOS DO C3 VIA HTTP
// =====================================================

void consultarDadosC3()
{
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  HTTPClient http;

  http.setTimeout(1000);

  if (!http.begin(urlDados)) {
    Serial.println("Falha ao iniciar HTTP");
    return;
  }

  int codigo = http.GET();

  if (codigo != 200) {

    Serial.print("Erro HTTP /dados: ");
    Serial.println(codigo);

    http.end();
    return;
  }

  // Resposta esperada:
  // T=26.5;U=64.1

  char resposta[64] = {0};

  WiFiClient* stream = http.getStreamPtr();

  size_t indice = 0;

  unsigned long inicio = millis();

  while (
    millis() - inicio < 1000 &&
    indice < sizeof(resposta) - 1
  ) {

    while (
      stream->available() &&
      indice < sizeof(resposta) - 1
    ) {

      resposta[indice++] =
        (char)stream->read();
    }

    resposta[indice] = '\0';

    float t = 0;
    float u = 0;

    if (
      sscanf(
        resposta,
        "T=%f;U=%f",
        &t,
        &u
      ) == 2
    ) {
      break;
    }

    if (
      !http.connected() &&
      !stream->available()
    ) {
      break;
    }

    delay(1);
  }

  http.end();

  float temperaturaRecebida = 0;
  float umidadeRecebida = 0;

  int quantidadeLida = sscanf(
    resposta,
    "T=%f;U=%f",
    &temperaturaRecebida,
    &umidadeRecebida
  );

  if (quantidadeLida != 2) {
    Serial.println("Resposta invalida do C3");
    return;
  }

  if (
    !isfinite(temperaturaRecebida) ||
    !isfinite(umidadeRecebida)
  ) {
    Serial.println("Dados invalidos");
    return;
  }

  // Valores inteiros na interface.

  temperaturaAtual =
    (int)roundf(temperaturaRecebida);

  umidadeAtual =
    (int)roundf(umidadeRecebida);

  dadosValidos = true;

  Serial.print("Temperatura: ");
  Serial.print(temperaturaAtual);

  Serial.print(" C | Umidade: ");
  Serial.print(umidadeAtual);

  Serial.println(" %");

  atualizarDadosNaTela();

  // Primeiro ponto imediatamente
  // apos receber os primeiros dados.

  if (!primeiroPontoRegistrado) {

    registrarNoGrafico(
      temperaturaAtual,
      umidadeAtual
    );

    primeiroPontoRegistrado = true;

    ultimoRegistroGrafico = millis();
  }

  verificarBuzzerAutomatico();
}

// =====================================================
// ABRIR TECLADO WI-FI
// =====================================================

void eventoCampoSenha(lv_event_t* e)
{
  lv_event_code_t codigo =
    lv_event_get_code(e);

  if (
    codigo != LV_EVENT_CLICKED &&
    codigo != LV_EVENT_FOCUSED
  ) {
    return;
  }

  lv_obj_set_parent(
    ui_KeyboardWifi,
    ui_telaConfiguracao
  );

  lv_obj_set_size(
    ui_KeyboardWifi,
    800,
    210
  );

  lv_obj_align(
    ui_KeyboardWifi,
    LV_ALIGN_BOTTOM_MID,
    0,
    0
  );

  lv_keyboard_set_textarea(
    ui_KeyboardWifi,
    ui_TextareaSenhaWifi
  );

  lv_obj_clear_flag(
    ui_KeyboardWifi,
    LV_OBJ_FLAG_HIDDEN
  );

  lv_obj_move_foreground(
    ui_KeyboardWifi
  );
}

// =====================================================
// TECLADO WI-FI
// =====================================================

void eventoTecladoWifi(lv_event_t* e)
{
  lv_event_code_t codigo =
    lv_event_get_code(e);

  if (
    codigo == LV_EVENT_READY ||
    codigo == LV_EVENT_CANCEL
  ) {

    lv_obj_add_flag(
      ui_KeyboardWifi,
      LV_OBJ_FLAG_HIDDEN
    );

    lv_obj_clear_state(
      ui_TextareaSenhaWifi,
      LV_STATE_FOCUSED
    );
  }
}

// =====================================================
// BOTAO CONECTAR
// =====================================================

void eventoConectarWifi(lv_event_t* e)
{
  if (
    conectandoWiFi ||
    solicitarConexaoWiFi
  ) {
    return;
  }

  const char* senha =
    lv_textarea_get_text(
      ui_TextareaSenhaWifi
    );

  if (
    senha == NULL ||
    strlen(senha) == 0
  ) {

    lv_label_set_text(
      ui_LabelConectar,
      "DIGITE A SENHA"
    );

    return;
  }

  senhaWiFiPendente = String(senha);

  lv_obj_add_flag(
    ui_KeyboardWifi,
    LV_OBJ_FLAG_HIDDEN
  );

  lv_obj_clear_state(
    ui_TextareaSenhaWifi,
    LV_STATE_FOCUSED
  );

  lv_label_set_text(
    ui_LabelConectar,
    "CONECTANDO..."
  );

  momentoSolicitacaoWiFi = millis();

  solicitarConexaoWiFi = true;
}

// =====================================================
// INICIAR CONEXAO WI-FI
// =====================================================

void iniciarConexaoWifi()
{
  if (!solicitarConexaoWiFi) return;

  if (
    millis() - momentoSolicitacaoWiFi <
    atrasoAntesWiFi
  ) {
    return;
  }

  solicitarConexaoWiFi = false;

  WiFi.disconnect();

  delay(50);

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    wifiSSID,
    senhaWiFiPendente.c_str()
  );

  conectandoWiFi = true;

  inicioConexaoWiFi = millis();

  Serial.println(
    "Conectando ao Wi-Fi DIP_4..."
  );
}

// =====================================================
// VERIFICAR CONEXAO WI-FI
// =====================================================

void verificarConexaoWifi()
{
  if (!conectandoWiFi) return;

  if (WiFi.status() == WL_CONNECTED) {

    conectandoWiFi = false;

    Serial.println("Wi-Fi conectado");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    if (esp_lv_adapter_lock(1000) == ESP_OK) {

      lv_label_set_text(
        ui_LabelConectar,
        "CONECTADO"
      );

      esp_lv_adapter_unlock();
    }

    senhaWiFiPendente = "";

    // Primeira leitura imediata.
    consultaImediata = true;

    return;
  }

  if (
    millis() - inicioConexaoWiFi >=
    tempoLimiteWiFi
  ) {

    conectandoWiFi = false;

    WiFi.disconnect();

    if (esp_lv_adapter_lock(1000) == ESP_OK) {

      lv_label_set_text(
        ui_LabelConectar,
        "TENTAR NOVAMENTE"
      );

      esp_lv_adapter_unlock();
    }

    senhaWiFiPendente = "";

    Serial.println(
      "Falha ao conectar ao Wi-Fi"
    );
  }
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  Serial.println();
  Serial.println("==============================");
  Serial.println(" MAVPRO - WAVESHARE ESP32-S3");
  Serial.println("==============================");

  // ---------------- BOARD ----------------

  Board* board = new Board();

  if (
    board == nullptr ||
    !board->init()
  ) {

    Serial.println("Erro ao iniciar board");

    while (true) {
      delay(1000);
    }
  }

  // ---------------- DISPLAY ----------------

  const esp_lv_adapter_rotation_t rotation =
    ESP_LV_ADAPTER_ROTATE_0;

  const esp_lv_adapter_tear_avoid_mode_t tear_mode =
    ESP_LV_ADAPTER_TEAR_AVOID_MODE_DEFAULT_RGB;

  const uint8_t frame_buffer_count = 1;

  LCD* lcd = board->getLCD();

  if (lcd == nullptr) {

    Serial.println("LCD nao encontrado");

    while (true) {
      delay(1000);
    }
  }

  auto* lcd_bus = lcd->getBus();

  if (
    lcd_bus->getBasicAttributes().type ==
    ESP_PANEL_BUS_TYPE_RGB
  ) {

    lcd->configFrameBufferNumber(
      frame_buffer_count
    );

    static_cast<BusRGB*>(lcd_bus)
      ->configRGB_BounceBufferSize(
        lcd->getFrameWidth() * 10
      );
  }

  if (!board->begin()) {

    Serial.println(
      "Falha ao iniciar display"
    );

    while (true) {
      delay(1000);
    }
  }

  // ---------------- LVGL ----------------

  esp_lv_adapter_config_t adapter_config =
    ESP_LV_ADAPTER_DEFAULT_CONFIG();

  adapter_config.task_stack_size = 12 * 1024;
  adapter_config.task_priority = 2;
  adapter_config.task_core_id = ARDUINO_RUNNING_CORE;

  ESP_ERROR_CHECK(
    esp_lv_adapter_init(
      &adapter_config
    )
  );

  // ---------------- REGISTRAR DISPLAY ----------------

  esp_lv_adapter_display_config_t disp_config =
    ESP_LV_ADAPTER_DISPLAY_RGB_DEFAULT_CONFIG(
      lcd,
      lcd->getFrameWidth(),
      lcd->getFrameHeight(),
      rotation
    );

  disp_config.profile.use_psram = true;

  lv_display_t* disp =
    esp_lv_adapter_register_display(
      &disp_config
    );

  assert(disp != nullptr);

  // ---------------- TOUCH ----------------

  if (board->getTouch() != nullptr) {

    esp_lv_adapter_touch_config_t touch_config =
      ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(
        disp,
        board->getTouch()
      );

    lv_indev_t* touch =
      esp_lv_adapter_register_touch(
        &touch_config
      );

    assert(touch != nullptr);
  }

  // ---------------- INICIAR LVGL ----------------

  ESP_ERROR_CHECK(
    esp_lv_adapter_start()
  );

  // ---------------- SQUARELINE ----------------

  ESP_ERROR_CHECK(
    esp_lv_adapter_lock(-1)
  );

  ui_init();

  // ---------------- EVENTOS ----------------

  lv_obj_add_event_cb(
    ui_ButtonMais,
    eventoTemperaturaMais,
    LV_EVENT_CLICKED,
    NULL
  );

  lv_obj_add_event_cb(
    ui_ButtonMenos,
    eventoTemperaturaMenos,
    LV_EVENT_CLICKED,
    NULL
  );

  lv_obj_add_event_cb(
    ui_SwitchModoBuzzer,
    eventoModoBuzzer,
    LV_EVENT_VALUE_CHANGED,
    NULL
  );

  lv_obj_add_event_cb(
    ui_ButtonAcionarBuzzer,
    eventoAcionarBuzzer,
    LV_EVENT_CLICKED,
    NULL
  );

  lv_obj_add_event_cb(
    ui_TextareaSenhaWifi,
    eventoCampoSenha,
    LV_EVENT_ALL,
    NULL
  );

  lv_obj_add_event_cb(
    ui_KeyboardWifi,
    eventoTecladoWifi,
    LV_EVENT_ALL,
    NULL
  );

  lv_obj_add_event_cb(
    ui_ButtonConectar,
    eventoConectarWifi,
    LV_EVENT_CLICKED,
    NULL
  );

  // ---------------- DROPDOWN ----------------

  lv_dropdown_set_options(
    ui_DropdownWifi,
    "DIP_4"
  );

  // ---------------- TECLADO ----------------

  lv_keyboard_set_textarea(
    ui_KeyboardWifi,
    ui_TextareaSenhaWifi
  );

  lv_obj_add_flag(
    ui_KeyboardWifi,
    LV_OBJ_FLAG_HIDDEN
  );

  // ---------------- TEMPERATURA MAXIMA ----------------

  temperaturaMaxima = 30;

  atualizarTemperaturaMaxima();

  // ---------------- BUZZER ----------------

  modoManualBuzzer = false;
  buzzerLigado = false;

  lv_obj_clear_state(
    ui_SwitchModoBuzzer,
    LV_STATE_CHECKED
  );

  atualizarStatusBuzzer();

  // ---------------- LABEL WI-FI ----------------

  lv_label_set_text(
    ui_LabelConectar,
    "CONECTAR"
  );

  esp_lv_adapter_unlock();

  // ---------------- WI-FI ----------------

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // ---------------- INFORMACOES ----------------

  Serial.println();
  Serial.println("Sistema MAVPRO pronto");
  Serial.println("HTTP ativado");
  Serial.println("Sensor: 2 segundos");
  Serial.println("Grafico: 1 minuto");
  Serial.println("PSRAM: true");
  Serial.println("Framebuffer: 1");
  Serial.println("Bounce: x10");
}

// =====================================================
// LOOP PRINCIPAL
// =====================================================

void loop()
{
  // 1 - Controle da conexao Wi-Fi

  iniciarConexaoWifi();

  verificarConexaoWifi();

  // 2 - Consulta do sensor a cada 2 segundos

  if (WiFi.status() == WL_CONNECTED) {

    if (
      consultaImediata ||
      millis() - ultimaConsulta >= intervaloConsulta
    ) {

      consultaImediata = false;

      ultimaConsulta = millis();

      consultarDadosC3();
    }
  }

  // 3 - Comandos do buzzer

  processarComandoBuzzer();

  // 4 - Historico a cada 1 minuto

  if (
    dadosValidos &&
    primeiroPontoRegistrado &&
    WiFi.status() == WL_CONNECTED &&
    millis() - ultimoRegistroGrafico >=
      intervaloGrafico
  ) {

    ultimoRegistroGrafico = millis();

    registrarNoGrafico(
      temperaturaAtual,
      umidadeAtual
    );
  }

  delay(10);
}
