// подключение библиотек
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
// определение переменных
#define PERIOD_SENSORS 10000  // период опроса датчиков в мс
#define PERIOD_SEND 15000  // период отправки данных в мс
#define WIFI_SSID "darknet_s"  // название сети wifi
#define WIFI_PASS "1912rotor!"  // пароль сети
int alarm_smoke = -1; // состояние датчика дыма   
int alarm_flame = -1; // состояние датчика огня
int pin_smoke = A0;   // номер пина датчика дыма
int pin_flame = D1;   // номер пина датчика огня
int value_smoke = 0;  // значение датчика дыма
int value_flame = 0;  // значение датчика огня
const String num_smoke = "1"; // номер датчика дыма в api сервере
const String num_flame = "2"; // номер датчика огня в api сервере
const String serverName = "http://192.168.0.111:5000/alyarma/api/v1.0/sensors/";

// инициализация
void setup() { 
  Serial.begin(112500); 
  Serial.println("Init start");
  pinMode(pin_flame, INPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.println(WiFi.localIP());
  Serial.println("Init end");
}

// основной цикл
void loop() {
  checkSmoke();
  checkFlame();
  sendSmoke();
}

void checkSmoke() {
  static uint32_t tmr1;   // переменная таймера
  if (millis() - tmr1 >= PERIOD_SENSORS) {  // ищем разницу
    tmr1 = millis();                   // сброс таймера
    value_smoke = analogRead(pin_smoke);
    if (value_smoke >= 900){
      alarm_smoke = 1;
    }
    else if (value_smoke == 0){
      alarm_smoke = 0;
    }
    else {
      alarm_smoke = -1;
    }
    Serial.println(value_smoke);
  }
}

void checkFlame() {
  static uint32_t tmr1;   // переменная таймера
  if (millis() - tmr1 >= PERIOD_SENSORS) {  // ищем разницу
    tmr1 = millis();                   // сброс таймера
    value_flame = digitalRead(pin_flame);
    if (value_flame == 0){
      alarm_flame = 1;
    }
    else if (value_flame == 1){
      alarm_flame = -1;
    }
    else {
      alarm_flame = 0;
    }
    Serial.print(value_flame);
  }
}
void sendSmoke() {
  static uint32_t tmr1;   // переменная таймера
  static char buff[20];
  if (millis() - tmr1 >= PERIOD_SEND) {  // ищем разницу
    tmr1 = millis();                   // сброс таймера
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      static String sensorPath = serverName + num_smoke;
      // Your Domain name with URL path or IP address with path
      http.begin(client, sensorPath);
      static String json1 = "{\"val\":";
      static String charValue = itoa(value_smoke, buff, 10);
      static String charAlarm = itoa(alarm_smoke, buff, 10);

      // If you need an HTTP request with a content type: application/json, use the following:
      static String jsonString = json1 + charValue + ",\"alarm\":" + charAlarm + "}";
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.PUT(jsonString);

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}