#include <driver/adc.h>
#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED_RED 14
#define LED_YELLOW 12
#define LED_GREEN 13
#define BUZZER 27

// GPIO35
#define MIC_PIN ADC1_CHANNEL_7 
// Limite de decibéis para som alto
#define NOISE_THRESHOLD 80




#define useMaxValue true
#define useAverageValue false

int adcValue = 0;
#define WINDOW_SIZE 100

// Média móvel
int samples[WINDOW_SIZE]; // Array para armazenar as leituras
int sampleIndex = 0;      // Índice atual no array
long sampleSum = 0;       // Soma acumulada das amostras



// Configurações do OLED
#define SCREEN_WIDTH 128     // Largura do OLED
#define SCREEN_HEIGHT 64     // Altura do OLED
#define OLED_RESET    -1     // Reset não utilizado no ESP32
#define SCREEN_ADDRESS 0x3C  // Endereço I2C do OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);





// Constantes para a função logarítmica
double a = 56.79;
double c = -364.60;

// Coeficientes da função reta (entre 2050 e 2262)
double m = (74.72 - 65.58) / (2262 - 2050);  // Inclinação (∆y / ∆x)
double b = 65.58 - m * 2050;                 // Intercepto (y - mx)

// Coeficientes do polinômio (após 2262)
double p2 = 0.000135;
double p1 = -0.5123;
double p0 = 538.10;

// Função para a parte logarítmica
double log_func(double x) {
  return a * log(x) + c;
}

// Função para a parte polinomial
double poly_func(double x) {
  return p2 * x * x + p1 * x + p0;
}

// Função para a parte linear
double linear_func(double x) {
  return m * x + b;
}

// Função conjunta
double piecewise_func(double x) {
  if (x <= 2050) {
    return log_func(x);
  } else if (x >= 2262) {
    return poly_func(x);
  } else {
    return linear_func(x);
  }
}


void setting_for_leds_buzzer() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}



void setup() {
  Serial.begin(115200);

  setting_for_leds_buzzer();

  // Configura o ADC
  // Resolução de 12 bits
  adc1_config_width(ADC_WIDTH_BIT_12);               
  // Atenuação para 0-3.3V
  adc1_config_channel_atten(MIC_PIN, ADC_ATTEN_DB_11); 

  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Falha ao inicializar o OLED!"));
    for (;;);
  }
  display.clearDisplay();

  display.setTextSize(1);                 // Tamanho do texto
  display.setTextColor(SSD1306_WHITE);    // Cor do texto
  display.setCursor(0, 0);
  display.print("Inicializando...");
  display.display();
  
  delay(2000);
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
    double decibels = piecewise_func(adcValue);

    // Garante que o valor seja válido
    if (decibels < 0) decibels = 0;

    // Mostra o nível de som em dB
    Serial.print("Média dB: ");
    Serial.println(decibels);


    // Atualiza o display OLED
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Nivel de Som:");

    display.setTextSize(2);
    display.setCursor(0, 20);
    display.print(decibels);
    display.print(" dB");
    display.display();

    // Verifica se o som está muito alto
    if (decibels > NOISE_THRESHOLD) {
      display.setTextSize(1);
      display.setCursor(0, 50);
      display.print("ALERTA: Muito alto!");
      display.display();

      digitalWrite(LED_RED, true);
      digitalWrite(BUZZER, true);
    }

    if(useMaxValue) {
      adcValue = 0;
    }

    // Reinicia o contador
    sampleCount = 0;
  }

  // Pequeno atraso
  // delay(10); 
}
