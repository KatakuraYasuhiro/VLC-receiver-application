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


int i=1;
int j = 1;
__int64_t timestamp;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  while(i<501){
  timestamp = esp_timer_get_time();
  printf(" %"PRId64"\n",timestamp);
  i++;
  }
}
