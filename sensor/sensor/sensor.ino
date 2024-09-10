// определение переменных
#define PERIOD_SENSORS 1000  // период в мс
int alarm_smoke = -1;     
int alarm_flame = -1;    
int pin_smoke = A0;
int pin_flame = D1;


// инициализация
void setup() { 
  Serial.begin(112500); 
  Serial.println("Init start");
  pinMode(pin_flame, INPUT);
  Serial.println("Init end");
}

// основной цикл
void loop() {
  checkSmoke();
  checkFlame();
}

void checkSmoke() {
  static uint32_t tmr1;   // переменная таймера
  static int a;
  if (millis() - tmr1 >= PERIOD_SENSORS) {  // ищем разницу
    tmr1 = millis();                   // сброс таймера
    a = analogRead(pin_smoke);
    if (a >= 900){
      alarm_smoke = 1;
    }
    else if (a == 0){
      alarm_smoke = 0;
    }
    else {
      alarm_smoke = -1;
    }
    Serial.println(a);
  }
}

void checkFlame() {
  static uint32_t tmr1;   // переменная таймера
  static int a;
  if (millis() - tmr1 >= PERIOD_SENSORS) {  // ищем разницу
    tmr1 = millis();                   // сброс таймера
    a = digitalRead(pin_flame);
    if (a >= 900){
      alarm_flame = 1;
    }
    else if (a == 0){
      alarm_flame = 0;
    }
    else {
      alarm_flame = -1;
    }
    Serial.print(a);
  }
}