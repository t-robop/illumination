#include <Adafruit_NeoPixel.h>
#include "Udp.h"
#define PIN 16       //信号端子のピン番号
#define LED_NUM 50  //LEDの数

Adafruit_NeoPixel led = Adafruit_NeoPixel(LED_NUM, PIN, NEO_RGB + NEO_KHZ800);  //3つめのパラメータは使うLEDテープの制御ICがWS2812ならNEO_KHZ800 / WS2811ならNEO_KHZ400
int colorR = 0;
int colorG = 0;
int colorB = 0;
int brightness = 0;
bool oneByOneShineFlag = true;

Udp udp;
char ssid[] = "ERS-AP";
char password[] = "robop0304";
IPAddress local_IP(192, 168, 1, 101);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);
  udp.setup_udp(ssid, password, local_IP, gateway, subnet);
  Serial.println("setup start!");
  led.begin();
  led.show();   //一旦全てOFFの状態で反映
}

void loop() {
  getUdpPacket();

  //TODO いい感じの光らせ方を何パターンかつくる
  allShine();
}
void getUdpPacket() {
  udp.recieve_packet();
  String commands = udp.get_packet_buffer();
  if (commands.length() > 0) {
    udpPacketToColor(commands);
    udp.clear_packet_buffer();
    oneByOneShineFlag = true;
  }
  delay(1000);
}

void udpPacketToColor(String command) {
  // 中身 Rカラー,Gカラー,Bカラー,明るさ
  colorR = command.substring(0, 3).toInt();
  colorG = command.substring(3, 6).toInt();
  colorB = command.substring(6, 9).toInt();
  brightness = command.substring(9, 12).toInt();
}

void allShine(){
  for(int i=0; i<LED_NUM; i++){
    led.setPixelColor(i, led.Color(colorR, colorG, colorB));
    led.setBrightness(brightness);
  }
  led.show();
}

void oneByOneShine(){
  if (oneByOneShineFlag) {
    for (int i = 0; i < LED_NUM; i++) {
      led.setPixelColor(i, led.Color(colorR, colorG, colorB));
      led.setBrightness(brightness);
      delay(100);
      led.show();
      oneByOneShineFlag = false;
    }
  }
}

void fiveFlowShine(){
  for(int i=0; i<LED_NUM; i=i+5){
    fiveFlowShineExe(i);
  }
}

void fiveFlowShineExe(int led_num){
  int i;
  for (i = led_num; i < i + 5; i++)
  {
    led.setPixelColor(i, led.Color(colorR, colorG, colorB));
    led.setBrightness(brightness);
  }
  led.show();
  delay(800);

  for (i = led_num; i < i + 5; i++)
  {
    led.setPixelColor(i, led.Color(0, 0, 0));
    led.setBrightness(0);
  }
  led.show();
}
