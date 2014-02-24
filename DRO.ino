
// DRO.ino
//#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#include "utility/twi.h"
#include "Wire.h"

#include "LCDSmoothiePanel.h"



#define LE_ENCA 2
#define LE_ENCB 3

Encoder lme(LE_ENCA, LE_ENCB);

int readEncoderDelta(char &old_AB)
{
    static char enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
    char state;
    old_AB <<= 2;                   //remember previous state
    old_AB |= ( digitalRead(LE_ENCA) + ( digitalRead(LE_ENCB) * 2 ) );  // add current state
    state = enc_states[(old_AB & 0x0f)];
    return state;
}

void clear() {
    lcd_clear();
    // Print a message to the LCD.
    lcd_setCursor(0, 0);
    lcd_print(".....DRO.....");
}

void setup()
{
    //Serial.begin(115200);
    pinMode(LE_ENCA, INPUT);
    pinMode(LE_ENCB, INPUT);

    lcd_init();
    clear();

    //Wire.begin();
}

long pos = 0;
long lastpos = 0;
unsigned long lt1 = 0, lt2 = 0;
char enc1= 0, enc2= 0;
bool inches= false;
const float umperthou= 0.039370079;
bool display= true;

void loop()
{
    char buf[10];
    // Wire.beginTransmission(0x20);
    // for (int i = 0; i < 10; ++i) {
    //    Wire.write((uint8_t)i);
    // }
    // Wire.endTransmission();

    // pos += readEncoderDelta(enc1);
    pos= lme.read();

    if(display || (pos != lastpos && millis() > lt1)) {
        //Serial.println(pos);
        //lcd_setCursor(0, 1);
        //lcd_printf("%10.3f", pos);
        //lcd_print("               ");
        long p= pos * 25; // seems one step is 25um not 15um
        lcd_setCursor(0, 1);
        if(inches){
            long thou= round(p*umperthou);
            lcd_printf("%10ld thou", thou);
            lcd_setCursor(0, 2);
            lcd_printf("%6ld.%03ld inch", thou/1000, abs(thou%1000));
        }else{
            lcd_printf("%10ld um", p);
            lcd_setCursor(0, 2);
            lcd_printf("%6ld.%03ld mm", p/1000, abs(p%1000));
        }
        lastpos = pos;
        lt1 = millis() + 100;
        display= false;
    }

    if(millis() > lt2) {
        // read buttons
        uint8_t b= lcd_readButtons();
        if(b & BUTTON_LEFT) {
            //pos= 0;
            lme.write(0);
            display= true;
        }
        if(b & BUTTON_PAUSE) {
            inches= !inches;
            clear();
            display= true;
        }
        lt2= millis() + 200;
    }

}

