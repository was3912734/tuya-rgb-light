#include <TuyaWifi.h>
#include <SoftwareSerial.h>


// Simple demonstration on using an input device to trigger changes on your
// NeoPixels. Wire a momentary push button to connect from ground to a
// digital IO pin. When the button is pressed it will change to a new pixel
// animation. Initial state has all pixels off -- press the button once to
// start the first animation. As written, the button does not interrupt an
// animation in-progress, it works only when idle.

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Digital IO pin connected to the button. This will be driven with a
// pull-up resistor so the switch pulls the pin to ground momentarily.
// On a high -> low transition the button press logic will execute.
#define BUTTON_PIN   2

#define PIXEL_PIN    11  // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT  8  // Number of NeoPixels

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

boolean oldState = HIGH;
int     mode     = 0;    // Currently-active animation mode, 0-9


TuyaWifi my_device;

unsigned char led_state = 0;
/* Connect network button pin */

int key_pin = 12;
int i=0;
long int state = 0;
int cdpid = 0;
//开关(可下发可上报)
//备注:
#define DPID_SWITCH_LED 20
//模式(可下发可上报)
//备注:
#define DPID_WORK_MODE 21
//亮度值(可下发可上报)
//备注:
#define DPID_BRIGHT_VALUE 22
//冷暖值(可下发可上报)
//备注:
#define DPID_TEMP_VALUE 23
//彩光(可下发可上报)
//备注:类型：字符；
//Value: 000011112222；
//0000：H（色度：0-360，0X0000-0X0168）；
//1111：S (饱和：0-1000, 0X0000-0X03E8）；
//2222：V (明度：0-1000，0X0000-0X03E8)
#define DPID_COLOUR_DATA 24
//场景(可下发可上报)
//备注:类型：字符; 
//Value: 0011223344445555666677778888;
//00：情景号;
//11：单元切换间隔时间（0-100）;
//22：单元变化时间（0-100）;
//33：单元变化模式（0静态1跳变2渐变）;
//4444：H（色度：0-360，0X0000-0X0168）;
//5555：S (饱和：0-1000, 0X0000-0X03E8);
//6666：V (明度：0-1000，0X0000-0X03E8);
//7777：白光亮度（0-1000）;
//8888：色温值（0-1000）;
//注：数字1-8的标号对应有多少单元就有多少组
#define DPID_SCENE_DATA 25
//倒计时剩余时间(可下发可上报)
//备注:
#define DPID_COUNTDOWN 26
//音乐灯(只下发)
//备注:类型：字符串；
//Value: 011112222333344445555；
//0：   变化方式，0表示直接输出，1表示渐变；
//1111：H（色度：0-360，0X0000-0X0168）；
//2222：S (饱和：0-1000, 0X0000-0X03E8）；
//3333：V (明度：0-1000，0X0000-0X03E8）；
//4444：白光亮度（0-1000）；
//5555：色温值（0-1000）
#define DPID_MUSIC_DATA 27
//调节(只下发)
//备注:类型：字符串 ;
//Value: 011112222333344445555  ;
//0：   变化方式，0表示直接输出，1表示渐变;
//1111：H（色度：0-360，0X0000-0X0168）;
//2222：S (饱和：0-1000, 0X0000-0X03E8);
//3333：V (明度：0-1000，0X0000-0X03E8);
//4444：白光亮度（0-1000）;
//5555：色温值（0-1000）
#define DPID_CONTROL_DATA 28
//入睡(可下发可上报)
//备注:灯光按设定的时间淡出直至熄灭
#define DPID_SLEEP_MODE 31
//唤醒(可下发可上报)
//备注:灯光按设定的时间逐渐淡入直至设定的亮度
#define DPID_WAKEUP_MODE 32
//断电记忆(可下发可上报)
//备注:通电后，灯亮起的状态
#define DPID_POWER_MEMORY 33
//勿扰模式(可下发可上报)
//备注:适用经常停电区域，开启通电勿扰，通过APP关灯需连续两次上电才会亮灯
//Value：ABCCDDEEFFGG
//A：版本，初始版本0x00；
//B：模式，0x00初始默认值、0x01恢复记忆值、0x02用户定制；
//CC：色相 H，0~360；
//DD：饱和度 S，0~1000；
//EE：明度 V，0~1000；
//FF：亮度，0~1000；
//GG：色温，0~1000；
#define DPID_DO_NOT_DISTURB 34
//炫彩情景(可下发可上报)
//备注:专门用于幻彩灯带场景
//Value：ABCDEFGHIJJKLLM...
//A：版本号；
//B：情景模式编号；
//C：变化方式（0-静态、1-渐变、2跳变、3呼吸、4-闪烁、10-流水、11-彩虹）
//D：单元切换间隔时间（0-100）;
//E：单元变化时间（0-100）；
//FGH：设置项；
//I：亮度（亮度V：0~100）；
//JJ：颜色1（色度H：0-360）；
//K：饱和度1 (饱和度S：0-100)；
//LL：颜色2（色度H：0-360）；
//M：饱和度2（饱和度S：0~100）；
//注：有多少个颜色单元就有多少组，最多支持20组；
//每个字母代表一个字节
#define DPID_DREAMLIGHT_SCENE_MODE 51
//麦克风音乐律动(可下发可上报)
//备注:类型：  字符串
//Value：  AABBCCDDEEFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNN
//AA  版本
//BB  0-关闭，1-打开
//CC  模式编号，自定义从201开始
//DD  变换方式：0 - 呼吸模式，1 -跳变模式 ， 2 - 经典模式
//EE  变化速度
//FF  灵敏度
//GGGG  颜色1-色相饱和度
//HHHH  颜色2-色相饱和度
//......
//NNNN  颜色8-色相饱和度
#define DPID_MIC_MUSIC_DATA 52
//炫彩本地音乐律动(可下发可上报)
//备注:专门用于幻彩灯带本地音乐
//Value：ABCDEFGHIJKKLMMN...
//A：版本号；
//B：本地麦克风开关（0-关、1-开）；
//C：音乐模式编号；
//D：变化方式；
//E：变化速度（1-100）;
//F：灵敏度(1-100)；
//GHI：设置项；
//J：亮度（亮度V：0~100）；
//KK：颜色1（色度H：0-360）；
//L：饱和度1 (饱和度S：0-100)；
//MM：颜色2（色度H：0-360）；
//N：饱和度2（饱和度S：0~100）；
//注：有多少个颜色单元就有多少组，最多支持8组；
//每个字母代表一个字节
//#define DPID_DREAMLIGHTMIC_MUSIC_DATA 52

//点数/长度设置(可下发可上报)
//备注:幻彩灯带裁剪之后重新设置长度
#define DPID_LIGHTPIXEL_NUMBER_SET 53

///* Current device DP values */
unsigned char dp_bool_value = 0;
long dp_value_value = 0;
unsigned char dp_enum_value = 0;
unsigned char dp_string_value[21] = {"0"};
uint16_t Hue=0; //HSV
uint8_t Sat=0;
uint8_t Val=0;
uint8_t scene_mode=0;
unsigned char hex[10] = {"0"};
String comdata = "";
//uint8_t Read[14];
int mark = 0;
const unsigned char value;
u16 length;
u8 string_data[60];
//unsigned char dp_raw_value[30] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//int dp_fault_value = 0x01;

/* Stores all DPs and their types. PS: array[][0]:dpid, array[][1]:dp type. 
 *                                     dp type(TuyaDefs.h) : DP_TYPE_RAW, DP_TYPE_BOOL, DP_TYPE_VALUE, DP_TYPE_STRING, DP_TYPE_ENUM, DP_TYPE_BITMAP
*/
unsigned char dp_array[][2] = {
  {DPID_SWITCH_LED, DP_TYPE_BOOL},
  {DPID_WORK_MODE, DP_TYPE_ENUM},
  {DPID_BRIGHT_VALUE, DP_TYPE_VALUE},
  {DPID_TEMP_VALUE, DP_TYPE_VALUE},
  {DPID_COLOUR_DATA, DP_TYPE_STRING},
  {DPID_SCENE_DATA, DP_TYPE_STRING},
  {DPID_COUNTDOWN, DP_TYPE_VALUE},
  {DPID_MUSIC_DATA, DP_TYPE_STRING},
  {DPID_CONTROL_DATA, DP_TYPE_STRING},
  {DPID_SLEEP_MODE, DP_TYPE_RAW},
  {DPID_WAKEUP_MODE, DP_TYPE_RAW},
  {DPID_POWER_MEMORY, DP_TYPE_RAW},
  {DPID_DO_NOT_DISTURB, DP_TYPE_BOOL},
  {DPID_MIC_MUSIC_DATA, DP_TYPE_RAW},
  {DPID_DREAMLIGHT_SCENE_MODE, DP_TYPE_RAW},
  //{DPID_DREAMLIGHTMIC_MUSIC_DATA, DP_TYPE_RAW},
  {DPID_LIGHTPIXEL_NUMBER_SET, DP_TYPE_VALUE},
};

unsigned char pid[] = {"wxi6vc4mwq5kvs3a"};//*********处替换成涂鸦IoT平台自己创建的产品的PIDn3yma7htd6crf3we
unsigned char mcu_ver[] = {"1.0.0"};

unsigned long last_time = 0;
//int sensorValue = 0;
SoftwareSerial DebugSerial(8,9);
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.show();  // Initialize all pixels to 'off'

DebugSerial.begin(9600);

  Serial.begin(9600);
 //Initialize led port, turn off led.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
//Initialize networking keys.
  pinMode(key_pin, INPUT_PULLUP);

 //传入所有DP及其类型数组，DP编号
   //输入PID和MCU软件版本
    my_device.init(pid, mcu_ver);
    my_device.set_dp_cmd_total(dp_array, 16);
    //注册DP下载处理回调函数
    my_device.dp_process_func_register(dp_process);
    //my_device.dp_process_func_register(statestate);
    //注册上传所有DP回调函数
    my_device.dp_update_all_func_register(dp_update_all);

    last_time = millis();
    //void statestate(unsigned char,const unsigned char,u16);
    //void colour_data_control1( const unsigned char, u16);
    //const unsigned char value, u16 length;
     
}

void loop() {
  //按下Pin7时，进入配网模式
  if (digitalRead(key_pin) == LOW) {
    delay(20);
    if (digitalRead(key_pin) == LOW) {
      my_device.mcu_set_wifi_mode(SMART_CONFIG);
    }
  }

my_device.uart_service();

if (cdpid == 51)
{
    if (state==51000)
        {
            DebugSerial.println("51000");
            //colour_data_control1(string_data , length);
            //theaterChaseRainbow(50);
        }
    if (state==51001)
        {
            DebugSerial.println("51001");
            //colour_data_control1(string_data , length);
            //theaterChaseRainbow(50);
        }
    if (state==51002)
        {
            DebugSerial.println("51002");
            //colour_data_control1(string_data , length);
            //theaterChaseRainbow(50);
        }
    if (state==51003)
        {
            DebugSerial.println("51003");
            //colour_data_control1(string_data , length);
            //theaterChaseRainbow(50);
        }
    if (state==51004)
        {
            DebugSerial.println("51004");
            //colour_data_control1(string_data , length);
            //theaterChaseRainbow(50);
        }
    if (state==51005)
        {
            DebugSerial.println("51005");
            //colour_data_control1(string_data , length);
            //theaterChaseRainbow(50);
        }
    if (state==51006)
        {
            DebugSerial.println("51006");
            //colour_data_control1(string_data , length);
            //theaterChaseRainbow(50);
        }
    if (state==51007)
        {
            DebugSerial.println("51007");
            //colour_data_control1(string_data , length);
            //theaterChaseRainbow(50);
        }
    if (state==51008)
        {
            DebugSerial.println("51008");
            //colour_data_control1(string_data , length);
            theaterChaseRainbow(50);
        }
    if (state==51009)
        {
            DebugSerial.println("51009");
            //colour_data_control1(string_data, length);
            //theaterChaseRainbow(50);
        }
    if (state==51200)
        {
            DebugSerial.println("51200");
            //colour_data_control1(string_data , length);
            //theaterChaseRainbow(50);
        }
    if (state==51201)
        {
            DebugSerial.println("51201");
            //colour_data_control1(value, length);
            //theaterChaseRainbow(50);
        }
    if (state==51202)
        {
            DebugSerial.println("51202");
            //colour_data_control1(value, length);
            //theaterChaseRainbow(50);
        }
    if (state==51203)
        {
            DebugSerial.println("51203");
            //colour_data_control1(value, length);
            //theaterChaseRainbow(50);
        }
    if (state==51204)
        {
            DebugSerial.println("51204");
            //colour_data_control1(value, length);
            //theaterChaseRainbow(50);
        }
    if (state==51205)
        {
            DebugSerial.println("51205");
            //colour_data_control1(value, length);
            //theaterChaseRainbow(50);
        }
    if (state==51206)
        {
            DebugSerial.println("51206");
            //colour_data_control1(value, length);
            //theaterChaseRainbow(50);
        }
    if (state==51207)
        {
            DebugSerial.println("51207");
            //colour_data_control1(value, length);
            //theaterChaseRainbow(50);
        }
    if (state==51208)
        {
            DebugSerial.println("51208");
            //colour_data_control1(value, length);
            //theaterChaseRainbow(50);
        }
}

if (cdpid == 52)
{
    //DebugSerial.println("52");
    if (state==520)
    {
        DebugSerial.println("520");
        /* code */
    }
    if (state==521)
    {
        DebugSerial.println("521");
        /* code */
    }
    if (state==522)
    {
        DebugSerial.println("522");
        /* code */
    }
    if (state==523)
    {
        DebugSerial.println("523");
        /* code */
    }
    music(); 
}

  /* 网络连接时LED闪烁 */
  if ((my_device.mcu_get_wifi_work_state() != WIFI_LOW_POWER) && (my_device.mcu_get_wifi_work_state() != WIFI_CONN_CLOUD) && (my_device.mcu_get_wifi_work_state() != WIFI_SATE_UNKNOW)) {
    if (millis()- last_time >= 500) {
      last_time = millis();

      if (led_state == LOW) {
        led_state = HIGH;
      } else {
        led_state = LOW;
      }
      digitalWrite(LED_BUILTIN, led_state);
      //DebugSerial.println("on");
    }
  }

  
  // 获取当前按钮状态
  boolean newState = digitalRead(BUTTON_PIN);

  // 检查状态是否由高变低（按按钮）
  if((newState == LOW) && (oldState == HIGH)) {
    // 短延时去按钮抖动
    delay(10);
    // 检查按钮在去抖动后是否仍然处于低位
    newState = digitalRead(BUTTON_PIN);
    if(newState == LOW) {      // Yes, still low
      if(++mode > 8) mode = 0; // 前进到下一个模式，#8后环绕
      switch(mode) {           // 启动新模式
        case 0:
          colorWipe(strip.Color(  0,   0,   0), 50);    // Black/off
          break;
        case 1:
          colorWipe(strip.Color(255,   0,   0), 50);    // Red
          break;
        case 2:
          colorWipe(strip.Color(  0, 255,   0), 50);    // Green
          break;
        case 3:
          colorWipe(strip.Color(  0,   0, 255), 50);    // Blue
          break;
        case 4:
          theaterChase(strip.Color(127, 127, 127), 50); // White
          break;
        case 5:
          theaterChase(strip.Color(127,   0,   0), 50); // Red
          break;
        case 6:
          theaterChase(strip.Color(  0,   0, 127), 50); // Blue
          break;
        case 7:
          rainbow(10);
          break;
        case 8:
          theaterChaseRainbow(50);
          break;
      }
    }
  }

  // 将上次读取按钮状态设置为旧状态.
  oldState = newState;
}

// 用颜色一个接一个地填充条形像素。. Strip is NOT cleared first;
// 任何一个都会被像素覆盖. 传递颜色
// (作为单个“压缩”32位值, which you can get by calling
// strip.Color(red, green, blue) 如上面的loop（）函数所示),
// 像素之间的延迟时间（毫秒）。
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights.  
// 传入一个颜色（32位值，一个strip.color（r，g，b）如上所述），
// 以及帧之间的延迟时间（ms）。
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// 灯效沿着整个灯带循环。帧之间的传递延迟时间（毫秒）
void rainbow(int wait) {
  // 第一个像素的Hue通过灯带运行3个完整的循环。
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 3*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// 灯效沿着整个灯带循环。帧之间的传递延迟时间（毫秒）
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}


/**
 * @description: DP download callback function.
 * @param {unsigned char} dpid
 * @param {const unsigned char} value
 * @param {unsigned short} length
 * @return {unsigned char}
 */
unsigned char dp_process(unsigned char dpid,const unsigned char value[], unsigned short length)
{

if (dpid == DPID_DREAMLIGHT_SCENE_MODE)
 {
    state =1;
    //DebugSerial.println(state);

 }
else if ( DPID_DREAMLIGHT_SCENE_MODE > dpid > 0)
{
    state =2;
    //DebugSerial.println(state);
}
  switch(dpid) {
    case DPID_SWITCH_LED:
      dp_bool_value = my_device.mcu_get_dp_download_data(dpid, value, length); /* 获取下载DP命令的值 */
      cdpid = DPID_SWITCH_LED;  
      if (dp_bool_value) {
        //Turn on
        colorfill (strip.Color(  255, 255,  255)); //上一次状态
      } else {
        //Turn off
        colorfill (strip.Color(  0, 0,   0));
      }
      //Status changes should be reported.
      my_device.mcu_dp_update(dpid, value, length);
    break;

    case DPID_WORK_MODE:
    //colorfill (strip.Color( 255, 255,  0));
      cdpid = DPID_WORK_MODE;  
      //dp_enum_value  = my_device.mcu_get_dp_download_data(dpid, value, length); /* Get the value of the down DP command 
      switch(dp_enum_value){
        case 0: // white mode
          colorfill (strip.Color(  255, 255,  255));
          //my_device.mcu_dp_update(DPID_WORK_MODE, dp_enum_value, length);
        break;
        case 1: // colour mode
          rainbow(10);
          //my_device.mcu_dp_update(DPID_WORK_MODE, dp_enum_value, length);
        break;
        case 2: // scene mode
          //theaterChaseRainbow(50);
        break;
        case 3: // music mode

        break;
      }
      //Status changes should be reported.
      my_device.mcu_dp_update(DPID_WORK_MODE, value, length);
    break;
    
    case DPID_BRIGHT_VALUE://亮度
      dp_value_value = my_device.mcu_get_dp_download_data(DPID_BRIGHT_VALUE, value, length);
      cdpid = DPID_BRIGHT_VALUE;  
      strip.setBrightness(dp_value_value);
      strip.show(); 
      my_device.mcu_dp_update(DPID_BRIGHT_VALUE, dp_value_value, length);
    break;

    case DPID_TEMP_VALUE://冷暖
        //dp_value_value = my_device.mcu_get_dp_download_data(DPID_TEMP_VALUE, value, length);
        //strip.setBrightness(dp_value_value);
        //strip.show(); 
        my_device.mcu_dp_update(DPID_TEMP_VALUE, value, length);   
        cdpid = DPID_TEMP_VALUE;    
    break;

    case DPID_COLOUR_DATA://彩光
        my_device.mcu_dp_update(DPID_COLOUR_DATA, value, length); 
        cdpid = DPID_COLOUR_DATA;    
    break;

    case DPID_SCENE_DATA://场景
        my_device.mcu_dp_update(DPID_SCENE_DATA, value, length);   
        cdpid = DPID_SCENE_DATA; 
    break;

    case DPID_COUNTDOWN:  //倒计时
      cdpid = DPID_COUNTDOWN;
      dp_value_value = my_device.mcu_get_dp_download_data(dpid, value, length);
//      if (dp_value_value == 0) 
//      {
//        colorWipe(strip.Color(  0,   0,   0), 50);  
//        strip.show(); 
//        my_device.mcu_dp_update(dpid, 0, length);
//      } 
      my_device.mcu_dp_update(DPID_COUNTDOWN, dp_value_value, length);
    break;

    case DPID_MUSIC_DATA: //音乐律动  
      //my_device.mcu_dp_update(dpid, value, length);
      cdpid = DPID_MUSIC_DATA;
      colour_data_control(value, length);
    break;

    case DPID_CONTROL_DATA: //调节
      my_device.mcu_dp_update(dpid, value, length);
      cdpid = dpid;
    break;
    
    case DPID_SLEEP_MODE: //入睡
      my_device.mcu_dp_update(DPID_SLEEP_MODE, value, length);
      cdpid = dpid;
    break;

    case DPID_WAKEUP_MODE: //唤醒
      my_device.mcu_dp_update(DPID_WAKEUP_MODE, value, length);
      cdpid = dpid;
    break;

    case DPID_POWER_MEMORY: //断电记忆
      my_device.mcu_dp_update(DPID_POWER_MEMORY, value, length);
      cdpid = dpid;
    break;

    case DPID_DO_NOT_DISTURB: //勿扰模式
      my_device.mcu_dp_update(DPID_DO_NOT_DISTURB, value, length);
      cdpid = dpid;
    break;

    case DPID_DREAMLIGHT_SCENE_MODE: //炫彩情景
      my_device.mcu_dp_update(DPID_DREAMLIGHT_SCENE_MODE, value, length);
      cdpid = DPID_DREAMLIGHT_SCENE_MODE;
      scene_mode=value[1];
      //DebugSerial.print(dpid);
      //DebugSerial.println(scene_mode);

      switch(scene_mode){
        case 0:
          state=51000;
          colorWipe(strip.Color(255,   0,   0), 50);    // Red   //(  0,   0,   0), 50);    // Black/off 
          //colour_data_control1(value, length);
          break;
        case 1:
          state=51001;
          colorWipe(strip.Color(  0,   0, 255), 50);    // Blue
          //colour_data_control1(value, length);
          break;
        case 2:
          state=51002;
          //colorWipe(strip.Color(  255, 165,   0), 50);  //Orange //(  0, 255,   0), 50);    // Green
          colour_data_control1(value, length);
          break;
        case 3:
          state=51003;
          //colorWipe(strip.Color(  0,  255, 255), 50);    //
          colour_data_control1(value, length);
          break;
        case 4:
          state=51004;
          //theaterChase(strip.Color(127, 127, 127), 50); // White
          colour_data_control1(value, length);
          break;
        case 5:
          state=51005;
          //theaterChase(strip.Color(127,   0,   0), 50); // Red
          colour_data_control1(value, length);
          break;
        case 6:
          state=51006;
          //theaterChase(strip.Color(  0,   0, 127), 50); // Blue
          colour_data_control1(value, length);
          break;
        case 7:
          state=51007;
          //rainbow(10);
          colour_data_control1(value, length);
          break;
        case 8:
          state=51008;
          theaterChaseRainbow(50);
          //colour_data_control1(value, length);
          break;
        case 9:
          state=51009;
          //theaterChaseRainbow(50);
          colour_data_control1(value, length);
          break;
        case 200://c8
          state=51200;
          colour_data_control1(value, length);
          break;
        case 201://c9
          state=51201;
          colour_data_control1(value, length);
          break;
        case 202://ca
          state=51202;
          colour_data_control1(value, length);
          break;
        case 203://cb
          state=51203;
          colour_data_control1(value, length);
          break;
        case 204://cd
          state=51204;
          colour_data_control1(value, length);
          break;
        case 205://ce
          state=51205;
          colour_data_control1(value, length);
          break;
        case 206://cf
          state=51206;
          colour_data_control1(value, length);
          break;
        //default:break;  
      }
      DebugSerial.println("break");
      break;
    case DPID_MIC_MUSIC_DATA: //mic音乐律动
      my_device.mcu_dp_update(DPID_MIC_MUSIC_DATA, value, length);
       //music(); 
      cdpid=DPID_MIC_MUSIC_DATA;
      scene_mode=value[2];
      switch(scene_mode){
        case 0:
          //colour_data_control(value, length);
          state=520;
          break;
        case 1:
          //colour_data_control(value, length);
          state=521;
          break;
        case 2:
          //colour_data_control(value, length);
          state=522;
          break;
        case 3:
          //colour_data_control(value, length);
          state=523;
          break;
       }
      break;    

    case DPID_LIGHTPIXEL_NUMBER_SET: //长度设置
      my_device.mcu_dp_update(DPID_LIGHTPIXEL_NUMBER_SET, value, length);
      cdpid=DPID_LIGHTPIXEL_NUMBER_SET;
      break;
    default:break;
  }
  return SUCCESS;
}

void music(void) 
{
      //DebugSerial.println(analogRead(A0));
       int hh = 0;
       int ss = 0; 
       int vv = 0; 
       if (analogRead(A0)>15)
       {
        hh = (analogRead(A0)-15)*2184;
        ss = 255;
        vv = 255;
       }
       else
       {
        hh = 0;
        ss = 0;
        vv = 0;
       }
      strip.fill(strip.gamma32(strip.ColorHSV(hh,ss,vv)),0,PIXEL_COUNT);
        //for(int i=0; i<strip.numPixels(); i++)
        //{
        //uint32_t color = strip.gamma32(strip.ColorHSV(analogRead(A0)*127)); // hue -> RGB
        //strip.setPixelColor(i, strip.gamma32(strip.ColorHSV((analogRead(A0)-15)*127))); // Set pixel 'c' to value 'color'
        //}
      strip.show();
      delay(200); 
}


/**
 * @description: Upload all DP status of the current device.
 * @param {*}
 * @return {*}
 */
void dp_update_all(void)
{
  my_device.mcu_dp_update(DPID_SWITCH_LED, led_state, 1);
  //my_device.mcu_dp_update(DPID_BRIGHT_VALUE,dp_value_value,1);
/*  my_device.mcu_dp_update(DPID_WORK_MODE, DP_TYPE_ENUM},
  {DPID_TEMP_VALUE, DP_TYPE_VALUE},
  {DPID_COLOUR_DATA, DP_TYPE_STRING},
  {DPID_SCENE_DATA, DP_TYPE_STRING},
  {DPID_COUNTDOWN, DP_TYPE_VALUE},
  {DPID_MUSIC_DATA, DP_TYPE_STRING},
  {DPID_CONTROL_DATA, DP_TYPE_STRING},
  {DPID_SLEEP_MODE, DP_TYPE_RAW},
  {DPID_WAKEUP_MODE, DP_TYPE_RAW},
  {DPID_POWER_MEMORY, DP_TYPE_RAW},
  {DPID_DO_NOT_DISTURB, DP_TYPE_BOOL},
  {DPID_MIC_MUSIC_DATA, DP_TYPE_RAW},
  {DPID_DREAMLIGHT_SCENE_MODE, DP_TYPE_RAW},
  {DPID_LIGHTPIXEL_NUMBER_SET, DP_TYPE_VALUE},
*/
}






//拓展
void colorfill(uint32_t color) {
 strip.fill(color,0,PIXEL_COUNT);
    strip.show();                          //  Update strip to match   
  
}

void colour_data_control1( const unsigned char value[], u16 length)
 {
    //DebugSerial.println(value[]);
    u8 string_data[60];
    //u16 h, s, v;
    //u8 r, g, b;
    u16 hue;
    u8 sat,val;
    u32 c=0;
    
  for (uint8_t i = 0; i < length; i++)
  {
    string_data[i] = value[i]; //版本
    //string_data[1] = value[1]; //mode
    //string_data[2] = value[2]; //变化
    //string_data[3] = value[3]; //切换时间
    //string_data[4] = value[4]; //变化时间
    //string_data[5] = value[5]; //颜色过度
    //string_data[6] = value[6];
    //string_data[7] = value[7];
    //string_data[8] = value[8]; //亮度
    //string_data[9] = value[9]; //颜色1
    //string_data[10] = value[10]; //颜色2
    //string_data[11] = value[11]; //饱和度
  }
  
    int color_val_len,color_time;
    val=value[8]*2.55;
    color_val_len = (length -9)/3;
    color_time = value[4];
    //mode = value[2];
    DebugSerial.println(color_time);

    for (uint8_t i = 0; i < color_val_len; i++)
    {
        hue = value[9 + i*3];
        hue <<= 8;
        hue += value[10 + i*3];
        sat = value[11 + i*3] * 2.55;
        c = strip.gamma32(strip.ColorHSV(hue*182,sat,val)); // hue -> RGB
    //DebugSerial.println(hue);
    //DebugSerial.println(sat);
    //DebugSerial.println(val);
    strip.fill(c,i*(PIXEL_COUNT/color_val_len),PIXEL_COUNT);
    //strip.fill(c,i*(PIXEL_COUNT/color_val_len),(i*(PIXEL_COUNT/color_val_len)+(PIXEL_COUNT/color_val_len)-1));

    //for(int n=(i*(PIXEL_COUNT/color_val_len)); n<(i*(PIXEL_COUNT/color_val_len)+(PIXEL_COUNT/color_val_len)); n++) 
    //{ // For each pixel in strip...
    //  strip.setPixelColor(n, c);         //  Set pixel's color (in RAM)
    //  strip.show();                          //  Update strip to match
      //delay(wait);                           //  Pause for a moment
    //  delay(color_time*10);
    //}

    //strip.show(); 
    //delay(color_time*5);   
    }
    strip.show(); 
 }

 void colour_data_control2( const unsigned char string_data[], u16 length)
 {
    u16 hue;
    u8 sat,val;
    u32 c=0;
    int color_val_len,color_time;
    val=string_data[8]*2.55;
    color_val_len = (length -9)/3;
    color_time = string_data[4];
    //mode = value[2];
    DebugSerial.println(color_time);

    for (uint8_t i = 0; i < color_val_len; i++)
    {
        hue = string_data[9 + i*3];
        hue <<= 8;
        hue += string_data[10 + i*3];
        sat = string_data[11 + i*3] * 2.55;
        c = strip.gamma32(strip.ColorHSV(hue*182,sat,val)); // hue -> RGB
    strip.fill(c,i,PIXEL_COUNT);
    strip.show(); 
    delay(color_time*5);   
    }
 }

 void colour_data_control( const unsigned char value[], u16 length)
 {
   
   u8 string_data[13];
    u16 h, s, v;
    u8 r, g, b;
    u16 hue;
    u8 sat,val;

    u32 c=0;
  
    string_data[0] = value[0]; //渐变、直接输出
    string_data[1] = value[1];
    string_data[2] = value[2];
    string_data[3] = value[3];
    string_data[4] = value[4];
    string_data[5] = value[5];
    string_data[6] = value[6];
    string_data[7] = value[7];
    string_data[8] = value[8];
    string_data[9] = value[9];
    string_data[10] = value[10];
    string_data[11] = value[11];
    string_data[12] = value[12];
  
    h = __str2short(__asc2hex(string_data[1]), __asc2hex(string_data[2]), __asc2hex(string_data[3]), __asc2hex(string_data[4]));
    s = __str2short(__asc2hex(string_data[5]), __asc2hex(string_data[6]), __asc2hex(string_data[7]), __asc2hex(string_data[8]));
    v = __str2short(__asc2hex(string_data[9]), __asc2hex(string_data[10]), __asc2hex(string_data[11]), __asc2hex(string_data[12]));

    // if (v <= 10) {
    //     v = 0;
    // } else {
    //     v = color_val_lmt_get(v);
    // }
    
    //hsv2rgb((float)h, (float)s / 1000.0, (float)v / 1000.0, &r , &g, &b);

    // c= r<<16|g<<8|b;
  hue=h*182;
  sat=s/4;
  val=v/4;
    c = strip.gamma32(strip.ColorHSV(hue,sat,val)); // hue -> RGB
    //DebugSerial.println(hue);
    //DebugSerial.println(sat);
    //DebugSerial.println(val);
 
    
    strip.fill(c,0,PIXEL_COUNT);
    
    strip.show(); // Update strip with new contents

    //tuya_light_gamma_adjust(r, g, b, &mcu_default_color.red, &mcu_default_color.green, &mcu_default_color.blue);
  
    //printf("r=%d,g=%d,b=%d\r\n", mcu_default_color.red, mcu_default_color.green, mcu_default_color.blue);
    //rgb_init(mcu_default_color.red, mcu_default_color.green, mcu_default_color.blue);
 }

/**
 * @brief  str to short
 * @param[in] {a} Single Point
 * @param[in] {b} Single Point
 * @param[in] {c} Single Point
 * @param[in] {d} Single Point
 * @return Integrated value
 * @note   Null
 */
u32 __str2short(u32 a, u32 b, u32 c, u32 d)
{
    return (a << 12) | (b << 8) | (c << 4) | (d & 0xf);
}

/**
  * @brief ASCALL to Hex
  * @param[in] {asccode} 当前ASCALL值
  * @return Corresponding value
  * @retval None
  */
u8 __asc2hex(u8 asccode)
{
    u8 ret;
    
    if ('0' <= asccode && asccode <= '9')
        ret = asccode - '0';
    else if ('a' <= asccode && asccode <= 'f')
        ret = asccode - 'a' + 10;
    else if ('A' <= asccode && asccode <= 'F')
        ret = asccode - 'A' + 10;
    else
        ret = 0;
    
    return ret;
}

/**
  * @brief Normalized
  * @param[in] {dp_val} dp value
  * @return result
  * @retval None
  */
u16 color_val_lmt_get(u16 dp_val)
{
    u16 max = 255 * 100 / 100;
    u16 min = 255 * 1 / 100;
    
    return ((dp_val - 10) * (max - min) / (1000 - 10) + min);
}

/**
  * @brief hsv to rgb
  * @param[in] {h} tone
  * @param[in] {s} saturation
  * @param[in] {v} Lightness
  * @param[out] {color_r} red
  * @param[out] {color_g} green
  * @param[out] {color_b} blue
  * @retval None
  */
void hsv2rgb(float h, float s, float v, u8 *color_r, u8 *color_g, u8 *color_b)
{
    float h60, f;
    u32 h60f, hi;
  
    h60 = h / 60.0;
    h60f = h / 60;
  
    hi = ( signed int)h60f % 6;
    f = h60 - h60f;
  
    float p, q, t;
  
    p = v * (1 - s);
    q = v * (1 - f * s);
    t = v * (1 - (1 - f) * s);
  
    float r, g, b;
  
    r = g = b = 0;
    if (hi == 0) {
        r = v;          g = t;        b = p;
    } else if (hi == 1) {
        r = q;          g = v;        b = p;
    } else if (hi == 2) {
        r = p;          g = v;        b = t;
    } else if (hi == 3) {
        r = p;          g = q;        b = v;
    } else if (hi == 4) {
        r = t;          g = p;        b = v;
    } else if (hi == 5) {
        r = v;          g = p;        b = q;
    }
  
    DebugSerial.println(r);
    DebugSerial.println(g);
    DebugSerial.println(b);
    r = (r * (float)255);
    g = (g * (float)255);
    b = (b * (float)255);
  
    *color_r = r;
    *color_g = g;
    *color_b = b;
    
    // r *= 100;
    // g *= 100;
    // b *= 100;
  
    // *color_r = (r + 50) / 100;
    // *color_g = (g + 50) / 100;
    // *color_b = (b + 50) / 100;
}


void statestate(unsigned char dpid,const unsigned char value[],u16 length)
{
    //my_device.uart_service();
  if (dpid == DPID_DREAMLIGHT_SCENE_MODE)
   {
      state =1;
      DebugSerial.println(state);
   }
  else if ( DPID_DREAMLIGHT_SCENE_MODE > dpid > 0)
  {
      state =2;
      DebugSerial.println(state);
  }
}

void judgeSerial(void) 
{
    comdata = String("");
    int j=0;
    mark = 0;
   while (Serial.available() > 0)
    {
        comdata += char(Serial.read());
        delay(2);
        mark = 1;
    }
    if(mark == 1)
    {
     //DebugSerial.print(comdata);
    }
}