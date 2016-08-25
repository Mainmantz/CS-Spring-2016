
#include <stm32f30x.h>  // Pull in include files for F30x standard drivers 
#include <f3d_led.h>     // Pull in include file for the local drivers
#include <f3d_uart.h>
#include <f3d_gyro.h>
#include <f3d_lcd_sd.h>
#include <f3d_i2c.h>
#include <f3d_accel.h>
#include <f3d_mag.h>
#include <f3d_nunchuk.h>
#include <f3d_rtc.h>
#include <f3d_systick.h>
#include <ff.h>
#include <diskio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <f3d_dac.h>
#include "queue.h"
#include <time.h>
#include <f3d_timer2.h>
//#include "player.h"
#define TIMER 20000
#define AUDIOBUFSIZE 128

struct ckhd {
  uint32_t ckID;
  uint32_t cksize;
};

struct fmtck {
  uint16_t wFormatTag;
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
};

extern uint8_t Audiobuf[AUDIOBUFSIZE];
extern int audioplayerHalf;
extern int audioplayerWhole;
FRESULT rc;			/* Result code */
DIR dir;			/* Directory object */
FILINFO fno;			/* File information object */
UINT bw, br;
unsigned int retval;
int bytesread;
FATFS Fatfs;		/* File system object */
FIL fid;		/* File object */
BYTE Buff[512];		/* File read buffer */
int ret;

void die (FRESULT rc) {
  printf("Failed with rc=%u.\n", rc);
  while (1);
}

void readckhd(FIL *fid, struct ckhd *hd, uint32_t ckID) {
  f_read(fid, hd, sizeof(struct ckhd), &ret);
  if (ret != sizeof(struct ckhd)) {
    printf("po, exiting\n");
    exit(-1);
  }
  if (ckID && (ckID != hd->ckID)) {
    printf("ckID && (ckID != hd->ckID)\n");
    exit(-1);
  }
}


int playsound(){

  rc = f_open(&fid, "thermo.wav", FA_READ);
  
  if (!rc) {
    struct ckhd hd;
    uint32_t  waveid;
    struct fmtck fck;
    
    readckhd(&fid, &hd, 'FFIR');
    
    f_read(&fid, &waveid, sizeof(waveid), &ret);
    if ((ret != sizeof(waveid)) || (waveid != 'EVAW'))
      return -1;
    
    readckhd(&fid, &hd, ' tmf');
    
    f_read(&fid, &fck, sizeof(fck), &ret);
    
    // skip over extra info
    
    if (hd.cksize != 16) {
      printf("extra header info %d\n", hd.cksize - 16);
      f_lseek(&fid, hd.cksize - 16);
    }
    
    printf("audio format 0x%x\n", fck.wFormatTag);
    printf("channels %d\n", fck.nChannels);
    printf("sample rate %d\n", fck.nSamplesPerSec);
    printf("data rate %d\n", fck.nAvgBytesPerSec);
    printf("block alignment %d\n", fck.nBlockAlign);
    printf("bits per sample %d\n", fck.wBitsPerSample);
    
    // now skip all non-data chunks !
    
    while(1){
      readckhd(&fid, &hd, 0);
      if (hd.ckID == 'atad')
	break;
      f_lseek(&fid, hd.cksize);
    }
    
    printf("Samples %d\n", hd.cksize);
    
    // Play it !
    
    //         audioplayerInit(fck.nSamplesPerSec);
    
    f_read(&fid, Audiobuf, AUDIOBUFSIZE, &ret);
    hd.cksize -= ret;
    audioplayerStart();
    while (hd.cksize) {
      int next = hd.cksize > AUDIOBUFSIZE/2 ? AUDIOBUFSIZE/2 : hd.cksize;
      if (audioplayerHalf) {
	if (next < AUDIOBUFSIZE/2)
	  bzero(Audiobuf, AUDIOBUFSIZE/2);
	f_read(&fid, Audiobuf, next, &ret);
	hd.cksize -= ret;
	audioplayerHalf = 0;
      }
      if (audioplayerWhole) {
	if (next < AUDIOBUFSIZE/2)
	  bzero(&Audiobuf[AUDIOBUFSIZE/2], AUDIOBUFSIZE/2);
	f_read(&fid, &Audiobuf[AUDIOBUFSIZE/2], next, &ret);
	hd.cksize -= ret;
	audioplayerWhole = 0;
      }
    }
    audioplayerStop();
  }
  
  printf("\nClose the file.\n"); 
  rc = f_close(&fid);
  
  if (rc) die(rc);
}

void draw_rect(uint8_t x, uint8_t y, uint16_t color,int size) {
  int i;
  int j;
  for (i=x; i < x+size; i++){
    for (j=y; j < y+size; j++){
      f3d_lcd_drawPixel(i,j,color);
    }
  }
}

  int vic = 0;
  int x=62;
  int y=1;
  int z=62;
  int u=151;
  int size = 8;

  int eat = 801;
  int eat1 = 801;
  int eat2 = 801;
  int eat3 = 801;
  int eat4 = 801;
  int score = 0;
  int score2 = 0;
int dirr = 0;

void start() {
  dirr = 0;
  vic = 0;
  x=62;
  y=1;
  z=62;
  u=151;
  eat = 801;
  eat1 = 801;
  eat2 = 801;
  eat3 = 801;
  eat4 = 801;
  score = 0;
  score2 = 0;
  f3d_lcd_fillScreen(WHITE);
  f3d_lcd_drawString(50,77,"AGARIO",RED,WHITE);
  delay(1000);
  f3d_lcd_drawString(50,77,"AGARIO",WHITE, WHITE);
  f3d_lcd_drawString(60,77,"3",RED,WHITE);
  delay(700);
  f3d_lcd_drawString(60,77,"3",WHITE,WHITE);
  f3d_lcd_drawString(60,77,"2",RED,WHITE);
  delay(700);
  f3d_lcd_drawString(60,77,"2",WHITE,WHITE);
  f3d_lcd_drawString(60,77,"1",RED,WHITE);
  delay(700);
  f3d_lcd_drawString(60,77,"1",WHITE,WHITE);
  f3d_lcd_drawString(60,77,"GO!",RED,WHITE);
  delay(700);
  f3d_lcd_fillScreen(WHITE);


}

int main(void) { 
  
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  f3d_i2c1_init();
  delay(10);
  f3d_accel_init();
  delay(10);
  f3d_mag_init();
  delay(10);
  f3d_lcd_init();
  f3d_nunchuk_init();
  f3d_gyro_init(); 
  f3d_uart_init();  
  f3d_user_btn_init();
  f3d_uart_init();  
  f3d_user_btn_init();
  start();

 
  f_mount(0, &Fatfs);/* Register volume work area */



  nunchuk_t nun;
  f3d_nunchuk_read(&nun);

 
  void check_eat() {
 
    if (eat > 400) {
      draw_rect(30,40,RED,size);
    }
    if (eat1 > 400) {
      draw_rect(90,40,RED,size);
    }
    if (eat2 > 400) {
      draw_rect(62,77,RED,size);
    }
    if (eat3 > 400) {
      draw_rect(30,110,RED,size);
    }
    if (eat4 > 400) {
      draw_rect(90,110,RED,size);
    }
  }

  void movements() {
    //playsound();
    char input = getchar();
    if (dirr  == 1) {
      u = u + 4;
      draw_rect(z,u-4,WHITE,size);
      draw_rect(z,u,BLUE,size);
    }
    if (dirr == 2) {
      z = z + 4;
      draw_rect(z-4,u,WHITE,size);
      draw_rect(z,u,BLUE,size);

    }
    if (dirr == 3) {
      z = z - 4;
      draw_rect(z+4,u,WHITE,size);
      draw_rect(z,u,BLUE,size);

    }
    if (dirr  == 4) {
      u = u - 4;
      draw_rect(z,u+4,WHITE,size);
      draw_rect(z,u,BLUE,size);
 
    }
    if(input == 'w') {
      dirr = 4 ;
    }

    if(input == 'd') {
      dirr = 2;
    }
    if (input == 'a') {
      dirr = 3;
    }
    
    if(input == 's') {
      dirr = 1;
    }
    
    if(dirr == 0) {
      draw_rect(z,u,BLUE,size);
    }
    if (nun.jy == 0) {
      y = y + 4;
      draw_rect(x,y-4,WHITE,size);
      draw_rect(x,y,BLUE,size);

    }
    if (nun.jy == 255) {
      y = y - 4;
      draw_rect(x,y+4,WHITE,size);
      draw_rect(x,y,BLUE,size);
 
    }
    if (nun.jx == 255) {
      x = x + 4;
      draw_rect(x-4,y,WHITE,size);
      draw_rect(x,y,BLUE,size);

    }
    if (nun.jx == 0) {
      x = x - 4;
      draw_rect(x+4,y,WHITE,size);
      draw_rect(x,y,BLUE,size);
    }
  }
  void collision() {
    if (x < 30 + size && x + size > 30 && y < 40 + size && y + size > 40 && eat > 200) {
      eat = 0;
      score = score + 1;
      draw_rect(30,40,WHITE,size);
    }
    if (x < 90 + size && x + size > 90 && y < 40 + size && y + size > 40 && eat1 > 200) {
      eat1 = 0;
      score = score + 1;
      draw_rect(90,40,WHITE,size);
    }
    if (x < 62 + size && x + size > 62 && y < 77 + size && y + size > 77 && eat2 > 200) {
      eat2 = 0;
      score = score + 1;
      draw_rect(62,77,WHITE,size);
    }
    if (x < 30 + size && x + size > 30 && y < 110 + size && y + size > 110 && eat3 > 200) {
      eat3 = 0;
      score = score + 1;
      draw_rect(30,110,WHITE,size);
    }
    if (x < 90 + size && x + size > 90 && y < 110 + size && y + size > 110 && eat4 > 200) {
      eat4 = 0;
      score = score + 1;
      draw_rect(90, 110, WHITE,size);
    }

    if (z < 30 + size && z + size > 30 && u < 40 + size && u + size > 40 && eat > 200) {
      eat = 0;
      score2 = score2 + 1;
      draw_rect(30, 40, WHITE,size);
    }
    if (z < 90 + size && z + size > 90 && u < 40 + size && u + size > 40 && eat1 > 200) {
      eat1 = 0;
      score2 = score2 + 1;
      draw_rect(90, 40, WHITE,size);
    }
    if  (z < 62 + size && z + size > 62 && u < 77 + size && u + size > 77 && eat2 > 200) {
      eat2 = 0;
      score2 = score2 + 1;
      draw_rect(62, 77, WHITE,size);
    }
    if (z < 30 + size && z + size > 30 && u < 110 + size && u + size > 110 && eat3 > 200) {
      eat3 = 0;
      score2 = score2 + 1;
      draw_rect(30, 110, WHITE,size);
    }
    if (z < 90 + size && z + size > 90 && u < 110 + size && u + size > 110 && eat4 > 200) {
      eat4 = 0;
      score2 = score2 + 1;
      draw_rect(90, 110, WHITE,size);
    }
    if(x < 0 || x > 128 - size || y < 0 || y > 160 - size) {
      vic = 2;
    }
    if(z < 0 || z > 128 - size || u < 0 || u > 160 - size) {
      vic = 1;
    }
    if(x < z + size && x + size > z && y < u + size && y + size > u) {
      if(score > score2) {
	vic = 1;
      } else if (score2 > score){
	vic = 2;
      }
    }
  }
  char buffer[50];
  char buffer2[50];
  f3d_lcd_drawString(0,0,"P1",MAGENTA,WHITE);
  f3d_lcd_drawString(0,150,"P2",MAGENTA,WHITE);
  while(1) {
    if (vic == 2) {
      f3d_lcd_fillScreen(WHITE);
      f3d_lcd_drawString(20,77,"PLAYER TWO WINS",RED,WHITE);    
      playsound();
    } if (vic == 1){
      f3d_lcd_fillScreen(WHITE);
      f3d_lcd_drawString(20,77,"PLAYER ONE WINS",RED,WHITE);     
      playsound();
    } else {
    sprintf(buffer, "%d", score);
    sprintf(buffer2, "%d", score2);
    if(score2 > 9) {
      vic = 2;
  }
    if(score > 9) {
      vic = 1;
    }
    f3d_lcd_drawString(20,0,buffer,MAGENTA,WHITE);
    f3d_lcd_drawString(20,150,buffer2,MAGENTA, WHITE);
    check_eat();
    f3d_nunchuk_read(&nun);
    draw_rect(x,y,BLUE,size);
    draw_rect(z,u,BLUE,size);
    eat = eat + 4;
    eat1 = eat1 + 4;
    eat2 = eat2 + 4;
    eat3 = eat3 + 4;
    eat4 = eat4 + 4;
      
    movements();
    collision();
    }
    
  }
    // play_selected_file(samples[0]); 
    /* if ((x - size1 == z || x == z - size2) && (y - size1 == u || y == u - size2)) { */
    /*   f3d_lcd_drawString(0,0,"YOU LOSE",BLUE,RED); */
    /*   play_selected_file(samples[0]); */
    /* } */
    /* if ((x - size1 == z || x == z - size2) && (y - size1 == u || y == u - size2)) { */
    /*   f3d_lcd_drawString(0,0,"YOU LOSE",BLUE,RED); */
    /*   play_selected_file(samples[0]); */
    /* } */
   
    
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
  /* Infinite loop */
  /* Use GDB to find out why we're here */
  while (1);
}
#endif

/* main.c ends here */
