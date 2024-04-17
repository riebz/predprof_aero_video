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
const int maxMessages = 4; // Максимальное кол-во сохраняемых сообщений
String messages[maxMessages];
int yPos[maxMessages];
//--------------------- ПЕРЕМЕННЫЕ ----------------------


void setup() {
  Serial.begin(115200);
  myOLED.begin();
  myOLED.setFont(SmallFont);
  radioSetup();
  // Инициализируем позиции сообщений
  for (int i = 0; i < maxMessages; i++) {
    yPos[i] = i * 12; // высота строки 12 пикселей
  }
}

void loop() {
  while (radio.available(&pipeNo)) {
    radio.read(&recieved_data, sizeof(recieved_data));
    String newMessage;
    for (int i = 0; i < (sizeof(recieved_data) / sizeof(*recieved_data)); i++) {
      if (recieved_data[i] != 0) {
        newMessage += (char)recieved_data[i];
      }
    }
    Serial.println(newMessage);
    displayNewMessage(newMessage);
    
    // Здесь остается ваш код, связанный с телеметрией и отправкой данных...
  }
}

void displayNewMessage(String message) {
  myOLED.clrScr(); // Очищаем экран
  // Сдвигаем предыдущие сообщения вниз
  for (int i = maxMessages - 1; i > 0; i--) {
    messages[i] = messages[i - 1];
    yPos[i] = yPos[i - 1] + 12;
  }
  // Добавляем новое сообщение на верх экрана
  messages[0] = message;
  yPos[0] = 0;
  // Выводим сообщения на экран
  for (int i = 0; i < maxMessages; i++) {
    if (yPos[i] < myOLED.getHeight()) { // Если сообщение находится в пределах экрана
      myOLED.print(messages[i], CENTER, yPos[i]);
    }
  }
  myOLED.update();
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
