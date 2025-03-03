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

void setup() {
  Serial.begin(115200);
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

void sendCloudData() {
  // Atualiza o tempo
  timeClient.update();

  float sensorValue = random(20, 30);

  String timestamp = String(timeClient.getEpochTime());

  // Enviar dados ao Firebase
  if (Firebase.RTDB.setFloat(&fbdo, "/sensor/historico/" + timestamp + "/temperatura", sensorValue)) {
    Serial.println("Dado do sensor enviado com sucesso!");
  } else {
    Serial.println("Falha ao enviar dado: " + fbdo.errorReason());
  }

  // Enviar dados ao Firebase
  if (Firebase.RTDB.setString(&fbdo, "/sensor/historico/" + timestamp + "/data_hora", timeClient.getFormattedTime())) {
    Serial.println("Dado de tempo enviado com sucesso!");
  } else {
    Serial.println("Falha ao enviar dado: " + fbdo.errorReason());
  }

  delay(5000);
}