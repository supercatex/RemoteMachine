#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 _radio(7, 8);
const byte _ADDR[][6] = {"00002", "00001"};

Servo _thruster_L, _thruster_R;
int _thruster_PWM_L, _thruster_PWM_R;
#define THRUSTER_PIN_L 9
#define THRUSTER_PIN_R 10
#define THRUSTER_PWM_MIN 1100
#define THRUSTER_PWM_MAX 1900
#define THRUSTER_PWM_MID 1500


void setup() 
{
    Serial.begin(115200);

    _radio.begin();
    _radio.openWritingPipe(_ADDR[1]);
    _radio.openReadingPipe(0, _ADDR[0]);
    _radio.setPALevel(RF24_PA_MIN);

    _thruster_PWM_L = THRUSTER_PWM_MID;
    _thruster_PWM_R = THRUSTER_PWM_MID;
    _thruster_L.attach(THRUSTER_PIN_L);
    _thruster_R.attach(THRUSTER_PIN_R);
    _thruster_L.writeMicroseconds(_thruster_PWM_L);
    _thruster_R.writeMicroseconds(_thruster_PWM_R);
    delay(1000);
}

void loop() 
{
    _radio.startListening();
    if (_radio.available()) {
        char cmd[16] = "";
        _radio.read(&cmd, sizeof(cmd));

        int x = int(cmd[0]) * 128 + int(cmd[1]);
        int y = int(cmd[2]) * 128 + int(cmd[3]);
        int b[7];
        for (int i = 0; i < 7; i++) b[6 - i] = bool(cmd[4] & (1 << i));

        double vx = x - 512, vy = y - 512;
        vx = vx / 512, vy = vy / 512;
        _thruster_PWM_L = THRUSTER_PWM_MID + (vy + vx) / 2 * 100;
        _thruster_PWM_R = THRUSTER_PWM_MID + (vy - vx) / 2 * 100;
        //Serial.print("  vx:"); Serial.print(_thruster_PWM_L);
        //Serial.print(", vy:"); Serial.print(_thruster_PWM_R);
        //Serial.println();
//*
        Serial.print("  x:"); Serial.print(x);
        Serial.print(", y:"); Serial.print(y);
        for (int i = 0; i < 7; i++) {
            Serial.print(", "); 
            Serial.print(char('A' + i)); 
            Serial.print(":"); 
            Serial.print(b[i]);
        }
        Serial.println("");

        for (int i = 0; i < 16; i++) cmd[i] = 'A';
        //_radio.stopListening();
        //_radio.write(&cmd, sizeof(cmd));
//*/
    }

    _thruster_L.writeMicroseconds(_thruster_PWM_L);
    _thruster_R.writeMicroseconds(_thruster_PWM_R);
}
