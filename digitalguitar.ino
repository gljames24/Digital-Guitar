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
#define BRIGHTNESS 60


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

#define A   0
#define Bb  1
#define B   2
#define C   3
#define Db  4
#define D   5
#define Ed  6
#define E   7
#define F   8
#define Gb  9
#define G   10
#define Ab  11

FretKey guitar[ROW][COL+1];
#define strumResponse 32
#define keyDelay 5//millis
//EADGBE tuning, would like to make adjustable in the future
const int tuningOffset [] = {7, 0, 5, 10, 2, 7};
const int octiveOffset [] = {2,2,3,3,3,4};

void setup() {
  //Setup keyboard
  //Keyboard col port output
  DDRC |= 0x0F;
  PORTC = B00000001;
  //Keyboard row input
  // for(uint8_t y=0;y<ROW;y++){
  //   pinMode(rowPins[y],INPUT);
  // }

  //String Pins
  pinMode(A7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);

  //Fret Pins
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);

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
  //
  int handShape[ROW] ={0};
  for(uint8_t x=0;x<COL;x++){
    PORTC &= 0xF0; //clear the four bits we want to increment
    PORTC |= x;    //set the four bits we want to increment
    //Read each row for button presses
    int rowRead[ROW];
    for(int i=0;i<ROW;i++){
      rowRead[i] = digitalRead(i+2);     
    }
    //set handshape iff the value isnt already assigned
    for(uint8_t y=0;y<ROW;y++){
      if (handShape[y] == 0){
        if(rowRead[y]){
          handShape[y] = COL - x;
        }
      }
    }      
  }

  //draw fretkey lights based on tuning
  for (uint8_t i=0; i<ROW; i++) {
    for (uint8_t j=1; j<COL+1; j++) {
      //printf("%X\t",guitar[i][j-1].color);    
      matrix->drawPixel(i,j-1,guitar[i][j-1].color);  
    }
  }

  for(int i=0;i<ROW;i++){
    if(digitalRead(rowPins[i])){
        if(strumCounter[i] == -1){
          strumCounter[i] = strumResponse;//
        }
        else if(strumCounter[i] == 0){
        }
        else{
          strumCounter[i]--;
        }
    }
    else if(strumCounter[i]>-1){
      if(strumCounter[i]>0){
        printf("Strum!%d",strumCounter[i]);
        printf("%d\t",handShape[i]);
        printf("\n");             
      }
     strumCounter[i] = -1;
    } 
    
  //  //guitar[i][handShape[i]+1].setKeyWhite();
  }
   
  

  matrix->show();
  delay(keyDelay);
  matrix->clear();
}

void setupTuning(){
  int note = 0;
  int octive = 0;

  for (uint8_t row=0; row<ROW; row++) {//Set guitar strings
    note = tuningOffset[5-row];//fret and string 0,0 is high E
    octive = octiveOffset[5-row];
    for (uint8_t col=0; col<COL+1; col++) {//Set guitar frets off by one for open string note hovering column 
      guitar[row][col] = FretKey(COLORS[note], note, row, col, octive);
      //printf("%d:%d %X\t",note,octive,COLORS[note]);
      note++;
      note %= 12;
      if(note == C){//C is where the octive raises
        octive++;
      }            
    }
    //printf("\n");
  }  
}
