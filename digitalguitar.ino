// FastLED_NeoMatrix example for single NeoPixel Shield.
// By Marc MERLIN <marc_soft@merlins.org>
// Contains code (c) Adafruit, license BSD

#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>
#include <Keypad.h>
#include <LibPrintf.h>

// Allow temporaly dithering, does not work with ESP32 right now

#define delay FastLED.delay


#define PIN 13

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

struct FretKey{
  uint16_t color;
  Note note;
  uint8_t row;
  uint8_t col;
  uint8_t octive;

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

};

FretKey guitar[ROW][COL+1];

// Convert a BGR 4/4/4 bitmap to RGB 5/6/5 used by Adafruit_GFX
void fixdrawRGBBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
    uint16_t RGB_bmp_fixed[w * h];
    for (uint16_t pixel=0; pixel<w*h; pixel++) {
	uint8_t r,g,b;
	uint16_t color = pgm_read_word(bitmap + pixel);

	//Serial.print(color, HEX);
	b = (color & 0xF00) >> 8;
	g = (color & 0x0F0) >> 4;
	r = color & 0x00F;
	//Serial.print(" ");
	//Serial.print(b);
	//Serial.print("/");
	//Serial.print(g);
	//Serial.print("/");
	//Serial.print(r);
	//Serial.print(" -> ");
	// expand from 4/4/4 bits per color to 5/6/5
	b = map(b, 0, 15, 0, 31);
	g = map(g, 0, 15, 0, 63);
	r = map(r, 0, 15, 0, 31);
	//Serial.print(r);
	//Serial.print("/");
	//Serial.print(g);
	//Serial.print("/");
	//Serial.print(b);
	RGB_bmp_fixed[pixel] = (r << 11) + (g << 5) + b;
	//Serial.print(" -> ");
	//Serial.println(RGB_bmp_fixed[pixel], HEX);
    }
    matrix->drawRGBBitmap(x, y, RGB_bmp_fixed, w, h);
}


//
void draw_guitar() {
    matrix->clear();
    uint16_t notes [] = {RED, ORANGE, YELLOW, CHARTREUSE, GREEN, VERDIGRIS, CYAN, AZURE, BLUE, PURPLE, MAGENTA, ROSE};
    int offSet [] = {7, 0, 5, 10, 2, 7};
    for (uint16_t i=0; i<ROW; i++) {
      int k = offSet[i];
  	  for (uint16_t j=0; j<COL; j++) {
        k++;
        k %= 12;
        matrix->drawPixel(i, j, notes[k]);
    }
	}
  matrix->show();
}

void draw_rainbow_animation(){
  for(uint16_t hue = 0; hue < 360; hue++){
    uint16_t color = getHue(hue);
    
    matrix->fillScreen(color);
    matrix->show();
    delay(5);

  }
}

uint16_t getHue(uint16_t hue){
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  uint16_t phase = hue/60;
  hue *= 4.25;
    switch (phase){
      case 0 : 
        red = 255;
        green = hue;
        blue = 0;
        break;
      case 1 :
        red = 255-hue%255;
        green = 255;
        blue = 0;
        break;
      case 2 :
        red = 0;
        green = 255;
        blue = hue%255;
        break;
      case 3 :
        red = 0;
        green = 255-hue%255;
        blue = 255;            
        break;
      case 4 :
        red = hue%255;
        green = 0;
        blue = 255;          
        break;      
      case 5 :
        red = 255;
        green = 0;
        blue = 255-hue%255;
        break;          
      default:
        break;      
    }
    green *= 0.9;
    blue *= 0.65;
    //printf("%d, %d, %d, %d\n", red, green, blue, hue);
    double normVec = 255/sqrt((uint32_t)red*red+(uint32_t)green*green+(uint32_t)blue*blue);
    red *= normVec;
    green *= normVec;
    blue *= normVec;

    //y(t) = Asin(2πft + φ) = Asin(ωt + φ)
    // #define degToRads 0.017453293 //PI/180
    // #define angle 2.09439516 //120°
    // #define period 3.14
    // red = (uint16_t) 255 * sin((hue*180) * degToRads);
    // green = (uint16_t) 255 * sin((hue*180 - 120) * degToRads);
    // blue = (uint16_t) 255 * sin((hue*180 - 240) * degToRads);

    // red = (red > 255)? 0 : red;
    // green = (green > 255)? 0 : green;
    // blue = (blue > 255)? 0 : blue;

    printf("%d, %d, %d, %f\n", red, green, blue, normVec);
    
    int r = map(red, 0, 255, 0, 31);
    int g = map(green, 0, 255, 0, 63);
    int b = map(blue, 0, 255, 0, 31);
    
    return (r << 11) + (g << 5) + b;
}

void color_check(){
  for(int i=0;i<12;i++){
    uint16_t color = getHue(i*30);
    printf("%X\n",color);
    matrix->fillScreen(color);
    matrix->show();
    delay(2500);
  }
}

uint16_t hsv2rgb565(uint8_t hue, uint8_t sat, uint8_t val){
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;

  uint8_t phase = (uint8_t) hue/42.5;
  uint8_t vSat = map(sat, 0, 255, val, 0);
  uint8_t chroma = 255*(val/255)*(sat/255);

  //printf("%d, %d, %d, %d\n", phase, vSat, chroma, hue);

  switch (phase){
    case 0 : 
      red = val;
      green = (hue*6);
      blue = vSat;
      break;
    case 1 :
      red = chroma-(hue*6)%chroma;
      green = val;
      blue = vSat;
      break;
    case 2 :
      red = vSat;
      green = val;
      blue = (hue*6)%chroma;
      break;
    case 3 :
      red = vSat;
      green = chroma-(hue*6)%chroma;
      blue = val;            
      break;
    case 4 :
      red = (hue*6)%chroma;
      green = vSat;
      blue = val;          
      break;      
    case 5 :
      red = val;
      green = vSat;
      blue = chroma-(hue*6)%chroma;
      break;          
    default:
      red = val;
      green = vSat;
      blue = vSat;      
      break;      
    }
    
    //green *= 0.9;
    //blue *= 0.65;

    if(chroma!=0){
      double normVec = chroma/sqrt((uint32_t)red*red+(uint32_t)green*green+(uint32_t)blue*blue);
      red *= normVec;
      green *= normVec;
      blue *= normVec;
      printf("%d, %d, %d, %f\n", blue, red, green, normVec);
    }
   

    
    
    return (map(red, 0, 255, 0, 31) << 11) + (map(green, 0, 255, 0, 63) << 5) + map(blue, 0, 255, 0, 31);
}

int * getHandShape(){
  static int handShape[ROW] = {-1};
  
  for(uint8_t x=0;x<COL;x++){
    PORTC &= 0xF0; //clear the four bits we want to increment
    PORTC |= x;    //set the four bits we want to increment
    for(uint8_t y=0;y<ROW;y++){
      if(handShape[y] ==-1 && digitalRead(rowPins[y])){
        handShape[y] = x;
      }      
    }
  }
  return handShape;
}

uint8_t rowPins[] = {21,8,9,10,11,12};

void setup() {

  //Setup keyboard
  //Keyboard col port output
  DDRC |= 0x0F;
  PORTC = B00000001;
  //Keyboard row input
  for(uint8_t y=0;y<ROW;y++){
    pinMode(rowPins[y],INPUT);
  }

  // Time for serial port to work?
  delay(1000);
  Serial.begin(115200);
  printf("Init on pin: %d\nMatrix Size: %d\n %d\n %d\n", PIN, ROW, COL, NUMMATRIX);


  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(BRIGHTNESS);
  Serial.println("If the code crashes here, decrease the brightness or turn off the all white display below");
  // Test full bright of all LEDs. If brightness is too high
  // for your current limit (i.e. USB), decrease it.
  //#define DISABLE_WHITE
  #ifndef DISABLE_WHITE
      matrix->fillScreen(WHITE);
      matrix->show();
      delay(3000);
      matrix->clear();
  #endif

  FretKey guitar[ROW][COL+1];

  
  const int tuningOffset [] = {7, 0, 5, 10, 2, 7};//EADGBE tuning, would like to make adjustable in the future
  const int octiveOffset [] = {2,2,3,3,3,4};
  for (uint8_t i=0; i<ROW; i++) {//Set guitar strings
    int k = tuningOffset[5-i];//fret and string 0,0 is high E
    int l = octiveOffset[5-i];
    for (uint8_t j=0; j<COL+1; j++) {//Set guitar frets
      guitar[i][j].color = new FretKey (COLORS[k], k, i, j, l);
      k++;
      k %= 12;
      if(k=3){
        l++;
      }            
    }
  }  
}

void loop() {

// You can't use millis to time frame fresh rate because it uses cli() which breaks millis()
// So I use my stopwatch to count 200 displays and that's good enough
  #if 0
      // 200 displays in 13 seconds = 15 frames per second for 4096 pixels
      for (uint8_t i=0; i<100; i++) { 
    matrix->fillScreen(BLUE);
    matrix->show();
    matrix->fillScreen(RED);
    matrix->show();
      }
  #endif

  uint8_t * handShape = getHandShape();
  for(int i=0;i<ROW;i++){
   guitar[i][handShape[i]].setKeyWhite();
  }


         
  // for(int x=0; x<256; x++){
  //   matrix->fillScreen(hsv2rgb565(x,255,255));
  //   delay(100);
  //   matrix->show();
  // }  

}
