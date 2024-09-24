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
String jsonStringSmoke;
String jsonStringFlame;
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
  sendFlame();
  
}

void checkSmoke() {
  static uint32_t tmr1;   // переменная таймера
  if (millis() - tmr1 >= PERIOD_SENSORS) {  // ищем разницу
    tmr1 = millis();                   // сброс таймера
    value_smoke = analogRead(pin_smoke);
    if (value_smoke >= 500){
      alarm_smoke = 1;
    }
    else if (value_smoke == 0){
      alarm_smoke = 0;
    }
    else {
      alarm_smoke = -1;
    }
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
    //Serial.print(value_flame);
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

      static char charValue[20];
      sprintf(charValue, "%d", value_smoke);

      static char charAlarm[20];
      sprintf(charAlarm, "%d", alarm_smoke);
      static String str1 = "{\"val\":";
      static String str2 = ",\"alarm\":";
      static String str3 = "}";

      // If you need an HTTP request with a content type: application/json, use the following:
      jsonStringSmoke = str1 + charValue + str2 + charAlarm + str3;
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.PUT(jsonStringSmoke);
      
      //Serial.print("HTTP Response code: ");
      //Serial.println(httpResponseCode);
      if (httpResponseCode != 200) {
        Serial.println("Server error");
      }
      

      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}


void sendFlame() {
  static uint32_t tmr1;   // переменная таймера
  static char buff[20];
  if (millis() - tmr1 >= PERIOD_SEND) {  // ищем разницу
    tmr1 = millis();                   // сброс таймера
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      static String sensorPath = serverName + num_flame;
      // Your Domain name with URL path or IP address with path
      http.begin(client, sensorPath);
      static char charValue[20];
      sprintf(charValue, "%d", value_flame);

      static char charAlarm[20];
      sprintf(charAlarm, "%d", alarm_flame);
      static String str1 = "{\"val\":";
      static String str2 = ",\"alarm\":";
      static String str3 = "}";

      // If you need an HTTP request with a content type: application/json, use the following:
      jsonStringFlame = str1 + charValue + str2 + charAlarm + str3;
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.PUT(jsonStringFlame);
     
      //Serial.print("HTTP Response code: ");
      //Serial.println(httpResponseCode);
      if (httpResponseCode != 200) {
        Serial.println("Server error");
      }
      Serial.println(value_flame);
      Serial.println(alarm_flame);
      Serial.println(charValue);
      Serial.println(charAlarm);
      Serial.println(jsonStringFlame);
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}

