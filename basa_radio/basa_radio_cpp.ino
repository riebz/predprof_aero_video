// ЭТО СКЕТЧ ПРИЁМНИКА!!!

//--------------------- НАСТРОЙКИ ----------------------
#define CH_NUM 0x60   // номер канала (должен совпадать с передатчиком)
//--------------------- НАСТРОЙКИ ----------------------

//--------------------- БИБЛИОТЕКИ ----------------------
#include "OLED_I2C.h"           // подключаем библиотеку для экрана
OLED myOLED(SDA, SCL, 8);  // создаем объект myOLED

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
RF24 radio(9, 10); 
//--------------------- БИБЛИОТЕКИ ----------------------

//--------------------- ПЕРЕМЕННЫЕ ----------------------
byte pipeNo;

int recieved_data[100];   // массив принятых данных
int telemetry[1];       // массив данных телеметрии (то что шлём на передатчик)

// подключаем шрифты для текста и цифр
extern uint8_t SmallFont[];
//--------------------- ПЕРЕМЕННЫЕ ----------------------

void setup() {
  Serial.begin(115200);
   myOLED.begin();
  radioSetup();
}

void loop() {
  while (radio.available(&pipeNo)) {                    // слушаем эфир
    radio.read(&recieved_data, sizeof(recieved_data));  // чиатем входящий сигнал
    String res = "";
    
   myOLED.clrScr(); // очищаем экран от надписей
    for (int i = 0; i < (sizeof(recieved_data)/sizeof(*recieved_data)); i++){
  if (recieved_data[i] != 0) {
    Serial.print((char)recieved_data[i]);
    res+=(char)recieved_data[i];
    // выводим текст по центру экрана
    
  }
}
  myOLED.setFont(SmallFont);
      myOLED.print(res, CENTER, 10);
      myOLED.update();
  Serial.println();
    // формируем пакет данных телеметрии (напряжение АКБ, скорость, температура...)
    if (Serial.available()) {
    String message = Serial.readString();
    // Если сообщение содержит команду "update", отправляем команду обновления на 2-й кубсат
    if (message.indexOf("update") == 0) {
      telemetry[0] = 1; 
      Serial.println(message);
    }
    } else{
      telemetry[0] = 0; 
    }
    
    // отправляем пакет телеметрии
    radio.writeAckPayload(pipeNo, &telemetry, sizeof(telemetry));
  }
}

void radioSetup() {             // настройка радио
  radio.begin();                // активировать модуль
  radio.setAutoAck(1);          // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);      // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();     // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     // размер пакета, байт
  radio.openReadingPipe(1, 1234567); // хотим слушать трубу 0
  radio.setChannel(CH_NUM);     // выбираем канал (в котором нет шумов!)
  radio.setPALevel(RF24_PA_MIN);  // уровень мощности передатчика
  radio.setDataRate(RF24_1MBPS); // скорость обмена
  // должна быть одинакова на приёмнике и передатчике!
  // при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp();         // начать работу
  radio.startListening();  // начинаем слушать эфир, мы приёмный модуль
}





