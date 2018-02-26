#include "zLCD_base.h"

#if defined (_VARIANT_ARDUINO_STM32_)
SPIClass SPI_2(2);

void zLCD::initHwSpi(uint8_t number_spi, uint8_t _pin_rs)
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

void zLCD::initHwSpi(uint8_t _pin_rs)
{
  pin_rs = _pin_rs;
  pinMode(pin_rs, OUTPUT);
  type_transfer = 1;

  SPI.begin();
  SPI.beginTransaction(SPISettings(240000, MSBFIRST, SPI_MODE3));

  pinMode(pin_rs, OUTPUT);
}

void zLCD::initSwSpi(uint8_t _pin_rs, uint8_t _pin_e, uint8_t _pin_rw)
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

void zLCD::init8bit(uint8_t _d0, uint8_t _d1, uint8_t _d2, uint8_t _d3, uint8_t _d4, uint8_t _d5, uint8_t _d6, uint8_t _d7, uint8_t _pin_rs, uint8_t _pin_e, uint8_t _pin_rw)
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

void zLCD::begin(void)
{
  #if defined (_VARIANT_ARDUINO_STM32_)
  enableDebugPorts();
  #endif
  delay(50);
  transfer(0, type_transfer == 4 ? lcd_func_set_basic_8bit : lcd_func_set_basic);
  delayMicroseconds(100);
  transfer(0, type_transfer == 4 ? lcd_func_set_basic_8bit : lcd_func_set_basic);
  delayMicroseconds(37);
  transfer(0, lcd_disp_on);
  delayMicroseconds(100);
  transfer(0, lcd_clear);
  delayMicroseconds(10);
  transfer(0, lcd_ent_mode_set);
  delayMicroseconds(1800);

  transfer(0, type_transfer == 4 ? lcd_func_set_ext_8bit : lcd_func_set_ext );
  delayMicroseconds(72);
  transfer(0, type_transfer == 4 ? lcd_ext_func_set_8bit : lcd_ext_func_set);
  delayMicroseconds(72);
  transfer(0, lcd_set_gram_addr);
  delayMicroseconds(72);
  transfer(0, lcd_set_gram_addr);
}

void zLCD::free_spi()
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

uint16_t zLCD::utf8(uint8_t char_utf_8_data)
{
  uint16_t char_utf_8 = 0;

  if (char_count_utf_8 == 0){

  char_utf_8_1 |= char_utf_8_data;
  char_count_utf_8  = 1;
  }
  else if (char_count_utf_8 == 1){

  char_utf_8_2 |= char_utf_8_data;
  char_count_utf_8  = 2;
  }
  if (char_count_utf_8 == 2){

    char_utf_8 |= char_utf_8_1;
    char_utf_8 <<= 8;
    char_utf_8 |= char_utf_8_2;

    char_utf_8_1 = 0;
    char_utf_8_2 = 0;
    char_count_utf_8 = 0;

    return char_utf_8;
  }
  else return 0;
}

uint16_t zLCD::cp1251(uint16_t char_cp1251_data)
{
  char_cp1251_data -= 32;
  char_cp1251_data = char_cp1251_data * font_size_x;
  return char_cp1251_data;
}

size_t zLCD::write(uint8_t character)
{
  bool start = false;
  uint16_t character_n = 0;

  if(character <= 127){
    character_n = cp1251(character);
    char_count = 0;
    start = true;
  }
  else if (character > 127){
    character_n = utf8(character);
    //53392 53439

      if (character_n >= 0xD090 && character_n <= 0xD0BF){
        character_n -= 53200;
      }
      //53632 53647
      else if(character_n >= 0xD180 && character_n <= 0xD18F){
        character_n -= 53392;
      }
      if(character_n != 0){
        character_n = cp1251(character_n);
        char_count = 0;
        start = true;
      }
  }

  if (start == true){
    if(character_n == 0) {
      drawFillBox(font_x - 1, font_y, font_size_x + 1, font_size_y, inverting_text);
      font_x += font_size_x;
    }

    if (character_n > 0){
      uint8_t temp_data;
      int count_space=0;
      int count_var=0;

      for (int count_x = 0; count_x < font_size_x; count_x++ ) {
        if(font_data[count_x + character_n] == 0x0 && count_var == 0) count_space++;

        if(font_data[count_x + character_n] != 0x0  ){

          for (int count_y = 0; count_y < font_size_y; count_y++) {

            if (inverting_text == 0) temp_data = font_data[count_x + character_n];
            else
            if (inverting_text == 1) temp_data = ~font_data[count_x + character_n];

            temp_data <<= 7 - count_y;
            temp_data >>= 7;
            drawPixel(font_x + count_x - count_space , font_y + count_y, temp_data);
          }

          count_var = 1;
          if(font_data[count_x + character_n] != 0x0) char_count += 1;
        }
      }

      drawVLine(font_x + char_count, font_y, font_size_y, font_size_x, inverting_text);

      font_x += char_count + 1;
    }
  }
}

void zLCD::setFont(uint8_t size_x, uint8_t size_y, uint8_t* data)
{
  font_size_x = size_x;
  font_size_y = size_y;
  font_data = data;
}

void zLCD::invertingText(bool _inverting_text)
{
  inverting_text = _inverting_text;
}

void zLCD::drawText(uint8_t x, uint8_t y, char* data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}

void zLCD::drawText(uint8_t x, uint8_t y, const String &data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}
void zLCD::drawText(uint8_t x, uint8_t y, char data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}
void zLCD::drawText(uint8_t x, uint8_t y, const char data[])
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}
void zLCD::drawText(uint8_t x, uint8_t y, uint8_t data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}
void zLCD::drawText(uint8_t x, uint8_t y, int data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}
void zLCD::drawText(uint8_t x, uint8_t y, unsigned int data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}
void zLCD::drawText(uint8_t x, uint8_t y, long data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}
void zLCD::drawText(uint8_t x, uint8_t y, unsigned long data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}

#if not defined (__AVR__)
void zLCD::drawText(uint8_t x, uint8_t y, long long data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}
void zLCD::drawText(uint8_t x, uint8_t y, unsigned long long data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}
#endif

void zLCD::drawText(uint8_t x, uint8_t y, double data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}
void zLCD::drawText(uint8_t x, uint8_t y, const __FlashStringHelper *data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}
void zLCD::drawText(uint8_t x, uint8_t y, const Printable &data)
{
  char_count = 0;
  font_x = x;
  font_y = y;
  print(data);
  char_count = 0;
}

void zLCD::resumeDrawText(char* data)                         {  print(data); }
void zLCD::resumeDrawText(const String & data)                {  print(data); }
void zLCD::resumeDrawText(char data)                          {  print(data); }
void zLCD::resumeDrawText(const char data[])                  {  print(data); }
void zLCD::resumeDrawText(uint8_t data)                       {  print(data); }
void zLCD::resumeDrawText(int data)                           {  print(data); }
void zLCD::resumeDrawText(unsigned int data)                  {  print(data); }
void zLCD::resumeDrawText(long data)                          {  print(data); }
void zLCD::resumeDrawText(unsigned long  data)                {  print(data); }
#if not defined (__AVR__)
void zLCD::resumeDrawText(long long data)                     {  print(data); }
void zLCD::resumeDrawText(unsigned long long data)            {  print(data); }
#endif
void zLCD::resumeDrawText(double data)                        {  print(data); }
void zLCD::resumeDrawText(const __FlashStringHelper * data)   {  print(data); }
void zLCD::resumeDrawText(const Printable& data)              {  print(data); }

void zLCD::drawPixel(uint8_t x, uint8_t y, uint8_t pixel)
{
  if (x <= 127 && x >= 0 && y <= 63 && y >= 0) {
    int x_byte = 0;
    int x_bit = 0;

    if (x > 7) {
      while (x - x_byte * 8 > 7) {
        x_byte++;
      }
    }

    x_bit = x - x_byte * 8;
    if(pixel == 0){
        frame_buffer[y][x_byte] &= ~(1 << (7 - x_bit));
    }
    else if(pixel == 1){
        frame_buffer[y][x_byte] |= (1 << (7 - x_bit));
    }
  }
}

void zLCD::drawHLine(uint8_t x, uint8_t y, uint8_t end_line, uint8_t line_width, uint8_t pixel)
{
    for (int i = 0; i < end_line; i++) {
      for (int i2 = 0; i2 < line_width; i2++) {
        drawPixel(x + i, y + i2, pixel);
      }
    }
}

void zLCD::drawVLine(uint8_t x, uint8_t y, uint8_t end_line, uint8_t line_width, uint8_t pixel)
{
    for (int i = 0; i < end_line; i++) {
      for (int i2 = 0; i2 < line_width; i2++) {
        drawPixel(x + i2, y + i, pixel);
      }
    }
}

void zLCD::drawBox(uint8_t x, uint8_t y, uint8_t end_x, uint8_t end_y, uint8_t border_width, uint8_t pixel)
{
  drawHLine(x, y, end_x, border_width , pixel);
  drawHLine(x, y + end_y - border_width, end_x, border_width , pixel);

  drawVLine(x, y, end_y, border_width , pixel);
  drawVLine(x + end_x - border_width, y, end_y, border_width , pixel);
}

void zLCD::drawFillBox(uint8_t x, uint8_t y, uint8_t end_x, uint8_t end_y, uint8_t pixel)
{
  drawHLine(x, y, end_x, end_y , pixel);
}

void zLCD::drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t pixel)
{
  /*из wikipedia*/
  int x0 = x;
  int y0 = y;

  x = radius;
  y = 0;
  int dx = 1 - (radius <<1);
  int dy =  0;
  int err = 0;

  while (x >= y)
  {
    drawPixel(x0 + x, y0 + y, pixel);
    drawPixel(x0 + y, y0 + x, pixel);
    drawPixel(x0 - y, y0 + x, pixel);
    drawPixel(x0 - x, y0 + y, pixel);
    drawPixel(x0 - x, y0 - y, pixel);
    drawPixel(x0 - y, y0 - x, pixel);
    drawPixel(x0 + y, y0 - x, pixel);
    drawPixel(x0 + x, y0 - y, pixel);

    y++;
    err += dy;
    dy += 2;

    if ((err << 1) + dx > 0)
    {
      x--;
      err += dx;
      dx += 2;
    }
  }
}

void zLCD::drawFillCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t pixel)
{
   int x0 = radius;
   int y0 = 0;
   int xChange = 1 - (radius << 1);
   int yChange = 0;
   int radiusError = 0;

   while (x0 >= y0)
   {
       for (int i = x - x0; i <= x + x0; i++)
       {
           drawPixel(i, y + y0, pixel);
           drawPixel(i, y - y0, pixel);
       }
       for (int i = x - y0; i <= x + y0; i++)
       {
           drawPixel(i, y + x0, pixel);
           drawPixel(i, y - x0, pixel);
       }

       y0++;
       radiusError += yChange;
       yChange += 2;
       if (((radiusError << 1) + xChange) > 0)
       {
           x0--;
           radiusError += xChange;
           xChange += 2;
       }
   }
}

void zLCD::drawBitmap(uint8_t x, uint8_t y, float size_x, uint8_t size_y, uint8_t *bitmap_buffer, uint8_t pixel)
{
  int x_byte = 0;
  int x_bit = 0;

  if (x > 7) {
    while (x - x_byte * 8 > 7) {
      x_byte++;
    }
  }
  x_bit = x - x_byte * 8;

  if (x == 0 && y == 0 && size_x == 128 && size_y == 64)
  {
    int i = 0;
    for (int i2 = 0; i2 < 64; i2++) {
      for (int i3 = 0; i3 < 16; i3++) {

        if(pixel == 0) frame_buffer[i2][i3] &= ~bitmap_buffer[i];
        else
        if(pixel == 1) frame_buffer[i2][i3] |= bitmap_buffer[i];

        i++;
      }
    }
  }
  else if ((x_bit == 0) && (x + size_x <= 128))
  {
    int i3 = 0 ;
    for (int i = 0; i < size_y; i++) {
      for (int i2 = 0; i2 < round(size_x / 8); i2++) {

        if(pixel == 0) frame_buffer[i + y][i2 + x / 8 ] &= ~bitmap_buffer[i3];
        else
        if(pixel == 1) frame_buffer[i + y][i2 + x / 8 ] |= bitmap_buffer[i3];

        i3++;
      }
    }
  }
  else if ((x_bit != 0) || ((x_bit == 0) && (x + size_x >= 128)))
  {
      uint8_t temp_bitmap;
      int CountBuffer = 0;

      for (int CountY = 0; CountY < size_y; CountY++) {
        for (int i = 0; i < round(size_x / 8); i++) {
          for (int i2 = 0; i2 < 8; i2++) {

            if(pixel == 0) temp_bitmap = ~bitmap_buffer[i + CountBuffer];
            else
            if(pixel == 1) temp_bitmap = bitmap_buffer[i + CountBuffer];

            temp_bitmap <<= i2;
            temp_bitmap >>= 7;
            drawPixel(x + i2 + i * 8, y + CountY, temp_bitmap);
          }
        }
        CountBuffer += round(size_x / 8);
      }
    }
  }

void zLCD::fillBuffer(bool fill_type)
{
  memset(frame_buffer, fill_type ? 0xff : 0x0, 1024);
}

void zLCD::clearBuffer(void)
{
  fillBuffer(0);
}

void zLCD::clear(bool type_clear, bool fill_type)
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

void zLCD::sendBuffer(bool type_flush)
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

void zLCD::transfer(bool type, uint8_t data)
{
  if(type_transfer == 1) {
    digitalWrite(pin_rs, HIGH);

    SPI.transfer(type ? lcd_type_data : lcd_type_Command);
    SPI.transfer(data & 0XF0);
    SPI.transfer(data << 4);

    digitalWrite(pin_rs, LOW);
  }
  #if defined (_VARIANT_ARDUINO_STM32_)
  else if(type_transfer == 2) {

    digitalWrite(pin_rs, HIGH);

    SPI_2.transfer(type ? lcd_type_data : lcd_type_Command);
    SPI_2.transfer(data & 0XF0);
    SPI_2.transfer(data << 4);

    digitalWrite(pin_rs, LOW);
  }
  #endif
  else if(type_transfer == 3) {
    digitalWrite(pin_rs, HIGH);

    transfer_sw_spi(type ? lcd_type_data : lcd_type_Command);
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

void zLCD::transfer_sw_spi(uint8_t data)
{
  for (uint8_t i = 0; i < 8; ++i)
  {
    digitalWrite(pin_rw, (data & 0x80) ? HIGH : LOW);
    digitalWrite(pin_e, HIGH);
    digitalWrite(pin_e, LOW);
    data <<= 1;
  }
}

void zLCD::transfer_8_bit(uint8_t data)
{
  digitalWrite(d7, (data & 0x80) ? HIGH : LOW);
  digitalWrite(d6, (data <<1 & 0x80) ? HIGH : LOW);
  digitalWrite(d5, (data <<2 & 0x80) ? HIGH : LOW);
  digitalWrite(d4, (data <<3 & 0x80) ? HIGH : LOW);
  digitalWrite(d3, (data <<4 & 0x80) ? HIGH : LOW);
  digitalWrite(d2, (data <<5 & 0x80) ? HIGH : LOW);
  digitalWrite(d1, (data <<6 & 0x80) ? HIGH : LOW);
  digitalWrite(d0, (data <<7 & 0x80) ? HIGH : LOW);
}
