#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define _PIN_ANALOG_X 0
#define _PIN_ANALOG_Y 1
#define _PIN_BUTTON_A 2
#define _PIN_BUTTON_B 3
#define _PIN_BUTTON_C 4
#define _PIN_BUTTON_D 5
#define _PIN_BUTTON_E 6
#define _PIN_BUTTON_F 7
#define _PIN_BUTTON_K 8
#define _N_BUTTON 7

RF24 _radio(9, 10);
const byte _ADDR[][6] = {"00001", "00002"};

void setup() 
{
    Serial.begin(9600);
    for (int i = _PIN_BUTTON_A; i <= _PIN_BUTTON_A + _N_BUTTON; i++) {
        pinMode(i, INPUT);
        digitalWrite(i, HIGH);
    }

    _radio.begin();
    _radio.openWritingPipe(_ADDR[1]);
    _radio.openReadingPipe(0, _ADDR[0]);
    _radio.setPALevel(RF24_PA_MIN);
    
}

void loop() 
{
    delay(5);
    _radio.stopListening();
    
    int x = analogRead(_PIN_ANALOG_X);
    int y = analogRead(_PIN_ANALOG_Y);
    int b[_N_BUTTON];
    for (int i = 0; i < _N_BUTTON; i++) 
        b[i] = !digitalRead(i + 2);
/*
    Serial.print("  x:"); Serial.print(x);
    Serial.print(", y:"); Serial.print(y);
    for (int i = 0; i < _N_BUTTON; i++) {
        Serial.print(", "); 
        Serial.print(char('A' + i)); 
        Serial.print(":"); 
        Serial.print(b[i]);
    }
*/
                //0123456789ABCDEF
    char cmd[] = "0000000000000000";
    cmd[0] = '0' + (x % 10000) / 1000;
    cmd[1] = '0' + (x % 1000) / 100;
    cmd[2] = '0' + (x % 100) / 10;
    cmd[3] = '0' + (x % 10) / 1;
    cmd[4] = '0' + (y % 10000) / 1000;
    cmd[5] = '0' + (y % 1000) / 100;
    cmd[6] = '0' + (y % 100) / 10;
    cmd[7] = '0' + (y % 10) / 1;
    for (int i = 0; i < _N_BUTTON; i++) cmd[8 + i] = '0' + b[i];
/*
    Serial.print(" --> Send:"); Serial.println(cmd);
*/
    cmd[8] = '1';
    _radio.write(&cmd, sizeof(cmd));

/*
    delay(5);
    _radio.startListening();
    if (_radio.available()) {
        char data[16] = "";
        _radio.read(&data, sizeof(data));
        Serial.println(data);
    }
*/
}
