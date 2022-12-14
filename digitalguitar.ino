//Digital Guitar by James Garrard

#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>
#include <LibPrintf.h>
#include "FretKey.h"
#include <MIDIUSB.h>

#define delay FastLED.delay

#define LED_PIN 13
const uint8_t stringPins[] = {0,12,11,10,9,8};
const uint8_t colPins[] = {14,15,16,17};
const uint8_t rowPins[] = {7,6,5,4,3,2};

// Max is 255
#define BRIGHTNESS 128

// Define matrix width and height.
#define ROW 6
#define COL 16
#define NUMMATRIX (ROW*COL)
CRGB leds[NUMMATRIX];
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, ROW, COL, 
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);

// Color definitions
#define WHITE       0xFFFF
#define RED         0xF800
#define ORANGE      0xDB80
#define YELLOW      0xBD80
#define CHARTREUSE  0x7EE0
#define GREEN       0x07E0
#define MINT        0x06ED
#define CYAN        0x0595
#define AZURE       0x039B
#define BLUE        0x001F
#define PURPLE      0x981B
#define MAGENTA     0xA815
#define ROSE        0xD80D
#define BLACK       0x0000

const uint16_t COLORS [] = {CHARTREUSE, GREEN, MINT, CYAN, AZURE, BLUE, PURPLE, MAGENTA, ROSE, RED, ORANGE, YELLOW};

#define A   9
#define Bb  10
#define B   11
#define C   0
#define Db  1
#define D   2
#define Eb  3
#define E   4
#define F   5
#define Gb  6
#define G   7
#define Ab  8

FretKey guitar[ROW][COL+1];
#define strumResponse 32
#define keyDelay 5//millis

//EADGBE tuning, would like to make adjustable in the future
const int tuningOffset [] = {E, A, D, G, B, E};
const int octaveOffset [] = {2,2,3,3,3,4};

void setupTuning(){
  int note = 0;
  int octave = 0;

  for (uint8_t row=0; row<ROW; row++) {//Set guitar strings
    note = tuningOffset[5-row];//fret and string 0,0 is high E
    octave = octaveOffset[5-row];
    for (uint8_t col=0; col<COL+1; col++) {//Set guitar frets off by one for open string note hovering column 
      guitar[row][col] = FretKey(COLORS[note], note, octave, row, col);
      note++;
      note %= 12;
      if(note == C){//C is where the octave raises
        octave++;
      }            
    }
  }  
}

void setup() {
  //Setup keyboard
  //Keyboard col port output
  for(int col=0;col<COL;col++){
    pinMode(colPins[col], OUTPUT);
  }  

  //String and Fret Pins
  for(int row=0;row<ROW;row++){
    pinMode(rowPins[row], INPUT);
    pinMode(stringPins[row], INPUT);
  }

  delay(1000);
  Serial.begin(115200);
  printf("Init on pin: %d\nMatrix Size: %d\n %d\n %d\n", LED_PIN, ROW, COL, NUMMATRIX);

  FastLED.addLeds<NEOPIXEL,LED_PIN>(leds, NUMMATRIX).setCorrection(TypicalLEDStrip);
    Serial.print("Setup serial: ");
    Serial.println(NUMMATRIX);

  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(BRIGHTNESS);
  Serial.println("If the code crashes here, decrease the brightness or turn off the all white display below");

  //Boot Up Splash Screen Color Test
  for (uint8_t i=0; i<ROW; i++) {
    for (uint8_t j=1; j<COL+1; j++) {
      matrix->drawPixel(i,j-1,COLORS[(j-1)%12]);
    }
  }
  matrix->show();
  setupTuning();
  delay(2000); 
  matrix->clear();
}

int strumCounter[6] = {-1};

void loop() {  
  //draw fretkey lights based on tuning
  for (uint8_t row=0; row<ROW; row++) {
    for (uint8_t col=1; col<COL+1; col++) {//0 is the open fret which doesn't have an led on the hardware yet   
      matrix->drawPixel(row,col-1,guitar[row][col].color);  
    }
  }

  //Read FretKeys
  int handShape[ROW] ={0};
  for (int col = 0; col < COL; ++col) {
    // Use bitwise operators to get the first four bits of col and pass them to the colPins
    digitalWrite(colPins[0], col & 0b0001);
    digitalWrite(colPins[1], col & 0b0010);
    digitalWrite(colPins[2], col & 0b0100);
    digitalWrite(colPins[3], col & 0b1000);

    int rowRead[ROW];
    //set handshape iff the value isnt already assigned
    for(uint8_t row=0;row<ROW;row++){
      rowRead[row] = digitalRead(rowPins[row]);
      if (handShape[row] == 0){
        if(rowRead[row]){
          handShape[row] = col + 1;
          matrix->drawPixel(row,handShape[row]-1,WHITE);         
        }
      }
    }
  }

  for(int row=0;row<ROW;row++){
    if(digitalRead(stringPins[row])){
        if(strumCounter[row] == -1){
          strumCounter[row] = strumResponse;//
        }
        else if(strumCounter[row] > 0){
          strumCounter[row]--;
        }
    }
    else if(strumCounter[row]>-1){
      if(strumCounter[row]>0){
        printf("Strum! I:%d N:%d C:1 R:%d", strumCounter[row], guitar[row][handShape[row]].pitch, row);
        printf("\n");             
      }
     strumCounter[row] = -1;
    } 
  }

  matrix->show();
  delay(keyDelay);
  matrix->clear();
}
