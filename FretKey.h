class FretKey{

public:
  uint16_t color;
  int note;
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

  FretKey (uint16_t clr, int n, uint8_t r, uint8_t c, uint8_t o){
    color = clr;
    note = n;
    row = r;
    col = c;
    octive = o;    
  }

};
