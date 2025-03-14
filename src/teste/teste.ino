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

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Definição do Wi-Fi
const char* WIFI_SSID = "PNZ_NET - DANIEL";
const char* WIFI_PASSWORD = "principedapaz";

const char* email = "danielalencar746@gmai.com";
const char* password = "@nivel-sonoro123";

// Configuração do Firebase
#define API_KEY "AIzaSyCVFiI9HEj1sND2vBjMSE0zONs66YEBgKE"
#define DATABASE_URL "https://nivel-sonoro-default-rtdb.firebaseio.com/" 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Configuração do NTP (servidor de hora)
WiFiUDP udp;
// UTC time, atualiza a cada 60s
NTPClient timeClient(udp, "pool.ntp.org", 0, 60000);
// Fuso horário UTC-3 (Brasília)
const long utcOffsetInSeconds = -3 * 3600;

void setupCloud() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConectado ao Wi-Fi!");

  // Configurar o Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Autenticação anônima
  auth.user.email = email;
  auth.user.password = password;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);


  // Iniciar o NTP Client
  timeClient.begin();
  // Ajuste para o fuso horário UTC-3
  timeClient.setTimeOffset(utcOffsetInSeconds);
  timeClient.update();
}

void sendCloudData(double sensorValue) {
  // Atualiza o tempo
  timeClient.update();

  // Obtém a data como timestamp (epoch)
  unsigned long epochTime = timeClient.getEpochTime();
  String timestamp = String(epochTime);

  // Converte para uma string mais legível, por exemplo, "dd-mm-yyyy"
  String dateString = "14-03-2025";

  // Enviar dados ao Firebase
  if (Firebase.RTDB.setFloat(&fbdo, "/" + dateString + "/dados_microfone/" + timestamp + "/microfone", sensorValue)) {
    Serial.println("Dado do sensor enviado com sucesso!");
  } else {
    Serial.println("Falha ao enviar dado: " + fbdo.errorReason());
  }

  // Enviar dados ao Firebase
  if (Firebase.RTDB.setString(&fbdo, "/" + dateString + "/dados_microfone/" + timestamp + "/data_hora", timeClient.getFormattedTime())) {
    Serial.println("Dado de tempo enviado com sucesso!");
  } else {
    Serial.println("Falha ao enviar dado: " + fbdo.errorReason());
  }
}



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
  setupCloud();

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

    // Envia dados para a cloud
    sendCloudData(decibels);

    if(useMaxValue) adcValue = 0;
    sampleCount = 0;
  }
  delay(2);
}
