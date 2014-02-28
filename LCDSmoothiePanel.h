#ifndef _LCDSMOOTHIEPANEL_
#define _LCDSMOOTHIEPANEL_

#define BUTTON_SELECT 0x01
#define BUTTON_RIGHT  0x02
#define BUTTON_DOWN   0x04
#define BUTTON_UP     0x08
#define BUTTON_LEFT   0x10
#define BUTTON_PAUSE  0x20
#define BUTTON_AUX1   0x40
#define BUTTON_AUX2   0x80


void lcd_init();
void lcd_print(const char *);
int lcd_printf(const char* format, ...);
void lcd_setCursor(uint8_t col, uint8_t row);
uint8_t lcd_readButtons(void) ;
void lcd_clear();
void lcd_led(int);
#endif
