// подключение библиотек
#include <ESP8266WiFi.h>  // обеспечение сетевого подключения (локальное и Интернет) с помощью модуля esp8266.
#include <WiFiClient.h>  // обеспечивает отправку данных по сети
#include <ESP8266HTTPClient.h>  // обеспечивает легкую коммуникацию с HTTP-сервером
// определение переменных
#define PERIOD_SENSORS 10000  // период опроса датчиков в мс
#define PERIOD_SEND 15000  // период отправки данных в мс
#define smoke_threshold 500 // порог срабатывания датчика дыма
#define WIFI_SSID "darknet_s"  // название сети wifi
#define WIFI_PASS "1912rotor!"  // пароль сети
int alarm_smoke = -1; // состояние датчика дыма   
int alarm_flame = -1; // состояние датчика огня
int pin_smoke = A0;   // номер пина датчика дыма
int pin_flame = D1;   // номер пина датчика огня
int value_smoke = 0;  // значение датчика дыма
int value_flame = 0;  // значение датчика огня
String json_string_smoke;  // строка, отправляемая как json на HTTP-сервер, с данными с датчика дыма
String json_string_flame;  // строка, отправляемая как json на HTTP-сервер, с данными с датчика огня
const String num_smoke = "1"; // номер датчика дыма в api сервере
const String num_flame = "2"; // номер датчика огня в api сервере
const String server_name = "http://192.168.0.111:5000/alyarma/api/v1.0/sensors/"; // строка подключения к API-серверу

// инициализация
void setup() { 
  Serial.begin(112500); // подключение вывода данных с esp8266 в консоль
  Serial.println("Init start");  // вывод информации о начале инициализации программы в консоль
  pinMode(pin_flame, INPUT);  // изменение типа работы входа сигнала с датчика огня
  WiFi.begin(WIFI_SSID, WIFI_PASS); // подключение к сети WiFi
  while (WiFi.status() != WL_CONNECTED) { // ждем подключения к WiFi
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected"); // вывод на консоль успешного подключения к WiFi
  Serial.println(WiFi.localIP()); // вывод на консоль IP-адрес устройства
  Serial.println("Init end"); // вывод на консоль информации о конце инициализации
}

// основной цикл
void loop() {
  checkSmoke(); 
  checkFlame();
  sendSmoke();
  sendFlame();
  
}

void checkSmoke() { // функция считывания данных с датчика дыма
  static uint32_t tmr1;   // переменная таймера
  if (millis() - tmr1 >= PERIOD_SENSORS) {  // ищем разницу между текущем временем и временем, записанным в переменную таймер, если эта разница больше периода опроса, то считываем данные с датчика
    tmr1 = millis();                   // приравнивание таймера к текущему времени
    value_smoke = analogRead(pin_smoke); // считываем данные с датчика дыма
    if (value_smoke >= smoke_threshold) { // сравниваем данные с датчиком и значением порога срабатывания, и если данные с датчика больше, то устанавливается флаг о тревоге 
      alarm_smoke = 1; // устанавливается флаг о тревоге 
    }
    else if (value_smoke <= 10) { // если значение меньше 10, значит, что датчик не работает
      alarm_smoke = 0; // устанавливается флаг о отсутствии данных с датчика
    }
    else { // в других случаях датчик работает нормально 
      alarm_smoke = -1;
    }
  }
}

void checkFlame() { // функция считывания данных с датчика огня
  static uint32_t tmr1;   // переменная таймера
  if (millis() - tmr1 >= PERIOD_SENSORS) {  // ищем разницу между текущем временем и временем, записанным в переменную таймер, если эта разница больше периода опроса, то считываем данные с датчика
    tmr1 = millis();                   // приравнивание таймера к текущему времени
    value_flame = digitalRead(pin_flame); // считываем данные с датчика огня
    if (value_flame == 0) { // сравниваем данные с датчиком и значением порога срабатывания, и если данные с датчика больше, то устанавливается флаг о тревоге 
      alarm_flame = 1; // устанавливается флаг о тревоге
    }
    else if (value_flame == 1) { // датчик работает нормально
      alarm_flame = -1;
    }
    else { // в других случаях датчик работает неправильно
      alarm_flame = 0;
    }
  }
}
void sendSmoke() {  // функция отправки данных на api сервер с датчика дыма
  static uint32_t tmr1;   // переменная таймера
  if (millis() - tmr1 >= PERIOD_SEND) {  // ищем разницу
    tmr1 = millis();                   // сброс таймера
    if(WiFi.status()== WL_CONNECTED) { // проверяем подключение к WiFi
      WiFiClient client; // инициализируем подключение к API-серверу
      HTTPClient http;  // инициализируем отправку данных по HTTP
      static String sensorPath = server_name + num_smoke; // объеденяем название сервера и номер датчика
      http.begin(client, sensorPath);  // начинаем передачу данных

      static char char_value[20];  // переменная для преобразования типа данных
      sprintf(char_value, "%d", value_smoke);  // преобразуем типы данных

      static char char_alarm[20];  // переменная для преобразования типа данных
      sprintf(char_alarm, "%d", alarm_smoke);  // преобразуем типы данных
      static String str1 = "{\"val\":"; // вспомогательная переменная
      static String str2 = ",\"alarm\":";  // вспомогательная переменная
      static String str3 = "}";  // вспомогательная переменная


      json_string_smoke = str1 + char_value + str2 + char_alarm + str3; // строка, отправляемая как json на HTTP-сервер, с данными с датчика дыма
      http.addHeader("Content-Type", "application/json"); // формирование HTTP пакета
      int httpResponseCode = http.PUT(json_string_smoke); // отправка HTTP пакета
      
      if (httpResponseCode != 200) {  // если не получен положительный ответ от сервера, то сигнализируем об ошибке
        Serial.println("Server error"); // здесь будет отпрака сигнала с gsm
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
      static String sensorPath = server_name + num_flame;
      // Your Domain name with URL path or IP address with path
      http.begin(client, sensorPath);
      static char char_value[20];
      sprintf(char_value, "%d", value_flame);

      static char char_alarm[20];
      sprintf(char_alarm, "%d", alarm_flame);
      static String str1 = "{\"val\":";
      static String str2 = ",\"alarm\":";
      static String str3 = "}";

      // If you need an HTTP request with a content type: application/json, use the following:
      json_string_flame = str1 + char_value + str2 + char_alarm + str3;
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.PUT(json_string_flame);
     
      //Serial.print("HTTP Response code: ");
      //Serial.println(httpResponseCode);
      if (httpResponseCode != 200) {
        Serial.println("Server error");
      }
      Serial.println(value_flame);
      Serial.println(alarm_flame);
      Serial.println(char_value);
      Serial.println(char_alarm);
      Serial.println(json_string_flame);
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}

