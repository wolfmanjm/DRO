/*
   drive a SmoothiePanel from Arduino
   courtesy of Logxgen from Smoothie/src/modules/utils/panel/panels/Smoothiepanel.cpp
*/
#if defined(ARDUINO) && (ARDUINO >= 100) //scl
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "utility/twi.h"
#include "Wire.h"

#include "LCDSmoothiePanel.h"

#define PCA9505_ADDRESS 0x40
#define PCA9634_ADDRESS 0x50

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00


#define LCD_WRITE       0x00
#define LCD_READ        0x01
#define LCD_ACK         0x01

static inline void wiresend(uint8_t x) {
#if ARDUINO >= 100
  Wire.write((uint8_t)x);
#else
  Wire.send(x);
#endif
}

static inline uint8_t wirerecv(void) {
#if ARDUINO >= 100
  return Wire.read();
#else
  return Wire.receive();
#endif
}

void wait_ms(int n) {
	delay(n);
}

void i2c_write(int addr, char *data, int n, bool repeated= false) {
	Wire.beginTransmission(addr>>1);
    for (int i = 0; i < n; ++i) {
    	wiresend(*data++);
    }
    Wire.endTransmission();
}

void i2c_read(int addr, char *buf, int n= 1) {
  	Wire.requestFrom(addr>>1, n);
  	for (int i = 0; i < n; ++i) {
  		*buf++ = wirerecv();
  	}
}

void pca9505_write(int address, char reg, char val) {
    const int expander = PCA9505_ADDRESS | (address & 0x0E);
    char cmd[2];

    cmd[0] = reg;
    cmd[1] = val;
    i2c_write(expander, cmd, 2);
}

char pca9505_read(int address, char reg){
    const int expander = PCA9505_ADDRESS | (address & 0x0E);
    char cmd[1];

    cmd[0] = 0x04;
    i2c_write(expander, cmd, 1, false);
    i2c_read(expander, cmd, 1);
    return cmd[0];
}

void pca9634_init(int address){
    const int leds = PCA9634_ADDRESS | (address & 0x0E);
    char cmd[2];

    // initialize led controller
    cmd[0] = 0x00;
    cmd[1] = 0x00;
    i2c_write(leds, cmd, 2);
    cmd[0] = 0x01;
    cmd[1] = 0x02;
    i2c_write(leds, cmd, 2);
    cmd[0] = 0x0C;
    cmd[1] = 0xAA;
    i2c_write(leds, cmd, 2);
    cmd[0] = 0x0D;
    i2c_write(leds, cmd, 2);
}

void pca9634_setLed(int address, char led, char val){
    const int leds = PCA9634_ADDRESS | (address & 0x0E);
    char cmd[2];

    cmd[0] = led; // lcd blue
    cmd[1] = val;
    i2c_write(leds, cmd, 2);
}
void lcdbang_writenibble(char c, bool command = false){
    const int addr = 0x40;
    char cmd[2];
    c <<= 4;
    c &= 0xF0;
    c |= 0x01;
    if(!command) c |= 0x02;

    cmd[0] = 0x0C;
    cmd[1] = c;
    i2c_write(addr, cmd, 2);
    cmd[1] = c | 0x08;
    i2c_write(addr, cmd, 2);
    cmd[1] = c;
    i2c_write(addr, cmd, 2);
//    wait_ms(1);
}

void lcdbang_write(char c, bool command = false){
    if(command){
        lcdbang_writenibble(c, command);
    }else{
        lcdbang_writenibble(c >> 4, command);
        lcdbang_writenibble(c, command);
    }
}

void lcdbang_init(){
    const int addr = 0x40;
    char cmd[2];
    cmd[0] = 0x1C;
    cmd[1] = 0x00;
    i2c_write(addr, cmd, 2);

    lcdbang_write(0x3, true);
    wait_ms(50);
    lcdbang_write(0x3, true);
    wait_ms(10);
    lcdbang_write(0x3, true);
    wait_ms(10);
    lcdbang_write(0x2, true);
    wait_ms(1);

    lcdbang_write(0x2, true);
    lcdbang_write(0x8, true);
    wait_ms(1);

    lcdbang_write(0x0, true);
    lcdbang_write(0x8, true);
    wait_ms(1);

    lcdbang_write(0x0, true);
    lcdbang_write(0x1, true);
    wait_ms(1);

    lcdbang_write(0x0, true);
    lcdbang_write(0x6, true);
    wait_ms(1);

    lcdbang_write(0x0, true);
    lcdbang_write(0x2, true);
    wait_ms(1);

    lcdbang_write(0x0, true);
    lcdbang_write(0xC, true);
    wait_ms(1);
}

void lcdbang_print(const char* msg){
    for(int i=0;msg[i];i++){
        lcdbang_write(msg[i]);
    }
}

void i2cbang_init(){
    const int addr = 0x40;
    char cmd[2];
    cmd[0] = 0x1B;
    cmd[1] = 0x3F;
    i2c_write(addr, cmd, 2);
    cmd[0] = 0x0B;
    cmd[1] = 0xC0;
    i2c_write(addr, cmd, 2);
    wait_ms(1);
}

void i2cbang_start(){
    const int addr = 0x40;
    char cmd[2];
    cmd[0] = 0x0B;
    cmd[1] = 0xBF;
    i2c_write(addr, cmd, 2);
    wait_ms(1);
    cmd[1] = 0x3F;
    i2c_write(addr, cmd, 2);
    wait_ms(1);
}

void i2cbang_stop(){
    const int addr = 0x40;
    char cmd[2];
    cmd[0] = 0x0B;
    cmd[1] = 0xBF;
    i2c_write(addr, cmd, 2);
    wait_ms(1);
    cmd[1] = 0xFF;
    i2c_write(addr, cmd, 2);
    wait_ms(1);
}

void i2cbang_writebit(bool bit){
    const int addr = 0x40;
    char cmd[2];
    cmd[0] = 0x0B;
    if(bit){
        cmd[1] = 0x7F;
    }else{
        cmd[1] = 0x3F;
    }
    i2c_write(addr, cmd, 2);
    wait_ms(1);

    if(bit){
        cmd[1] = 0xFF;
    }else{
        cmd[1] = 0xBF;
    }
    i2c_write(addr, cmd, 2);
    wait_ms(1);

    if(bit){
        cmd[1] = 0x7F;
    }else{
        cmd[1] = 0x3F;
    }
    i2c_write(addr, cmd, 2);
    wait_ms(1);

    if(bit){
        cmd[1] = 0x3F;
        i2c_write(addr, cmd, 2);
    }
    wait_ms(1);
}

char i2cbang_readbit(){
    const int addr = 0x40;
    char cmd[2];
    char res;
    cmd[0] = 0x0B;
    cmd[1] = 0x7F;
    i2c_write(addr, cmd, 2);
    wait_ms(1);

    cmd[1] = 0xFF;
    i2c_write(addr, cmd, 2);
    wait_ms(1);

    cmd[0] = 0x03;
    i2c_write(addr, cmd, 1, false);
    i2c_read(addr, cmd, 1);
    res = cmd[0];
    wait_ms(1);

    cmd[0] = 0x0B;
    cmd[1] = 0x7F;
    i2c_write(addr, cmd, 2);
    wait_ms(1);

//    cmd[1] = 0x3F;
//    i2c.write(addr, cmd, 2);
//    wait_ms(1);

    //res = (~res) & 0x40;
    return res;
}

int i2cbang_write(char c){
    for (int i=0;i<8;i++){
        i2cbang_writebit((c&0x80) > 0);
        c<<=1;
    }

    return i2cbang_readbit();
/*
    const int addr = 0x40;
    char cmd[2];
    char d = 0x00;
    //data
    for (int i=7;i>=0;i--){
        i2c.write(0x3F | d);
        d = ((c>>i)&1)<<6;
        i2c.write(0x3F | d);
        i2c.write(0xBF | d);
    }
    //ack
    i2c.write(0x3F | d);
    i2c.write(0xBF);
    i2c.stop();
    cmd[0] = 0x1B;
    cmd[1] = 0x7F;
    i2c.write(addr, cmd, 2);
    cmd[0] = 0x03;
    i2c.write(addr, cmd, 1, false);
    i2c.start();
    i2c.write(addr | 0x01);
    cmd[1] = i2c.read(false);
//    int res = (~cmd[1]) & 0x40;
    int res = cmd[1];
    i2c.stop();
    cmd[0] = 0x1B;
    cmd[1] = 0x3F;
    i2c.write(addr, cmd, 2);
    cmd[0] = 0x0B;
    cmd[1] = 0xBF;
    i2c.write(addr, cmd, 2, false);
    return res;
*/
}

char i2cbang_read(bool ack){
    char res = 0;
    for(int i=0;i<8;i++){
        res<<=1;
        res |= i2cbang_readbit();
    }

    if(ack){
        i2cbang_writebit(0);
    }else{
        i2cbang_writebit(1);
    }

    wait_ms(1);

    return res;
}

void lcdbang_contrast(int contrast){
    // set dac pins as output and set dac
    i2cbang_init();
    i2cbang_start();
    i2cbang_write(0xC0);
    i2cbang_write(0x60);
    i2cbang_write(contrast >> 8);
    i2cbang_write((contrast << 8) & 0xF0 );
    i2cbang_stop();
}
void setPlayLED(uint8_t v) {
    const int leds = PCA9634_ADDRESS | 0;
    char cmd[2];

    cmd[0] = 0x02; // play
    cmd[1] = v;
    i2c_write(leds, cmd, 2);
}

void setBackLED(uint8_t v) {
    const int leds = PCA9634_ADDRESS | 0;
    char cmd[2];

    cmd[0] = 0x03; // back
    cmd[1] = v;
    i2c_write(leds, cmd, 2);
}

void setBacklightColor(uint8_t r, uint8_t g, uint8_t b) {
    const int leds = PCA9634_ADDRESS | 0;
    char cmd[2];

    cmd[0] = 0x07; // lcd blue
    cmd[1] = b;
    i2c_write(leds, cmd, 2);
    cmd[0] = 0x08; // lcd green
    cmd[1] = g;
    i2c_write(leds, cmd, 2);
    cmd[0] = 0x09; // lcd red
    cmd[1] = r;
    i2c_write(leds, cmd, 2);
}

void lcd_init() {

	Wire.begin();

	// init lcd and buzzer
    lcdbang_init();
    //lcdbang_contrast(0);
	pca9634_init(0);

    setBacklightColor(255, 255, 255);
    setPlayLED(0);
    setBackLED(0);

    pca9505_write(0, 0x18, 0xAA); // enable leds for button/led wing on port0
    pca9505_write(0, 0x08, 0x01); // enable leds for button/led wing on port0

}

void lcd_led(int ledmask) {
    if(ledmask & 1) setBackLED(255); else setBackLED(0);
    if(ledmask & 2) setPlayLED(255); else setPlayLED(0);
}

// cycle the buzzer pin at a certain frequency (hz) for a certain duration (ms)
void lcd_buzz(long duration, uint16_t freq) {
    const int expander = PCA9505_ADDRESS | 0;
    char cmd[2];
    char saved;

    // save register state
    cmd[0] = 0x04;
    i2c_write(expander, cmd, 1, false);
    i2c_read(expander, cmd, 1);
    saved = cmd[0];

    // buzz
    cmd[0] = 0x0C;
    cmd[1] = saved & 0xFE;
    i2c_write(expander, cmd, 2);
    wait_ms(duration); // TODO: Make this not hold up the whole system
    cmd[1] = saved;
    i2c_write(expander, cmd, 2);
}

uint8_t lcd_readButtons(void) {
    const int expander = PCA9505_ADDRESS | 0;
    uint8_t button_bits = 0x00;
    char cmd[1];

    cmd[0] = 0x03;
    i2c_write(expander, cmd, 1, false);
    i2c_read(expander, cmd, 1);
    //cmd[0] = ~cmd[0];
    if((cmd[0] & 0x10) > 0) button_bits |= BUTTON_SELECT; // encoder click
    if((cmd[0] & 0x02) > 0) button_bits |= BUTTON_LEFT; // back button
    if((cmd[0] & 0x01) > 0) button_bits |= BUTTON_PAUSE; // play button

	return button_bits;
}

static void command(uint8_t value) {
    lcdbang_write(value>>4, true);
    lcdbang_write(value, true);
}

void lcd_write(const char* line, int len) {
    for (int i = 0; i < len; ++i) {
        lcdbang_write(*line++);
    }
}

void lcd_print(const char* line) {
	lcd_write(line, strlen(line));
}

void lcd_clear()
{
    command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	wait_ms(100);  // this command takes a long time!
}

void lcd_home()
{
    command(LCD_RETURNHOME);  // set cursor position to zero
    wait_ms(2);  // this command takes a long time!
}

void lcd_setCursor(uint8_t col, uint8_t row)
{
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if ( row >= sizeof(row_offsets) ) row = sizeof(row_offsets) - 1;    // we count rows starting w/0
    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

#include "stdarg.h"
#include "stdio.h"

int lcd_printf(const char* format, ...){
    va_list args;
    va_start(args, format);
    char buffer[20]; // max length for display anyway
    int n= vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    lcd_write(buffer, n);
    return 0;
}
