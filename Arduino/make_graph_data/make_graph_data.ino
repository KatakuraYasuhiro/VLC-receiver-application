#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include "esp_system.h"
#include "esp_timer.h"
#include "SSD1306.h"

#define LED_PIN 32
#define DATA_MAX 20000
#define DATA_INTERVAL 200
SSD1306 display(0x3c, 21, 22, GEOMETRY_128_32);

int THRESHOLD = 100;

// struct data
// {
//   __int64_t t;
//   int v;
// };
// struct data d[DATA_MAX];

int data[DATA_MAX];
// int diff_time[DATA_MAX];
// char data[DATA_MAX][30];

void setup()
{
  // initialize both serial ports:
  Serial.begin(115200);
}

void loop()
{  
  print_v();
  delay(15 * 1000);
  //printf("%d\n", get_light_intensity());
  //delay(1);
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
  int i = 0;
  int value = 0, prev_v = 0;
  int read_flag = 0;
  __int64_t timestamp;
  // __int64_t prev_timestamp;
  __int64_t get_data_time;


  printf("5\n");
  delay(1*1000);
  printf("4\n");
  delay(1*1000);
  printf("3\n");
  delay(1*1000);
  printf("2\n");
  delay(1*1000);
  printf("1\n");
  delay(1*1000);
  printf("flash!!\n\n");

  while (1)
  {
    value = get_light_intensity();
    timestamp = esp_timer_get_time();

    if (prev_v == 0 && (value != 0 && 5 < value))
    {
      read_flag = 1;
      get_data_time = timestamp + DATA_INTERVAL; // 次にデータを取るべき時間のベースを設定
      // prev_timestamp = timestamp;
    }
    prev_v = value;

    // printf("%" PRId64 ",%d,%d\n", timestamp, THRESHOLD < value, value);
    if (read_flag == 1 && get_data_time <= timestamp)
    {
      // struct data の配列にそれぞれ timestampとvalueを格納
      // d[i].t = timestamp;
      // d[i].v = value;
      // i++;
      // sprintf(data[i++], "%" PRId64 ",%d\n", timestamp, value);
      // diff_time[i] = (int)timestamp - (int)prev_timestamp;
      data[i++] = value;
      get_data_time += DATA_INTERVAL; // 次の時間を求める
      // prev_timestamp = timestamp;
    }

    if (DATA_MAX < i)
    {
      // DATA_MAX 回だけ繰り返したら終了
      break;
    }
    // delay(1);
  }
  // 最後に data を出力
  for (i = 0; i < DATA_MAX; i++)
  {
    // printf("%" PRId64 ",%d\n", timestamp, value);
    printf("%d\n", data[i]);
    // printf("%d,%d\n", data[i], diff_time[i]);
  }
}
