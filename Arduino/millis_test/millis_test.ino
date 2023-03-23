#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include "esp_system.h"
#include "esp_timer.h"
#include "SSD1306.h"

#define LED_PIN 32
#define DATA_MAX 20000
SSD1306 display(0x3c, 21, 22, GEOMETRY_128_32);


int data[DATA_MAX];



void setup()
{
  Serial.begin(115200);
}

void loop()
{  
  print_time();
  while(1){
  }
}

void print_time(){
  int i = 0;
  int t=0;

  while(1){
  unsigned long curr = millis();    // 現在時刻を取得
  t =curr;                          //currをint型に変換
  printf("%d:%lo\n",t,curr);        
  data[i++] = t;
  if(curr == 3000) break;
  }
}
