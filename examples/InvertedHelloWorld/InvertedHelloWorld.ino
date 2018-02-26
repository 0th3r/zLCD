#include <zLCD.h> //Подключение библиотеки

zLCD lcd; //Создание объекта lcd

void setup() 
{
  /*Возможные режимы работы дисплея.
   *Для stm32 рекомендуется использовать SwSpi (~16-18 fps) или 8bit интерфейс (~24 fps)
   */
  //Инициализация режима работы дисплея
  //lcd.initHwSpi(/*RS*/ PB12);
  //lcd.initHwSpi(/*Номер SPI*/1, /*RS*/ PB12);
  //lcd.initSwSpi(/*RS*/ PB12, /*E*/ PB14, /*RW*/ PB13);
  lcd.init8bit(/*D0-D7*/ PB9,PB8,PB7,PB6,PB5,PB4,PB3,PA15, /*RS*/ PB12, /*E*/ PB14, /*RW*/ PB13);
  
  lcd.begin(); //Инициализация дисплея
  lcd.clear(0, 0); //Очистка дисплея
  lcd.clearBuffer(); //Очистка буфера в Arduino
  
  lcd.setFont(5,8,z_font_5x8); //Установка шрифта c заданным размером x = 5 и y = 8

  lcd.fillBuffer(1); //Заполнеие буфера единицами
}

void loop() 
{
  lcd.invertingText(1); //Включение инвертирования текста
  lcd.drawText(0,0, "Привет, мир!"); //Вывод надписи "Привет, мир!" с координатами x = 0 и y = 0, текст рисуется сверху вниз
  lcd.drawText(0,9, "Hello, world!"); //Output "Hello world!" with coordinates x = 0 and y = 0, the text is drawn from top to bottom
  lcd.invertingText(0); //Отключение инвертирования текста
  lcd.drawText(0,9*2, "Привет, мир!"); //Вывод надписи "Привет, мир!" с координатами x = 0 и y = 0, текст рисуется сверху вниз
  lcd.drawText(0,9*3, "Hello, world!"); //Output "Hello world!" with coordinates x = 0 and y = 0, the text is drawn from top to bottom
  
  lcd.sendBuffer(); //Отправка буфера в дисплей
}





