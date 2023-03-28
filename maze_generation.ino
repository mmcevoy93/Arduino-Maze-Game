#include <TFT_eSPI.h>
#include <Wire.h>  // Wire library - used for I2C communication
#include "noise.h"
#define cs 10
#define dc 9
#define rst 8
TFT_eSPI TFTscreen = TFT_eSPI();
int ADXL345 = 0x53;
void gen_maze(int x, int y);
void visit(int x, int y, int width, int height);
bool maze[36][24];
bool visited[36][24];


struct unvisted {
  bool left = false;
  bool right = false;
  bool up = false;
  bool down = false;
};



void setup() {
  // initialize the display
  randomSeed(analogRead(0));
  TFTscreen.begin();
  TFTscreen.setRotation(-45);
  Serial.begin(9600);  // Initiate serial communication for printing the results on the Serial monitor
  Wire.begin();        // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345);  // Start communicating with the device
  Wire.write(0x2D);                 // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8);  // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable
  Wire.endTransmission();
  delay(10);
  TFTscreen.fillScreen(0x000000);


  //int height = 23;
  //int width = 39;  //13x7 // 25x15 // 39x23
}

void loop() {
  start("Welcome to Maze Game", "To Start Level One", "In the labyrinth's heart, a passage lies", "A twisting path that nobody tries.", TFT_BLACK);
  while (true) {
    if (play_game(13, 7, 0, 0, 12, 6, true, 0, 1))  //easy
      start("Level One complete", "To Start Level Two", "A maze of mystery, treacherous track,", "But hidden clues will lead you back.", TFT_BLACK);
    else {
      start("You Lose", "To Start Again", "I wander through the twist and turns,", "With no escape, my spirit yearns.", TFT_RED);
      continue;
    }
    if (play_game(25, 15, 0, 0, 24, 14, true, 1, 1))  //medium
      start("Level Two complete", "To Start Level Three", "Beware the dead ends and endless loops,", "A puzzle box that forever dupes.", TFT_BLACK);
    else {
      start("You Lose", "To Start Again", "The walls are tall and ever-close,", "My hope and faith begin to dose.", TFT_RED);
      continue;
    }
    if (play_game(33, 19, 0, 0, 32, 18, true, 2, 1))  //hard
      start("Level Three complete", "You are feeling confused", "With each turn, the path changes pace," , "Insidious paths that begin to erase.", TFT_BLACK);
    else {
      start("You Lose", "To Start Again", "A never-ending maze, it seems,", "Lost in thoughts and hopeless dreams.", 0x0FF000);
      continue;
    }
    if (play_game(13, 7, 0, 0, 12, 6, true, 0, -1))  //easy
      start("Level Four complete", "To Start Level Five", "The walls close in a claustrophobic mess", "But keep your head and you'll progress.", TFT_BLACK);
    else {
      start("You Lose", "To Start Again", "The path I walk a cruel game,", "A puzzle with no name or aim.", TFT_RED);
      continue;
    }
    if (play_game(25, 15, 0, 0, 24, 14, true, 1, -1))  //medium
      start("Level Five complete", "To Start Level Six", "The key to success is to keep clear \nsight,", "Of the exit, beyond the darkness and \nlight.", TFT_BLACK);
    else {
      start("You Lose", "To Start Again", "I feel the darkness closing in,", "Engulfing me and all within.", 0xFF0000);
      continue;
    }
    if (play_game(33, 19, 0, 0, 32, 18, true, 2, -1))  //hard
      start("You've Won", "To Start Again", "The final turn, the end at last,", "A triumph over the maze, a memory to \nlast.", TFT_GOLD);
    else {
      start("You Lose", "To Start Again", "But alas, I wander still,", "Forever lost in this endless thrill.", 0x0FF000);
      continue;
    }
  }
}

bool movement(int x_maze, int y_maze, int width, int height, int scale, int end_x, int end_y, bool m, int song, char invert) {
  
  int myWidth = TFTscreen.width();
  int myHeight = TFTscreen.height();
  // set the fill color to black and erase the previous
  // position of the paddle if different from present
  //screen values
  int x_screen = x_maze * scale;
  int y_screen = y_maze * scale;
  int oldx = x_screen;
  int oldy = y_screen;
  long time = millis() + 31000;
  int thisNote = 0;
  int music_delay = 0;
  char eTime[4];
  while (true) {
    //accelerometer set up
    Wire.beginTransmission(ADXL345);
    Wire.write(0x32);  // Start with register 0x32 (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345, 6, true);              // Read 6 registers total, each axis value is stored in 2 registers
    float X_out = (Wire.read() | Wire.read() << 8)*invert;  // X-axis value
    float Y_out = (Wire.read() | Wire.read() << 8)*invert;  // Y-axis value
    float Z_out = (Wire.read() | Wire.read() << 8);  // Z-axis value

    if (Y_out < -40 and y_maze != 0 and maze[x_maze][y_maze - 1]) {
      y_maze -= 1;
      y_screen -= scale;
      y_screen = y_screen < 0 ? 0 : y_screen;
      TFTscreen.fillRect(oldx, oldy, scale, scale, 0x00AA00);
      TFTscreen.fillRect(x_screen, y_screen, scale, scale, TFT_GREEN);
      oldy = y_screen;
    }
    if (Y_out > 40 and y_maze != height - 1 and maze[x_maze][y_maze + 1]) {
      y_maze += 1;
      y_screen += scale;
      y_screen = y_screen > myHeight ? myHeight - scale : y_screen;
      TFTscreen.fillRect(oldx, oldy, scale, scale, 0x00AA00);
      TFTscreen.fillRect(x_screen, y_screen, scale, scale, TFT_GREEN);
      oldy = y_screen;
    }
    if (X_out > 40 and x_maze != 0 and maze[x_maze - 1][y_maze]) {
      x_maze -= 1;
      x_screen -= scale;
      x_screen = x_screen < 0 ? 0 : x_screen;
      TFTscreen.fillRect(oldx, oldy, scale, scale, 0x00AA00);
      TFTscreen.fillRect(x_screen, y_screen, scale, scale, TFT_GREEN);
      oldx = x_screen;
    }
    if (X_out < -40 and x_maze != width - 1 and maze[x_maze + 1][y_maze]) {
      x_maze += 1;
      x_screen += scale;
      x_screen = x_screen > myWidth ? myWidth - scale : x_screen;
      TFTscreen.fillRect(oldx, oldy, scale, scale, 0x00AA00);
      TFTscreen.fillRect(x_screen, y_screen, scale, scale, TFT_GREEN);
      oldx = x_screen;
    }
    if (x_maze == end_x and y_maze == end_y) {
      return true;  //won game leave
    }
    if (m and music_delay-- == 0) {
      
      music_delay = noteDuration(song, thisNote) / 50;
      thisNote = music(song, thisNote);

    } else if (m) {
      if (time < millis()) {
        return false;  //lose
      }
      sprintf(eTime, "%2ds", (time - millis()) / 1000);
      TFTscreen.setCursor(0, TFTscreen.height() - 25);
      TFTscreen.println(eTime);
    }

    delay(50);//good for long pauses not short ones
  }
}

void win_screen() {
  TFTscreen.setTextSize(3);
  TFTscreen.fillScreen(TFT_GREEN);
  TFTscreen.println("YOU WIN");
  TFTscreen.println("Time");
}


void gen_maze(int width, int height) {
  int x, y = 0;
  for (int j = 0; j < 24; j++) {
    for (int i = 0; i < 40; i++) {
      maze[i][j] = 0;  //walls
      visited[i][j] = 0;
    }
  }
  
  visit(x, y, width, height);
  
}



void visit(int x, int y, int width, int height) {

  visited[x][y] = true;
  maze[x][y] = 1;
  for (;;) {
    struct unvisted not_visited;
    //left check
    if (x > 1 && !visited[x - 2][y])
      not_visited.left = true;
    //right check
    if (x < width - 1 && !visited[x + 2][y])
      not_visited.right = true;
    //up check
    if (y > 1 && !visited[x][y - 2])
      not_visited.up = true;
    //down check
    if (y < height - 1 && !visited[x][y + 2])
      not_visited.down = true;
    //check to see if all are visited
    
    if (!not_visited.left and !not_visited.right and !not_visited.up and !not_visited.down)
      break;
    else {
      int rng = random(1, 5);
      while (1) {
        if (rng == 1 and not_visited.left) {
          maze[x - 1][y] = 1;
          x -= 2;
          break;
        }
        if (rng == 2 and not_visited.right) {
          maze[x + 1][y] = 1;
          x += 2;
          break;
        }
        if (rng == 3 and not_visited.up) {
          maze[x][y - 1] = 1;
          y -= 2;
          break;
        }
        if (rng == 4 and not_visited.down) {
          maze[x][y + 1] = 1;
          y += 2;
          break;
        } else
          rng = random(1, 5);
      }
      visit(x, y, width, height);
      
    }
  }
}

void start(char *text1, char *text2, char *text3, char *text4, int color) {
  TFTscreen.setCursor(0, 0);
  TFTscreen.setTextSize(3);
  TFTscreen.fillScreen(color);
  TFTscreen.println(text1);
  TFTscreen.println(text2);
  TFTscreen.println("Move Green Square to \nBlue Square to Begin");
  TFTscreen.setCursor(0, 250);
  TFTscreen.setTextSize(2);
  TFTscreen.println(text3);
  TFTscreen.println(text4);
  TFTscreen.setTextSize(3);
  for (int j = 0; j < 7; j++) {
    for (int i = 0; i < 13; i++) {
      if (j == 4)
        maze[i][j] = 1;
      else
        maze[i][j] = 0;
    }
  }
  (void)play_game(13, 7, 0, 4, 12, 4, false, -1, 1);
}

bool play_game(int width, int height, int start_x, int start_y, int end_x, int end_y, bool m, int song, char invert) {
  //Make blank maze

  int scale = TFTscreen.width() / width;
  if (m) {
    TFTscreen.fillScreen(0x000000);
    gen_maze(width, height);
  }

  //display maze
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      if (maze[i][j] == 1)
        TFTscreen.fillRect(i * scale, j * scale, scale, scale, 0xFFFFFF);
    }
  }
  //start square
  TFTscreen.fillRect(start_x * scale, start_y * scale, scale, scale, TFT_GREEN);
  //finish square
  TFTscreen.fillRect(end_x * scale, end_y * scale, scale, scale, 0x0000FF);
  return movement(start_x, start_y, width, height, scale, end_x, end_y, m, song, invert);
}