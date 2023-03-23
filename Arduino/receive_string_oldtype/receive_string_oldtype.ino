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
  // initialize both serial ports:
  draw_display("Setting now...");
  Serial.begin(115200);
}

void loop()
{
  char str[64] = "\0";
  // char str_display[100] = "\0";
  // printf("---start---\n");
  LED_read_binaly(str);
  printf("str = [%s]\n", str);
  // sprintf(str_display, "Data : [%s]\n", str);
  draw_display(str);
  delay(15 * 1000);
  // print_v();
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

void LED_read_binaly(char str_data[64])
{
  int value = 0, prev_v = 0;
  int value_bright = 0, value_dark = 0;
  int value_bright_num = 0, value_dark_num = 0;
  int n = 0;
  __int64_t timestamp;
  __int64_t prev_timestamp;
  __int64_t blink_next_time;
  __int64_t blink_time_interval = 700 * 1000;
  __int64_t sig_len = 0, sig_unit = 0;
  __int64_t sig_reader_bright = 0, sig_reader_dark = 0;
  __int64_t edge_up_time = 0, edge_down_time = 0;
  int bin_data_num = 0;
  int str_data_num = 0;
  unsigned char data = 0;

  pinMode(LED_PIN, INPUT);

  THRESHOLD = 0;

  while (n < 1000)  //閾値の設定
  {
    value = get_light_intensity();
    if (THRESHOLD < value)
    {
      THRESHOLD = value;
    }
    n++;
    delay(1);
  }
  THRESHOLD += 1;
  printf("THRESHOLD 1 : %d\n", THRESHOLD);

  draw_display("Ready to read.");

  timestamp = esp_timer_get_time();
  prev_timestamp = timestamp;
  blink_next_time = esp_timer_get_time() + blink_time_interval;

  while (value <= THRESHOLD) //電圧値が閾値を超えるまで待機
  {
    value = get_light_intensity();
    
    timestamp = esp_timer_get_time();
    if (blink_next_time < timestamp)
    {
      blink_next_time = timestamp + blink_time_interval;
    }
  }
 
  printf("in light 1\n");

  while (value <= ((THRESHOLD + 1) * 3))
  {
    value = get_light_intensity();
  }

  printf("in light 2\n");

  // リーダの取得
  while (1)
  {
    value = get_light_intensity();
    timestamp = esp_timer_get_time();
    prev_timestamp = timestamp;
    while ((THRESHOLD * 3) < value)
    {
      value_bright_num++;
      value_bright += value;
      timestamp = esp_timer_get_time();
      sig_reader_bright += (timestamp - prev_timestamp);
      prev_timestamp = timestamp;
      value = get_light_intensity();
    }
    
    printf("bright %" PRId64 "\n", sig_reader_bright);

    timestamp = esp_timer_get_time();
    prev_timestamp = timestamp;
    
    if(value_bright_num > 0){
      THRESHOLD = value_bright / value_bright_num;
      break;
    }
  }

  printf("THRESHOLD 2 : %d\n", THRESHOLD);

  sig_unit = sig_reader_bright;
   
  printf("sig_unit : %" PRId64 "\n", sig_unit);

  value = 0;
  prev_v = 0;

  // データの取得
  while (1)
  {
    timestamp = esp_timer_get_time();
    if ((sig_unit * 5 < (timestamp - edge_up_time)) && (edge_down_time < edge_up_time))
    {
      break;
    }

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
      }
    }
    else if (prev_v != 0 && value == 0)
    {
      // If edge down
      edge_down_time = timestamp;
      sig_len = edge_down_time - edge_up_time;
      if (sig_unit / 2 < sig_len && str_data_num < 64)
      {
        printf(" %" PRId64 "\n", sig_len); // print sig_len
        if (sig_unit < sig_len * 0.6)
        {
          data |= 0x01;
        }

        if (bin_data_num == 7)
        {
          printf("char : %c\n", data);
          str_data[str_data_num] = data;
          str_data_num++;
          bin_data_num = 0;
          data = 0;
        }
        else
        {
          data <<= 1;
          bin_data_num++;
        }
      }
    }
    prev_v = value;
  }
  str_data[str_data_num] = '\0';
}
