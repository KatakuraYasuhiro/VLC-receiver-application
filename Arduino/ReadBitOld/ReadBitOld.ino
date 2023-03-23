#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include "esp_system.h"
#include "esp_timer.h"
#include "SSD1306.h"

#define LED_PIN 32
SSD1306 display(0x3c, 21, 22, GEOMETRY_128_32);

int n = 0;
int THRESHOLD = 500;
int value = 0, prev_v = 0;
int bin_data_num = 0;
int str_data_num = 0;
int data[10];
int value_bright = 1, value_dark = 1;
int value_bright_num = 1, value_dark_num = 1;

__int64_t timestamp;
__int64_t prev_timestamp;
__int64_t sig_len = 0, sig_unit = 0; //sig_unit = `基準時間T`
__int64_t sig_reader_bright = 1, sig_reader_dark = 0;
__int64_t edge_up_time = 0, edge_down_time = 0;
__int64_t sig_trailer = 1;
__int64_t whole_deadline = 2500000;

void setup()
{
  // initialize both serial ports:
  Serial.begin(115200);

}

void loop()
{
  draw_display("Hello.");
  int n;

 for (n = 0; n < 1; n++)
 {
   printf("---%d---\n", n + 1);
   LED_read_bit();
   delay(500);
 }
  draw_display("Finished.");
  while(1){    
  }
  
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
  printf("%"PRId64" read finish\n",timestamp);
  return intensity;
}


// リーダの取得
__int64_t LED_read_leader()
{
  timestamp = esp_timer_get_time();
  prev_timestamp = timestamp;

  while (1)
  {
    printf("%"PRId64"\n", timestamp);
    value = get_light_intensity();
    //timestamp = esp_timer_get_time();
    prev_timestamp = esp_timer_get_time();
    
  while ((THRESHOLD * 3) < value)
  {
    printf("%"PRId64" get leader now\n", timestamp);
    value_bright_num++;
    value_bright += value;
    timestamp = esp_timer_get_time();
    sig_reader_bright += (timestamp - prev_timestamp);
    prev_timestamp = timestamp;
    value = get_light_intensity();
  }

   printf("%"PRId64" go out\n", timestamp);
    
    timestamp = esp_timer_get_time();
    prev_timestamp = timestamp;
    
    if(value_bright_num > 0){
     THRESHOLD = value_bright / value_bright_num;
     break;
    }
  }
 sig_unit = sig_reader_bright; 
 return sig_unit;
}

// データの取得
void LED_read_data()
{
  int bit_state=0,prev_bit_state = 0;
  bin_data_num = 0;
  
  while (bin_data_num < 10)
  {
    value = get_light_intensity() - THRESHOLD;
    timestamp = esp_timer_get_time();
    printf("%"PRId64" get data now\n", timestamp);

  //通信失敗処理
   if(whole_deadline < timestamp - prev_timestamp){
    printf("Failed!!\n");
    break;
   }
    
    if (value <= 0)
    {
      bit_state = 0;
    }else if(0 < value){
        bit_state = 1;
    }
    
    if (prev_bit_state == 0 && bit_state == 1)
    { // If edge up
        edge_up_time = timestamp;
    }
    else if (prev_bit_state == 1 && bit_state == 0)
    {
      // If edge down
      edge_down_time = timestamp;
      sig_len = edge_down_time - edge_up_time; //bit_up -> bit_down の時間を計算(点灯時間)

      if (sig_unit / 2 < sig_len){
        if (sig_unit < sig_len*0.6)
        {
          data[bin_data_num++] = 1;
        }
        else
        {
          data[bin_data_num++] = 0;
        }
      }
    }
    prev_bit_state = bit_state;
  }
  printf("%"PRId64" finish data reading\n", timestamp);
  //データ読み取り完了
}

//トレイラの読み取り
/*void LED_read_trailer()
{
  while(1){
    printf("get trailer data now\n");
    value = get_light_intensity();
    timestamp = esp_timer_get_time();
    if(0 < value) {
      sig_trailer += (timestamp - prev_timestamp);
    }
    prev_timestamp = timestamp;
    if(sig_unit*4.5 < sig_trailer){
      printf("finish trailer\n");
      break;
    }
  }
}
*/
void LED_read_bit()
{
  pinMode(LED_PIN, INPUT);

  THRESHOLD = 150;

  draw_display("Ready to read.");

  timestamp = esp_timer_get_time();
  prev_timestamp = timestamp;

  while (value <= THRESHOLD)
  {
    value = get_light_intensity();
    // read intensity
    timestamp = esp_timer_get_time();
  }

//この辺がまだ
  sig_unit = LED_read_leader();

  LED_read_data();
  
  //LED_read_trailer();
//この辺がまだ

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
