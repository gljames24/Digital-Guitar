#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>

class FretKey{

public:
  uint16_t color;
  int note;
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

  FretKey (uint16_t clr, int n, uint8_t r, uint8_t c, uint8_t o){
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
