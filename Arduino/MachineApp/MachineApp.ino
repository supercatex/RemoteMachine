#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>


#define _LISTEN_LIMIT 500

RF24 _radio(7, 8);
const byte _ADDR[][6] = {"00002", "00001"};
long long int _last_send_time = 0;

Servo _thruster_L, _thruster_R;
int _thruster_PWM_L, _thruster_PWM_R;
#define THRUSTER_PIN_L 9
#define THRUSTER_PIN_R 10
#define THRUSTER_PWM_MIN 1100
#define THRUSTER_PWM_MAX 1900
#define THRUSTER_PWM_MID 1500

struct Data
{
    int x, y;
    int buttons[7];
    int send_id;
    Data(char *cmd)
    {
        this->x = int(cmd[0]) * 128 + int(cmd[1]);
        this->y = int(cmd[2]) * 128 + int(cmd[3]);
        for (int i = 0; i < 7; i++) 
            this->buttons[6 - i] = bool(cmd[4] & (1 << i));
        this->send_id = cmd[15];
    }
};

Data* _data = NULL;

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

Data* receive_data()
{
    char cmd[16] = "";
    _radio.read(&cmd, sizeof(cmd));
    return new Data(cmd);
}

void send_data()
{
    char cmd[16] = "";
    if (_data != NULL)
        cmd[15] = _data->send_id;
    _radio.write(&cmd, sizeof(cmd));
    _last_send_time = millis();
}

void loop() 
{
    delete _data;
    _radio.startListening();
    while (millis() - _last_send_time < _LISTEN_LIMIT)
    {
        if (_radio.available()) 
        {
            _data = receive_data();

            Serial.print("  x:"); Serial.print(_data->x);
            Serial.print(", y:"); Serial.print(_data->y);
            for (int i = 0; i < 7; i++) {
                Serial.print(", "); 
                Serial.print(char('A' + i)); 
                Serial.print(":"); 
                Serial.print(_data->buttons[i]);
            }
            Serial.println("");

            double vx = _data->x - 512, vy = _data->y - 512;
            vx = vx / 512, vy = vy / 512;
            _thruster_PWM_L = THRUSTER_PWM_MID + (vy + vx) / 2 * 100;
            _thruster_PWM_R = THRUSTER_PWM_MID + (vy - vx) / 2 * 100;
            break;
        }
    }

    _radio.stopListening();
    send_data();
/*
    _radio.startListening();
    if (_radio.available()) {
        char cmd[16] = "";
        _radio.read(&cmd, sizeof(cmd));

        int x = int(cmd[0]) * 128 + int(cmd[1]);
        int y = int(cmd[2]) * 128 + int(cmd[3]);
        int b[7];
        for (int i = 0; i < 7; i++) b[6 - i] = bool(cmd[4] & (1 << i));
        int send_id = cmd[15];

        memset(cmd, 0, sizeof(cmd));
        cmd[15] = send_id;
        _radio.stopListening();
        _radio.write(&cmd, sizeof(cmd));
        _radio.startListening();

        double vx = x - 512, vy = y - 512;
        vx = vx / 512, vy = vy / 512;
        _thruster_PWM_L = THRUSTER_PWM_MID + (vy + vx) / 2 * 100;
        _thruster_PWM_R = THRUSTER_PWM_MID + (vy - vx) / 2 * 100;
        //Serial.print("  vx:"); Serial.print(_thruster_PWM_L);
        //Serial.print(", vy:"); Serial.print(_thruster_PWM_R);
        //Serial.println();
/*
        Serial.print("  x:"); Serial.print(x);
        Serial.print(", y:"); Serial.print(y);
        for (int i = 0; i < 7; i++) {
            Serial.print(", "); 
            Serial.print(char('A' + i)); 
            Serial.print(":"); 
            Serial.print(b[i]);
        }
        Serial.println("");
//
    }

    _thruster_L.writeMicroseconds(_thruster_PWM_L);
    _thruster_R.writeMicroseconds(_thruster_PWM_R);
*/
}
