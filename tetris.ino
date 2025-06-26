#include <Elegoo_GFX.h>    // Core graphics library
#include "TouchScreen.h"
#include <Elegoo_TFTLCD.h>

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

#define TS_MINX 130
#define TS_MAXX 905
#define TS_MINY 75
#define TS_MAXY 930
#define MINPRESSURE 20

Elegoo_GFX_Button buttons[5];

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint p;

#define GRAY  0x8410

uint8_t level, downSet, rotary, nextPiece, Piece, lop, lop2, lop3, lop4, row, column, color;
uint16_t colRow[12][22];
int16_t Score;

bool check, next, win = false;
bool start = false;
unsigned long previousMillis = 0;
unsigned long interval = 500;
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

Elegoo_TFTLCD tft(A3, A2, A1, A0, A4);

const bool piece[20][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 1, 0, 0,  //1
    0, 1, 0, 0,
    0, 1, 0, 0,
    0, 1, 0, 0
  },

  { 0, 0, 0, 0,  //2
    0, 0, 0, 0,
    1, 1, 0, 0,
    1, 1, 0, 0
  },

  { 0, 0, 0, 0,  //3
    1, 0, 0, 0,
    1, 0, 0, 0,
    1, 1, 0, 0
  },

  { 0, 0, 0, 0,  //4
    0, 1, 0, 0,
    0, 1, 0, 0,
    1, 1, 0, 0
  },

  { 0, 0, 0, 0,  //5
    0, 0, 0, 0,
    0, 1, 0, 0,
    1, 1, 1, 0
  },

  { 0, 0, 0, 0,  //6
    0, 0, 0, 0,
    0, 1, 1, 0,
    1, 1, 0, 0
  },

  { 0, 0, 0, 0,  //7
    0, 0, 0, 0,
    1, 1, 0, 0,
    0, 1, 1, 0
  },

  { 0, 0, 0, 0,  //8
    0, 0, 0, 0,
    0, 0, 0, 0,
    1, 1, 1, 1
  },

  { 0, 0, 0, 0,  //9
    0, 0, 0, 0,
    0, 0, 1, 0,
    1, 1, 1, 0
  },

  { 0, 0, 0, 0,  //10
    1, 1, 0, 0,
    0, 1, 0, 0,
    0, 1, 0, 0
  },

  { 0, 0, 0, 0,  //11
    0, 0, 0, 0,
    1, 1, 1, 0,
    1, 0, 0, 0
  },

  { 0, 0, 0, 0,  //12
    0, 0, 0, 0,
    1, 1, 1, 0,
    0, 0, 1, 0
  },

  { 0, 0, 0, 0,  //13
    1, 1, 0, 0,
    1, 0, 0, 0,
    1, 0, 0, 0
  },

  { 0, 0, 0, 0,  //14
    0, 0, 0, 0,
    1, 0, 0, 0,
    1, 1, 1, 0
  },

  { 0, 0, 0, 0,  //15
    0, 1, 0, 0,
    1, 1, 0, 0,
    0, 1, 0, 0
  },

  { 0, 0, 0, 0,  //16
    0, 0, 0, 0,
    1, 1, 1, 0,
    0, 1, 0, 0
  },

  { 0, 0, 0, 0,  //17
    1, 0, 0, 0,
    1, 1, 0, 0,
    1, 0, 0, 0
  },

  { 0, 0, 0, 0,  //18
    1, 0, 0, 0,
    1, 1, 0, 0,
    0, 1, 0, 0
  },

  { 0, 0, 0, 0,  //19
    0, 1, 0, 0,
    1, 1, 0, 0,
    1, 0, 0, 0
  }
};
 

void setup() {
  // Timer 1

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 48000; 
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << TOIE1);
  interrupts();          
  Serial.begin(9600);
}

/*  ISR(TIMER1_OVF_vect)        
{
  TCNT1 = 48000;            
  thisNote += 1;
  if(thisNote==64) thisNote=0;
  //iterate over the notes of the melody:
    /*
      to calculate the note duration, take one second divided by the note type.
      e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    */

    /*
      to distinguish the notes, set a minimum time between them.
      the note's duration + 30% seems to work well:
     */
 
   
    //noTone(A5); //stop the tone playing:
//  }
//  digitalWrite(A5,LOW);  } */
void loop() {
  for (lop = 1; lop < 11; lop++) {
    for (lop2 = 1; lop2 < 21; lop2++) {
      colRow[lop][lop2] = 0;
    }
  }
  for (lop = 0; lop < 22; lop++) {
    colRow[0][lop] = 21;
    colRow[11][lop] = 21;
  }
  gameStart();
  newBlock();
  previousMillis = millis();
  do {
    if (millis() - previousMillis > interval) {
      previousMillis = millis();
      row += 1;
      check = true;
      checkDown();
      if (check == false) newBlock();
      else {
        delPiece(column,row,Piece);
        setPiece(column,row,Piece);
      }
    }
    readTouch();
    if (p.z > MINPRESSURE) {
      if ((buttons[3].contains(p.x, p.y)&&rotary==0)) {
        switch(Piece){
        case 1:
          rotaryBlock(1, 8);
          break;
        case 8:
          rotaryBlock(8, 1);
         break;
        case 3:
          rotaryBlock(3, 9);
         break;
        case 9:
          rotaryBlock(9, 10);
        break;
        case 10:
        rotaryBlock(10, 11);
        break;
        case 11:
          rotaryBlock(11, 3);
        break;
        case 4:
          rotaryBlock(4, 12);
        break;
        case 12:
          rotaryBlock(12, 13);
        break;
        case 13:
        rotaryBlock(13, 14);
        break;
        case 14:
          rotaryBlock(14, 4);
        break;
        case 5:
          rotaryBlock(5, 15);
        break;
        case 15:
          rotaryBlock(15, 16);
        break;
        case 16:
          rotaryBlock(16, 17);
        break;
        case 17:
          rotaryBlock(17, 5);
        break;
        case 6:
          rotaryBlock(6, 18);
        break;
        case 18:
        rotaryBlock(18, 6);
        break;
        case 7:
          rotaryBlock(7, 19);
        break;
        case 19:
          rotaryBlock(19, 7);
        break;
        }
        rotary = 0;
        delay(50);
        }
      if ((buttons[2].contains(p.x, p.y))) {
        row += 1;
        check = true;
        checkDown();
        if (check == false) newBlock();
        else {
          delPiece(column,row,Piece);
          setPiece(column,row,Piece);
          }
      }
      if ((buttons[1].contains(p.x, p.y))) {
        column += 1;
        check = true;
        checkDown();
        if (check == false) column -= 1;
        else {
          column -= 1;
            row += 1;
  delPiece(column,row,Piece);
  column += 1;
  row -= 1;
  setPiece(column,row,Piece);
        }
      }
      if ((buttons[0].contains(p.x, p.y))) {
        column -= 1;
        check = true;
        checkDown();
        if (check == false) column += 1;
        else {
          column += 1;
  row += 1;
  delPiece(column,row,Piece);
  column -= 1;
  row -= 1;
  setPiece(column,row,Piece);
        }
      }
      delay(100);
    }
  } while (!win);
delay(1000);
start=true;
win=false;

}


void checkDown() {
  if (row > 20){ 
    check = false;}
  if (check == true){
    if(row>0){ 
    if((colRow[column + 3][row] > 20 && piece[Piece][15]) ||
       (colRow[column + 2][row] > 20 && piece[Piece][14]) ||
       (colRow[column + 1][row] > 20 && piece[Piece][13]) ||
       (colRow[column + 0][row] > 20 && piece[Piece][12]))check = false;
    }
    if(row>1){
     if((colRow[column + 3][row - 1] > 20 && piece[Piece][11]) ||
       (colRow[column + 2][row - 1] > 20 && piece[Piece][10]) ||
       (colRow[column + 1][row - 1] > 20 && piece[Piece][9]) ||
       (colRow[column + 0][row - 1] > 20 && piece[Piece][8]))check = false;
    }
    if(row>2){
     if((colRow[column + 3][row - 2] > 20 && piece[Piece][7]) ||
       (colRow[column + 2][row - 2] > 20 && piece[Piece][6]) ||
       (colRow[column + 1][row - 2] > 20 && piece[Piece][5]) ||
       (colRow[column + 0][row - 2] > 20 && piece[Piece][4]))check = false;
    }
     if(row>3){
     if((colRow[column + 3][row - 3] > 20 && piece[Piece][3]) ||
       (colRow[column + 2][row - 3] > 20 && piece[Piece][2]) ||
       (colRow[column + 1][row - 3] > 20 && piece[Piece][1]) ||
       (colRow[column + 0][row - 3] > 20 && piece[Piece][0]))check = false;
}
}
if(check==false && row==1){
  start=true;
  loop();
}
}
void rotaryBlock(uint8_t oldPice, uint8_t newPice) {
  Piece = newPice;
  check = true;
  checkDown();
  if (check == false) Piece = oldPice;
  else {
    Piece = oldPice;
    row += 1;
    delPiece(column,row,Piece);
    row -= 1;
    Piece = newPice;
    setPiece(column,row,Piece);
  } rotary = 1;
}
void setPiece(uint8_t xx,uint8_t yy, uint8_t zz){
  color=zz;
  if(color==8) color=1;
  if(color>8 && color<12) color=3;
  if(color>11 && color <15) color=4;
  if(color>14 && color <18) color=5;
  if(color==18) color=6;
  if(color==19) color=7;
     if(yy>0){
      if(piece[zz][15]) setBlock(xx + 3, yy,color+downSet);
      if(piece[zz][14]) setBlock(xx + 2,yy,color+downSet);
      if(piece[zz][13]) setBlock(xx + 1, yy,color+downSet);
      if(piece[zz][12]) setBlock(xx + 0,yy,color+downSet);
      }
      if(yy>1){
      //if(piece[zz][11]) setBlock(xx + 3, yy-1,color+downSet);
      if(piece[zz][10]) setBlock(xx + 2,yy-1,color+downSet);
      if(piece[zz][9]) setBlock(xx + 1, yy-1,color+downSet);
      if(piece[zz][8]) setBlock(xx + 0,yy-1,color+downSet);
      }
      if(yy>2){
      //if(piece[zz][7]) setBlock(xx + 3, yy-2,color+downSet);
      if(piece[zz][6]) setBlock(xx + 2,yy-2,color+downSet);
      if(piece[zz][5]) setBlock(xx + 1, yy-2,color+downSet);
      if(piece[zz][4]) setBlock(xx + 0,yy-2,color+downSet);
      }
      if(yy>3){
      //if(piece[zz][3]) setBlock(xx + 3, yy-3,color+downSet);
      if(piece[zz][2]) setBlock(xx + 2,yy-3,color+downSet);
      if(piece[zz][1]) setBlock(xx + 1, yy-3,color+downSet);
      if(piece[zz][0]) setBlock(xx + 0,yy-3,color+downSet);
      }
}
void delPiece(uint8_t xxx,uint8_t yyy, uint8_t zzz){
  if(yyy>1){
      if(piece[zzz][15]) delBlock(xxx + 3, yyy-1);
      if(piece[zzz][14]) delBlock(xxx + 2,yyy-1);
      if(piece[zzz][13]) delBlock(xxx + 1, yyy-1);
      if(piece[zzz][12]) delBlock(xxx + 0,yyy-1);
      }
      if(yyy>2){
      //if(piece[zzz][11]) delBlock(xxx + 3, yyy-2);
      if(piece[zzz][10]) delBlock(xxx + 2,yyy-2);
      if(piece[zzz][9]) delBlock(xxx + 1, yyy-2);
      if(piece[zzz][8]) delBlock(xxx + 0,yyy-2);
      }
      if(yyy>3){
      //if(piece[zzz][7]) delBlock(xxx + 3, yyy-3);
      if(piece[zzz][6]) delBlock(xxx + 2,yyy-3);
      if(piece[zzz][5]) delBlock(xxx + 1, yyy-3);
      if(piece[zzz][4]) delBlock(xxx + 0,yyy-3);
      }
      if(yyy>4){
      //if(piece[zzz][3]) delBlock(xxx + 3, yyy-4);
      if(piece[zzz][2]) delBlock(xxx + 2,yyy-4);
      if(piece[zzz][1]) delBlock(xxx + 1, yyy-4);
      if(piece[zzz][0]) delBlock(xxx + 0,yyy-4);
      }
}
void delBlock(uint8_t x, uint8_t y) {
  tft.fillRect(x * 20, y * 20 + 40, 18, 18, color);
  if(!start) colRow[x][y] = 0;
}
void setBlock(uint8_t x, uint8_t y, uint8_t z) {
  {
    tft.fillRect(x * 20 + 1 , y * 20 + 41, 16, 16, color);
    tft.drawRect(x * 20 , (y + 2) * 20 , 18, 18,  WHITE);
  }
  if(!start) colRow[x][y] = z;
}
void newBlock() {
  Score+=4;
  downSet=20;
  row-=1;
  setPiece(column,row,Piece);
  for (lop4 = 0; lop4 < 4; lop4++) {
    lop = 21;
    do {
      lop -= 1;
      if (colRow[1][lop] > 20 && colRow[2][lop] > 20 && colRow[3][lop] > 20 && colRow[4][lop] > 20 && colRow[5][lop] > 20
          && colRow[6][lop] > 20 && colRow[7][lop] > 20 && colRow[8][lop] > 20 && colRow[9][lop] > 20 && colRow[10][lop] > 20) {
            Score+=10;
        for (lop2 = 1; lop2 < 11; lop2++) {
          delBlock(lop2, lop);
          lop3 = lop;
          do {
            lop3 -= 1;
            if (colRow[lop2][lop3] > 0) {
              setBlock(lop2, lop3 + 1, colRow[lop2][lop3]);
            } else {
              delBlock(lop2, lop3 + 1);
            }
          } while (lop3 > 0);
         }
        lop = 0;
      }
    } while (lop > 0);
  }
  downSet=0;
  tft.fillRect(240,90,60,14,BLUE);
  tft.setTextSize(2);
  tft.setCursor(240, 90);
  tft.setTextColor(WHITE);
  if(Score<10) {level=1;tft.print("0000"+String(Score));}
  if(Score<100 && Score >9) {level=2;interval=400;tft.print("000"+String(Score));}
  if(Score<1000 && Score >99) {level=3;interval=300;tft.print("00"+String(Score));}
  if(Score<10000 && Score >999) {level=4; interval=200;tft.print("0"+String(Score));}
  if(Score<100000 && Score >9999) {level=5;interval=100;tft.print(Score);}
  tft.fillRect(265,140,14,14,BLUE);
  tft.setCursor(265, 140);
  tft.print(level);
  Piece=nextPiece;
  tft.fillRect(240,200,80,80,BLUE);
  nextPiece=random(1,20);
  start=true;
  setPiece(12,11,nextPiece);
  start=false;
  column = 6;
  row = 0;
  }
void gameStart() {
  nextPiece=random(1,20);
  Score=-4;
  if(!start){
  tft.fillScreen(BLUE);
  tft.setTextSize(4);
  tft.setCursor(32, 15);
  tft.setTextColor(WHITE);
  tft.print("T E T R I S");
  tft.setCursor(30, 12);
  tft.setTextColor(BLACK);
  tft.print("T E T R I S");
  tft.fillRect(20, 60, 200, 400, BLACK);
  tft.setTextSize(2);
  tft.setCursor(240, 70);
  tft.setTextColor(WHITE);
  tft.print("Score");
  tft.setCursor(240, 90);
  tft.print("00000");
  tft.setCursor(240, 120);
  tft.print("Level");
  buttons[0].initButton(&tft, 246, 416, 45, 30, WHITE, GREEN, BLACK, "<", 1);
  buttons[0].drawButton();
  buttons[1].initButton(&tft, 294, 416, 45, 30, WHITE, GREEN, BLACK, ">", 1);
  buttons[1].drawButton();
  buttons[2].initButton(&tft, 270, 450, 45, 30, WHITE, GREEN, BLACK, "D", 1);
  buttons[2].drawButton();
  buttons[3].initButton(&tft, 270, 382, 45, 30, WHITE, GREEN, BLACK, "R", 1);
  buttons[3].drawButton();
  }
rollo();
}


void readTouch() {
  p = ts.getPoint();
  p.x = p.x + p.y;
  p.y = p.x - p.y;
  p.x = p.x - p.y;
  p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
  p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
}

void rollo(){
    start=true;
  for(lop=1;lop<21;lop++){
  for(lop2=1;lop2<11;lop2++){
  setBlock(lop2, lop,2);
  }
  }
  delay(250);
  for(lop=20;lop>0;lop--){
  for(lop2=10;lop2>0;lop2--){
  delBlock(lop2, lop);
  }
    start=false;
    delay(20);
  }
}