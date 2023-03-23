#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include "esp_system.h"
#include "esp_timer.h"
#include "SSD1306.h"

#define LED_PIN 32
#define DATA_MAX 2000
#define DATA_INTERVAL 200
SSD1306 display(0x3c, 21, 22, GEOMETRY_128_32);


int data[DATA_MAX];
int t[DATA_MAX];


unsigned long prev, interval;


void setup()
{
  Serial.begin(115200);
}

void loop()
{
  print_v();
  while(1){
  }
}

int get_light_intensity()
{
  int intensity = 0;
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 0);
  pinMode(LED_PIN, ANALOG);
  intensity = analogRead(LED_PIN);
  return intensity;
}

void print_v()
{
  int i=0;
  unsigned long Time;
  int value = 0;

  unsigned long start;

  while(1){
    start = millis();
    value = get_light_intensity();
    if(150 < value) break;
  }
  
  
  while (1)
  {
    value = get_light_intensity();

    unsigned long curr = millis();    // 現在時刻を取得
    Time = curr - start;
    printf("%lu : %d\n",Time,value);
    
    if(Time == 1000){
      printf("finished!");
      break;
    }
    delay(1);
  }
}
