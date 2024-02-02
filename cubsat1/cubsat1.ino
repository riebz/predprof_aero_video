#include <Arduino.h>
#include <Hamming.h>

const int laserPin = 9; // Пин для лазера
const int laserPin2 = 10; // Пин для лазера2

unsigned long start_time; // Переменная для хранения времени старта


Hamming<4> buf;

void setup() {
  pinMode(laserPin, OUTPUT);
  pinMode(laserPin2, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  char message[]  = "Status: Warning"; // Сообщение для передачи
  
  buf.pack(message);
  for (int i = 0; i<2;i++){
    buf.pack(message);
    digitalWrite(laserPin, 255);
    digitalWrite(laserPin2, 255);  
    delay(1000);
    peredatchik();
  }
    char message1[]  = "Status: Working";
    buf.pack(message1);
    digitalWrite(laserPin, 255);
    digitalWrite(laserPin2, 255);  
    delay(1000);
    peredatchik();
}


void stringToAscii(String input, char *output) {
  for (int i = 0; i < input.length(); i++) {
    output[i] = input.charAt(i);
  }
}

void peredatchik(){
  int len = buf.length();
  for (int i = 0; i < (len); i++) {
     
      String m = intToBinary(buf.buffer[i]);
       Serial.print(m);
       Serial.print(" ");
      
      for (int j = 0; j < m.length(); j++) {
        start_time = micros();
        if (m[j] == '1') {
          digitalWrite(laserPin, 255);
          digitalWrite(laserPin2, 255);
        }
        // If the current character is a 0, turn the laser off
        else {
          digitalWrite(laserPin, 0);
          digitalWrite(laserPin2, 0);
        }
        delayMicroseconds(600000-(micros()-start_time));
      }
    } 
  digitalWrite(laserPin, 0);
  digitalWrite(laserPin2, 0);
  delay(500);
  Serial.println();
    
}

String intToBinary(int value) {
  String binaryString = "";
  for (int i = 8; i>=0; i--) {
    if (bitRead(value, i)) {
      binaryString += "1";
    } else {
      binaryString += "0";
    }
  }
  return binaryString;

}
