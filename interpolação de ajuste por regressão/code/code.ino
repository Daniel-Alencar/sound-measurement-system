#include <driver/adc.h>
#include <math.h>

// GPIO35
#define MIC_PIN ADC1_CHANNEL_7 
// Limite de decibéis para som alto
#define NOISE_THRESHOLD 80

// Constantes calculadas para a função logarítmica
double a = 56.79;
double c = -364;



#define useMaxValue true
#define useAverageValue false

int adcValue = 0;
#define WINDOW_SIZE 100

// Média móvel
int samples[WINDOW_SIZE]; // Array para armazenar as leituras
int sampleIndex = 0;      // Índice atual no array
long sampleSum = 0;       // Soma acumulada das amostras




void setup() {
  Serial.begin(115200);

  // Configura o ADC
  // Resolução de 12 bits
  adc1_config_width(ADC_WIDTH_BIT_12);               
  // Atenuação para 0-3.3V
  adc1_config_channel_atten(MIC_PIN, ADC_ATTEN_DB_11); 
}

void loop() {
  // Lê o sinal analógico
  int audioValue = adc1_get_raw(MIC_PIN);
  // Serial.println(audioValue);



  static int sampleCount = 0;
  if(useMaxValue) {
    // Calcula o valor máximo (pico) em uma janela de tempo
    if (audioValue > adcValue) {
      adcValue = audioValue;
    }
  } else if(useAverageValue) {
    // Atualiza a soma acumulada para a média móvel
    sampleSum -= samples[sampleIndex];  // Remove o valor mais antigo
    samples[sampleIndex] = audioValue;  // Adiciona o novo valor
    sampleSum += audioValue;            // Adiciona o novo valor à soma

    // Atualiza o índice, mantendo-o circular
    sampleIndex = (sampleIndex + 1) % WINDOW_SIZE;
  }


  sampleCount++;
  if (sampleCount >= WINDOW_SIZE) {

    if(useAverageValue) {
      // Calcula a média
      adcValue = (double)sampleSum / WINDOW_SIZE;
    }
    // Serial.print("Value: ");
    Serial.println(adcValue);

    // Calcula o nível em dB
    double decibels = a * log(adcValue) + c;

    // Garante que o valor seja válido
    if (decibels < 0) decibels = 0;

    // Mostra o nível de som em dB
    Serial.print("Média dB: ");
    Serial.println(decibels);

    // Verifica se o som está muito alto
    if (decibels > NOISE_THRESHOLD) {
      Serial.println("ALERTA: Som muito alto!");
    }

    if(useMaxValue)
      adcValue = 0;

    // Reinicia o contador
    sampleCount = 0;
  }

  // Pequeno atraso
  delay(10); 
}
