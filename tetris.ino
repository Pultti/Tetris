// tuodaan kirjastot näytön käyttöä varten
#include <Elegoo_GFX.h>     // Core graphics library
#include <Elegoo_TFTLCD.h>  // Hardware-specific library


// luodaan vakiot 16-bittisille väriarvoille
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define GRAY 0xCDCD

// luodaan tft olio jolla ohjataan näyttöä ja määritetään TFT LCD:n käytäämät pinnit
Elegoo_TFTLCD tft(A3, A2, A1, A0, A4); 

// joystick
// luodaan muutujat joystickin käyttöä varten
int VRx = A14;
int VRy = A15;
int SW = 53;

bool input = false;

int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int mapX = 0;
int mapY = 0;
// /joystick

// globaali muutuja pisteiden laskua varten
int playerScore = 0;

//pelialue luodaan globaalina muuttujana
//lähes kaikki funkitot joko lukevat tai kirjoittavat arvoja pelialueelle.

//      playArea[y][x]
uint8_t playArea[23][10]{
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  // (0,0) -> x+
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //   |
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //   Y+
// kolme ylintä riviä ei näy näytöllä. uudetpalikat ja game over tilan testaus tapahtuu tämän alueen avulla.  
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};



// määritetään ruudulle piirrettävien neliöiden koko pikseleeinä
const int sqSize = 16;


// määritetään palikkatyypit joita voi esiintyä näytöllä
// erillaisia palikoita on 7
// jokaisellla on neljä eri asentoa
// toistuvat asennot on kuitenkin jätetty pois
// esimerkiksi alla oleva neliö palikka ei muutu pyörittäessä jolloin sillä tarvitsee olla vain yksi asento.

// palikat muodostuvat keskipalikan ympärille jolla on koordinaatti CB.loc muuttujassa.
// allaoleva koodi muodostaa lopun muodosta keskipalikan ympärille suhteessa keskipalikkaan.
// pyöritys tapahtuu keskipalikan ympäri.
int blocks[19][3][2] = {


  //[][]
  //[][]
  { { 1, 0 }, { 0, 1 }, { 1, 1 } },




  //[][][]
  //  []
  { { 1, 0 }, { 0, 1 }, { -1, 0 } },

  //[]
  //[][]
  //[]
  { { 1, 0 }, { 0, 1 }, { 0, -1 } },

  //  []
  //[][][]
  { { 1, 0 }, { 0, -1 }, { -1, 0 } },

  //  []
  //[][]
  //  []
  { { 0, 1 }, { 0, -1 }, { -1, 0 } },




  //[][][][]
  { { -1, 0 }, { 1, 0 }, { 2, 0 } },

  //[]
  //[]
  //[]
  //[]
  { { 0, -1 }, { 0, 1 }, { 0, 2 } },




  //  [][]
  //[][]
  { { 1, 0 }, { 0, 1 }, { -1, 1 } },

  //[]
  //[][]
  //  []
  { { 0, -1 }, { 1, 0 }, { 1, 1 } },




  //[][]
  //  [][]
  { { -1, 0 }, { 0, 1 }, { 1, 1 } },

  //  []
  //[][]
  //[]
  { { 0, -1 }, { -1, 0 }, { -1, 1 } },




  //    []
  //[][][]
  { { 1, -1 }, { -1, 0 }, { 1, 0 } },

  //[][]
  //  []
  //  []
  { { -1, -1 }, { 0, -1 }, { 0, 1 } },

  //[][][]
  //[]
  { { -1, 0 }, { 1, 0 }, { -1, 1 } },

  //[]
  //[]
  //[][]
  { { 0, -1 }, { 0, 1 }, { 1, 1 } },




  //[]
  //[][][]
  { { -1, -1 }, { -1, 0 }, { 1, 0 } },

  //  []
  //  []
  //[][]
  { { -1, 1 }, { 0, -1 }, { 0, 1 } },

  //[][][]
  //    []
  { { -1, 0 }, { 1, 0 }, { 1, 1 } },

  //[][]
  //[]
  //[]
  { { 0, -1 }, { 0, 1 }, { 1, -1 } },

};

// määrittää alotus motojen indeksit blocks taulukosta uuden palikan luontia varten.
int startShapes[7] = { 0, 1, 5, 7, 9, 11, 15 };

// tämä struct säilyttää pelialueella olevan aktiivisen palikan tiedot
struct {
  int loc[2];  // location {x,y} keskipalikan sijainti koordinaatit
  int shape; // blocks taulukon muoto joka on aktiivinen
  int dir;  // direction suunta käytetään pyörittäessä seuraavan muodon hakemiseen
  int type; // palikan tyyppi 1-7 määrittää mm. värin, käytetään myöskin pyörityksessä. 
} CB;  //current block: luodun globaalin structin nimi

// muuttujat ajan tallentamiseen
unsigned long time = 0 , time2 = 0;
// aika joka odotetaan napin painallusten välillä ennen kuin otetaan seuraava input
int inputDelay = 40;


void setup() {
  
  Serial.begin(9600);

  initJoy();
  tft.begin(0x9341); // käynnistää näytön
  tft.setRotation(2); // märittää näytön suunnan
  aloitusRuutu(); // piirtää aloitusnäytön

  // odottaa napin painallusta jatkamista varten
  while(1){
    if(joysticK() == 4){
      break;
    }
  }
  randomSeed(millis());  // pohjustaa satunnais numero generaattorin

  drawUI(); // piirrä käyttöliittymä
  drawPlayArea(); // piirrä pelialue
  spawnBlock(); // luo ensimmäisen palikan aloittaen pelin
}
void loop() {

  // liikuttaa palikkaa alaspäin ajoittain
  if (millis() > time) {
    move(2);
    time = millis() + 500;
  }
  // liikuttaa palikkaa pelajan inputin mukaan
  int dir = joysticK();
  if (dir != -1) {
    if (dir == 4) {
      rotate();
    } else move(dir);
  }
}

// piirtää käyttöliitymän
void drawUI(){
  tft.setTextSize(2);
  tft.fillScreen(GRAY);
  tft.setCursor(165, 16);
  tft.print("SCORE ");
  tft.setCursor(165, 32);
  tft.print("0");
  // tft.setCursor(165, 64);
  // tft.print("NEXT  ");
}


// Ottaa raa'an datan ja muuttaa ne ohjelman ymmärtämiksi inputeiksi
// joystick
int joysticK(){
  
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);
  mapX = map(xPosition, 0, 1023, -512, 512);
  mapY = map(yPosition, 0, 1023, -512, 512);

  if(!input){
    if(mapX > 100){
      input = true;
      time2 = millis() + inputDelay;
      return 2;
    }else if(mapY > 100){
      input = true;
      time2 = millis() + inputDelay;
      return 1;      
    }else if(mapY < -100){
      input = true;
      time2 = millis() + inputDelay;
      return 3;
    }else if(!SW_state){
      input = true;
      time2 = millis() + inputDelay;
      return 4;
    }

  }
  if(input){
    if(time2 < millis()){
      input = false;
    }
  }
  return -1;
}
// määrittää joystickin käyttämät pinnit
void initJoy(){
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(52, OUTPUT);
  digitalWrite(52,HIGH);
}
// /joystick

// tarkistaa kusuttaessa onko peli alueella täysiä rivejä poistaa ne ja palauttaa niiden lukumäärän
int checkRows(){
  int clears = 0;
  bool rowClear;
  for(int i = 22;i >= 0;i--){
    rowClear = true;    
    for(int j = 0;j<10;j++){
      if(playArea[i][j] == 0){
        rowClear = false;
      }
    }
    if(rowClear){
      clearRow(i);
      clears++;
      i++;  
    }
    
  }
  drawPlayArea();
  return clears;
}

// poistaa rivin ja siirtää ylemmät rivit alaspäin
void clearRow(int row){
  for(int i = row;i>= 1;i--){
    for(int j = 0;j<10;j++){
      playArea[i][j] = playArea[i-1][j];
    }
  }
}

// tarkistaa pelin tilan kun palikka jämähtää paikalleen
int checkState(){
  if(gameover()) return -1;
  int rows = checkRows();
  addScore(rows);
  return 0;
}

// käy läpi ensimmäisen pelialueen ulkopuolella olevan rinvin. jos rivillä on muuta kuin 0 (eliseillä on palikka) lopettaa pelin
bool gameover(){
  for(int i = 0;i<10;i++){
    if(playArea[2][i] != 0) return true;
  }
  drawPlayArea();
  return false;
}

// seuraavan palikan tyypin arvonta
int nextBlock() {

  return random(1, 8);
}

// seuraavan palikan alustus ja luonti 
void spawnBlock() {
  CB.loc[0] = 4;
  CB.loc[1] = 1;
  CB.dir = 0;

  CB.type = nextBlock();
  CB.shape = startShapes[CB.type - 1];
  writeBlock(CB.type);
}

//pyörittää palikkaa CB.type ja CB.dir muutujien perusteella valitsee seuraavan muodon
void rotate() {
  writeBlock(0); // poistaa palikan pelialueelta liikuttamista varten
  int prewShape = CB.shape;// tallentaa aikaisemmat arvot jos liikkuminen ei ole mahdollista palauttaa nämä arvot
  int prewDir = CB.dir;
  switch (CB.type) {
    case 1:
      //[][]
      //[][]
      CB.shape = 0;
      break;

    case 2:
      //[][][]
      //  []
      switch (CB.dir) {
        case 0:
          CB.shape = 2;
          CB.dir = 1;
          break;

        case 1:
          CB.shape = 3;
          CB.dir = 2;
          break;

        case 2:
          CB.shape = 4;
          CB.dir = 3;
          break;

        case 3:
          CB.shape = 1;
          CB.dir = 0;
          break;
      }
      break;

    case 3:
      //[][][][]
      switch (CB.dir) {
        case 0:
        case 2:
          CB.shape = 6;
          CB.dir = 1;
          break;

        case 1:
        case 3:
          CB.shape = 5;
          CB.dir = 0;
          break;
      }
      break;

    case 4:
      //  [][]
      //[][]
      switch (CB.dir) {
        case 0:
        case 2:
          CB.shape = 8;
          CB.dir = 1;
          break;

        case 1:
        case 3:
          CB.shape = 7;
          CB.dir = 0;
          break;
      }
      break;

    case 5:
      //[][]
      //  [][]
      switch (CB.dir) {
        case 0:
        case 2:
          CB.shape = 10;
          CB.dir = 1;
          break;

        case 1:
        case 3:
          CB.shape = 9;
          CB.dir = 0;
          break;
      }
      break;

    case 6:
      //    []
      //[][][]
      switch (CB.dir) {
        case 0:
          CB.shape = 12;
          CB.dir = 1;
          break;

        case 1:
          CB.shape = 13;
          CB.dir = 2;
          break;

        case 2:
          CB.shape = 14;
          CB.dir = 3;
          break;

        case 3:
          CB.shape = 11;
          CB.dir = 0;
          break;
      }
      break;

    case 7:
      //[]
      //[][][]
      switch (CB.dir) {
        case 0:
          CB.shape = 16;
          CB.dir = 1;
          break;

        case 1:
          CB.shape = 17;
          CB.dir = 2;
          break;

        case 2:
          CB.shape = 18;
          CB.dir = 3;
          break;

        case 3:
          CB.shape = 15;
          CB.dir = 0;
          break;
      }
      break;
  }
  if (checkBound(4)) { // checkbound funktion palauttaman arvon perusteella liike hyväksytään tai hylätään
    CB.shape = prewShape;
    CB.dir = prewDir;
    writeBlock(CB.type);
  } else {
    writeBlock(CB.type);  
    drawPlayArea();
  }
}

// tarkistaa onko uusi sijainti "laillinen"
// suunta tarvitaan halutun testin määrittämiseen

int checkBound(int dir) {
  // return:
  // 0 ok
  // 1 sivu osuma operaatio ei ole mahdollinen
  // 2 alas osuma operaatio ei mahdollinen ja palikka pysähtyy
  switch (dir) {
    //sivu liike
    case 1:
    case 3:
      //tarkista reunat
      // tarkistaa onko kaikkipalikat pelialueen sisällä      
      if (CB.loc[0] < 0 || CB.loc[0] > 10) return 1;// keskipalkka
      for (int i = 0; i < 3; i++) {
        if ((CB.loc[0] + blocks[CB.shape][i][0]) < 0 || (CB.loc[0] + blocks[CB.shape][i][0]) > 9) return 1;// muut palikat
      }
      //tarkista palikat
      // tarkistaa onko palikoiden kohdalla muita palikoita
      if (playArea[CB.loc[1]][CB.loc[0]] != 0) return 1;
      for (int i = 0; i < 3; i++) {
        if (playArea[CB.loc[1] + blocks[CB.shape][i][1]][CB.loc[0] + blocks[CB.shape][i][0]] != 0) return 1;
      }
      break;

    //alas liike
    case 2:
      //tarkista reunat
      if (CB.loc[1] > 22) return 2;
      for (int i = 0; i < 3; i++) {
        if ((CB.loc[1] + blocks[CB.shape][i][1]) > 22) return 2;
      }
      //tarkista palikat
      if (playArea[CB.loc[1]][CB.loc[0]] != 0) return 2;
      for (int i = 0; i < 3; i++) {
        if (playArea[CB.loc[1] + blocks[CB.shape][i][1]][CB.loc[0] + blocks[CB.shape][i][0]] != 0) return 2;
      }
      break;

    //pyöritys
    case 4:
      //tarkista reunat
      if (CB.loc[0] < 0 || CB.loc[0] > 10 || CB.loc[1] > 22) return 1;
      for (int i = 0; i < 3; i++) {
        if ((CB.loc[0] + blocks[CB.shape][i][0]) < 0 || (CB.loc[0] + blocks[CB.shape][i][0]) > 9 || (CB.loc[1] + blocks[CB.shape][i][1]) > 22) return 1;
      }
      //tarkista palikat
      if (playArea[CB.loc[1]][CB.loc[0]] != 0) return 1;
      for (int i = 0; i < 3; i++) {
        if (playArea[CB.loc[1] + blocks[CB.shape][i][1]][CB.loc[0] + blocks[CB.shape][i][0]] != 0) return 1;
      }
      break;

    default:
      return -1;
  }
  return 0;
}

// kirjoittaa annetun arvon aktiivisen palikan paikalle playarea arrayhin
void writeBlock(int val) {
  playArea[CB.loc[1]][CB.loc[0]] = val;
  for (int i = 0; i < 3; i++) {
    playArea[CB.loc[1] + blocks[CB.shape][i][1]][CB.loc[0] + blocks[CB.shape][i][0]] = val;
  }
}

// liikuttaa palikkaa muuttamalla CB.loc arvoja
void move(int dir) {
  // tallenna aikaisempi sijainti 
  int prewX = CB.loc[0];
  int prewY = CB.loc[1];
  writeBlock(0);
  //drawBlock(0);
  switch (dir) {
    case 1:  //vasemmalle
      CB.loc[0] -= 1;
      break;
    case 2:  //alas
      CB.loc[1] += 1;
      break;
    case 3:  //oikealle
      CB.loc[0] += 1;
      break;
    default:
      break;
  }
  int result = checkBound(dir);
  if (result) {
    CB.loc[0] = prewX;
    CB.loc[1] = prewY;
    writeBlock(CB.type);
    if (result == 2) {  // jos checkbound antaa arvon 2 tarkistetaan pelin tila ja jatketaan seuraavalle kierrokselle 
      if(checkState()== -1) reset();
      spawnBlock();

    }
  } else {
    writeBlock(CB.type);
    drawPlayArea();
  }
}

// piirtää game over tekstin näytölle
void gameOverScreen(){
  tft.fillRect(0, 130, 240, 60, BLACK);
  tft.setTextSize(4);
  tft.setCursor(14, 135);
  tft.print("GAME OVER");
  tft.setTextSize(1);
  tft.setCursor(40, 173);
  tft.print("Press button to play again");
// odottaa napinpainallusta  jatkaakseen
  while(1){
    if(joysticK() == 4){
      break;
    }
  }  
}

// nollaa pelialueen ja muita arvoja uutta peliä varten
void reset(){
  gameOverScreen();
  for (int i = 0; i < 23; i++) {

    for (int j = 0; j < 10; j++) {
      playArea[i][j]=0;
    }
  }
  
  CB.dir = 0;
  CB.shape = 0;
  CB.type = 0;
  CB.loc[0]= 0;
  CB.loc[1]= 0;
  playerScore = 0;
  drawUI();
  drawPlayArea();  
  printScore();
}

// käy läpi playArea arrayn kolme ensimmäistä riviä poislukien ja piirtää koordinaattien mukaan ja arvojen perusteella Play area arrayn sisällön näytölle.
void drawPlayArea() {
  uint16_t color;
  //tulostaa pelialueen globaalista muuttujasta näytölle
  for (int i = 0; i < 20; i++) {

    for (int j = 0; j < 10; j++) {
      switch (playArea[i + 3][j]) {
        case 0:
          color = BLACK;
          break;
        case 1:
          color = RED;
          break;
        case 2:
          color = BLUE;
          break;
        case 3:
          color = YELLOW;
          break;
        case 4:
          color = CYAN;
          break;
        case 5:
          color = MAGENTA;
          break;
        case 6:
          color = 0xABBA;
          break;
        case 7:
          color = GREEN;
          break;
        default:
          color = WHITE;
          break;
      }
      // !!! muuttujat i ja j ovat eripäin kuin playArea arrayssä
      tft.fillRect(j * sqSize, i * sqSize, sqSize, sqSize, color);
      if (!color == BLACK) {
        tft.drawRect(j * sqSize, i * sqSize, sqSize, sqSize, WHITE);
      }
    }
  }
}

// tulostaa pisteet näytölle
void printScore()
{
  tft.fillRect(165, 32, 75, 32, GRAY);
  tft.setCursor(165, 32);  
  tft.print(playerScore);
}

// lisää pisteeitä täytettyjen rivien määrän mukaan
void addScore(int rows)
{
  switch (rows)
  {
    case 1: playerScore += 100;
    break;
    case 2: playerScore += 300;
    break;
    case 3: playerScore += 500;
    break;
    case 4: playerScore += 1200;
    break;
    default:
    playerScore += 25;
  }
printScore();
}

// piirtää aloitus ruudun grafiikan näytölle
void aloitusRuutu(void) {
    
tft.fillScreen(BLACK); 

tft.setTextColor(RED);
tft.setTextSize(2);
tft.setCursor(0, 0);
tft.println("|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|");

  tft.fillRect(45, 50, 145, 50, MAGENTA); 
  tft.drawRect(45, 50, 145, 50, CYAN); 
  tft.fillRect(90, 99, 50, 50, MAGENTA);
  tft.drawLine(90, 99, 90, 149, CYAN);
  tft.drawLine(90, 149, 140, 149, CYAN);
  tft.drawLine(140, 149, 140, 99, CYAN);

  tft.setTextColor(CYAN); 
  tft.setTextSize(3); 
  tft.setCursor(67, 65); 
  tft.println("T"); 

  tft.setTextColor(YELLOW); 
  tft.setTextSize(3); 
  tft.setCursor(85, 65); 
  tft.println("E");

  tft.setTextColor(GREEN); 
  tft.setTextSize(3); 
  tft.setCursor(103, 65); 
  tft.println("T");

  tft.setTextColor(YELLOW); 
  tft.setTextSize(3); 
  tft.setCursor(121, 65); 
  tft.println("R");

tft.setTextColor(GREEN); 
  tft.setTextSize(3); 
  tft.setCursor(134, 65); 
  tft.println("I");

  tft.setTextColor(CYAN); 
  tft.setTextSize(3); 
  tft.setCursor(147, 65); 
  tft.println("S");

tft.setTextColor(CYAN); 
  tft.setTextSize(2); 
  tft.setCursor(45, 250); 
  tft.println("Press button          to play!"); 


//delay(10000);  //Tämä pyyhitään kun nappi aktivoidaan

}
