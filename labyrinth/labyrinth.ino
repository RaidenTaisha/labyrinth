#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED дисплей, ширина в пикселях
#define SCREEN_HEIGHT 64 // OLED дисплей, высота в пикселях

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//размер лабиринта
#define width 15
#define height 7

class lab //класс лабиринта
{
public:
  byte Left;
  byte Right;
  byte Top;
  byte Bottom;
  byte room;
};

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  srand(analogRead(0));
  
  lab labyrinth[width][height];
  //заполняем начальные данные для ячеек
  for (uint8_t y = 0; y < height; y++)
    for (uint8_t x = 0; x < width; x++)
    {
      labyrinth[x][y].Left = 1;
      labyrinth[x][y].Right = 1;
      labyrinth[x][y].Top = 1;
      labyrinth[x][y].Bottom = 1;
      labyrinth[x][y].room = 5;

    }
  //выбираем начало генерации
  byte X = rand() % width;
  byte Y = rand() % height;
  labyrinth[X][Y].room = 3;
  //completed - считает количество сделанных(белых) ячеек лабиринта
  //side - рандомный выбор в какую сторону мы осуществим следующий шаг
  byte side = 0, completed = 0;
  //cicle - если часть кода с первого по четвертый кейс будет выполняться более 20раз,
  //то мы посчитаем что программа зациклилась, так как у нее нет других вариантов, и
  //перекинем ее на следущий шаг
  byte cicle = 0;

  while (completed < height * width)
    switch (side = (cicle > 20) ? 5 : (rand() % 4) + 1)
    {
    case 1:
      if (X - 1 >= 0)
      if (labyrinth[X - 1][Y].room == 5)
      { 
        cicle = 0;
        labyrinth[X - 1][Y].room = 3;
        labyrinth[X][Y].Left = 2;
        labyrinth[X - 1][Y].Right = 2;
        X = X - 1;
        Y = Y;
      }
      cicle++;
      break;
    case 2:
      if (X + 1 <= width-1)
      if (labyrinth[X + 1][Y].room == 5)
      {
        cicle = 0;
        labyrinth[X + 1][Y].room = 3;
        labyrinth[X][Y].Right = 2;
        labyrinth[X + 1][Y].Left = 2;
        X = X + 1;
        Y = Y;
      }
      cicle++;
      break;
    case 3:
      if (Y - 1 >= 0)
      if (labyrinth[X][Y - 1].room == 5)
      {
        cicle = 0;
        labyrinth[X][Y - 1].room = 3;
        labyrinth[X][Y].Top = 2;
        labyrinth[X][Y - 1].Bottom = 2;
        X = X;
        Y = Y - 1;
      }
      cicle++;
      break;
    case 4:
      if (Y + 1 <= height-1)
      if (labyrinth[X][Y + 1].room == 5)
      {
        cicle = 0;
        labyrinth[X][Y + 1].room = 3;
        labyrinth[X][Y].Bottom = 2;
        labyrinth[X][Y + 1].Top = 2;
        X = X;
        Y = Y + 1;
      }
      cicle++;
      break;
    default:
      cicle = 0;
      if (completed == width * height - 1)
      {
        labyrinth[X][Y].room = 4;
        completed++;
        break;
      }
      if (labyrinth[X][Y].Left == 2 && labyrinth[X - 1][Y].room != 4 && X - 1 >= 0)
      {
        labyrinth[X][Y].room = 4;
        completed++;
        X = X - 1;
        Y = Y;
        break;
      }
      if (labyrinth[X][Y].Right == 2 && labyrinth[X + 1][Y].room != 4 && X + 1 <= width)
      {
        labyrinth[X][Y].room = 4;
        completed++;
        X = X + 1;
        Y = Y;
        break;
      }
      if (labyrinth[X][Y].Top == 2 && labyrinth[X][Y - 1].room != 4 && Y - 1 >= 0)
      {
        labyrinth[X][Y].room = 4;
        completed++;
        X = X;
        Y = Y - 1;
        break;
      }
      if (labyrinth[X][Y].Bottom == 2 && labyrinth[X][Y + 1].room != 4 && Y + 1 <= height)
      {
        labyrinth[X][Y].room = 4;
        completed++;
        X = X;
        Y = Y + 1;
        break;
      }
      break;
    }

  labyrinth[0][0].Left = 2;
  labyrinth[width-1][height-1].Right = 2;
  for(;;){
    for (uint8_t y = 0; y < height; y++)
      for (uint8_t x = 0; x < width; x++){
        if (labyrinth[x][y].Top == 1)
          display.drawLine(x*8, y*8, x*8+8, y*8, SSD1306_WHITE);
        if (labyrinth[x][y].Right == 1)
          display.drawLine(x*8+8, y*8, x*8+8, y*8+8, SSD1306_WHITE); 
        if (labyrinth[x][y].Bottom == 1)
         display.drawLine(x*8, y*8+8, x*8+8, y*8+8, SSD1306_WHITE);
        if (labyrinth[x][y].Left == 1)
          display.drawLine(x*8, y*8, x*8, y*8+8, SSD1306_WHITE);
    }
    display.display();
  }
}

void loop() {
  
}
