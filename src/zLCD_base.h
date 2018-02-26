#ifndef zLCD_base_h
#define zLCD_base_h

#include <Arduino.h>
#include <SPI.h>
#include <Print.h>

class zLCD : public Print
{
  public:
    void initHwSpi(uint8_t number_spi, uint8_t _pin_rs); //for stm32

    void initHwSpi(uint8_t _pin_rs); //for avr and others

    void initSwSpi(uint8_t _pin_rs, uint8_t _pin_e, uint8_t _pin_rw);

    void init8bit(uint8_t _d0, uint8_t _d1, uint8_t _d2, uint8_t _d3, uint8_t _d4, uint8_t _d5, uint8_t _d6, uint8_t _d7, uint8_t _pin_rs, uint8_t _pin_e, uint8_t _pin_rw);

    void begin(void);

    void free_spi();

    virtual size_t write(uint8_t);

    void setFont(uint8_t size_x, uint8_t size_y, uint8_t* data);

    void invertingText(bool _inverting_text);

    void drawText(uint8_t x, uint8_t y, char* );
    void drawText(uint8_t x, uint8_t y, const String &);
    void drawText(uint8_t x, uint8_t y, char);
    void drawText(uint8_t x, uint8_t y, const char[]);
    void drawText(uint8_t x, uint8_t y, uint8_t);
    void drawText(uint8_t x, uint8_t y, int);
    void drawText(uint8_t x, uint8_t y, unsigned int);
    void drawText(uint8_t x, uint8_t y, long);
    void drawText(uint8_t x, uint8_t y, unsigned long );
    #if not defined (__AVR__)
    void drawText(uint8_t x, uint8_t y, long long);
    void drawText(uint8_t x, uint8_t y, unsigned long long);
    #endif
    void drawText(uint8_t x, uint8_t y, double);
    void drawText(uint8_t x, uint8_t y, const __FlashStringHelper *);
    void drawText(uint8_t x, uint8_t y, const Printable&);

    void resumeDrawText(char*);
    void resumeDrawText(const String &);
    void resumeDrawText(char);
    void resumeDrawText(const char[]);
    void resumeDrawText(uint8_t);
    void resumeDrawText(int);
    void resumeDrawText(unsigned int);
    void resumeDrawText(long);
    void resumeDrawText(unsigned long );
    #if not defined (__AVR__)
    void resumeDrawText(long long);
    void resumeDrawText(unsigned long long);
    #endif
    void resumeDrawText(double);
    void resumeDrawText(const __FlashStringHelper *);
    void resumeDrawText(const Printable&);

    void drawPixel(uint8_t x, uint8_t y, uint8_t pixel);

    void drawHLine(uint8_t x, uint8_t y, uint8_t end_line, uint8_t line_width, uint8_t pixel);
    void drawVLine(uint8_t x, uint8_t y, uint8_t end_line, uint8_t line_width, uint8_t pixel);

    void drawBox(uint8_t x, uint8_t y, uint8_t end_x, uint8_t end_y, uint8_t border_width, uint8_t pixel);
    void drawFillBox(uint8_t x, uint8_t y, uint8_t end_x, uint8_t end_y, uint8_t pixel);

    void drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t pixel);
    void drawFillCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t pixel);

    void drawBitmap(uint8_t x, uint8_t y, float size_x, uint8_t size_y, uint8_t *bitmap_buffer, uint8_t pixel);

    void fillBuffer(bool fill_type);
    void clearBuffer(void);
    void clear(bool type_clear, bool fill_type);

    void sendBuffer(bool type_flush = 0);

  private:

    const uint8_t lcd_type_data = 0xFA; //11111010
    const uint8_t lcd_type_Command = 0xF8; //11111000
    //BASIC
    const uint8_t lcd_clear = 0x1; //00000001
    const uint8_t lcd_disp_on = 0xC;//00001100
    const uint8_t lcd_func_set_ext = 0x24; //00100100
    const uint8_t lcd_func_set_basic = 0x20; //00100000

    const uint8_t lcd_func_set_ext_8bit =    B00110100; //00100100
    const uint8_t lcd_func_set_basic_8bit =  B00110000; //00100000

    const uint8_t lcd_ent_mode_set = 0x06;
    //EXTENDED
    const uint8_t lcd_standby = 0x1; //00000001
    const uint8_t lcd_set_gram_addr = 0x80; //10000000
    const uint8_t lcd_ext_func_set = 0x26; //00100110

    const uint8_t lcd_ext_func_set_8bit = B00110110; //00100110

    uint8_t char_count;

    uint8_t font_x;
    uint8_t font_y;

    uint8_t font_size_x;
    uint8_t font_size_y;

    bool inverting_text = 0;

    uint8_t char_utf_8_1;
    uint8_t char_utf_8_2;

    uint16_t char_count_utf_8;

    uint16_t utf8(uint8_t char_utf_8_data);
    uint16_t cp1251(uint16_t char_cp1251_data);

    uint8_t* font_data;
    uint8_t frame_buffer[64][16];

    uint8_t type_transfer, pin_rs, pin_e, pin_rw;
    uint8_t d0, d1, d2, d3, d4, d5, d6, d7;

    void transfer(bool type, uint8_t data);

    void transfer_sw_spi(uint8_t data);
    void transfer_8_bit(uint8_t data);

};

#endif
