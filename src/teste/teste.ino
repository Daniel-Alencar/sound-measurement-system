#include <driver/adc.h>
#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED_RED 14
#define LED_YELLOW 12
#define LED_GREEN 13
#define BUZZER 27

#define MIC_PIN ADC1_CHANNEL_7 
#define NOISE_THRESHOLD 80

#define useMaxValue true
#define useAverageValue false

int adcValue = 0;
#define WINDOW_SIZE 100

int samples[WINDOW_SIZE];
int sampleIndex = 0;
long sampleSum = 0;

#define SCREEN_WIDTH 128     
#define SCREEN_HEIGHT 64     
#define OLED_RESET -1        
#define SCREEN_ADDRESS 0x3C  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Dados para interpolação
const int adc_points[] = {1419, 1434, 1456, 1480, 1500, 1520, 1650, 1900, 2100, 2300};
const int db_points[]  = {40, 45, 52, 56, 59, 61, 65, 70, 75, 80};
const int num_points = sizeof(adc_points) / sizeof(adc_points[0]);

// Interpolação Linear
double interpolate_linear(int adc_value) {
  if (adc_value <= adc_points[0]) return db_points[0];
  if (adc_value >= adc_points[num_points - 1]) return db_points[num_points - 1];

  for (int i = 0; i < num_points - 1; i++) {
    if (adc_value >= adc_points[i] && adc_value <= adc_points[i + 1]) {
      double slope = (double)(db_points[i + 1] - db_points[i]) 
                        / (adc_points[i + 1] - adc_points[i]);
      return db_points[i] + slope * (adc_value - adc_points[i]);
    }
  }
  return db_points[0];
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

  adc1_config_width(ADC_WIDTH_BIT_12);               
  adc1_config_channel_atten(MIC_PIN, ADC_ATTEN_DB_11); 

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Falha ao inicializar o OLED!"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Inicializando...");
  display.display();
  delay(2000);
}

void loop() {
  int audioValue = adc1_get_raw(MIC_PIN);
  
  static int sampleCount = 0;
  if(useMaxValue) {
    if (audioValue > adcValue) adcValue = audioValue;
  } else if(useAverageValue) {
    sampleSum -= samples[sampleIndex];
    samples[sampleIndex] = audioValue;
    sampleSum += audioValue;
    sampleIndex = (sampleIndex + 1) % WINDOW_SIZE;
  }

  sampleCount++;
  if (sampleCount >= WINDOW_SIZE) {
    if(useAverageValue) {
      adcValue = (double)sampleSum / WINDOW_SIZE;
    }

    Serial.println(adcValue);
    double decibels = interpolate_linear(adcValue);
    if (decibels < 0) decibels = 0;

    Serial.print("Média dB: ");
    Serial.println(decibels);

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Nivel de Som:");

    display.setTextSize(2);
    display.setCursor(0, 20);
    display.print(decibels);
    display.print(" dB");
    display.display();

    if (decibels >= NOISE_THRESHOLD) {
      display.setTextSize(1);
      display.setCursor(0, 50);
      display.print("ALERTA: Muito alto!");
      display.display();
      digitalWrite(LED_RED, true);
      digitalWrite(BUZZER, true);
    }

    if(useMaxValue) adcValue = 0;
    sampleCount = 0;
  }
  delay(2);
}
