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
}

void loop() 
{
  lcd.drawCircle(30,30,20,1); //Рисование коробки с координатами x = 10 и y = 10, шириной 30, высотой 30 пикселей, толшиной рамки 1 пиксель, цвет 1
  lcd.drawFillCircle(127-30,30,20,1);//lcd.drawFillBox(127-30-10,10,30,30,1); //Рисование залитой коробки с координатами x = 127-30-10(87) и y = 10, шириной 30, высотой 30 пикселей, цвет 1
  
  lcd.sendBuffer(); //Отправка буфера в дисплей
}

