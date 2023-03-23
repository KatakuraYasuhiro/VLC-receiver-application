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
char str_data[64];

__int64_t timestamp,start_time;
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
  draw_display("Stand by ....");
  delay(1*500);
  int n,success = 0;

for(n = 0;n < 10; n++){
   printf("---%d---\n",n + 1);

   LED_read_bit();
   printf("str = [%s]\n",str_data);
   //送信成功回数のカウント
   if(strcmp(str_data,"example") == 0 ){
      success = success +1;
    }
   delay(500);
 }
  printf("Precision is : %d / 10\n",success);
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


// リーダの取得
__int64_t LED_read_leader()
{
  int value_bright = 0;
  int value_bright_num = 0;
  
  //printf("read_leader\n");
  timestamp = esp_timer_get_time();
  prev_timestamp = timestamp;
  sig_reader_bright = 0;
  
  while (THRESHOLD < value)
  {
    timestamp = esp_timer_get_time();
    sig_reader_bright += (timestamp - prev_timestamp);
    prev_timestamp = timestamp;
    value = get_light_intensity();
  }

 timestamp = esp_timer_get_time();
 prev_timestamp = timestamp;
  
 sig_unit = sig_reader_bright/5; 
 //printf("T=%"PRId64"\n",sig_unit);
 return sig_unit;
}

// データの取得
void LED_read_data()
{
  //printf("read_data\n");
  __int64_t low_time = esp_timer_get_time();
  unsigned char data = 0;
  bin_data_num = 0;

  while (1)
  {
    start_time = esp_timer_get_time();
    if(sig_unit * 20 < (start_time - low_time)) break;
    value = get_light_intensity() - THRESHOLD;
    
    if(0 < value){
     low_time = esp_timer_get_time();
     while(0 < value){
      timestamp = esp_timer_get_time();
      sig_len = timestamp - start_time;
      value = get_light_intensity() - THRESHOLD;
      }
      
     if(sig_len <= sig_unit *1.6){
      //printf("0\n");
      data <<= 1;
      bin_data_num++;
       }
     else if(sig_unit <= sig_len){
       //printf("1\n");
       data <<= 1;
       data |= 0x01;
       bin_data_num++;
     }
   }
   
   if(bin_data_num > 7){
      //printf("char: %c\n",data);
      str_data[str_data_num] = data;
      str_data_num++;
      bin_data_num = 0;
      data = 0;
   }
   
  }
  //データ読み取り完了
}

void LED_read_bit()
{
  pinMode(LED_PIN, INPUT);
  THRESHOLD = 150;
  //data = 0;

  draw_display("Ready to read.");

  timestamp = esp_timer_get_time();
  prev_timestamp = timestamp;

  while (value <= THRESHOLD)
  {
    value = get_light_intensity();
  }

  sig_unit = LED_read_leader();

  LED_read_data();

  str_data[str_data_num] = '\0';
  str_data_num = 0;

}
