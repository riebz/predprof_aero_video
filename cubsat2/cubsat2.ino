
// ЭТО СКЕТЧ ПЕРЕДАТЧИКА!!!

//--------------------- НАСТРОЙКИ ----------------------
#define CH_NUM 0x60   // номер канала (должен совпадать с приёмником)
#define ANALOG_PIN A0
#define SAMPLE_SIZE 100
#define BAIT_SIZE 24
#define NOISE 1.2
//--------------------- НАСТРОЙКИ ----------------------

//--------------------- БИБЛИОТЕКИ ----------------------
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Hamming.h>
RF24 radio(9, 10); 
Hamming<4> buf;
//--------------------- БИБЛИОТЕКИ ----------------------

//--------------------- ПЕРЕМЕННЫЕ ----------------------

int transmit_data[100];     // массив пересылаемых данных
int telemetry[1];   // массив принятых от приёмника данных телеметрии

float samples[SAMPLE_SIZE]; // Массив для хранения сэмплов
String bait[BAIT_SIZE]; // Массив для хранения битов
uint8_t ham[BAIT_SIZE]; // Массив для хранения хамов
unsigned long start_time; // Переменная для хранения времени старта
float average; // Среднее значение сэмплов
bool start_bit_received = false; // Флаг для отслеживания получения стартового бита
String bit = "";

//--------------------- ПЕРЕМЕННЫЕ ----------------------

void setup() {
  Serial.begin(115200); // открываем порт для связи с ПК
  radioSetup();

  start_time = millis();
  calculateAverage();
}

void loop() {
  int value = analogRead(ANALOG_PIN); // Считываем значение с аналогового пина
  if (value > (average * 0.3164750816664893 + 204.52282398275705)) {
      
    } else {
     start_time = millis(); // Выводим 0, если значение ниже порогового значения
    }

  
  if (millis() - start_time > 990) {
    delayMicroseconds(600000); // Задержка в 40 миллисекунд между битами
    
    priem();
     // Устанавливаем флаг получения стартового бита
     start_time = millis();

  }
  
  delayMicroseconds(50); // Задержка в 40 миллисекунд между битами
}

void priem() {
  for (int i = 0; i < BAIT_SIZE; i++) {
    bit = "";
    for (int j = 0; j < 9; j++){ 
      start_time = micros();
      int value = analogRead(ANALOG_PIN); // Считываем значение с аналогового пина
      if (value > (average * 0.3164750816664893 + 204.52282398275705)) {
       // Выводим 1, если значение выше порогового значения
       bit += "1";
      } else {
         // Выводим 0, если значение ниже порогового значения
        bit += "0";
      }
      delayMicroseconds(600000-(micros()-start_time)); // Задержка в 40 миллисекунд между битами
    }
    bait[i] = bit;
  }
  Serial.println();
  for (int i = 0; i < BAIT_SIZE; i++){
    ham[i] = binaryToInt(bait[i]);
  }
  buf.unpack(ham, BAIT_SIZE);
  Serial.println((char*)buf.buffer);
  for (int i = 0; i < buf.length(); i++){
    transmit_data[i] = buf.buffer[i];
  }

  if (radio.write(&transmit_data, sizeof(transmit_data))) {
      if (!radio.available()) {  
        Serial.println(1); // если получаем пустой ответ
      } else {
        while (radio.available() ) {                    // если в ответе что-то есть
          radio.read(&telemetry, sizeof(telemetry));    // читаем
          Serial.println(telemetry[0]);
          if (telemetry[0]!=0){
            Serial.println(1);
            start_time = millis();
            calculateAverage();
          }
          
          // получили забитый данными массив telemetry ответа от приёмника
        }
      }
    } else{
      Serial.println(0);
    }
    
}
void radioSetup() {
  radio.begin();              // активировать модуль
  radio.setAutoAck(1);        // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();   // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);   // размер пакета, в байтах
  radio.openWritingPipe(1234567);   // мы - труба 0, открываем канал для передачи данных
  radio.setChannel(CH_NUM);            // выбираем канал (в котором нет шумов!)
  radio.setPALevel(RF24_PA_LOW);         // уровень мощности передатчика
  radio.setDataRate(RF24_1MBPS);        // скорость обмена
  // должна быть одинакова на приёмнике и передатчике!
  // при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp();         // начать работу
  radio.stopListening();   // не слушаем радиоэфир, мы передатчик
}

void calculateAverage() {
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    samples[i] = analogRead(ANALOG_PIN); // Считываем SAMPLE_SIZE сэмплов с аналогового пина
  }
  float sum = 0;
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    sum += samples[i]; // Суммируем все сэмплы
  }
  average = sum / SAMPLE_SIZE; // Вычисляем среднее значение сэмплов
  Serial.println(average);
}


int binaryToInt(String binaryString) {
  int value = 0;
  for (int i = 0; i < binaryString.length(); i++) {
    value = value << 1;
    if (binaryString[i] == '1') {
      value = value | 1;
    }
  }
  return value;
}
