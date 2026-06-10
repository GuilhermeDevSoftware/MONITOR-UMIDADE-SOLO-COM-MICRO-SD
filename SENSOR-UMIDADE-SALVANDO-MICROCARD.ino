#include <SPI.h>
#include <SD.h>

// ==================================================
// SENSOR DE UMIDADE DO SOLO
// ==================================================

#define PINO_SENSOR 34

// ==================================================
// LEDS
// ==================================================

#define LED_VERDE     25
#define LED_AMARELO   26
#define LED_VERMELHO  27

// ==================================================
// MICROSD
// ==================================================

#define PINO_SD_CS    5
#define PINO_SD_SCK   18
#define PINO_SD_MISO  19
#define PINO_SD_MOSI  23

// ==================================================
// CALIBRAÇÃO DO SENSOR
// ==================================================

// Ajuste depois de observar os valores no Monitor Serial
#define VALOR_SECO    3200
#define VALOR_UMIDO   1300

unsigned long numeroLeitura = 0;

void desligarLeds() {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
}

void atualizarLeds(int umidade) {
  desligarLeds();

  if (umidade >= 70) {
    digitalWrite(LED_VERDE, HIGH);
  }
  else if (umidade >= 35) {
    digitalWrite(LED_AMARELO, HIGH);
  }
  else {
    digitalWrite(LED_VERMELHO, HIGH);
  }
}

String obterEstado(int umidade) {
  if (umidade >= 70) {
    return "UMIDO";
  }
  else if (umidade >= 35) {
    return "INTERMEDIARIO";
  }
  else {
    return "SECO";
  }
}

void criarArquivoCSV() {
  if (!SD.exists("/umidade.csv")) {
    File arquivo = SD.open("/umidade.csv", FILE_WRITE);

    if (arquivo) {
      arquivo.println("leitura,valor_adc,umidade_percentual,estado");
      arquivo.close();

      Serial.println("Arquivo umidade.csv criado.");
    }
    else {
      Serial.println("Erro ao criar o arquivo CSV.");
    }
  }
}

void salvarNoCartao(int valorADC, int umidade, String estado) {
  File arquivo = SD.open("/umidade.csv", FILE_APPEND);

  if (!arquivo) {
    Serial.println("Erro ao abrir o arquivo.");
    return;
  }

  arquivo.print(numeroLeitura);
  arquivo.print(",");
  arquivo.print(valorADC);
  arquivo.print(",");
  arquivo.print(umidade);
  arquivo.print(",");
  arquivo.println(estado);

  arquivo.close();

  Serial.println("Leitura salva no microSD.");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  desligarLeds();

  analogReadResolution(12);

  SPI.begin(
    PINO_SD_SCK,
    PINO_SD_MISO,
    PINO_SD_MOSI,
    PINO_SD_CS
  );

  Serial.println();
  Serial.println("Iniciando sistema...");

  if (!SD.begin(PINO_SD_CS, SPI)) {
    Serial.println("Erro ao iniciar o microSD.");

    while (true) {
      digitalWrite(LED_VERMELHO, HIGH);
      delay(300);
      digitalWrite(LED_VERMELHO, LOW);
      delay(300);
    }
  }

  Serial.println("MicroSD iniciado com sucesso.");

  criarArquivoCSV();
}

void loop() {
  int valorADC = analogRead(PINO_SENSOR);

  int umidade = map(
    valorADC,
    VALOR_SECO,
    VALOR_UMIDO,
    0,
    100
  );

  umidade = constrain(umidade, 0, 100);

  String estado = obterEstado(umidade);

  numeroLeitura++;

  atualizarLeds(umidade);

  Serial.println("----------------------------");

  Serial.print("Leitura: ");
  Serial.println(numeroLeitura);

  Serial.print("Valor ADC: ");
  Serial.println(valorADC);

  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println("%");

  Serial.print("Estado: ");
  Serial.println(estado);

  salvarNoCartao(valorADC, umidade, estado);

  delay(3000);
}