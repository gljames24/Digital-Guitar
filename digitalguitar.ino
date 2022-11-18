//Digital Guitar by James Garrard

#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>
#include <LibPrintf.h>
#include "FretKey.h"

// Allow temporaly dithering, does not work with ESP32 right now

#define delay FastLED.delay

#define PIN 13
const uint8_t rowPins[] = {21,8,9,10,11,12};

// Max is 255, 32 is a conservative value to not overload
// a USB power supply (500mA) for 12x12 pixels.
#define BRIGHTNESS 96


#define ROW 6
#define COL 16
#define NUMMATRIX (ROW*COL)
CRGB leds[NUMMATRIX];
// Define matrix width and height.
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, ROW, COL, 
  NEO_MATRIX_BOTTOM     + NEO_MATRIX_LEFT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);

// Color definitions
#define WHITE       0xFFFF
#define RED         0xF800
#define ORANGE      0xDB80
#define YELLOW      0xBD80
#define CHARTREUSE  0x7EE0
#define GREEN       0x07E0
#define VERDIGRIS   0x06ED
#define CYAN        0x0595
#define AZURE       0x039B
#define BLUE        0x001F
#define PURPLE      0x981B
#define MAGENTA     0xA815
#define ROSE        0xD80D
#define BLACK       0x0000

const uint16_t COLORS [] = {RED, ORANGE, YELLOW, CHARTREUSE, GREEN, VERDIGRIS, CYAN, AZURE, BLUE, PURPLE, MAGENTA, ROSE};

FretKey guitar[ROW][COL+1];

void getHandShape(int* handShape){
  for(uint8_t x=0;x<COL;x++){
    PORTC &= 0xF0; //clear the four bits we want to increment
    PORTC |= x;    //set the four bits we want to increment
    // for(int y=0;y<ROW;y++){
    //   if(handShape[y] ==-1 && digitalRead(rowPins[y])){
    //     handShape[y] = x;
    //   }      
    // }
    int rowRead[ROW];
    rowRead[0] = digitalRead(A7);
    rowRead[1] = digitalRead(8);
    rowRead[2] = digitalRead(9);
    rowRead[3] = digitalRead(10);
    rowRead[4] = digitalRead(11);
    rowRead[5] = digitalRead(12);
    for(uint8_t y=0;y<ROW;y++){
      if (handShape[y] == -1){
        if(rowRead[y]!=0){
          handShape[y] = x;
        }
      }
    }
    delay(100);
  }
}

void setup() {
  //Setup keyboard
  //Keyboard col port output
  DDRC |= 0x0F;
  PORTC = B00000001;
  //Keyboard row input
  // for(uint8_t y=0;y<ROW;y++){
  //   pinMode(rowPins[y],INPUT);
  // } const uint8_t rowPins[] = {21,8,9,10,11,12};
  pinMode(A7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);

  delay(1000);
  Serial.begin(115200);
  printf("Init on pin: %d\nMatrix Size: %d\n %d\n %d\n", PIN, ROW, COL, NUMMATRIX);

  FastLED.addLeds<NEOPIXEL,PIN>(leds, NUMMATRIX).setCorrection(TypicalLEDStrip);
    Serial.print("Setup serial: ");
    Serial.println(NUMMATRIX);

  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(BRIGHTNESS);
  Serial.println("If the code crashes here, decrease the brightness or turn off the all white display below");

  matrix->fillScreen(WHITE);
  matrix->show();
  delay(3000);
  matrix->clear();

  matrix->fillScreen(PURPLE);
  matrix->show();
  delay(3000);
  matrix->clear();

  const int tuningOffset [] = {7, 0, 5, 10, 2, 7};//EADGBE tuning, would like to make adjustable in the future
  const int octiveOffset [] = {2,2,3,3,3,4};
  for (uint8_t i=0; i<ROW; i++) {//Set guitar strings
    int k = tuningOffset[5-i];//fret and string 0,0 is high E
    int l = octiveOffset[5-i];
    for (uint8_t j=0; j<COL+1; j++) {//Set guitar frets
      guitar[i][j].color = new FretKey(COLORS[k], k, i, j, l);

      k++;
      k %= 12;
      if(k=3){
        l++;
      }            
    }
  }  
}

void loop() {
  for (uint8_t i=0; i<ROW; i++) {
    for (uint8_t j=1; j<COL+1; j++) {
      // guitar[i][j-1].setKeyColor();  
      matrix->drawPixel(i,j-1,COLORS[(j-1)%12]);   
    }
  }
  // //
  int handShape[ROW] ={0};
  for(uint8_t x=0;x<COL;x++){
    PORTC &= 0xF0; //clear the four bits we want to increment
    PORTC |= x;    //set the four bits we want to increment
    int rowRead[ROW];
    rowRead[0] = digitalRead(2);
    rowRead[1] = digitalRead(3);
    rowRead[2] = digitalRead(4);
    rowRead[3] = digitalRead(5);
    rowRead[4] = digitalRead(6);
    rowRead[5] = digitalRead(7);
    for(uint8_t y=0;y<ROW;y++){
      if (handShape[y] == 0){
        if(rowRead[y]){
          handShape[y] = COL - x;
        }
      }
    }      
    //printf("%d:\t%d%d%d%d%d%d\n",x,digitalRead(7),digitalRead(6),digitalRead(5),digitalRead(4),digitalRead(3),digitalRead(2));
    
    delay(100);
  }    
  // }
  // getHandShape(handShape);
  for(int i=0;i<ROW;i++){
    printf("%d\t",handShape[i]);
  //  //guitar[i][handShape[i]+1].setKeyWhite();
  }
  printf("\n"); 

  matrix->show();
  delay(10);
  matrix->clear();
}
