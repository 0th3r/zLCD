#include "zLCD_st7920.h"

#if defined (_VARIANT_ARDUINO_STM32_)
SPIClass SPI_2(2);

void ST7920::initHwSpi(uint8_t number_spi, uint8_t _pin_rs)
{
    pin_rs = _pin_rs;
    pinMode(pin_rs, OUTPUT);
    if(number_spi == 1){
        type_transfer = 1;
        
        SPI.begin();
        SPI.beginTransaction(SPISettings(540000, MSBFIRST, SPI_MODE3));
        
        pinMode(pin_rs, OUTPUT);
    }
    else if(number_spi == 2){
        type_transfer = 2;
        
        SPI_2.begin();
        SPI_2.beginTransaction(SPISettings(540000, MSBFIRST, SPI_MODE3));
        
        pinMode(pin_rs, OUTPUT);
    }
}
#endif

void ST7920::initHwSpi(uint8_t _pin_rs)
{
    pin_rs = _pin_rs;
    pinMode(pin_rs, OUTPUT);
    type_transfer = 1;
    
    SPI.begin();
    SPI.beginTransaction(SPISettings(240000, MSBFIRST, SPI_MODE3));
    
    pinMode(pin_rs, OUTPUT);
}

void ST7920::initSwSpi(uint8_t _pin_rs, uint8_t _pin_e, uint8_t _pin_rw)
{
    type_transfer = 3;
    
    pin_rs = _pin_rs;
    pin_e = _pin_e;
    pin_rw = _pin_rw;
    
    pinMode(pin_rs, OUTPUT);
    pinMode(pin_e, OUTPUT);
    pinMode(pin_rw, OUTPUT);
    
    digitalWrite(pin_rs, LOW);
}

void ST7920::init8bit(uint8_t _d0, uint8_t _d1, uint8_t _d2, uint8_t _d3, uint8_t _d4, uint8_t _d5, uint8_t _d6, uint8_t _d7, uint8_t _pin_rs, uint8_t _pin_e, uint8_t _pin_rw)
{
    type_transfer = 4;
    
    d0 = _d0;
    d1 = _d1;
    d2 = _d2;
    d3 = _d3;
    d4 = _d4;
    d5 = _d5;
    d6 = _d6;
    d7 = _d7;
    
    pin_rs = _pin_rs;
    pin_e = _pin_e;
    pin_rw = _pin_rw;
    
    pinMode(d0, OUTPUT);
    pinMode(d1, OUTPUT);
    pinMode(d2, OUTPUT);
    pinMode(d3, OUTPUT);
    pinMode(d4, OUTPUT);
    pinMode(d5, OUTPUT);
    pinMode(d6, OUTPUT);
    pinMode(d7, OUTPUT);
    
    pinMode(pin_rs, OUTPUT);
    pinMode(pin_e, OUTPUT);
    pinMode(pin_rw, OUTPUT);
    
    digitalWrite(pin_rw, LOW);
}

void ST7920::begin(void)
{
#if defined (zLCD_DEBUG)
    enableDebugPorts();
#endif
    delay(50);
    transfer(0, type_transfer == 4 ? ST7920_base.lcd_func_set_basic_8bit : ST7920_base.lcd_func_set_basic);
    delayMicroseconds(100);
    transfer(0, type_transfer == 4 ? ST7920_base.lcd_func_set_basic_8bit : ST7920_base.lcd_func_set_basic);
    delayMicroseconds(37);
    transfer(0, ST7920_base.lcd_disp_on);
    delayMicroseconds(100);
    transfer(0, ST7920_base.lcd_clear);
    delayMicroseconds(10);
    transfer(0, ST7920_base.lcd_ent_mode_set);
    delayMicroseconds(1800);
    
    transfer(0, type_transfer == 4 ? ST7920_base.lcd_func_set_ext_8bit : ST7920_base.lcd_func_set_ext );
    delayMicroseconds(72);
    transfer(0, type_transfer == 4 ? ST7920_base.lcd_ext_func_set_8bit : ST7920_base.lcd_ext_func_set);
    delayMicroseconds(72);
    transfer(0, ST7920_base.lcd_set_gram_addr);
    delayMicroseconds(72);
    transfer(0, ST7920_base.lcd_set_gram_addr);
}

void ST7920::free_spi()
{
    if(type_transfer == 1){
        SPI.endTransaction();
        SPI.end();
        digitalWrite(pin_rs, LOW);
    }
#if defined (_VARIANT_ARDUINO_STM32_)
    else if(type_transfer == 2){
        SPI_2.endTransaction();
        SPI_2.end();
        digitalWrite(pin_rs, LOW);
    }
#endif
}

void ST7920::fillBuffer(bool fill_type)
{
    memset(frame_buffer, fill_type ? 0xff : 0x0, 1024);
}

void ST7920::clearBuffer(void)
{
    fillBuffer(0);
}

void ST7920::clear(bool type_clear, bool fill_type)
{
    if (type_clear == 0)
    {
        for (byte i = 0; i < 32; i++) {
            transfer(0, 0x80 | i);
            transfer(0, 0x80);
            for (int i = 0; i < 32; i++) {
                transfer(1, fill_type ? 0xff : 0x00);
            }
        }
    }
    else if (type_clear == 1)
    {
        for(int count=0;count <= 8;count+=8){
            for (byte i = 0; i < 32; i++) {
                transfer(0, 0x80 | i);
                transfer(0, 0x80 | count);
                for (int i = 0; i < 16; i++) {
                    transfer(1, fill_type ? 0xff : 0x00);
                }
            }
        }
    }
}

void ST7920::sendBuffer(bool type_flush)
{
    uint8_t temp_buffer = 0x0;
    if (type_flush == 0)
    {
        for (uint8_t row = 0; row < 32; row++) {
            transfer(0, 0x80 | row);
            transfer(0, 0x80);
            for (uint8_t ByteBuffer = 0 ; ByteBuffer < 16; ByteBuffer++) {
                temp_buffer = frame_buffer[row][ByteBuffer];
                transfer(1, temp_buffer);
                temp_buffer = 0x0;
            }
            for (uint8_t ByteBuffer = 0 ; ByteBuffer < 16; ByteBuffer++) {
                temp_buffer = frame_buffer[row + 32][ByteBuffer];
                transfer(1, temp_buffer);
                temp_buffer = 0x0;
            }
        }
    }
    else if (type_flush == 1)
    {
        for (uint8_t row = 0; row < 64; row++) {
            if ( row < 32) {
                transfer(0, 0x80 | row );
                transfer(0, 0x80);
            } else {
                transfer(0, 0x80 | row - 32);
                transfer(0, 0x80 | 8);
            }
            for (uint8_t ByteBuffer = 0 ; ByteBuffer < 16; ByteBuffer++) {
                temp_buffer = frame_buffer[row][ByteBuffer];
                transfer(1, temp_buffer );
                temp_buffer = 0x0;
            }
        }
    }
}

void ST7920::transfer(bool type, uint8_t data)
{
    if(type_transfer == 1) {
        digitalWrite(pin_rs, HIGH);
        
        SPI.transfer(type ? ST7920_base.lcd_type_data : ST7920_base.lcd_type_Command);
        SPI.transfer(data & 0XF0);
        SPI.transfer(data << 4);
        
        digitalWrite(pin_rs, LOW);
    }
#if defined (_VARIANT_ARDUINO_STM32_)
    else if(type_transfer == 2) {
        
        digitalWrite(pin_rs, HIGH);
        
        SPI_2.transfer(type ? ST7920_base.lcd_type_data : ST7920_base.lcd_type_Command);
        SPI_2.transfer(data & 0XF0);
        SPI_2.transfer(data << 4);
        
        digitalWrite(pin_rs, LOW);
    }
#endif
    else if(type_transfer == 3) {
        digitalWrite(pin_rs, HIGH);
        
        transfer_sw_spi(type ? ST7920_base.lcd_type_data : ST7920_base.lcd_type_Command);
        transfer_sw_spi(data & 0XF0);
        transfer_sw_spi(data << 4);
        
        digitalWrite(pin_rs, LOW);
    }
    else if(type_transfer == 4) {
        digitalWrite(pin_e, LOW);
        digitalWrite(pin_rs, type);
        digitalWrite(pin_rw, LOW);
        
        delayMicroseconds(1);
        
        digitalWrite(pin_e, HIGH);
        
        transfer_8_bit(data);
        
        delayMicroseconds(1);
        
        digitalWrite(pin_e, LOW);
        
        delayMicroseconds(27);
    }
}

void ST7920::transfer_sw_spi(uint8_t data)
{
    for (uint8_t i = 0; i < 8; ++i)
    {
        digitalWrite(pin_rw, (data & 0x80) ? HIGH : LOW);
        digitalWrite(pin_e, HIGH);
        digitalWrite(pin_e, LOW);
        data <<= 1;
    }
}

void ST7920::transfer_8_bit(uint8_t data)
{
    digitalWrite(d7, (data & 0x80) ? HIGH : LOW);
    digitalWrite(d6, (data & 0x40) ? HIGH : LOW);
    digitalWrite(d5, (data & 0x20) ? HIGH : LOW);
    digitalWrite(d4, (data & 0x10) ? HIGH : LOW);
    digitalWrite(d3, (data & 0x8)  ? HIGH : LOW);
    digitalWrite(d2, (data & 0x4)  ? HIGH : LOW);
    digitalWrite(d1, (data & 0x2)  ? HIGH : LOW);
    digitalWrite(d0, (data & 0x1)  ? HIGH : LOW);
}
