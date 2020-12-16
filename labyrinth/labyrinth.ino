#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

struct lab                // Класс лабиринта
{
  byte Left;
  byte Right;
  byte Top;
  byte Bottom;
  byte room;
};

#define SCREEN_WIDTH 128 // OLED дисплей, ширина в пикселях
#define SCREEN_HEIGHT 64 // OLED дисплей, высота в пикселях
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define width 11         // Ширина лабиринта
#define height 8         // Высота лабиринта
lab **maze = NULL;

void setup() {
  pinMode(2, INPUT_PULLUP);               //Вверх  
  pinMode(3, INPUT_PULLUP);               //Вниз
  pinMode(4, INPUT_PULLUP);               //Влево
  pinMode(5, INPUT_PULLUP);               //Вправо
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  srand(analogRead(0));
}

void loop() {
  static uint8_t pointerX = 0, pointerY = 0;
  bool UP = !digitalRead(2),              //Вверх
       DOWN = !digitalRead(3),            //Вниз
       LEFT = !digitalRead(4),            //Влево
       RIGHT = !digitalRead(5);           //Вправо
  
  display.clearDisplay();
  if (maze == NULL)                       //Если лабиринт не создан, создаем его
    maze = mazemake();
  if (UP && maze[pointerX][pointerY].Top == 2) {
    delay(100);
    pointerY--;
  }
  if (DOWN && maze[pointerX][pointerY].Bottom == 2) {
    delay(100);
    pointerY++;
  }
  if (LEFT && maze[pointerX][pointerY].Left == 2) {
    delay(100);
    pointerX--;
  }
  if (RIGHT && maze[pointerX][pointerY].Right == 2) {
    delay(100);
    pointerX++;
  }
  visual (maze, pointerX, pointerY);    //Выводим лабиринт
}


void visual(lab** labyrinth, uint8_t pointerX, uint8_t pointerY) {                                  //Функция визуализации лабиринта
  for (uint8_t y = 0; y < height; y++)
    for (uint8_t x = 0; x < width; x++){
      if (labyrinth[x][y].Top == 1)
        display.drawLine(x*8, y*8, x*8+7, y*8, SSD1306_WHITE);      //Верхняя стенка
      if (labyrinth[x][y].Right == 1)
        display.drawLine(x*8+7, y*8, x*8+7, y*8+7, SSD1306_WHITE);  //Правая стенка
      if (labyrinth[x][y].Bottom == 1)
       display.drawLine(x*8, y*8+7, x*8+7, y*8+7, SSD1306_WHITE);   //Нижняя стенка
      if (labyrinth[x][y].Left == 1)
        display.drawLine(x*8, y*8, x*8, y*8+7, SSD1306_WHITE);      //Левая стенка
    }
  visualunit(pointerX, pointerY);
  display.display();                                                //Обновляем дисплей
}

void visualunit(uint8_t pointerX, uint8_t pointerY) {
  display.drawPixel((pointerX*8)+2, (pointerY*8)+2, SSD1306_WHITE);
  display.drawPixel((pointerX*8)+5, (pointerY*8)+2, SSD1306_WHITE);
  display.drawPixel((pointerX*8)+3, (pointerY*8)+3, SSD1306_WHITE);
  display.drawPixel((pointerX*8)+4, (pointerY*8)+4, SSD1306_WHITE);
  display.drawPixel((pointerX*8)+3, (pointerY*8)+4, SSD1306_WHITE);
  display.drawPixel((pointerX*8)+4, (pointerY*8)+3, SSD1306_WHITE);
  display.drawPixel((pointerX*8)+2, (pointerY*8)+5, SSD1306_WHITE);
  display.drawPixel((pointerX*8)+5, (pointerY*8)+5, SSD1306_WHITE);
}

lab** mazemake() {                                                  //Функция генерации лабиринта
  lab** labyrinth = new lab* [width];
  for (uint8_t i = 0; i < width; i++)
    labyrinth[i] = new lab[height];
  //Заполняем начальные данные для ячеек
  //Сторона может быть открыта(2) или закрыта(1)
  //Комната может быть посещённой(3), непосещённой(5) и выполненной(4)
  //Сначала все комнаты непосещённы(5), а все стороны закрыты(1)
  for (uint8_t y = 0; y < height; y++)
    for (uint8_t x = 0; x < width; x++)
    {
      labyrinth[x][y].Left = 1;
      labyrinth[x][y].Right = 1;
      labyrinth[x][y].Top = 1;
      labyrinth[x][y].Bottom = 1;
      labyrinth[x][y].room = 5;

    }
  //Выбираем комнату с которой начнем генерацию лабиринта
  //Делаем её посещённой
  byte X = rand() % width;
  byte Y = rand() % height;
  labyrinth[X][Y].room = 3;
  //completed - считает количество выполненных(4) комнат лабиринта
  //side - сторона, выбирается рандомно, в которую мы осуществим следующий шаг
  byte side = 0, completed = 0;
  //cicle - если часть кода с первого по четвертый кейс будет выполняться более 20раз,
  //то мы посчитаем что программа зациклилась, так как у нее нет других вариантов, и
  //перекинем ее на следущий шаг
  byte cicle = 0;

  while (completed < height * width)                                //Пока количество выполненных комнат меньше общего количества комнат
    switch (side = (cicle > 20) ? 5 : (rand() % 4) + 1)             //Если код ещё не зациклился, выбираем сторону в которую осуществим следующий шаг, иначе пытаемся выйти из такой ситуации
    {
    case 1:                                                         //Рандом велит идти влево
      if (X - 1 >= 0)                                               //Если левая комната не выходит за границы лабиринта
      if (labyrinth[X - 1][Y].room == 5)                            //Если левая комната непосещённая
      {                                                             //Открываем необходимые двери и идем в эту комнату
        cicle = 0;
        labyrinth[X - 1][Y].room = 3;
        labyrinth[X][Y].Left = 2;
        labyrinth[X - 1][Y].Right = 2;
        X = X - 1;
        Y = Y;
      }
      cicle++;
      break;
    case 2:                                                         //Рандом велит идти вправо
      if (X + 1 <= width-1)                                         //Если правая комната не выходит за границы лабиринта
      if (labyrinth[X + 1][Y].room == 5)                            //Если правая комната непосещённая
      {                                                             //Открываем необходимые двери и идем в эту комнату
        cicle = 0;
        labyrinth[X + 1][Y].room = 3;
        labyrinth[X][Y].Right = 2;
        labyrinth[X + 1][Y].Left = 2;
        X = X + 1;
        Y = Y;
      }
      cicle++;
      break;
    case 3:                                                         //Рандом велит идти вверх
      if (Y - 1 >= 0)                                               //Если верхняя комната не выходит за границы лабиринта
      if (labyrinth[X][Y - 1].room == 5)                            //Если верхняя комната не непосещённая
      {                                                             //Открываем необходимые двери и идем в эту комнату
        cicle = 0;
        labyrinth[X][Y - 1].room = 3;
        labyrinth[X][Y].Top = 2;
        labyrinth[X][Y - 1].Bottom = 2;
        X = X;
        Y = Y - 1;
      }
      cicle++;
      break;
    case 4:                                                         //Рандом велит идти вниз
      if (Y + 1 <= height-1)                                        //Если нижняя комната не выходит за границы лабиринта
      if (labyrinth[X][Y + 1].room == 5)                            //Если нижняя комната не непосещённая
      {                                                             //Открываем необходимые двери и идем в эту комнату
        cicle = 0;
        labyrinth[X][Y + 1].room = 3;
        labyrinth[X][Y].Bottom = 2;
        labyrinth[X][Y + 1].Top = 2;
        X = X;
        Y = Y + 1;
      }
      cicle++;
      break;
    default:                                                        //Если код зациклился (нам некуда пойти, все соседи не непосещённые),
                                                                    //идем на комнату назад, и возвращаемся к выбору комнаты в которую пойдем
      cicle = 0;
      if (completed == width * height - 1)                          //Если количество выполненных комнат равно (количеству всех комнат - 1)
      {                                                             //текущяя комната является последней, делаем её выполненной и выходим
        labyrinth[X][Y].room = 4;
        completed++;
        break;
      }
                                                                    //Если дверь в комнату слева открыта и комната не выполненная, идем в нее
                                                                    //и делаем предыдующюю комнату выполненной
      if (labyrinth[X][Y].Left == 2 && labyrinth[X - 1][Y].room != 4 && X - 1 >= 0)
      {
        labyrinth[X][Y].room = 4;
        completed++;
        X = X - 1;
        Y = Y;
        break;
      }
                                                                    //Если дверь в комнату справа открыта и комната не выполненная, идем в нее
                                                                    //и делаем предыдующюю комнату выполненной
      if (labyrinth[X][Y].Right == 2 && labyrinth[X + 1][Y].room != 4 && X + 1 <= width)
      {
        labyrinth[X][Y].room = 4;
        completed++;
        X = X + 1;
        Y = Y;
        break;
      }
                                                                    //Если дверь в комнату сверху открыта и комната не выполненная, идем в нее
                                                                    //и делаем предыдующюю комнату выполненной
      if (labyrinth[X][Y].Top == 2 && labyrinth[X][Y - 1].room != 4 && Y - 1 >= 0)
      {
        labyrinth[X][Y].room = 4;
        completed++;
        X = X;
        Y = Y - 1;
        break;
      }
                                                                    //Если дверь в комнату снизу открыта и комната не выполненная, идем в нее
                                                                    //и делаем предыдующюю комнату выполненной
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

  //После конца алгоритма помещаем вход и выход из лабиринта в верхний левый и нижний правый угол
  labyrinth[0][0].Left = 2;
  labyrinth[width-1][height-1].Right = 2;
  //Возвращаем указатель на динамический массив лабиринта
  return labyrinth;
}
