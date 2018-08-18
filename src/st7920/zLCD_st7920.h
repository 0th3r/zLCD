#ifndef zLCD_st7920_h
#define zLCD_st7920_h

//#define zLCD_DEBUG

#include <Arduino.h>
#include <SPI.h>
#include <zLCD.h>

struct {
    const uint8_t lcd_type_data = 0xFA; //11111010
    const uint8_t lcd_type_Command = 0xF8; //11111000
    //BASIC
    const uint8_t lcd_clear = 0x1; //00000001
    const uint8_t lcd_disp_on = 0xC;//00001100
    const uint8_t lcd_func_set_ext = 0x24; //00100100
    const uint8_t lcd_func_set_basic = 0x20; //00100000
    
    const uint8_t lcd_func_set_ext_8bit = 0x34; //00100100
    const uint8_t lcd_func_set_basic_8bit = 0x30; //00100000
    
    const uint8_t lcd_ent_mode_set = 0x06;
    //EXTENDED
    const uint8_t lcd_standby = 0x1; //00000001
    const uint8_t lcd_set_gram_addr = 0x80; //10000000
    const uint8_t lcd_ext_func_set = 0x26; //00100110
    
    const uint8_t lcd_ext_func_set_8bit = 0x36; //00100110
} ST7920_base;

class ST7920 : public zLCD_base
{
public:
    void initHwSpi(uint8_t number_spi, uint8_t _pin_rs); //for stm32
    
    void initHwSpi(uint8_t _pin_rs); //for avr and others
    
    void initSwSpi(uint8_t _pin_rs, uint8_t _pin_e, uint8_t _pin_rw);
    
    void init8bit(uint8_t _d0, uint8_t _d1, uint8_t _d2, uint8_t _d3,
                  uint8_t _d4, uint8_t _d5, uint8_t _d6, uint8_t _d7,
                  uint8_t _pin_rs, uint8_t _pin_e, uint8_t _pin_rw);
    
    void begin(void);
    
    void free_spi();
    
    void fillBuffer(bool fill_type);
    void clearBuffer(void);
    void clear(bool type_clear, bool fill_type);
    
    void sendBuffer(bool type_flush = 0);
    
private:
    uint8_t type_transfer, pin_rs, pin_e, pin_rw;
    uint8_t d0, d1, d2, d3, d4, d5, d6, d7;
    
    void transfer(bool type, uint8_t data);
    
    void transfer_sw_spi(uint8_t data);
    void transfer_8_bit(uint8_t data);
};
#endif
