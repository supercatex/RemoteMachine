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
#define _PIN_RADIO_CNS 9
#define _PIN_RADIO_MOSI 10
#define _LISTEN_LIMIT 50

RF24 _radio(_PIN_RADIO_CNS, _PIN_RADIO_MOSI);
const byte _ADDR[][6] = {"00001", "00002"};
long long int _last_send_time = 0;
int _send_id = 0;

void setup()
{
    Serial.begin(115200);
    for (int i = _PIN_BUTTON_A; i <= _PIN_BUTTON_A + _N_BUTTON; i++) 
    {
        pinMode(i, INPUT);
        digitalWrite(i, HIGH);
    }

    _radio.begin();
    _radio.openWritingPipe(_ADDR[1]);
    _radio.openReadingPipe(0, _ADDR[0]);
    _radio.setPALevel(RF24_PA_MIN);

    _last_send_time = 0;
    _send_id = 0;
}

void send_data()
{
    int x = analogRead(_PIN_ANALOG_X);
    int y = analogRead(_PIN_ANALOG_Y);
    int b = 0;
    for (int i = 0; i < _N_BUTTON; i++) 
        b = (b << 1) | !digitalRead(i + 2);
    
                //0123456789ABCDEF
    char cmd[] = "0000000000000000";
    cmd[0] = int((x % 10000) / 128);   // x high
    cmd[1] = int((x % 10000) % 128);   // x low
    cmd[2] = int((y % 10000) / 128);   // y high
    cmd[3] = int((y % 10000) % 128);   // y low
    cmd[4] = b;
    cmd[15] = _send_id;
    _radio.write(&cmd, sizeof(cmd));

    _last_send_time = millis();
    _send_id = (_send_id + 1) % 128;

    //Serial.print(_send_id);
    //Serial.print("->");
    //Serial.println("Send data.");
}

void receive_data()
{
    char cmd[16] = "";
    _radio.read(&cmd, sizeof(cmd));
    Serial.print("->");
    Serial.print(int(cmd[15]));
    Serial.println("");
}

void loop()
{
    _radio.stopListening();
    send_data();
    _radio.startListening();

    while (millis() - _last_send_time < _LISTEN_LIMIT)
    {
        if (_radio.available()) 
        {
            receive_data();
            break;
        }
    }
}
