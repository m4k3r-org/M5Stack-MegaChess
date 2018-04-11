 //ArduinoMega Chess 1.0 M5Stack
//engine 1.4 
//Sergey Urusov, ususovsv@gmail.com

#include <M5Stack.h> 
#include <Preferences.h>

Preferences prefs;

uint16_t CBLACK,CBLUE,CRED,CGREEN,CCYAN,CMAGENTA,CYELLOW,CWHITE,CGRAY,CDARK,CGRAY2,CBLACKF,CWHITEF,CWHITEFIG,CBLACKFIG,CBLACKCONT;

int cycle=0; // 
const signed char fp=1;
const signed char fn=2;
const signed char fb=3;
const signed char fr=4;
const signed char fq=5;
const signed char fk=6;
const int fig_weight[]={0,100,320,330,500,900,0};
const char fig_symb[]="  NBRQK";
unsigned long count;
boolean rotate=false;
const signed char polezero[8][8] PROGMEM={
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             };  
const signed char polestart[8][8] PROGMEM={
             {-fr,-fn,-fb,-fq,-fk,-fb,-fn,-fr},
             {-fp,-fp,-fp,-fp,-fp,-fp,-fp,-fp},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             { fp, fp, fp, fp, fp, fp, fp, fp},
             { fr, fn, fb, fq, fk, fb, fn, fr},
             };  
             
signed char pole[8][8]={ //  3 
             {-fr,-fn,-fb,-fq,-fk,-fb,-fn,-fr},
             {-fp,-fp,-fp,-fp,-fp,-fp,-fp,-fp},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             {  0,  0,  0,  0,  0,  0,  0,  0},
             { fp, fp, fp, fp, fp, fp, fp, fp},
             { fr, fn, fb, fq, fk, fb, fn, fr},
             };  
             
signed char pole0[8][8];  //   
signed char poledisp[8][8]; // 
signed char polechoice[7]; // 
boolean w00,w000,b00,b000;             
signed char blinkstep;
typedef struct {
  signed char fig1, fig2; //    
  signed char x1,y1,x2,y2; //     
  signed char check; //  
  signed char type; //  1-  ,2- ,3- ,4-5-6-7-  ,,,
  int weight; // , 
} step_type;
const int MAXSTEPS=1000; //.      
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int MINDEPTH; //.   
int MAXDEPTH; //.   
int LIMDEPTH; //.       
const signed char MAXCUTS=10; //.. -  - (10-  ,  20)
boolean TRACE=0;
boolean solving=false;
boolean choice=false;
boolean sound=1;
short cur_step=1; //  , 1....
short limit=0; //  , 0-2;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
const long limits[3]={4,15,60};
//                     0  1  2
step_type steps[MAXSTEPS]; //  
step_type cuts[MAXCUTS]; //  -
step_type lastbest,lastanimatedgreen;
int lastscore;
int minbeta,maxalpha;
int startweight;
int cur_level; //   ()
int start_var; //   , 1....
int cur_var; //  , 1....
int current_var; //     , 1....
int cur_choice; //   
boolean check_on_table; //  
boolean isstatus;
signed char WKJ=0,WKI=0,BKJ=0,BKI=0; // 
boolean endspiel=false; // 
signed char progress; //   0-100
boolean only_action=false; // -  
unsigned long starttime,endtime,limittime,quitime=0;             
int menu=0; // 
unsigned long lastpressed,lastpressedleft,lastpressedright,lastpressedany;
boolean submenu=0;
boolean movesload=0;
boolean isbeta=1;
boolean hidden=false;

const uint8_t fig[6][72] PROGMEM={
{0x0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x7E, 0x0,  0x0,  0xFF, 0x0,
0x0,  0xFF, 0x0,  0x0,  0xFF, 0x0,  0x0,  0x7E, 0x0,  0x0,  0x3C, 0x0,  0x1,  0xFF, 0x80, 0x1,  0xFF, 0x80,
0x0,  0x3C, 0x0,  0x0,  0x3C, 0x0,  0x0,  0x7E, 0x0,  0x0,  0x7E, 0x0,  0x0,  0xFF, 0x0,  0x1,  0xFF, 0x80,
0x3,  0xFF, 0xC0, 0x7,  0xFF, 0xE0, 0x7,  0xFF, 0xE0, 0x7,  0xFF, 0xE0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0
}, //
{0x0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x66, 0x0,  0x0,  0xFF, 0x0,  0x1,  0xFF, 0x80, 0x3,  0xFF, 0xC0,
0x7,  0xFF, 0xE0, 0xF,  0xFF, 0xF0, 0x1F, 0xFF, 0xF8, 0x1F, 0x3F, 0xF8, 0x1E, 0x3F, 0xF8, 0xC,  0x7F, 0xF8,
0x0,  0xFF, 0xF0, 0x1,  0xFF, 0xE0, 0x1,  0xFF, 0xC0, 0x1,  0xFF, 0x80, 0x1,  0xFF, 0x0,  0x0,  0xFE, 0x0,
0x0,  0x7E, 0x0,  0x7,  0xFF, 0xE0, 0xF,  0xFF, 0xF0, 0xF,  0xFF, 0xF0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0
},  //
{0x0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x3C, 0x0,  0x0,  0x7E, 0x0,  0x0,  0x7E, 0x0,  0x0,  0x3C, 0x0,
0x1,  0xFF, 0x80, 0x3,  0xFF, 0xC0, 0x7,  0xFF, 0xE0, 0x7,  0xFF, 0xE0, 0x7,  0xFF, 0xE0, 0x7,  0xFF, 0xE0,
0x7,  0xFF, 0xE0, 0x7,  0xFF, 0xE0, 0x7,  0xFF, 0xE0, 0x3,  0xFF, 0xC0, 0x1,  0xFF, 0x80, 0x0,  0xFF, 0x0,
0x8,  0x7E, 0x10, 0x1C, 0x7E, 0x38, 0x3F, 0xFF, 0xFC, 0x3F, 0xFF, 0xFC, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0
}, // 
{0x0, 0x0,  0x0,  0x0,  0x0,  0x0,  0xF,  0x3C, 0xF0, 0xF,  0x3C, 0xF0, 0xF,  0xFF, 0xF0, 0xF,  0xFF, 0xF0,
0x7,  0xFF, 0xE0, 0x3,  0xFF, 0xC0, 0x3,  0xFF, 0xC0, 0x3,  0xFF, 0xC0, 0x3,  0xFF, 0xC0, 0x3,  0xFF, 0xC0,
0x3,  0xFF, 0xC0, 0x3,  0xFF, 0xC0, 0x3,  0xFF, 0xC0, 0x3,  0xFF, 0xC0, 0x3,  0xFF, 0xC0, 0x3,  0xFF, 0xC0,
0x3,  0xFF, 0xC0, 0x7,  0xFF, 0xE0, 0xF,  0xFF, 0xF0, 0xF,  0xFF, 0xF0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0
}, //
{0x0, 0x0,  0x0,  0x0,  0x18, 0x0,  0x3,  0x3C, 0xC0, 0x7,  0x99, 0xE0, 0x33, 0x18, 0xCC, 0x7B, 0x18, 0xDE,
0x33, 0x18, 0xCC, 0x33, 0x18, 0xCC, 0x33, 0x18, 0xCC, 0x33, 0x18, 0xCC, 0x33, 0x18, 0xCC, 0x33, 0xBD, 0xCC,
0x1B, 0xBD, 0xD8, 0x1F, 0xFF, 0xF8, 0x1F, 0xFF, 0xF8, 0xF,  0xFF, 0xF0, 0xF,  0xFF, 0xF0, 0x7,  0xFF, 0xE0,
0x3,  0xFF, 0xC0, 0x7,  0xFF, 0xE0, 0xF,  0xFF, 0xF0, 0xF,  0xFF, 0xF0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0
}, //
{0x0, 0x0,  0x0,  0x0,  0x3C, 0x0,  0x0,  0x3C, 0x0,  0x0,  0x3C, 0x0,  0xF,  0x3C, 0xF0, 0x1F, 0xFF, 0xF8,
0x3F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFE, 0x7F, 0xFF, 0xFE, 0x7F, 0xFF, 0xFE, 0x7F, 0xFF, 0xFE, 0x7F, 0xFF, 0xFE,
0x7F, 0xFF, 0xFE, 0x7F, 0xFF, 0xFE, 0x3F, 0xFF, 0xFC, 0x1F, 0xFF, 0xF8, 0xF,  0xFF, 0xF0, 0x7,  0xFF, 0xE0,
0x3,  0xFF, 0xC0, 0x7,  0xFF, 0xE0, 0xF,  0xFF, 0xF0, 0xF,  0xFF, 0xF0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0
}  //
};
const uint8_t fig_cont[6][72] PROGMEM={
{0x0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x7E, 0x0,  0x0,  0x81, 0x0,  0x1,  0x0,  0x80,
0x1,  0x0,  0x80, 0x1,  0x0,  0x80, 0x0,  0x81, 0x0,  0x3,  0xC3, 0xC0, 0x2,  0x0,  0x40, 0x2,  0x0,  0x40,
0x3,  0xC3, 0xC0, 0x0,  0x42, 0x0,  0x0,  0x81, 0x0,  0x0,  0x81, 0x0,  0x1,  0x0,  0x80, 0x2,  0x0,  0x40,
0x4,  0x0,  0x20, 0x8,  0x0,  0x10, 0x8,  0x0,  0x10, 0x8,  0x0,  0x10, 0xF,  0xFF, 0xF0, 0x0,  0x0,  0x0
}, //
{0x0, 0x0,  0x0,  0x0,  0x66, 0x0,  0x0,  0x99, 0x0,  0x1,  0x0,  0x80, 0x2,  0xC0, 0x40, 0x4,  0xC0, 0x20,
0x8,  0x0,  0x10, 0x10, 0x0,  0x8,  0x20, 0x0,  0x4,  0x20, 0xC0, 0x4,  0x21, 0x40, 0x4,  0x12, 0x80, 0x4,
0xD,  0x0,  0x8,  0x2,  0x0,  0x10, 0x2,  0x0,  0x20, 0x2,  0x0,  0x40, 0x2,  0x0,  0x80, 0x1,  0x1,  0x0,
0x7,  0x81, 0xE0, 0x8,  0x0,  0x10, 0x10, 0x0,  0x8,  0x10, 0x0,  0x8,  0x1F, 0xFF, 0xF8, 0x0,  0x0,  0x0
},  //
{0x0, 0x0,  0x0,  0x0,  0x3C, 0x0,  0x0,  0x42, 0x0,  0x0,  0x81, 0x0,  0x0,  0x81, 0x0,  0x1,  0xC3, 0x80,
0x2,  0x18, 0x40, 0x4,  0x18, 0x20, 0x8,  0x18, 0x10, 0x8,  0x18, 0x10, 0x9,  0xFF, 0x90, 0x9,  0xFF, 0x90,
0x8,  0x18, 0x10, 0x8,  0x18, 0x10, 0x8,  0x18, 0x10, 0x4,  0x18, 0x20, 0x2,  0x18, 0x40, 0x9,  0x0,  0x90,
0x14, 0x81, 0x28, 0x23, 0x81, 0xC4, 0x40, 0x0,  0x2,  0x40, 0x0,  0x2,  0x7F, 0xFF, 0xFE, 0x0,  0x0,  0x0
}, // 
{0x0, 0x0,  0x0,  0x1F, 0x3C, 0xF8, 0x10, 0xC3, 0x8,  0x10, 0xC3, 0x8,  0x10, 0x0,  0x8,  0x10, 0x0,  0x8,
0xB,  0xFF, 0xD0, 0x4,  0x0,  0x20, 0x4,  0x0,  0x20, 0x4,  0x0,  0x20, 0x4,  0x0,  0x20, 0x4,  0x0,  0x20,
0x4,  0x0,  0x20, 0x4,  0x0,  0x20, 0x4,  0x0,  0x20, 0x4,  0x0,  0x20, 0x4,  0x0,  0x20, 0x4,  0x0,  0x20,
0x4,  0x0,  0x20, 0xB,  0xFF, 0xD0, 0x10, 0x0,  0x8,  0x10, 0x0,  0x8,  0x1F, 0xFF, 0xF8, 0x0,  0x0,  0x0
}, //
{0x0, 0x18, 0x0,  0x3,  0x24, 0xC0, 0x4,  0xC3, 0x20, 0x38, 0x66, 0x1C, 0x4C, 0xA5, 0x32, 0x84, 0xA5, 0x21,
0x4C, 0xA5, 0x32, 0x4C, 0xA5, 0x32, 0x4C, 0xA5, 0x32, 0x4C, 0xA5, 0x32, 0x4C, 0xA5, 0x32, 0x4C, 0x42, 0x32,
0x24, 0x42, 0x24, 0x20, 0x0,  0x4,  0x20, 0x0,  0x4,  0x10, 0x0,  0x8,  0x10, 0x0,  0x8,  0x8,  0x0,  0x10,
0x4,  0x0,  0x20, 0xF,  0xFF, 0xF0, 0x10, 0x0,  0x8,  0x10, 0x0,  0x8,  0x1F, 0xFF, 0xF8, 0x0,  0x0,  0x0
}, //
{0x0, 0x7E, 0x0,  0x0,  0x42, 0x0,  0x0,  0x42, 0x0,  0xF,  0xC3, 0xF0, 0x10, 0xC3, 0x8,  0x20, 0x7E, 0x4,
0x40, 0x3C, 0x2,  0x80, 0x18, 0x1,  0x80, 0x18, 0x1,  0x80, 0x18, 0x1,  0x80, 0x18, 0x1,  0x80, 0x18, 0x1,
0x80, 0x18, 0x1,  0x80, 0x18, 0x1,  0x40, 0x18, 0x2,  0x20, 0x18, 0x4,  0x10, 0x18, 0x8,  0x8,  0x18, 0x10,
0x4,  0x18, 0x20, 0xB,  0xFF, 0xD0, 0x10, 0x0,  0x8,  0x10, 0x0,  0x8,  0x1F, 0xFF, 0xF8, 0x0,  0x0,  0x0
}  //
};


const int pos[7][8][8] PROGMEM={
 {{  0,  0,  0,  0,  0,  0,  0,  0}, // 
  {100,100,100,100,100,100,100,100},  //{ 50, 50, 50, 50, 50, 50, 50, 50},
  { 20, 30, 40, 50, 50, 40, 30, 20},  //{ 10, 10, 20, 30, 30, 20, 10, 10},
  {  5,  5, 10, 25, 25, 10,  5,  5},
  {  0,  0,  0, 20, 20,  0,  0,  0},
  {  5, -5,-10,  0,  0,-10, -5,  5},
  {  5, 10, 10,-20,-20, 10, 10,  5},  //{  5, 10, 10,-20,-20, 10, 10,  5},
  {  0,  0,  0,  0,  0,  0,  0,  0}},
  
 {{-50,-40,-30,-30,-30,-30,-40,-50}, //
  {-40,-20,  0,  0,  0,  0,-20,-40},
  {-30,  0, 10, 15, 15, 10,  0,-30},
  {-30,  5, 15, 20, 20, 15,  5,-30},
  {-30,  0, 15, 20, 20, 15,  0,-30},
  {-30,  5, 10, 15, 15, 10,  5,-30},
  {-40,-20,  0,  5,  5,  0,-20,-40},
  {-50,-40,-30,-30,-30,-30,-40,-50}}, 
  
 {{-20,-10,-10,-10,-10,-10,-10,-20}, //
  {-10,  0,  0,  0,  0,  0,  0,-10},
  {-10,  0,  5, 10, 10,  5,  0,-10},
  {-10,  5,  5, 10, 10,  5,  5,-10},
  {-10,  0, 10, 10, 10, 10,  0,-10},
  {-10, 10, 10, 10, 10, 10, 10,-10},
  {-10,  5,  0,  0,  0,  0,  5,-10},
  {-20,-10,-10,-10,-10,-10,-10,-20}},  

 {{  0,  0,  0,  0,  0,  0,  0,  0}, //
  {  5, 10, 10, 10, 10, 10, 10,  5},
  { -5,  0,  0,  0,  0,  0,  0, -5},
  { -5,  0,  0,  0,  0,  0,  0, -5},
  { -5,  0,  0,  0,  0,  0,  0, -5},
  { -5,  0,  0,  0,  0,  0,  0, -5},
  { -5,  0,  0,  0,  0,  0,  0, -5},
  {  0,  0,  0,  5,  5,  0,  0,  0}},
  
 {{-20,-10,-10, -5, -5,-10,-10,-20}, //
  {-10,  0,  0,  0,  0,  0,  0,-10},
  {-10,  0,  5,  5,  5,  5,  0,-10},
  { -5,  0,  5,  5,  5,  5,  0, -5},
  {  0,  0,  5,  5,  5,  5,  0, -5},
  {-10,  5,  5,  5,  5,  5,  0,-10},
  {-10,  0,  5,  0,  0,  0,  0,-10},
  {-20,-10,-10, -5, -5,-10,-10,-20}},  

 {{-30,-40,-40,-50,-50,-40,-40,-30}, //
  {-30,-40,-40,-50,-50,-40,-40,-30},
  {-30,-40,-40,-50,-50,-40,-40,-30},
  {-30,-40,-40,-50,-50,-40,-40,-30},
  {-20,-30,-30,-40,-40,-30,-30,-20},
  {-10,-20,-20,-20,-20,-20,-20,-10},
  { 10, 10,-10,-10,-10,-10, 10, 10},  //{ 20, 20,  0,  0,  0,  0, 20, 20},
  { 10, 40, 30,  0,  0,  0, 50, 10}}, //{ 20, 30, 10,  0,  0, 10, 30, 20}},

 {{-50,-40,-30,-20,-20,-30,-40,-50}, // 
  {-30,-20,-10,  0,  0,-10,-20,-30},
  {-30,-10, 20, 30, 30, 20,-10,-30},
  {-30,-10, 30, 40, 40, 30,-10,-30},
  {-30,-10, 30, 40, 40, 30,-10,-30},
  {-30,-10, 20, 30, 30, 20,-10,-30},
  {-30,-30,  0,  0,  0,  0,-30,-30},
  {-50,-30,-30,-30,-30,-30,-30,-50}}  
};

//********************************** 
void drawBitmap(int16_t x, int16_t y,
            const uint8_t *bitmap, int16_t w, int16_t h,
            uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        M5.Lcd.drawPixel(x+i, y+j, color);
      }      
    }
  }
}
//****************************
uint8_t music[1000]; 
void tone_volume(uint16_t frequency, uint32_t duration) { 
  float interval=0.001257*float(frequency);  
  float phase=0;
  for (int i=0;i<1000;i++) {
   music[i]=127+126*sin(phase);  
   phase+=interval;
  }   
  music[999]=0;
  int remains=duration;
  for (int i=0;i<duration;i+=200) {   
   if (remains<200) {
    music[remains*999/200]=0;
   }
   M5.Speaker.playMusic(music,5000);  
   remains-=200;
  } 
}
//****************************
void beep(int leng) {
  if (sound) M5.Speaker.setVolume(1); else M5.Speaker.setVolume(0);            
  tone_volume(1000,leng); 
}  
//****************************
void definecolors() {
 CBLACK  =0x0000;
 CBLUE   =0x07FF;  //0x001F;
 CRED    =0xF800;
 CGREEN  =0x07E0;
 CCYAN   =0x07FF;
 CMAGENTA=0xF81F;
 CYELLOW =0xFFE0;
 CWHITE  =0xFFFF;
 CWHITEFIG  =getColor(200,200,140); 
 CGRAY   =0x7BEF;
 CDARK   =getColor(32,32,32); 
 CGRAY2   =getColor(16,16,16); 
 CBLACKF  =getColor(114,75,0);
 CWHITEF  =getColor(180,114,0); 
 CBLACKFIG  =getColor(15,24,8); 
 CBLACKCONT =getColor(160,160,120);  
}
//****************************
uint16_t getColor(uint8_t red, uint8_t green, uint8_t blue)
{
  red   >>= 3;
  green >>= 2;
  blue  >>= 3;
  return (red << 11) | (green << 5) | blue;
}
//****************************
void clearstatus() {
  M5.Lcd.fillRect(240,166,80,20,BLACK);             
  M5.Lcd.drawFastHLine(10,237,220,BLACK);   
  M5.Lcd.drawFastHLine(10,239,220,BLACK);  
}  
//****************************
void setup() {      
  Serial.begin(115200);
  Serial.flush();  
  Serial.println(F("Start"));       
  M5.begin();   
  
  prefs.begin("AM_Chess", false);  
            
  M5.Lcd.setBrightness(200);    // BRIGHTNESS = MAX 255
  M5.Lcd.fillScreen(BLACK);     // CLEAR SCREEN
  M5.Lcd.setRotation(0);        // SCREEN ROTATION = 0  
  definecolors();
  beep(100);      
  for (int i=0;i<MAXSTEPS;i++) {
    steps[i].x1=0; steps[i].y1=0;
    steps[i].x2=0; steps[i].y2=0;
    steps[i].fig1=0; steps[i].fig2=0;
    steps[i].check=0;
    steps[i].type=0;
    steps[i].weight=0;
  }     
  M5.Lcd.setTextColor(getColor(230,230,200));
  M5.Lcd.setTextSize(2);  
  M5.Lcd.setCursor(50,30); M5.Lcd.print("Arduino Mega Chess");
  M5.Lcd.setCursor(88,50); M5.Lcd.print("for M5Stack");
  M5.Lcd.setTextColor(CGRAY);
  M5.Lcd.setCursor(62,70); M5.Lcd.print("by Sergey Urusov");
  M5.Lcd.setTextSize(1);  
  M5.Lcd.setCursor(100,95); M5.Lcd.print("urusovsv@gmail.com");
  M5.Lcd.setCursor(85,110); M5.Lcd.print("version 1.0, engine 1.4");
  M5.Lcd.setCursor(80,220); M5.Lcd.print("press any key to READ this");
  M5.Lcd.setTextColor(CWHITEFIG);
  M5.Lcd.drawRect(38,150,60,30,CGRAY);    
  M5.Lcd.drawRect(130,150,60,30,CGRAY);    
  M5.Lcd.drawRect(225,150,60,30,CGRAY);    
  M5.Lcd.setTextSize(1);  
  M5.Lcd.setCursor(57,162); M5.Lcd.print("Left");
  M5.Lcd.setCursor(143,162); M5.Lcd.print("Choice");
  M5.Lcd.setCursor(240,162); M5.Lcd.print("Right");
  M5.Lcd.setCursor(110,190); M5.Lcd.print("long press - menu");
  M5.Lcd.setCursor(110,200); M5.Lcd.print("or exit from menu");
  
  uint32_t tim=millis();
  while (millis()-tim<1500) { 
     M5.update();
    if (M5.BtnA.wasPressed()||M5.BtnB.wasPressed()||M5.BtnC.wasPressed()) {
      beep(100);
      M5.Lcd.fillRect(75,220,180,12,CBLACK);    
      M5.Lcd.setTextColor(CGRAY);
      M5.Lcd.setCursor(77,220); M5.Lcd.print("press any key to ENTER game");
      while (millis()-tim<30000) { 
        M5.update();
        if (M5.BtnA.wasPressed()||M5.BtnB.wasPressed()||M5.BtnC.wasPressed()) break;
      }
      break;   
    }
  }
  beep(100);      
  M5.Lcd.fillScreen(BLACK);     // CLEAR SCREEN
  initboard();
}
//****************************
void initboard() {        
  for (int i=0;i<8;i++)
   for (int j=0;j<8;j++) poledisp[j][i]=-100; //          
  M5.Lcd.fillScreen(CBLACK);      
  M5.Lcd.setTextColor(CGRAY);
  M5.Lcd.setTextSize(1);  
  for (int j=1;j<9;j++) {
   M5.Lcd.setCursor(0,j*28-17);  
   if (rotate) M5.Lcd.print(j); else M5.Lcd.print(9-j);
  }
  for (byte i=1;i<9;i++) { 
   M5.Lcd.setCursor(i*28-9,227);    
   if (rotate) M5.Lcd.print(char(96+9-i)); else M5.Lcd.print(char(96+i));  
  }         
  initscreen();          
}
//****************************
void initscreen() {        
 show_board();   
 show_steps();      
}
//****************************
void loop() {        
  gui();  
  if (solving) {        
    for (int i=1;i<3;i++) { tone_volume(400+400*i,100); delay(70); }       
    hidden=false;
    lastscore=solve_step();          
    float tim=float(millis()-starttime)/1000;
    for (int i=2;i>=1;i--) { tone_volume(400+400*i,100); delay(70); }
    if (lastscore>-9000&&lastscore!=8999) {             
      movestep(cur_step);       
      cur_step++; steps[cur_step].fig1=0; 
    }     
    initscreen();            
    animate_step(cur_step-1,false);    
    clearstatus();    
    if (lastscore>9000) {       
      M5.Lcd.setTextSize(1);    
      M5.Lcd.setTextColor(CGREEN,CBLACK);       
      M5.Lcd.setCursor(260,176); 
      if (lastscore<9999) {
       M5.Lcd.print("# in "); 
       M5.Lcd.print((9999-lastscore)/2+1); M5.Lcd.print(F(" st"));        
      } else M5.Lcd.print("Checkmate"); 
    } else if (lastscore<-9000) {
      M5.Lcd.setTextSize(1);    
      M5.Lcd.setTextColor(CRED,CBLACK);       
      M5.Lcd.setCursor(260,176);  
      steps[cur_step].fig1=0; 
      M5.Lcd.print("GIVE UP!");       
      show_steps();
    } else if (lastscore==8999) { //Draw
      M5.Lcd.setTextSize(1);    
      M5.Lcd.setTextColor(CYELLOW);       
      M5.Lcd.setCursor(260,176); 
      M5.Lcd.print("Draw");       
    }         
    M5.Lcd.setTextSize(1);    
    M5.Lcd.setTextColor(CGRAY);       
    M5.Lcd.setCursor(240,176); 
    M5.Lcd.print(tim,0); 
    M5.Lcd.print("s ");    
    if (abs(lastscore)<5000) {   
     M5.Lcd.print(lastscore);     
    }    
    M5.Lcd.setCursor(240,166); 
    M5.Lcd.print(count); 
    M5.Lcd.print("pos");   
    Serial.print(F("Positions estimated=")); Serial.println(count);
    Serial.print(F("Time=")); Serial.print(tim,1); Serial.println("s");    
    Serial.print(F("Speed=")); Serial.print(count/tim,1); Serial.println("nps");    
    lastpressedany=millis();        
  }
  delay(10);  
  
  
}
//****************************
void load_moves() {
 start_var=cur_step+21;
 only_action=false;     
 lastbest.fig1=0;
 steps[0].fig1=0; 
 load_variants(cur_step);    
 current_var=start_var;           
 int cv=cur_var; 
 hidden=true;
 uint32_t tim=millis();
 solve_step();
 if (millis()-tim<1000) delay(millis()-tim);
 cur_var=cv; 
 sort_variants(cur_step+21,cur_step+20+cur_var);      
 
}
//********************************** 
void gui() {  
   if (isstatus) {
    if (millis()-quitime>1000) {
     if (isstatus) show_status(); 
     quitime=millis();
    }    
   }   
   M5.update(); 
   if (M5.BtnB.wasPressed()) 
    if (lastpressed==0) lastpressed=millis();    // 
   if (M5.BtnA.wasPressed()) 
    if (lastpressedleft==0) lastpressedleft=millis();    // 
   if (M5.BtnC.wasPressed()) 
    if (lastpressedright==0) lastpressedright=millis();    //    

   lastpressedany=max(max(max(lastpressed,lastpressedleft),lastpressedright),lastpressedany);

   if (!menu&&!isstatus&&rotate==cur_step%2) { // AI
    if (millis()>lastpressedany+3000) {
     delay(100);            
     solving=true;     
     Serial.println(F("\nAutostart solving!"));     
     return;    
    }
    float angle=3.1416-6.2832*(millis()-lastpressedany)/3000;
    M5.Lcd.drawLine(308,175,308+9*sin(angle),175+9*cos(angle),CGRAY);   
    delay(200);    
   }
 
   if (!menu&&!isstatus&&rotate!=cur_step%2) {  //  
     if (!movesload) {
      load_moves();
      movesload=1;
      animate_step(cur_step-1,true); 
     }            
     animategreen_step(current_var,false);
     delay(100);
   }   
   
    if (M5.BtnB.isReleased()&&lastpressed) 
      if (millis()-lastpressed>500) { //     
         if (!isstatus) {
          if (movesload) animategreen_step(current_var,true);
          movesload=0;
          lastpressed=0;     
          beep(50);
          if (menu==0) menu=1; else menu=0;         
          submenu=0;
          show_menu();
         } 
         return;   
      } else { 
       if (movesload&&cur_var>0) animategreen_step(current_var,true); 
       beep(5);
       lastpressed=0;       
       switch (menu) {
       case 0: //          
        if (solving) { // 
         beep(100); 
         solving=false;           
        } else {  //    AI         
         if (movesload) {          
          movesload=0;    
          float tim=float(millis()-starttime)/1000;
          tone_volume(600,100); 
          steps[cur_step]=steps[current_var];
          movestep(cur_step);       
          cur_step++; steps[cur_step].fig1=0; 
          initscreen();        
          animate_step(cur_step-1,false);
          clearstatus();          
         } else {          
          delay(100);
          solving=1;
          animate_step(cur_step,false);          
         }
        } 
        return;
       case 1: 
       if (submenu) { //new game
        beep(200);        
        cur_step=1;
        steps[1].fig1=0;
        for (int i=0;i<8;i++)
         for (int j=0;j<8;j++) pole[j][i]=(signed char)pgm_read_byte(&polestart[j][i]);                 
        lastanimatedgreen.x1==99; 
        kingpositions();        
        initboard();
        clearstatus();
        menu=0;
        initscreen();               
        show_menu();                               
       } else if (cur_step>1) { // 
        beep(100);                
        cur_step--;       
        animate_step(cur_step,true);
        lastscore=0;              
        backstep(cur_step);               
        steps[cur_step].fig1=0;
        lastscore=0;              
        show_board();       
        menu=0;
        show_steps();               
        clearstatus();       
        return;         
       } break;
       case 2:  
        if (submenu) { //load game
         short cur_step_=prefs.getShort("cur_step");
         short limit_=prefs.getShort("limit");
         if (cur_step>0&&cur_step<1000&&limit>=0&&limit<3) {
          initboard();
          cur_step=cur_step_; limit=limit_;
          sound=prefs.getBool("sound");
          rotate=prefs.getBool("rotate");
          prefs.getBytes("pole",pole,sizeof(pole)); 
          prefs.getBytes("steps",steps,sizeof(step_type)*cur_step); 
          beep(100);
         } else {
          beep(1000); sound=1; rotate=0; limit=0; cur_step=1;
         }          
         //File file = SD.open("/AM_Chess.sav"); 
         //if (file)  {
         // Serial.println("file open"); 
         // file.read((uint8_t*)pole,64);           
         // uint8_t c[sizeof(step_type)];           
         // file.read(c,sizeof(cur_step)); memcpy(&cur_step,c,sizeof(cur_step));                        
         // file.read(c,sizeof(limit)); memcpy(&limit,c,sizeof(limit));                        
         // file.read(c,sizeof(sound)); memcpy(&sound,c,sizeof(sound));                        
         // step_type t;
         // for (int i=1;i<cur_step;i++) {
         //  t=steps[i];            
         //  file.read(c,sizeof(step_type));
         //  memcpy(&t,c,sizeof(step_type));                       
         // }          
         // file.close();  
         // beep(100);
        //} else { beep(1000); Serial.println("SD card error!"); } 
         steps[0].fig1=0;         
         steps[cur_step].fig1=0;  
         lastanimatedgreen.x1==99;                 
         initboard();    
         kingpositions();
         clearstatus();
         menu=0;
         show_menu();                       
         initscreen();         
         return;       
       } else { // 
        beep(100); 
        rotate=!rotate;                
        initboard();            
        return;
       } break;  
       case 3: 
       if (submenu) { //save game
         prefs.putShort("cur_step",cur_step);
         prefs.putShort("limit",limit);
         prefs.putBool("sound",sound);
         prefs.putBool("rotate",rotate);
         prefs.putBytes("pole",pole,sizeof(pole)); 
         prefs.putBytes("steps",steps,sizeof(step_type)*cur_step); 
         beep(50); delay(50); beep(50); delay(50); beep(50);                                 
        //File file = SD.open("/AM_Chess.sav", FILE_WRITE);                
        //if (file) {          
        //  Serial.println("file open for write"); 
        //  file.write((const uint8_t*)pole,64);             
        //  uint8_t c[sizeof(step_type)];   
        //  memcpy(c,&cur_step,sizeof(cur_step)); file.write(c,sizeof(cur_step));                               
        //  memcpy(c,&limit,sizeof(limit)); file.write(c,sizeof(limit));                               
        //  memcpy(c,&sound,sizeof(sound)); file.write(c,sizeof(sound));                                          
        //  step_type t;
        //  for (int i=1;i<cur_step;i++) {
        //   t=steps[i];
        //   memcpy(c,&t,sizeof(step_type));
        //   file.write(c,sizeof(step_type));                       
        //  }          
        //  file.close();  
        //  beep(100);
        //} else { beep(1000); Serial.println("SD card error!"); } 
        menu=0; 
        show_menu();                       
        return;         
       } else { // Game 
        submenu=1; menu=1;
        show_menu();
       } break;  
       case 4: 
        if (submenu) { //   USB
          if (!load_usb()) return;
          limit=2;
          beep(200);                
          menu=0;      
          initboard();    
          kingpositions();        
          clearstatus();
          initscreen();                       
          return;          
        } else { //
         beep(100);  
         limit++; if (limit>2) limit=0;       
         show_menu();
         return;
        } 
       case 5: //
        if (sound==1) { beep(100); sound=0; } else { sound=1; beep(200); }       
        show_menu();
        return; 
      } //switch  
        
    }
    if (M5.BtnA.wasPressed()) {    
      lastpressedleft=0;     
      if (menu==0) {
        if (movesload&&cur_var>0) { 
          animategreen_step(current_var,true);
          current_var--;  
          if (current_var<cur_step+21) current_var=cur_step+21+cur_var-1;          
        }
      } else {        
        beep(100);
        menu--; 
        if (menu<1)         
         if (submenu) menu=4; else menu=5;        
        show_menu();
        delay(200);
      }       
    }
    if (M5.BtnC.wasPressed()) {    
      lastpressedright=0;     
      if (menu==0) {
        if (movesload&&cur_var>0) { 
          animategreen_step(current_var,true);
          current_var++;  
          if (current_var>=cur_step+21+cur_var) current_var=cur_step+21;          
        }
      } else {
        beep(100);
        menu++; if (menu>5) menu=1;
        if (submenu&&menu>4) menu=1;
        show_menu();
        delay(200);
      }       
    }
        
  
}
//********************************** 
void animategreen_step(int nstep, boolean hide) {     
 if (cur_var==0) return;
 int j=steps[nstep].x1;
 int j2=steps[nstep].x2;
 int dj=j2-j;
 int i=steps[nstep].y1;
 int i2=steps[nstep].y2;
 int di=i2-i; 
 if (di!=0) di=di/abs(di); 
 if (dj!=0) dj=dj/abs(dj);    
 if (lastanimatedgreen.x1==steps[nstep].x1&&lastanimatedgreen.y1==steps[nstep].y1&&
     lastanimatedgreen.x2==steps[nstep].x2&&lastanimatedgreen.y2==steps[nstep].y2&&
     lastanimatedgreen.fig1==steps[nstep].fig1&&!hide) {     
  int show=(millis()/500)%2;    
  movestep(nstep);    
  if (show) show_cont(i2,j2);   
  backstep(nstep);   
  if (!show) show_fig(i2,j2);   
  if (!rotate) M5.Lcd.drawRect(i2*28+8,j2*28+1,27,27,CGREEN);    
   else M5.Lcd.drawRect((7-i2)*28+8,(7-j2)*28+1,27,27,CGREEN);          
  return;
 }
 lastanimatedgreen=steps[nstep]; 
 if (hide) show_fig(i,j);
 while (j!=steps[nstep].x2||i!=steps[nstep].y2) {  
  show_fig(i,j); 
  if (!hide) 
   if (!rotate) M5.Lcd.drawRect(i*28+8,j*28+1,27,27,CGREEN);    
    else M5.Lcd.drawRect((7-i)*28+8,(7-j)*28+1,27,27,CGREEN);    
  int mj=0;  
  if (j!=steps[nstep].x2) { j+=dj; mj=1; }   
  if (abs(steps[nstep].fig1)==fn&&mj==1) continue;
  if (i!=steps[nstep].y2) i+=di;  
 }
 if (hide) { show_fig(i,j); lastanimatedgreen.x1=99; }
  
}
//****************************
void show_cont(int i,int j) {
uint16_t color,color_cont;  
   color=CBLACKF;   
   if ((i+j+2)%2==0) color=CWHITEF;           
   int jj=j, ii=i;
   if (rotate) { jj=7-j; ii=7-i; }   
   M5.Lcd.fillRect(ii*28+7,jj*28,29,29,color);    
   //M5.Lcd.drawRect(ii*28+7,jj*28,29,29,CGREEN);  
   color=CBLACK; 
   if (pole[j][i]>0) color=CWHITE;
   if (pole[j][i]!=0) {   
    drawBitmap(ii*28+10, jj*28+3,&fig_cont[abs(pole[j][i])-1][0], 24, 24,color);     
   }
}
//********************************** 
void animate_step(int nstep, boolean hide) {     
uint16_t color=getColor(220,220,200);   
 if (!hide&&nstep-1>0&&steps[nstep-1].fig1!=0) animate_step(nstep-1,true); 
 if (nstep<1||steps[nstep].fig1==0) return;
 int j=steps[nstep].x1;
 int dj=steps[nstep].x2-steps[nstep].x1;
 int i=steps[nstep].y1;
 int di=steps[nstep].y2-steps[nstep].y1; 
 if (di!=0) di=di/abs(di); 
 if (dj!=0) dj=dj/abs(dj);  
 if (hide) show_fig(i,j);
 while (j!=steps[nstep].x2||i!=steps[nstep].y2) {  
  show_fig(i,j); 
  if (!hide) 
   if (!rotate) M5.Lcd.drawRect(i*28+8,j*28+1,27,27,color);    
    else M5.Lcd.drawRect((7-i)*28+8,(7-j)*28+1,27,27,color);    
  int mj=0;  
  if (j!=steps[nstep].x2) { j+=dj; mj=1; }   
  if (abs(steps[nstep].fig1)==fn&&mj==1) continue;
  if (i!=steps[nstep].y2) i+=di;  
 } 
 show_fig(i,j); 
 if (!hide) 
  if (!rotate) M5.Lcd.drawRect(i*28+8,j*28+1,27,27,color);    
   else M5.Lcd.drawRect((7-i)*28+8,(7-j)*28+1,27,27,color);     
}
//********************************** 
void show_status() {               
   if (rotate!=cur_step%2) return;
   int tim=(millis()-starttime)/1000; 
   int cur=220000*tim/(limittime-starttime);
   if (cur>220) { cur=220; solving=false; }      
   M5.Lcd.drawFastHLine(10,237,cur,CGRAY);                  
   M5.Lcd.drawFastHLine(10,239,progress*2,CGRAY);    
   int m=tim/60;
   int s=tim%60;
   M5.Lcd.setTextWrap(1);
   M5.Lcd.setTextColor(CGRAY);
   M5.Lcd.setTextSize(1);
   M5.Lcd.fillRect(240,166,80,10,CBLACK);                
   M5.Lcd.setCursor(242,166); 
   if (m>0) { M5.Lcd.print(m); M5.Lcd.print(":"); }
   if (s>0) {
    if (s<10&&m>0) M5.Lcd.print("0"); 
    M5.Lcd.print(s);
   } else if (m>0) M5.Lcd.print("00"); else M5.Lcd.print("0");   
   M5.Lcd.setCursor(280,166);   
   M5.Lcd.print(cur_level);   
   M5.Lcd.setCursor(242,176); 
   if (cur_step%2==1) M5.Lcd.setTextColor(CWHITE); else M5.Lcd.setTextColor(CGRAY);   
   if (lastbest.fig1!=steps[0].fig1||lastbest.x1!=steps[0].x1||lastbest.y1!=steps[0].y1||
       lastbest.x2!=steps[0].x2||lastbest.y2!=steps[0].y2) {     
     lastbest=steps[0];         
     M5.Lcd.fillRect(240,176,80,10,CBLACK);                  
     M5.Lcd.setCursor(242,176);
     M5.Lcd.setTextColor(CGRAY);     
     M5.Lcd.print(str_step(0));    
     blinkstep=0;
   }  
   if (steps[0].fig1!=0&&blinkstep==0&&tim>5) {
    signed char poleb[8][8];     
    for (int i=0;i<8;i++)
     for (int j=0;j<8;j++) { poleb[j][i]=pole[j][i]; pole[j][i]=pole0[j][i]; }         
    movestep(0);    
    show_board(); 
    delay(100);  
    backstep(0);   
    show_board();      
    for (int i=0;i<8;i++)
     for (int j=0;j<8;j++) pole[j][i]=poleb[j][i];           
    kingpositions();  
   }
   blinkstep++;      
   if (blinkstep>2) blinkstep=0;              
}
//****************************
void show_fig(int i,int j) {
uint16_t color,color_cont;  
   color=CBLACKF;   
   if ((i+j+2)%2==0) color=CWHITEF;           
   int jj=j, ii=i;
   if (rotate) { jj=7-j; ii=7-i; }   
   M5.Lcd.fillRect(ii*28+7,jj*28,29,29,color);    
   M5.Lcd.drawRect(ii*28+7,jj*28,29,29,CGRAY);  
   color=CBLACKFIG; color_cont=CBLACKCONT;  //CGRAY;
   if (pole[j][i]>0) { color=CWHITEFIG; color_cont=CBLACK; }
   if (pole[j][i]!=0) {
    drawBitmap(ii*28+10, jj*28+3,&fig[abs(pole[j][i])-1][0], 24, 24,color);          
    drawBitmap(ii*28+10, jj*28+3,&fig_cont[abs(pole[j][i])-1][0], 24, 24,color_cont);     
   }
}
//****************************
void show_board() {   
 for (int i=0;i<8;i++)
  for (int j=0;j<8;j++) {
    if (poledisp[j][i]!=pole[j][i]) show_fig(i,j);      
    poledisp[j][i]=pole[j][i];
  }  
}
//********************************** 
boolean load_usb() {
char s='x',i=0,j=0;  boolean load=false; 
  Serial.println(F("Wait for FEN position"));
  for (int i=0;i<8;i++)
      for (int j=0;j<8;j++) {
        pole0[j][i]=pole[j][i];
        pole[j][i]=(char)pgm_read_byte(&polezero[j][i]);                 
      }       
  while (s!=' ') {
    s=Serial.read();       
    if (i>7) { i=0; j++; }
    if (!getpole(j,i)) break;
    switch (s) {      
    case '/': i=0; break;  
    case 'p': pole[j][i]=-fp; i++; break;
    case 'P': pole[j][i]=fp; i++; break;
    case 'n': pole[j][i]=-fn; i++; break;
    case 'N': pole[j][i]=fn; i++; break;
    case 'b': pole[j][i]=-fb; i++; break;
    case 'B': pole[j][i]=fb; i++; break;
    case 'r': pole[j][i]=-fr; i++; break;
    case 'R': pole[j][i]=fr; i++; break;
    case 'q': pole[j][i]=-fq; i++; break;
    case 'Q': pole[j][i]=fq; i++; break;
    case 'k': pole[j][i]=-fk; i++; break;
    case 'K': pole[j][i]=fk; i++; break;
    case '1': i++; break;
    case '2': i+=2; break;
    case '3': i+=3; break;
    case '4': i+=4; break;
    case '5': i+=5; break;
    case '6': i+=6; break;
    case '7': i+=7; break;
    case '8': i=0; j++; break;    
    case ' ': break;    
    }    
    delay(20);
    if (i+j>0&&Serial.available()==0) break;
  }  
  s=0;
  if (Serial.available()>0) s=Serial.read();
  while (Serial.available()>0) Serial.read();
  if (s=='w'||s==0) { cur_step=1; load=true; rotate=1; }
   else if (s=='b') { cur_step=2; load=true; rotate=0; }
     else load=false;  
  if (load) {
     steps[1].fig1=0; steps[2].fig1=0;
     Serial.println(F("Position loaded"));
  } else {  
    for (int i=0;i<8;i++)
      for (int j=0;j<8;j++) 
        pole[j][i]=pole0[j][i];     
  }   
  return load;  
}
//********************************** 
String str_step(int i) {
  String s="";
   if (steps[i].fig1==0) return s;
   if (steps[i].type==2) s="0-0";
    else if (steps[i].type==3) s="0-0-0";
   else  {
    if (abs(steps[i].fig1)>1) s=fig_symb[abs(steps[i].fig1)];   
    if (abs(steps[i].fig1<5)) {
     s=s+char(97+steps[i].y1);  
     s=s+String(8-steps[i].x1);    
     if (steps[i].fig2==0) s=s+"-";    
    }
    if (steps[i].fig2!=0) {
      s=s+"x";    
      if (abs(steps[i].fig2)>1) s=s+fig_symb[abs(steps[i].fig2)];
    }    
    s=s+char(97+steps[i].y2);  
    s=s+String(8-steps[i].x2);
   } 
   if (steps[i].type>3) s=s+fig_symb[steps[i].type-2];
   if (steps[i].check==1) s=s+"+"; else 
    if (steps[i].check==2) s=s+"#";
   return s;
}
//****************************
void show_steps() {
uint16_t color=getColor(200,200,180);  
  M5.Lcd.fillRect(234,0,90,160,getColor(8,8,8));  
  M5.Lcd.fillRect(234,0,90,24,CDARK);  
  M5.Lcd.setTextSize(1); 
  M5.Lcd.setTextColor(getColor(160,160,140),CDARK);    
  M5.Lcd.setCursor(241,2);    
  M5.Lcd.print(" ArduinoMega"); 
  M5.Lcd.setCursor(240,14);    
  M5.Lcd.print("Chess M5Stack");   
  int i=1; int y=27;  
  int cur=(cur_step+1)/2; //   
  int lim_step=cur_step;         
  if (cur>6) i=cur-5;
  while (i<=cur&&y<140) { //     
    M5.Lcd.setCursor(240,y);    
    M5.Lcd.setTextColor(CGRAY);    
    M5.Lcd.print(i); 
    M5.Lcd.print(". ");     
    if (i*2-1==cur_step-1) M5.Lcd.setTextColor(color);    
    if (steps[i*2-1].fig1!=0) M5.Lcd.print(str_step(i*2-1));
    if (steps[i*2].fig1==0||i*2>lim_step) break;            
    M5.Lcd.setTextColor(CBLACK);    
    M5.Lcd.setCursor(240,y+10);    
    M5.Lcd.print(i); 
    M5.Lcd.print("  "); 
    M5.Lcd.setTextColor(CGRAY);            
    if (i*2==cur_step-1) M5.Lcd.setTextColor(color);    
    M5.Lcd.print(str_step(i*2));    
    i++; y+=22; 
  }
  show_menu();
}
//****************************
void show_menu() {
uint16_t color=getColor(220,220,200);
  if (menu==0) submenu=0; else animategreen_step(current_var,true);
  
  M5.Lcd.fillRect(233,190,86,50,CBLACK);    
  M5.Lcd.setTextSize(1);   
  if (menu==1) M5.Lcd.setTextColor(CBLACK,CBLUE); else M5.Lcd.setTextColor(color,CGRAY2);   
  if (submenu) {
    M5.Lcd.setCursor(242,190); M5.Lcd.print(" New Game "); 
  } else {  
   M5.Lcd.setCursor(233,190); M5.Lcd.print(" Back "); 
  } 
  if (menu==2) M5.Lcd.setTextColor(CBLACK,CBLUE); else M5.Lcd.setTextColor(color,CGRAY2);   
  if (submenu) {
    M5.Lcd.setCursor(242,203); M5.Lcd.print(" Load Game "); 
  } else {    
   M5.Lcd.setCursor(271,190); M5.Lcd.print(" Rotate "); 
  } 
  if (menu==3) M5.Lcd.setTextColor(CBLACK,CBLUE); else M5.Lcd.setTextColor(color,CGRAY2);   
  if (submenu) {
    M5.Lcd.setCursor(242,216); M5.Lcd.print(" Save Game ");   
  } else {    
   M5.Lcd.setCursor(242,203); M5.Lcd.print(" Game menu ");   
  } 
  if (menu==4) M5.Lcd.setTextColor(CBLACK,CBLUE); else M5.Lcd.setTextColor(color,CGRAY2);   
  if (submenu) {
    M5.Lcd.setCursor(242,229); M5.Lcd.print(" USB FEN "); 
  } else {    
   M5.Lcd.setCursor(242,216); M5.Lcd.print(" Level "); 
   switch (limit) {
   case 0: M5.Lcd.print("LOW "); break;
   case 1: M5.Lcd.print("MED "); break;
   case 2: M5.Lcd.print("HIGH"); break;
   }   
  }   
  if (menu==5) M5.Lcd.setTextColor(CBLACK,CBLUE); else M5.Lcd.setTextColor(color,CGRAY2);   
  if (submenu) {
  } else {    
   M5.Lcd.setCursor(242,227); M5.Lcd.print(" Sound ");  
   if (sound) M5.Lcd.print("ON  "); else M5.Lcd.print("OFF ");
  } 
}

//****************************
boolean getdiagrowcheckw(signed char dj,signed char di) { //    
signed char d,j1,i1;           
   j1=WKJ; i1=WKI;
   for (d=1;d<8;d++) {    
    j1+=dj; i1+=di;
    if (getpole(j1,i1)) {      
      if (pole[j1][i1]==-fq||pole[j1][i1]==-fb) return true;      
      if (pole[j1][i1]!=0) break;      
    } else break;
   }
   return false;
}
//****************************
boolean getdiagrowcheckb(signed char dj,signed char di) { //    
signed char d,j1,i1;           
   j1=BKJ; i1=BKI;
   for (d=1;d<8;d++) {    
    j1+=dj; i1+=di;
    if (getpole(j1,i1)) {      
      if (pole[j1][i1]==fq||pole[j1][i1]==fb) return true;      
      if (pole[j1][i1]!=0) break;      
    } else break;
   }
   return false;
}
//****************************
boolean getstreightrowcheckw(signed char dj,signed char di) { //   - 
signed char d,j1,i1;  
   j1=WKJ; i1=WKI;
   for (d=1;d<8;d++) {    
    j1+=dj; i1+=di;    
    if (getpole(j1,i1)) {      
      if (pole[j1][i1]==-fq||pole[j1][i1]==-fr) return true;      
      if (pole[j1][i1]!=0) break;      
    } else break;
   }
   return false;
}
//****************************
boolean getstreightrowcheckb(signed char dj,signed char di) { //   - 
signed char d,j1,i1;  
   j1=BKJ; i1=BKI;
   for (d=1;d<8;d++) {    
    j1+=dj; i1+=di;    
    if (getpole(j1,i1)) {      
      if (pole[j1][i1]==fq||pole[j1][i1]==fr) return true;      
      if (pole[j1][i1]!=0) break;      
    } else break;
   }
   return false;
}
//****************************
boolean get_check(signed char king) { //      
  if (king==fk) { //             
    if (getdiagrowcheckw(-1, 1)) return true;    
    if (getdiagrowcheckw(-1,-1)) return true;              
    if (getdiagrowcheckw( 1,-1)) return true;
    if (getdiagrowcheckw( 1, 1)) return true;    
    if (getstreightrowcheckw(-1, 0)) return true;                  
    if (getstreightrowcheckw( 0, 1)) return true;    
    if (getstreightrowcheckw( 0,-1)) return true;    
    if (getstreightrowcheckw( 1, 0)) return true;            
    if (getpole(WKJ-2,WKI-1)&&pole[WKJ-2][WKI-1]==-fn) return true;           
    if (getpole(WKJ-2,WKI+1)&&pole[WKJ-2][WKI+1]==-fn) return true;            
    if (getpole(WKJ-1,WKI-2)&&pole[WKJ-1][WKI-2]==-fn) return true;        
    if (getpole(WKJ-1,WKI+2)&&pole[WKJ-1][WKI+2]==-fn) return true;        
    if (getpole(WKJ+1,WKI-2)&&pole[WKJ+1][WKI-2]==-fn) return true;        
    if (getpole(WKJ+1,WKI+2)&&pole[WKJ+1][WKI+2]==-fn) return true;        
    if (getpole(WKJ+2,WKI-1)&&pole[WKJ+2][WKI-1]==-fn) return true;        
    if (getpole(WKJ+2,WKI+1)&&pole[WKJ+2][WKI+1]==-fn) return true;              
    if (getpole(WKJ-1,WKI-1)&&pole[WKJ-1][WKI-1]==-fp) return true;    
    if (getpole(WKJ-1,WKI+1)&&pole[WKJ-1][WKI+1]==-fp) return true;                
  } else { //                 
    if (getdiagrowcheckb( 1,-1)) return true;
    if (getdiagrowcheckb( 1, 1)) return true;
    if (getdiagrowcheckb(-1, 1)) return true;
    if (getdiagrowcheckb(-1,-1)) return true;      
    if (getstreightrowcheckb( 1, 0)) return true;    
    if (getstreightrowcheckb( 0, 1)) return true;
    if (getstreightrowcheckb( 0,-1)) return true;
    if (getstreightrowcheckb(-1, 0)) return true;              
    if (getpole(BKJ+2,BKI-1)&&pole[BKJ+2][BKI-1]==fn) return true;    
    if (getpole(BKJ+2,BKI+1)&&pole[BKJ+2][BKI+1]==fn) return true;            
    if (getpole(BKJ+1,BKI-2)&&pole[BKJ+1][BKI-2]==fn) return true;    
    if (getpole(BKJ+1,BKI+2)&&pole[BKJ+1][BKI+2]==fn) return true;                
    if (getpole(BKJ-1,BKI-2)&&pole[BKJ-1][BKI-2]==fn) return true;    
    if (getpole(BKJ-1,BKI+2)&&pole[BKJ-1][BKI+2]==fn) return true;    
    if (getpole(BKJ-2,BKI-1)&&pole[BKJ-2][BKI-1]==fn) return true;   
    if (getpole(BKJ-2,BKI+1)&&pole[BKJ-2][BKI+1]==fn) return true;           
    if (getpole(BKJ+1,BKI-1)&&pole[BKJ+1][BKI-1]==fp) return true;
    if (getpole(BKJ+1,BKI+1)&&pole[BKJ+1][BKI+1]==fp) return true;      
  }    
  if (abs(BKJ-WKJ)<=1&&abs(BKI-WKI)<=1) return true; //    
  return false;
}
//****************************
boolean getpole(signed char j,signed char i) { //         
  if (j>=0&&j<8&&i>=0&&i<8) return true; 
  return false; 
}
//****************************
void addstep(signed char j1,signed char i1,signed char j2,signed char i2,signed char type) {       
 int st=start_var+cur_var; 
 steps[st].x1=j1;
 steps[st].x2=j2;
 steps[st].y1=i1;
 steps[st].y2=i2;
 steps[st].fig1=pole[j1][i1];
 steps[st].fig2=pole[j2][i2];        
 if (type==1) { //  
  steps[st].fig2=-steps[st].fig1;        
 }
 steps[st].type=type;   
 signed char ko=-fk; //   
 if (steps[st].fig1>0) ko=fk;   
 movestep(st);  
 if (get_check(ko)) { backstep(st); return; } //     -    
 boolean che=get_check(-ko); //      
 backstep(st);
 steps[st].weight=abs(steps[st].fig2)-abs(steps[st].fig1);
 if (type>3) steps[st].weight+=fig_weight[type-2]; 
 if (endspiel&&steps[st].fig1==ko) steps[st].weight+=10; //   -   
 steps[st].check=che;
 if (che) steps[st].weight+=10;
 if (only_action) {
  if (steps[st].fig1==fp&&steps[st].x2==1||steps[st].fig1==-fp&&steps[st].x2==6) // 
   { cur_var++; return; }
  if (steps[st].fig2==0&&steps[st].type<4&&!che&&!check_on_table) return;    
 }
 
 cur_var++; 
}
//****************************
void getrowstepsw(signed char j,signed char i,signed char dj,signed char di) { //        
signed char d,j1,i1;     
   j1=j; i1=i; 
   for (d=1;d<8;d++) {    
    j1+=dj; i1+=di;
    if (getpole(j1,i1)) {      
      if (pole[j1][i1]<=0) addstep(j,i,j1,i1,0);
      if (pole[j1][i1]!=0) break;
    } else break;
   }
}
//****************************
void getrowstepsb(signed char j,signed char i,signed char dj,signed char di) { //    
signed char d,j1,i1;     
   j1=j; i1=i; 
   for (d=1;d<8;d++) {    
    j1+=dj; i1+=di;
    if (getpole(j1,i1)) {      
      if (pole[j1][i1]>=0) addstep(j,i,j1,i1,0);
      if (pole[j1][i1]!=0) break;
    } else break;
   }
}
//****************************
void getonestepw(signed char j,signed char i,signed char dj,signed char di) { //       
signed char j1,i1;  
  j1=j+dj; i1=i+di;
  if (getpole(j1,i1)) 
    if (pole[j1][i1]<=0) addstep(j,i,j1,i1,0);   
}
//****************************
void getonestepb(signed char j,signed char i,signed char dj,signed char di) { //   
signed char j1,i1;  
  j1=j+dj; i1=i+di;
  if (getpole(j1,i1)) 
    if (pole[j1][i1]>=0) addstep(j,i,j1,i1,0);   
}
//****************************
void sort_variants(int from, int to) { //     
   while (1) { 
    int mov=0;     
    for (int i=from;i<to;i++)  //      
     if (steps[i].weight<steps[i+1].weight) {    
       mov++; 
       step_type buf=steps[i];
       steps[i]=steps[i+1];
       steps[i+1]=buf;      
     }                   
    if (mov==0) break;
   }    
}
//****************************  
void load_variants(int nstep) { //       nstep
  cur_var=0;       
  if (pole[WKJ][WKI]!=fk||pole[BKJ][BKI]!=-fk) kingpositions(); 
  if (nstep%2==1) check_on_table=get_check(fk);   
   else check_on_table=get_check(-fk); //      
  for (signed char i=0;i<8;i++)
   for (signed char j=0;j<8;j++) 
    if (pole[j][i]>0&&nstep%2==1||pole[j][i]<0&&nstep%2==0) { 
     switch (pole[j][i]) { 
     case fp: //          
      if (getpole(j-1,i)&&pole[j-1][i]==0) 
        if (j!=1) addstep(j,i,j-1,i,0); else 
          for (signed char t=4;t<8;t++) addstep(1,i,0,i,t); //-...                        
      if (j==6&&pole[j-1][i]==0&&pole[j-2][i]==0) addstep(j,i,j-2,i,0);
      if (getpole(j-1,i-1)&&pole[j-1][i-1]<0) 
       if (j!=1) addstep(j,i,j-1,i-1,0); else 
          for (signed char t=4;t<8;t++) addstep(j,i,j-1,i-1,t); //-...                                      
      if (getpole(j-1,i+1)&&pole[j-1][i+1]<0) 
       if (j!=1) addstep(j,i,j-1,i+1,0); else 
         for (signed char t=4;t<8;t++) addstep(j,i,j-1,i+1,t); //-...                                      
      if (j==3&&steps[nstep-1].fig1==-fp&&steps[nstep-1].x2==3&&steps[nstep-1].x1==1) {          
          if (steps[nstep-1].y2-i==1) { //                
            addstep(j,i,j-1,i+1,1);
          } else if (steps[nstep-1].y2-i==-1) { //                 
            addstep(j,i,j-1,i-1,1);
          }
      }  
     break;
     case -fp: //                  
      if (getpole(j+1,i)&&pole[j+1][i]==0) 
        if (j!=6) addstep(j,i,j+1,i,0); else 
          for (signed char t=4;t<8;t++) addstep(j,i,j+1,i,t); //-...                                
      if (j==1&&pole[j+1][i]==0&&pole[j+2][i]==0) addstep(j,i,j+2,i,0);
      if (getpole(j+1,i-1)&&pole[j+1][i-1]>0) 
       if (j!=6) addstep(j,i,j+1,i-1,0); else
         for (signed char t=4;t<8;t++) addstep(j,i,j+1,i-1,t); //-...                                       
      if (getpole(j+1,i+1)&&pole[j+1][i+1]>0) 
       if (j!=6) addstep(j,i,j+1,i+1,0); else
        for (signed char t=4;t<8;t++) addstep(j,i,j+1,i+1,t); //-...                                       
      if (j==4&&steps[nstep-1].fig1==fp&&steps[nstep-1].x2==4&&steps[nstep-1].x1==6) {                                      
          if (steps[nstep-1].y2-i==1) { //                            
            addstep(j,i,j+1,i+1,1);
          } else if (steps[nstep-1].y2-i==-1) { //                 
            addstep(j,i,j+1,i-1,1);
          }
      }          
     break;   
     case fn: //   
      getonestepw(j,i,-2,-1);
      getonestepw(j,i,-2,1);
      getonestepw(j,i,-1,-2);
      getonestepw(j,i,-1,2);
      getonestepw(j,i,2,-1);
      getonestepw(j,i,2,1);
      getonestepw(j,i,1,-2);
      getonestepw(j,i,1,2);
      break;   
     case -fn: //          
      getonestepb(j,i,-2,-1);
      getonestepb(j,i,-2,1);
      getonestepb(j,i,-1,-2);
      getonestepb(j,i,-1,2);
      getonestepb(j,i,2,-1);
      getonestepb(j,i,2,1);
      getonestepb(j,i,1,-2);
      getonestepb(j,i,1,2);
      break;
     case fb: //   
      getrowstepsw(j,i,1,1);
      getrowstepsw(j,i,-1,-1);
      getrowstepsw(j,i,1,-1);
      getrowstepsw(j,i,-1,1);   
      break; 
     case -fb: //      
      getrowstepsb(j,i,1,1);
      getrowstepsb(j,i,-1,-1);
      getrowstepsb(j,i,1,-1);
      getrowstepsb(j,i,-1,1);   
      break;
     case fr: //      
      getrowstepsw(j,i,1,0);
      getrowstepsw(j,i,-1,0);
      getrowstepsw(j,i,0,1);
      getrowstepsw(j,i,0,-1);
      break;
     case -fr: //               
      getrowstepsb(j,i,1,0);
      getrowstepsb(j,i,-1,0);
      getrowstepsb(j,i,0,1);
      getrowstepsb(j,i,0,-1);
      break;
     case fq: // 
      getrowstepsw(j,i,1,1);
      getrowstepsw(j,i,-1,-1);
      getrowstepsw(j,i,1,-1);
      getrowstepsw(j,i,-1,1);   
      getrowstepsw(j,i,1,0);
      getrowstepsw(j,i,-1,0);
      getrowstepsw(j,i,0,1);
      getrowstepsw(j,i,0,-1);   
      break;
     case -fq: //    
      getrowstepsb(j,i,1,1);
      getrowstepsb(j,i,-1,-1);
      getrowstepsb(j,i,1,-1);
      getrowstepsb(j,i,-1,1);   
      getrowstepsb(j,i,1,0);
      getrowstepsb(j,i,-1,0);
      getrowstepsb(j,i,0,1);
      getrowstepsb(j,i,0,-1);   
      break;
     case fk: // 
      getonestepw(j,i, 1,-1); 
      getonestepw(j,i, 1, 0); 
      getonestepw(j,i, 1, 1);       
      getonestepw(j,i, 0,-1);                        
      getonestepw(j,i, 0, 1);  
      getonestepw(j,i,-1,-1); 
      getonestepw(j,i,-1, 0); 
      getonestepw(j,i,-1, 1);            
      if (!check_on_table) add_rok(j,i,nstep);
      break;       
     case -fk: //    
      getonestepb(j,i, 1,-1); 
      getonestepb(j,i, 1, 0); 
      getonestepb(j,i, 1, 1);       
      getonestepb(j,i, 0,-1);                        
      getonestepb(j,i, 0, 1);  
      getonestepb(j,i,-1,-1); 
      getonestepb(j,i,-1, 0); 
      getonestepb(j,i,-1, 1);            
      if (!check_on_table) add_rok(j,i,nstep);
      break;       
    } //switch
   } //if 

   if (nstep>cur_step) {
    for (int i=start_var;i<start_var+cur_var;i++) { //                
     for (signed char j=0;j<MAXCUTS;j++)    
      if (cuts[j].fig1==steps[i].fig1&&
       cuts[j].x1==steps[i].x1&&cuts[j].y1==steps[i].y1&&
       cuts[j].x2==steps[i].x2&&cuts[j].y2==steps[i].y2) {
        steps[i].weight+=100+cuts[j].weight;              
        break;
      }      
     if (nstep>1&&steps[nstep-1].fig2!=0)  //     -      
       if (steps[nstep-1].x2==steps[i].x2&&steps[nstep-1].y2==steps[i].y2) 
         steps[i].weight+=1000;              
    }    
   }   

   sort_variants(start_var,start_var+cur_var-1);   
        
 }
//****************************
void movestep(int nstep) {     
  pole[steps[nstep].x1][steps[nstep].y1]=0;
  pole[steps[nstep].x2][steps[nstep].y2]=steps[nstep].fig1;   
  if (steps[nstep].fig1==fk) {
   WKJ=steps[nstep].x2; WKI=steps[nstep].y2; 
  } else if (steps[nstep].fig1==-fk) {
   BKJ=steps[nstep].x2; BKI=steps[nstep].y2; 
  }
  if (steps[nstep].type==0) return; 
  if (steps[nstep].type==1)  //  
   if (steps[nstep].fig1>0)
    pole[steps[nstep].x2+1][steps[nstep].y2]=0;
   else 
    pole[steps[nstep].x2-1][steps[nstep].y2]=0; 
  else if (steps[nstep].type==2)  // 
   if (steps[nstep].fig1>0) { //     
    pole[7][4]=0; pole[7][5]=fr; pole[7][6]=fk; pole[7][7]=0; //show_board(); delay(3000);
   } else { //        
    pole[0][4]=0; pole[0][5]=-fr; pole[0][6]=-fk; pole[0][7]=0; //show_board(); delay(3000);
    }
  else if (steps[nstep].type==3)  // 
   if (steps[nstep].fig1>0) { //     
    pole[7][0]=0; pole[7][1]=0; pole[7][2]=fk; pole[7][3]=fr; pole[7][4]=0;
   } else { //
    pole[0][0]=0; pole[0][1]=0; pole[0][2]=-fk; pole[0][3]=-fr; pole[0][4]=0;    
   }  
   else if (steps[nstep].type>3)  //-....
   if (steps[nstep].fig1>0) pole[steps[nstep].x2][steps[nstep].y2]=steps[nstep].type-2;
    else pole[steps[nstep].x2][steps[nstep].y2]=2-steps[nstep].type;
}
//****************************  
void backstep(int nstep) {
  pole[steps[nstep].x1][steps[nstep].y1]=steps[nstep].fig1; 
  pole[steps[nstep].x2][steps[nstep].y2]=steps[nstep].fig2; 
  if (steps[nstep].fig1==fk) {
    WKJ=steps[nstep].x1; WKI=steps[nstep].y1; 
  } else if (steps[nstep].fig1==-fk) {
    BKJ=steps[nstep].x1; BKI=steps[nstep].y1; 
  }
  if (steps[nstep].type==0) return;
  if (steps[nstep].type==1) { //  
    pole[steps[nstep].x2][steps[nstep].y2]=0;
    if (steps[nstep].fig1>0)
     pole[steps[nstep].x2+1][steps[nstep].y2]=-fp; 
    else 
     pole[steps[nstep].x2-1][steps[nstep].y2]=fp; 
  } else if (steps[nstep].type==2)  // 
   if (steps[nstep].fig1>0) { // 
    pole[7][4]=fk; pole[7][5]=0; pole[7][6]=0; pole[7][7]=fr; 
   } else { //
    pole[0][4]=-fk; pole[0][5]=0; pole[0][6]=0; pole[0][7]=-fr; // show_board(); delay(3000);
   }
  else if (steps[nstep].type==3)  // 
   if (steps[nstep].fig1>0) { // 
    pole[7][0]=fr; pole[7][1]=0; pole[7][2]=0; pole[7][3]=0; pole[7][4]=fk;    
   } else { //
    pole[0][0]=-fr; pole[0][1]=0; pole[0][2]=0; pole[0][3]=0; pole[0][4]=-fk;
   }
     
}
//****************************
void get_wrocks(int nstep) {
  w00=true; w000=true; 
  for (int i=1;i<nstep;i++) {
   if (steps[i].fig1==fk) { w00=false; w000=false; return; }  
   if (steps[i].fig1==fr) 
    if (steps[i].x1==7&&steps[i].y1==7) w00=false;
     else if (steps[i].x1==7&&steps[i].y1==0) w000=false;
  } 
}
//****************************
void get_brocks(int nstep) {
  b00=true; b000=true; 
  for (int i=1;i<nstep;i++) {
   if (steps[i].fig1==-fk) { b00=false; b000=false; return; }  
   if (steps[i].fig1==-fr) 
    if (steps[i].x1==0&&steps[i].y1==7) b00=false;
     else if (steps[i].x1==0&&steps[i].y1==0) b000=false;
  } 
}
//****************************
void add_rok(signed char j,signed char i,int nstep) { //   
boolean che1,che2;   
 if (nstep%2==1) { //   
  if (j!=7||i!=4) return;  
  if (pole[7][5]==0&&pole[7][6]==0&&pole[7][7]==fr) { //  
    pole[7][4]=0; 
    WKI=5; che1=get_check(fk); 
    WKI=6; che2=get_check(fk); 
    WKI=4; pole[7][4]=fk; 
    get_wrocks(nstep);
    if (!che1&&!che2&&w00) addstep(7,4,7,6,2);
  }
  if (pole[7][0]==fr&&pole[7][1]==0&&pole[7][2]==0&&pole[7][3]==0) { //
    pole[7][4]=0; 
    WKI=2; che1=get_check(fk); 
    WKI=3; che2=get_check(fk); 
    WKI=4; pole[7][4]=fk; 
    get_wrocks(nstep);
    if (!che1&&!che2&&w000) addstep(7,4,7,2,3);
  }
 } else { //
  if (j!=0||i!=4) return;  
  if (pole[0][5]==0&&pole[0][6]==0&&pole[0][7]==-fr) { //    
    pole[0][4]=0;
    BKI=5; che1=get_check(-fk); 
    BKI=6; che2=get_check(-fk); 
    BKI=4; pole[0][4]=-fk; 
    get_brocks(nstep);
    if (!che1&&!che2&&b00) addstep(0,4,0,6,2);
  }
  if (pole[0][0]==-fr&&pole[0][1]==0&&pole[0][2]==0&&pole[0][3]==0) { //
    pole[0][4]=0;
    BKI=2; che1=get_check(-fk); 
    BKI=3; che2=get_check(-fk); 
    BKI=4; pole[0][4]=-fk; 
    get_brocks(nstep);
    if (!che1&&!che2&&b000) addstep(0,4,0,2,3);
  }
   
 }
}
//****************************
int addrowstepsw(signed char j,signed char i,signed char dj,signed char di,signed char dc) { //       
signed char d,j1,i1; int c=0;        
   j1=j; i1=i; 
   for (d=1;d<8;d++) {    
    j1+=dj; i1+=di;
    if (getpole(j1,i1)) {      
      if (pole[j1][i1]==0) c+=dc; 
      else if (pole[j1][i1]>0) { //           
        c+=1; break;
      } else {        
        if (cur_step>6) c+=-pole[j1][i1]; //               
        break;
      }
    } else break;
   }
   return c;
}
//****************************
int addrowstepsb(signed char j,signed char i,signed char dj,signed char di,signed char dc) { //   
signed char d,j1,i1; int c=0;    
   j1=j; i1=i; 
   for (d=1;d<8;d++) {    
    j1+=dj; i1+=di;
    if (getpole(j1,i1)) {      
      if (pole[j1][i1]==0) c-=dc; 
      else if (pole[j1][i1]<0) { //           
        c-=1; break;
      } else {        
        if (cur_step>6) c-=pole[j1][i1]; //               
        break;
      }
    } else break;
   }
   return c;
}
//****************************
int addonestepw(signed char j,signed char i,signed char dj,signed char di) { //       
signed char j1,i1;  
  j1=j+dj; i1=i+di;
  if (getpole(j1,i1)) 
    if (pole[j1][i1]==0) return 2;   
     else if (pole[j1][i1]>0) return 1;
      else return -pole[j1][i1];   
  return 0;    
}
//****************************
int addonestepb(signed char j,signed char i,signed char dj,signed char di) { //   
signed char j1,i1; 
  j1=j+dj; i1=i+di;
  if (getpole(j1,i1)) 
    if (pole[j1][i1]==0) return -2;   
     else if (pole[j1][i1]<0) return -1;
      else return -pole[j1][i1];   
  return 0;  
}
//****************************
int activity() { //      - +  
int c=0;
signed char pwj[8],pwi[8],pbj[8],pbi[8],ipw=0,ipb=0,nbw=0,nbb=0;   
  for (signed char i=0;i<8;i++)
   for (signed char j=0;j<8;j++) 
    if (pole[j][i]!=0) { 
     switch (pole[j][i]) { 
     case fp: //      
      if (getpole(j,i+1)&&pole[j][i+1]==fp) c+=2; //      
      if (getpole(j-1,i)&&pole[j-1][i]==fp) c-=20; // 
      pwj[ipw]=j; pwi[ipw]=i; ipw++;
     break;
     case -fp: //                  
      if (getpole(j,i-1)&&pole[j][i-1]==-fp) c-=2; //      
      if (getpole(j+1,i)&&pole[j+1][i]==-fp) c+=20; // 
      pbj[ipb]=j; pbi[ipb]=i; ipb++;
     break;   
     case fn: //   
      c+=addonestepw(j,i,-2,-1);
      c+=addonestepw(j,i,-2,1);
      c+=addonestepw(j,i,-1,-2);
      c+=addonestepw(j,i,-1,2);
      c+=addonestepw(j,i,2,-1);
      c+=addonestepw(j,i,2,1);
      c+=addonestepw(j,i,1,-2);
      c+=addonestepw(j,i,1,2);
      break;   
     case -fn: //          
      c+=addonestepb(j,i,-2,-1);
      c+=addonestepb(j,i,-2,1);
      c+=addonestepb(j,i,-1,-2);
      c+=addonestepb(j,i,-1,2);
      c+=addonestepb(j,i,2,-1);
      c+=addonestepb(j,i,2,1);
      c+=addonestepb(j,i,1,-2);
      c+=addonestepb(j,i,1,2);
      break;
     case fb: //   
      c+=addrowstepsw(j,i,1,1,2);  //!! 1.4
      c+=addrowstepsw(j,i,-1,-1,3);//!!
      c+=addrowstepsw(j,i,1,-1,2); //!!
      c+=addrowstepsw(j,i,-1,1,3); //!!  
      nbw++;
      break; 
     case -fb: //      
      c+=addrowstepsb(j,i,1,1,3);  //!! 1.4
      c+=addrowstepsb(j,i,-1,-1,2);//!!
      c+=addrowstepsb(j,i,1,-1,3); //!!
      c+=addrowstepsb(j,i,-1,1,2); //!!  
      nbb++;
      break;
     case fr: //      
      c+=addrowstepsw(j,i,1,0,2); // 
      c+=addrowstepsw(j,i,-1,0,2);
      c+=addrowstepsw(j,i,0,1,1);
      c+=addrowstepsw(j,i,0,-1,1);
      break;
     case -fr: //               
      c+=addrowstepsb(j,i,1,0,2); //
      c+=addrowstepsb(j,i,-1,0,2);
      c+=addrowstepsb(j,i,0,1,1);
      c+=addrowstepsb(j,i,0,-1,1);
      break;
     case fq: // 
      if (cur_step>10) {
       c+=addrowstepsw(j,i,1,1,1);
       c+=addrowstepsw(j,i,-1,-1,2);
       c+=addrowstepsw(j,i,1,-1,1);
       c+=addrowstepsw(j,i,-1,1,2);   
       c+=addrowstepsw(j,i,1,0,1);
       c+=addrowstepsw(j,i,-1,0,2);
       c+=addrowstepsw(j,i,0,1,1);
       c+=addrowstepsw(j,i,0,-1,1);   
      } 
      break;
     case -fq: //    
      if (cur_step>10) {
       c+=addrowstepsb(j,i,1,1,2);
       c+=addrowstepsb(j,i,-1,-1,1);
       c+=addrowstepsb(j,i,1,-1,2);
       c+=addrowstepsb(j,i,-1,1,1);   
       c+=addrowstepsb(j,i,1,0,2);
       c+=addrowstepsb(j,i,-1,0,1);
       c+=addrowstepsb(j,i,0,1,1);
       c+=addrowstepsb(j,i,0,-1,1);   
      } 
      break;    
    } //switch
    if (cur_step>6) {
     if (pole[j][i]>0) { //   
      if (abs(WKJ-j)<2&&abs(WKI-i)<2) c+=3;  
      if (abs(BKJ-j)<2&&abs(BKI-i)<2) c+=3;  
      if (abs(WKJ-j)<3&&abs(WKI-i)<3) c+=2;  
      if (abs(BKJ-j)<3&&abs(BKI-i)<3) c+=2;  
     } else { //   
      if (abs(WKJ-j)<2&&abs(WKI-i)<2) c-=3;  
      if (abs(BKJ-j)<2&&abs(BKI-i)<2) c-=3;  
      if (abs(WKJ-j)<3&&abs(WKI-i)<3) c-=2;  
      if (abs(BKJ-j)<3&&abs(BKI-i)<3) c-=2;  
     }    
    } 
   }    
   if (nbw>1) c+=30; // 
   if (nbb>1) c-=30;
   for (signed char w=0;w<ipw;w++) { //   
    boolean pass=1;
    for (signed char b=0;b<ipb;b++) {
      if (pwi[w]>0&&pbi[b]==pwi[w]-1&&pbj[b]<pwj[w]) pass=0; // 
      if (pbi[b]==pwi[w]&&pbj[b]<pwj[w]) pass=0;  // 
      if ( pwi[w]<7&&pbi[b]==pwi[w]+1&&pbj[b]<pwj[w])  pass=0; //       
      if (!pass) break;
    }    
    if (pass) {      
      c+=50; 
      if (!endspiel) break; //     
    }
   }
   for (signed char b=0;b<ipb;b++) { //   
    boolean pass=1;
    for (signed char w=0;w<ipw;w++) {
      if (pbi[b]>0&&pwi[w]==pbi[b]-1&&pwj[w]>pbj[b]) pass=0; // 
      if (pwi[w]==pbi[b]&&pwj[w]>pbj[b]) pass=0; // 
      if (pbi[b]<7&&pwi[w]==pbi[b]+1&&pwj[w]>pbj[b]) pass=0; //       
      if (!pass) break;
    }
    if (pass) {      
      c-=50; 
      if (!endspiel) break; //     
    }
   }      
   return c;      
}
//****************************
int evaluate(int nstep) { //   
  long ww=0, wb=0;      
  for (signed char i=0;i<8;i++)
   for (signed char j=0;j<8;j++)     
    if (pole[j][i]<0) {
     wb+=fig_weight[-pole[j][i]]+(short)pgm_read_word(&pos[-pole[j][i]-1][7-j][i]);                               
    }
    else if (pole[j][i]>0) {
     ww+=fig_weight[pole[j][i]]+(short)pgm_read_word(&pos[pole[j][i]-1][j][i]);                              
    }   
  count++; //                  
  long str=activity();   
  if (endspiel) { // 
   wb+=(short)pgm_read_word(&pos[6][7-BKJ][BKI])-(short)pgm_read_word(&pos[5][7-BKJ][BKI]);                               
   ww+=(short)pgm_read_word(&pos[6][WKJ][WKI])-(short)pgm_read_word(&pos[5][WKJ][WKI]);                                    
   if (wb<450&&ww>450) { //  -   -         
    str-=(abs(WKJ-BKJ)+abs(WKI-BKI))*30;
   } else if (ww<450&&wb>450) { // 
    str+=(abs(WKJ-BKJ)+abs(WKI-BKI))*30;
   }
  } 
  if (nstep>8) { //   
    if (steps[nstep-1].fig1==steps[nstep-5].fig1&&steps[nstep-2].fig1==steps[nstep-6].fig1&&
     steps[nstep-3].fig1==steps[nstep-7].fig1&&steps[nstep-4].fig1==steps[nstep-8].fig1)
      if (steps[nstep-1].x1==steps[nstep-5].x1&&steps[nstep-2].x1==steps[nstep-6].x1&&
       steps[nstep-3].x1==steps[nstep-7].x1&&steps[nstep-4].x1==steps[nstep-8].x1)
        if (steps[nstep-1].x2==steps[nstep-5].x2&&steps[nstep-2].x2==steps[nstep-6].x2&&
         steps[nstep-3].x2==steps[nstep-7].x2&&steps[nstep-4].x2==steps[nstep-8].x2)
          if (steps[nstep-1].y1==steps[nstep-5].y1&&steps[nstep-2].y1==steps[nstep-6].y1&&
           steps[nstep-3].y1==steps[nstep-7].y1&&steps[nstep-4].y1==steps[nstep-8].x1)
            if (steps[nstep-1].y2==steps[nstep-5].y2&&steps[nstep-2].y2==steps[nstep-6].y2&&
             steps[nstep-3].y2==steps[nstep-7].y2&&steps[nstep-4].y2==steps[nstep-8].y2)
               { 
                //Serial.println(F(" Draw - 3 repeat"));
                return 0;   
               }   
  }
  if (nstep%2==1) return 5000*(ww-wb)/(ww+wb+2000)+str; else return 5000*(wb-ww)/(ww+wb+2000)-str;    
}
//****************************  
void add_cut(int ind) {// ind       
  int minbeta=30000, minindex=MAXCUTS-1;   
  for (signed char i=0;i<MAXCUTS;i++) {   
   if (cuts[i].weight==0) { minindex=i; break; }//  
   if (cuts[i].fig1==steps[ind].fig1&&
       cuts[i].x1==steps[ind].x1&&cuts[i].y1==steps[ind].y1&&
       cuts[i].x2==steps[ind].x2&&cuts[i].y2==steps[ind].y2) {
       cuts[i].weight++; return; // -    1
   }     
   if (cuts[i].weight<minbeta) { //   
    minbeta=cuts[i].weight; 
    minindex=i;
   }      
  }
  cuts[minindex]=steps[ind];
  cuts[minindex].weight=1; //1 
}
//****************************  
int quiescence(int start, int nstep, int alpha, int beta ) {    
    if (nstep-cur_step>=LIMDEPTH||start>MAXSTEPS-100) return evaluate(nstep);                
    if (!solving&&progress==0) return -5000;                
    int score=-20000;           
    start_var=start; 
    only_action=true;   
    load_variants(nstep);    
    if (!check_on_table) {
     int stand_pat = evaluate(nstep);
     if (stand_pat >= score) score=stand_pat; 
     if (score>alpha) alpha=score;
     if (alpha>=beta&&isbeta) return alpha;            
    }                     
    if (cur_var==0) { 
      if (check_on_table) {      
       if (TRACE) Serial.println(F("checkmate?"));                        
       return -10000+nstep-cur_step;                     
     } else return evaluate(nstep);
    }
    int j=start+cur_var;      
    for (int i=start;i<j;i++) { //            
      if (TRACE) { //*****
       for (int u=0;u<nstep-cur_step;u++) Serial.print(F("   .   "));      
       Serial.println(str_step(i)); 
      }      
      movestep(i);            
      steps[nstep]=steps[i];              
      int tmp=-quiescence(j+1,nstep+1,-beta,-alpha);            
      backstep(i);              
      if (tmp>score) score=tmp;
      if (score>alpha) alpha=score;
      if (alpha>=beta&&isbeta)   {               
         add_cut(nstep);         
         return alpha;   
      }                
      gui();                              
    }    
    return score;
}
//****************************  
int alphaBeta(int start, int nstep, int alpha, int beta, int depthleft) {
//start -            
//nstep -    
int score=-20000,best;
   if( depthleft==0) return quiescence(start,nstep,alpha,beta) ;      
   if (start>MAXSTEPS-100) return evaluate(nstep);              
   start_var=start; 
   only_action=false;         
   if (nstep!=cur_step) load_variants(nstep);     
   if (cur_var==0) { 
      if (check_on_table) {
       if (TRACE) Serial.println(F("checkmate!"));                 
       return -10000+nstep-cur_step;              
      } 
     return 0;
   }    
   int j=start+cur_var;     
   best=start;   
   for (int i=start;i<j;i++) { //                  
      if (nstep==cur_step) {         
        if (!hidden) {     
         Serial.print(str_step(i)); Serial.print("  ");
         Serial.print(i-start+1); Serial.print("/"); Serial.print(j-start);                        
        } 
        if (steps[i].weight<-9000) { Serial.println(F(" checkmate")); continue; }
        if (steps[i].fig2!=0||steps[i].check||(alpha<-100&&isbeta)) 
        // , ,     -100    
         { LIMDEPTH=MAXDEPTH+2; if (!hidden) Serial.print(F("+2")); } else LIMDEPTH=MAXDEPTH; 
      } else {
        if (TRACE) { //*****
         for (int u=0;u<nstep-cur_step;u++) Serial.print(F("       "));      
         Serial.println(str_step(i));
        } 
      }                  
      movestep(i);      
      steps[nstep]=steps[i];        
      int tmp=-alphaBeta(j+1,nstep+1,-beta,-alpha,depthleft-1);            
      backstep(i);
      steps[i].weight=tmp;
      if (tmp>score) score=tmp;       
      if (score>alpha) {
        alpha=score;
        if (nstep>cur_step) add_cut(nstep); //           
        if (TRACE) { Serial.print(F("ALPHA+:")); Serial.println(score); }        
        best=i;         
        if (nstep==cur_step) {
         steps[0]=steps[best];           
         if (!hidden) Serial.print(F(" BEST")); 
        }
      }
      if (alpha>=beta&&isbeta)   {        
       if (nstep>cur_step) add_cut(nstep); //                              
       if (TRACE) { Serial.print(F("BETA CUT:"));  Serial.println(score); }
       return alpha;            
      }   
      if (nstep==cur_step) {         
        if (!hidden) { Serial.print(F("        ")); Serial.println(tmp);  }      
        progress=100*(i-start+1)/(j-start);      
        if (alpha==9999||alpha==-5000) break;                             
        if (!solving) {
         if (alpha>startweight&&cur_level>2) break;
          else { //      -      +50%           
           if (cur_level>1&&100*(millis()-starttime)/(limittime-starttime)>240-limit*20) break;           
         }  
        }                   
      }       
      if (nstep==cur_step+1&&!solving&&progress==0) break;        
      gui();                                    
   }
   if (nstep==cur_step) { steps[nstep]=steps[best]; steps[0]=steps[best]; }                         
   return score;
}
//****************************  
void kingpositions() { 
 for (signed char i=0;i<8;i++) // 
  for (signed char j=0;j<8;j++)     
   if (pole[j][i]==fk) {
      WKJ=j; WKI=i; 
   } else if (pole[j][i]==-fk) {
      BKJ=j; BKI=i; 
   }    
}
//****************************  
int get_endspiel() { //   
  int weight=0;  
  for (signed char i=0;i<8;i++)
   for (signed char j=0;j<8;j++)     
    if (pole[j][i]<0) 
     weight+=fig_weight[-pole[j][i]];                               
    else if (pole[j][i]>0) 
     weight+=fig_weight[pole[j][i]]; //   8000
  if (weight<3500) endspiel=true; else endspiel=false;  
  return weight;
}
//****************************  
boolean is_drawn() { //   
  boolean drawn=false;
  int cn=0,cbw=0,cbb=0,co=0,cb=0,cw=0;
  for (signed char i=0;i<8;i++)
   for (signed char j=0;j<8;j++)   {  
    if (abs(pole[j][i])==1) co++;  
    if (abs(pole[j][i])>3&&abs(pole[j][i])<6) co++; // , ,   
    if (abs(pole[j][i])==6) continue;  // 
    if (abs(pole[j][i])==2) cn++; // 
    if (abs(pole[j][i])==3&&(i+j+2)%2==0) cbb++; //  
    if (abs(pole[j][i])==3&&(i+j+2)%2==1) cbw++; //   
    if (pole[j][i]==3) cw++; //     
    if (pole[j][i]==-3) cb++; //     
  }
  if (cn==1&&co+cbb+cbw==0) drawn=true;  // 
  if (cbb+cbw==1&&co+cn==0) drawn=true;  // 
  if (co+cn+cbb==0||co+cn+cbw==0) drawn=true; //    
  if (co+cn==0&&cb==1&&cw==1) drawn=true; //    
  if (drawn) return drawn;
  int rep=1;
  for (int s=cur_step-1; s>0; s--) {
    if (steps[s].fig1==0) continue;
    backstep(s); 
    boolean eq=true;    
    for (signed char i=0;i<8;i++)
     for (signed char j=0;j<8;j++) 
      if (pole[j][i]!=pole0[j][i]) 
       { eq=false; break; }
    if (eq) rep++;
    if (rep>2) break;    
  }  
  if (rep>1) { Serial.print(rep); Serial.println(" repetitions"); }
  for (signed char i=0;i<8;i++)
    for (signed char j=0;j<8;j++) pole[j][i]=pole0[j][i];   //          
  if (rep>2) drawn=true;
  return drawn;
}
//****************************  
int solve_step() { 
const int LMIN[6]={2,3,4,5,6,7};  
const int LMAX[6]={4,6,8,10,12,14};
boolean check_on; 
 if (!hidden) {
  M5.Lcd.fillRect(240,166,80,20,CBLACK);             
  M5.Lcd.drawFastHLine(10,237,220,CDARK);   
 }   
 starttime=millis();    
 isstatus=1; 
 limittime=starttime+limits[limit]*1000; //       
 for (signed char i=0;i<8;i++)
    for (signed char j=0;j<8;j++) pole0[j][i]=pole[j][i];   //         
    
 isbeta=true;
 if (hidden) isbeta=false;  
 else if (cur_step<5) { //   FEN 
  isbeta=false;
  for (int i=cur_step-1;i>0;i--) 
   if (steps[i].fig1!=0) backstep(i);
  for (signed char i=0;i<8;i++)
    for (signed char j=0;j<8;j++) 
     if (polestart[j][i]!=pole[j][i]) { isbeta=true; break; }       
  for (signed char i=0;i<8;i++)
    for (signed char j=0;j<8;j++) pole[j][i]=pole0[j][i];   //          
 }
 Serial.print("isbeta="); Serial.println(isbeta);
    
 lastbest.fig1=0;    
 kingpositions(); 
 int wei=get_endspiel();     
 
 count=0; 
 startweight=evaluate(cur_step);
 if (!hidden) {
  Serial.println("");
  Serial.println(F("---------------"));
  if (endspiel) { Serial.print(F("Endspiel: ")); Serial.println(wei); }   
  if (cur_step%2==1) Serial.print(F("WHITE, ")); else Serial.print(F("BLACK, "));  
  Serial.print(F("start Score= ")); Serial.println(startweight);
 } 
 start_var=cur_step+21;
 only_action=false;     
 lastbest.fig1=0;
 steps[0].fig1=0;
 load_variants(cur_step);       
 check_on=check_on_table; 
 if (cur_var==0||is_drawn()) {  
  beep(500);
  M5.Lcd.drawFastHLine(10,237,220,CBLACK);   
  M5.Lcd.drawFastHLine(10,239,220,CBLACK);   
  isstatus=0;
  solving=false;  
  if (check_on_table) return -9999; else return 8999;
 }
 if (cur_var==1) { //  1
  M5.Lcd.drawFastHLine(10,237,220,CBLACK);   
  M5.Lcd.drawFastHLine(10,239,220,CBLACK);   
  isstatus=0;
  solving=false;  
  steps[cur_step]=steps[cur_step+21];
  return startweight; 
 }          
 int vars=cur_var;   
 int ALPHA=-20000; 
 int BETA=20000;
 int score; 
 solving=true;  
 int l=0;
 for (signed char i=0;i<MAXCUTS;i++) { cuts[i].weight=0; cuts[i].fig1=0; } //   
 for (int i=cur_step+21;i<cur_step+21+vars;i++) {  //   
  movestep(i);
  steps[i].weight=evaluate(cur_step);
  if (steps[i].fig2!=0) steps[i].weight-=steps[i].fig1;
  backstep(i);
 } 
 while (l<6) {
  cur_level=l+1;  
  progress=0;
  M5.Lcd.drawFastHLine(10,239,220,CDARK);  
  MINDEPTH=LMIN[l];  
  MAXDEPTH=LMAX[l];            
  if (!hidden) {
   if (l>0) Serial.println(""); Serial.print(F("******* LEVEL=")); Serial.print(l+1);   
   Serial.print(" "); Serial.print(MINDEPTH); Serial.print("-"); Serial.print(MAXDEPTH);     
   Serial.print("  "); Serial.print((millis()-starttime)/1000.,1); Serial.println("s");                                            
  } 
  sort_variants(cur_step+21,cur_step+20+vars);  //            
  cur_var=vars;
  check_on_table=check_on;
  for (int i=cur_step+21;i<cur_step+21+vars;i++) steps[i].weight=-8000; //// 
  score=alphaBeta(cur_step+21,cur_step,ALPHA,BETA,MINDEPTH);  
  if (score>9996) break;   
  if (score>9000&&limit<2) break;  
  if (100*(millis()-starttime)/(limittime-starttime)>75-l*8) break;          
  if (!solving||!isbeta||hidden) break;              
  //if (hidden||cur_step<5) break;
  l++;     
 } //while l    
 if (score<-9000) {   
   if (!hidden) Serial.println(F("GIVE UP!")); 
 } else {
     int ran=random(5,10);   ////           
  while (ran>0&&cur_step<5&&!isbeta) {   
   for (int i=cur_step+21;i<cur_step+21+vars;i++) 
    if (steps[i].weight>=score-10) {
     ran--;     
     if (ran==0) {
      if (millis()%2==1) 
       if (steps[cur_step].x1!=steps[i].x1||steps[cur_step].x2!=steps[i].x2||  
           steps[cur_step].y1!=steps[i].y1||steps[cur_step].y2!=steps[i].y2) {
         steps[cur_step]=steps[i];                
         if (!hidden) Serial.println(F("Best step replaced!"));
         break; 
       }       
     }
    }     
  }                       ////  
  if (!hidden) { Serial.print(F("STEP=")); Serial.println(str_step(cur_step)); } 
  if (score>9000) {   
   if (!hidden) Serial.print(F("CHECKMATE ")); 
   if (score<9999) {
     if (!hidden) {Serial.print(F("in ")); Serial.print((9999-score)/2+1); Serial.print(F(" steps")); }       
   } else steps[cur_step].check=2;
   if (!hidden) Serial.println("");     
  } 
 }
 M5.Lcd.drawFastHLine(10,237,220,CBLACK);   
 M5.Lcd.drawFastHLine(10,239,220,CBLACK);  
 progress=0;  
 solving=0; 
 isstatus=0;
 endtime=millis();
 return score;
 
 
}
//****************************  
