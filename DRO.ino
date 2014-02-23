
// DRO.ino

#include "utility/twi.h"
#include "Wire.h"

#include "LCDSmoothiePanel.h"

#define LE_ENCA 2
#define LE_ENCB 3

int readEncoderDelta(char &old_AB)
{
    static char enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
    char state;
    old_AB <<= 2;                   //remember previous state
    old_AB |= ( digitalRead(LE_ENCA) + ( digitalRead(LE_ENCB) * 2 ) );  // add current state
    state = enc_states[(old_AB & 0x0f)];
    return state;
}


void setup()
{
    //Serial.begin(115200);
    pinMode(LE_ENCA, INPUT);
    pinMode(LE_ENCB, INPUT);

    lcd_init();

    // Print a message to the LCD.
    lcd_setCursor(0, 0);
    lcd_print(".....DRO.....");

    //Wire.begin();
}

long pos = 0;
long lastpos = 0;
unsigned long lt1 = 0, lt2 = 0;
char enc1= 0, enc2= 0;
void loop()
{
    char buf[10];
    // Wire.beginTransmission(0x20);
    // for (int i = 0; i < 10; ++i) {
    //    Wire.write((uint8_t)i);
    // }
    // Wire.endTransmission();

    pos += readEncoderDelta(enc1);
    if(pos != lastpos && millis() > lt1) {
        //Serial.println(pos);
        //lcd_setCursor(0, 1);
        //lcd_printf("%10.3f", pos);
        //lcd_print("               ");
        long p= pos * 15;
        lcd_setCursor(0, 1);
        lcd_printf("%10ld um", p);
        lcd_setCursor(0, 2);
        lcd_printf("%5ld.%04ld mm", p/1000, abs(p%1000));
        lastpos = pos;
        lt1 = millis() + 1000;
    }

    if(millis() > lt2) {
        // read buttons
        uint8_t b= lcd_readButtons();
        if(b & BUTTON_LEFT) {
            pos= 0;
        }
        lt2= millis() + 200;
    }

}

