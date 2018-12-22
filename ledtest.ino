#include <Adafruit_NeoPixel.h>
#include "Udp.h"
#define PIN 16       //信号端子のピン番号
#define LED_NUM 50  //LEDの数

Adafruit_NeoPixel led = Adafruit_NeoPixel(LED_NUM, PIN, NEO_RGB + NEO_KHZ800);  //3つめのパラメータは使うLEDテープの制御ICがWS2812ならNEO_KHZ800 / WS2811ならNEO_KHZ400
int colorR = 0;
int colorG = 0;
int colorB = 0;

int colorRSecond = 0;
int colorGSecond = 0;
int colorBSecond = 0;

int colorRThird = 0;
int colorGThird = 0;
int colorBThird = 0;

int brightness = 0;
bool oneByOneShineFlag = true;

bool firstShineState = false;
bool secondShineState = false;
bool thirdShineState = false;
int firstShineIndex = 0;
int secondShineIndex = 0;
int thirdShineIndex = 0;

int colorRArray[50];
int colorGArray[50];
int colorBArray[50];
int state = 0;

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
  //TODO パケット情報に光らせ方のパターン値を追加し、分岐させて対応する関数を呼び出す
  //rasenShine();
  state1 = state - 10;
  state2 = state - 20;
  if (state <= 50 && state >= 0) {
    colorRArray[state] = colorR;
    colorRArray[state] = colorG;
    colorRArray[state] = colorB;
    gaoScene(state);
  }
  
  if (state1 <= 50 && state1 >= 0) {
    colorRArray[state2] = colorRSecond;
    colorRArray[state2] = colorGSecond;
    colorRArray[state2] = colorBSecond;
    gaoScene(state1);
  }
  if (state2 <= 50 && state2 >= 0) {
    colorRArray[state2] = colorRThird;
    colorRArray[state2] = colorGThird;
    colorRArray[state2] = colorBThird;
    gaoScene(state2);
  }

  led.show();
  delay(50);

}

void gaoScene(int num) {
  led.setPixelColor(num, led.Color(colorRArray[num], colorGArray[num], colorBArray[num]));
  state++;
}

void getUdpPacket() {
  int colorState = 0;
  udp.recieve_packet();
  String commands = udp.get_packet_buffer();
  if (commands.length() > 0) {
    if (firstShineState == false)
    {
      colorR = commands.substring(0, 3).toInt();
      colorG = commands.substring(3, 6).toInt();
      colorB = commands.substring(6, 9).toInt();

      firstShineState = true;
      delay(10);
    }
    else if (firstShineState == true && secondShineState == false)
    {
      colorRSecond = commands.substring(0, 3).toInt();
      colorGSecond = commands.substring(0, 3).toInt();
      colorBSecond = commands.substring(0, 3).toInt();
      secondShineState = true;
      delay(10);
    }
    else if(firstShineState == true && secondShineState == true && thirdShineState == false)
    {
      colorRThird = commands.substring(0, 3).toInt();
      colorGThird = commands.substring(0, 3).toInt();
      colorBThird = commands.substring(0, 3).toInt();
      thirdShineState = true;
      delay(10);
    }
    brightness = commands.substring(9, 12).toInt();
    //udpPacketToColor(commands, colorState);
    udp.clear_packet_buffer();
    oneByOneShineFlag = true;
  }
  delay(1000);
}

void udpPacketToColor(String command, int colorState) {
  // 中身 Rカラー,Gカラー,Bカラー,明るさ

  
  switch (colorState)
  {
    case 1:
      colorR = command.substring(0, 3).toInt();
      colorG = command.substring(3, 6).toInt();
      colorB = command.substring(6, 9).toInt();
      break;

    case 2:
      colorRSecond = command.substring(0, 3).toInt();
      colorGSecond = command.substring(0, 3).toInt();
      colorBSecond = command.substring(0, 3).toInt();
      break;

    case 3:
      colorRThird = command.substring(0, 3).toInt();
      colorGThird = command.substring(0, 3).toInt();
      colorBThird = command.substring(0, 3).toInt();
      break;

    default:
      break;
  }

  brightness = command.substring(9, 12).toInt();
}

//すべてのLEDが一色で一斉に光る
void allShine(){
  int i;
  for(i=0; i<LED_NUM; i++){
    led.setPixelColor(i, led.Color(colorR, colorG, colorB));
    led.setBrightness(brightness);
  }
  led.show();
}

//LEDが一つづつ光り始めて、全点灯のままになる(すべて同色) //TODO 3段階で色を変更する 色の指定は後から追えるようにする
void oneByOneShine(){
  int i;
  if (oneByOneShineFlag) {
    for (i = 0; i < LED_NUM; i++) {
      led.setPixelColor(i, led.Color(colorR, colorG, colorB));
      led.setBrightness(brightness);
      led.show();
      oneByOneShineFlag = false;
      delay(50);
    }
  }
}

void rasenShine(){
  while (firstShineIndex < LED_NUM && secondShineIndex < LED_NUM && thirdShineIndex < LED_NUM)
  {

    if (firstShineState == true && secondShineState == false && thirdShineState == false)
    {
      Serial.println("first");
      led.setPixelColor(firstShineIndex, led.Color(colorR, colorG, colorB));
      led.setBrightness(brightness);
      led.show();
      delay(50);
      firstShineIndex++;
    }
    else if (firstShineState == true && secondShineState == true && thirdShineState == false)
    {
      Serial.println("second");
      led.setPixelColor(firstShineIndex, led.Color(colorR, colorG, colorB));
      led.setPixelColor(secondShineIndex, led.Color(colorRSecond, colorGSecond, colorBSecond));
      led.setBrightness(brightness);
      led.show();
      delay(50);
      firstShineIndex++;
      secondShineIndex++;
    }
    else if (firstShineState == true && secondShineState == true && thirdShineState == true)
    {
      Serial.println("third");
      led.setPixelColor(firstShineIndex, led.Color(colorR, colorG, colorB));
      led.setPixelColor(secondShineIndex, led.Color(colorRSecond, colorGSecond, colorBSecond));
      led.setPixelColor(thirdShineIndex, led.Color(colorRThird, colorGThird, colorBThird));
      led.setBrightness(brightness);
      led.show();
      delay(50);
      firstShineIndex++;
      secondShineIndex++;
      thirdShineIndex++;
    }
  }
  firstShineIndex = 0;
  secondShineIndex = 0;
  thirdShineIndex = 0;
}


// i++;

//LED5個ずつが同色で光る→消えるを繰り返す
void fiveFlowShine(){
  int startIndex = 0;
  int lastIndex = 5;
  while(startIndex <= LED_NUM - 5){
    fiveFlowShineExe(startIndex, lastIndex);
    startIndex += 5;
    lastIndex += 5;
  }
}

void fiveFlowShineExe(int startIndex, int lastIndex){
  int i;
  for (i = startIndex; i < lastIndex; i++)
  {
    led.setPixelColor(i, led.Color(colorR, colorG, colorB));
    led.setBrightness(brightness);
  }
  led.show();
  delay(300);

  for (i = startIndex; i < lastIndex; i++)
  {
    led.setPixelColor(i, led.Color(0, 0, 0));
    led.setBrightness(0);
  }
  led.show();
}

//LEDを等間隔で光らせる (1個空け、全て同色)
void alternatelyShine(){
  evenLedShineExe();
  delay(1000);
  oddLedShineExe();
}

void evenLedShineExe(){
  int i;
  for (i = 1; i < LED_NUM; i = i + 2)
  {
    led.setPixelColor(i, led.Color(0, 0, 0));
    led.setBrightness(0);
  }
  led.show();

  for (i = 0; i < LED_NUM; i = i + 2)
  {
    led.setPixelColor(i, led.Color(colorR, colorG, colorB));
    led.setBrightness(brightness);
  }
  led.show();
}

void oddLedShineExe(){
  int i;
  for (i = 0; i < LED_NUM; i = i + 2)
  {
    led.setPixelColor(i, led.Color(0, 0, 0));
    led.setBrightness(0);
  }
  led.show();

  for(i=1; i<LED_NUM; i=i+2){
    led.setPixelColor(i, led.Color(colorR, colorG, colorB));
    led.setBrightness(brightness);
  }
  led.show();
}
