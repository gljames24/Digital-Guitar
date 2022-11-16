#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>

// Color definitions
#define WHITE       0xAAAA
#define RED         0xF800
#define ORANGE      0xDB80
#define YELLOW      0xAD80
#define CHARTREUSE  0x6EE0
#define GREEN       0x07E0
#define VERDIGRIS   0x06ED
#define CYAN        0x0595
#define AZURE       0x039B
#define BLUE        0x001F
#define PURPLE      0x681B
#define MAGENTA     0xA815
#define ROSE        0xD80D
#define BLACK       0x0000

const uint16_t COLORS [] = {RED, ORANGE, YELLOW, CHARTREUSE, GREEN, VERDIGRIS, CYAN, AZURE, BLUE, PURPLE, MAGENTA, ROSE};

enum Note{A,Bb,B,C,Db,D,Eb,E,F,Gb,G,Ab};

class FretKey{

  
public:
  uint16_t color;
  Note note;
  uint8_t row;
  uint8_t col;
  uint8_t octive;
  static FastLED_NeoMatrix matrix;

  FretKey (){
    color = 0;
    note = 0;
    row = 0;
    col = 0;
    octive = 0;     
  }

  FretKey (uint16_t clr, Note n, uint8_t r, uint8_t c, uint8_t o){
    color = clr;
    note = n;
    row = r;
    col = c;
    octive = o;    
  }

  void setKeyColor(){
     matrix->drawPixel(row,col,color);
  }

  void setKeyWhite(){
     matrix->drawPixel(row,col,WHITE);
  }

  static void setMatrix(FastLED_NeoMatrix &m){
    matrix = m;
  }

};
