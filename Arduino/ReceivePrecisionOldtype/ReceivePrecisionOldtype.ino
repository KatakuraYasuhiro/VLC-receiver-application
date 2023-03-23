#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include "esp_system.h"
#include "esp_timer.h"
#include "SSD1306.h"

#define LED_PIN 32
SSD1306 display(0x3c, 21, 22, GEOMETRY_128_32);

int THRESHOLD = 0;

void setup()
{
  int rate = 10;
  int n;
  // initialize both serial ports:
  draw_display("Hello.");
  Serial.begin(115200);

  printf("+++%dms+++\n", rate);
  for (n = 0; n < 100; n++)
  {
    printf("---%d---\n", n + 1);
    LED_read_binaly(10);
    delay(500);
  }

  //sleep(350);

  draw_display("Finished.");
}

void loop()
{
  
}

void draw_display(char *str)
{
  display.init();                    //ディスプレイを初期化
  display.setFont(ArialMT_Plain_16); //フォントを設定
  display.drawString(0, 0, str);     //(0,0)の位置にHello Worldを表示
  display.display();
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

void LED_read_binaly(int rate)
{
  int value = 0, prev_v = 0;
  int value_bright = 1, value_dark = 1;
  int value_bright_num = 1, value_dark_num = 1;
  int n = 0;
  int blink_flag = 1;
  __int64_t timestamp;
  __int64_t prev_timestamp;
  __int64_t blink_next_time;
  __int64_t blink_time_interval = 700 * 1000;
  __int64_t sig_len = 0, sig_unit = 1;
  __int64_t sig_reader_bright = 1, sig_reader_dark = 0;
  __int64_t edge_up_time = 0, edge_down_time = 0;
  int bin_data_num = 0;
  int str_data_num = 0;
  int data[10];

  pinMode(LED_PIN, INPUT);

  THRESHOLD = 0;

   
  THRESHOLD = 150;

  draw_display("Ready to read.");

  timestamp = esp_timer_get_time();
  prev_timestamp = timestamp;

  //フラッシュライトの光を待つ
  while (value <= THRESHOLD)
  {
    value = get_light_intensity();
    // read intensity
    timestamp = esp_timer_get_time();
  }


  
  // リーダの取得
  while (1)
  {
    value = get_light_intensity();
    timestamp = esp_timer_get_time();
    prev_timestamp = timestamp;

    
  while ((THRESHOLD * 3) < value)
    {
      value_bright_num++;
      //value_bright += value;
      timestamp = esp_timer_get_time();
      sig_reader_bright += (timestamp - prev_timestamp); //点灯時間(基準時間T)
      prev_timestamp = timestamp;
      value = get_light_intensity();
    }
    

    timestamp = esp_timer_get_time();
    prev_timestamp = timestamp;
    
    if(value_bright_num > 0){
      THRESHOLD = 150; 
      break;
    }
  }

  sig_unit = sig_reader_bright;
  printf("standard_time: %"PRId64"\n",sig_unit);
  value = 0;
  prev_v = 0;

  // データの取得
  while (bin_data_num < 10)
  {
    timestamp = esp_timer_get_time();

   

    value = get_light_intensity() - THRESHOLD;
    if (value < 0)
    {
      value = 0;
    }

    if (prev_v == 0 && value != 0)
    { // If edge up
      if ((sig_unit * 0.7) <= (timestamp - edge_down_time))
      {
        edge_up_time = timestamp;
        // printf("(in edge up) edge_up_time : %g\n", edge_up_time);
      }
    }
    else if (prev_v != 0 && value == 0)
    {
      // If edge down
      edge_down_time = timestamp;
      sig_len = edge_down_time - edge_up_time;
      if (sig_unit / 2 < sig_len && str_data_num < 64)
      {
        // printf(" %" PRId64 "\n", sig_len); // print sig_len
        if (sig_unit < sig_len * 0.6)
        {
          data[bin_data_num++] = 1;
        }
        else
        {
          data[bin_data_num++] = 0;
        }
      }
    }
    prev_v = value;
  }
  //データ読み取り完了

  int i;
  for (i = 0; i < bin_data_num; i++)
  {
    printf("%d", data[i]);
    if (i < bin_data_num - 1)
    {
      printf(",");
    }
  }
  printf("\n");
}
