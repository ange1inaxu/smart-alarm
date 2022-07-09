#include <string.h>
#include <math.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <WiFi.h> //Connect to WiFi Network
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <mpu6050_esp32.h>
#include <ArduinoJson.h>
#include <DFRobotDFPlayerMini.h>
#include <Keypad.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
MPU6050 imu; //imu object called, appropriately, imu

const uint32_t TWO_MIN = 120000;
uint32_t game_timer;

// response/request variables
const uint16_t RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 3500; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 3500; //size of buffer to hold HTTP response
const uint16_t JSON_BODY_SIZE = 3000;
char request[IN_BUFFER_SIZE];
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
//char json_body[JSON_BODY_SIZE];

// location, distance, and time variables
// char location_request[IN_BUFFER_SIZE];
// char location_response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char location_json_body[JSON_BODY_SIZE];
char locations[20][50] = {
    "Student Center",
    "Dorm Row",
    "East Campus"
  };
uint8_t location_length = 3;

char start_loc[50] = "";
char end_loc[50] = "";

// char request_buffer[IN_BUFFER_SIZE];
// char response_buffer[OUT_BUFFER_SIZE];

char distance_request[IN_BUFFER_SIZE];
char distance_response[OUT_BUFFER_SIZE];

uint16_t transport_mode;
float distance;
uint16_t travel_time;
char goal_time[10] = "";
uint16_t ready_duration;

// char calendar_request[IN_BUFFER_SIZE];
// char calendar_response[OUT_BUFFER_SIZE];

float distance_calendar;
uint32_t travel_time_calendar;
char goal_time_calendar[50] = "";

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {13, 12, 11, 10}; 
byte colPins[COLS] = {7, 6, 5, 4}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//Some constants and some resources:
const uint16_t IN_BUFFER_SIZE_K = 2000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE_K = 2000; //size of buffer to hold HTTP response
char request_buffer_key[IN_BUFFER_SIZE_K]; //char array buffer to hold HTTP request
char response_buffer_key[OUT_BUFFER_SIZE_K]; //char array buffer to hold HTTP response

int loggedIn = 0;

uint8_t key_state = 1;
uint8_t last_state = 0;

// States for the FSM
const uint8_t USERNAME = 1; 
const uint8_t PASSWORD = 2;
const uint8_t LOGIN = 3;
const uint8_t LOGIN_RESPONSE = 4;

const int USERNAME_SIZE = 20;
const int PASSWORD_SIZE = 10;

char username[USERNAME_SIZE];
char login_password[PASSWORD_SIZE];
char input_display[USERNAME_SIZE];
const char ENTER = '*';

// voice variables
const uint16_t DELAY = 1000;
const uint16_t SAMPLE_FREQ = 8000;                          // Hz, telephone sample rate
const uint16_t SAMPLE_DURATION = 5;                        // duration of fixed sampling (seconds)
const uint16_t NUM_SAMPLES = SAMPLE_FREQ * SAMPLE_DURATION;  // number of of samples
const uint16_t ENC_LEN = (NUM_SAMPLES + 2 - ((NUM_SAMPLES + 2) % 3)) / 3 * 4;  // Encoded length of clip

char speech_data[ENC_LEN + 200] = {0}; // global used for collecting speech data
char transcript[100] = {0};
bool time_set = false;
uint16_t DISPLAY_TIME = 2000;
uint8_t state = 0;

const uint8_t PIN = 34;
uint8_t button_state_voice = 0; //used for containing button state and detecting edges
uint8_t old_button_state = 0; //used for detecting button edges

const uint16_t VOICE_RESPONSE_TIMEOUT = 6000;
const uint16_t VOICE_OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
// char voice_response[VOICE_OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request

//Prefix to POST request:
const char VOICE_PREFIX[] = "{\"config\":{\"encoding\":\"MULAW\",\"sampleRateHertz\":8000,\"languageCode\": \"en-US\",\"speechContexts\": [{\"phrases\": [\"one\", \"two\", \"turn off\", \"Next House\", \"Student Center\"], \"boost\": 2}]}, \"audio\": {\"content\":\"";
// const char[] VOICE_PREFIX = "{\"config\":{\"encoding\":\"MULAW\",\"sampleRate\":8000},\"audio\": {\"content\":\"";
const char VOICE_SUFFIX[] = "\"}}"; //suffix to POST request
const uint8_t AUDIO_IN = 1; //pin where microphone is connected
// const char API_KEY_VOICE[] = "AIzaSyAQ9SzqkHhV-Gjv-71LohsypXUH447GWX8"; //don't change this

const char* CA_CERT = \
                      "-----BEGIN CERTIFICATE-----\n" \
                      "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\n" \
                      "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\n" \
                      "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\n" \
                      "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\n" \
                      "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\n" \
                      "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\n" \
                      "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\n" \
                      "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\n" \
                      "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\n" \
                      "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\n" \
                      "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\n" \
                      "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\n" \
                      "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\n" \
                      "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\n" \
                      "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\n" \
                      "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\n" \
                      "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\n" \
                      "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\n" \
                      "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\n" \
                      "-----END CERTIFICATE-----\n";

// weekly recap variables
int date = 5092022;
int dates[10];
int scores[10];
// char USER[50] = "team16";
bool first_get_made = false;

// state machine for whether the LCD Display is ON or OFF
uint8_t ON = 0;
uint8_t OFF = 1;
uint8_t on_state = ON;
uint8_t sleep_time;
const uint16_t BLACKOUT_TIME = 10000;

// variables for detecting motion
float imu_x, imu_y, imu_z;
float acceleration;
const float ZOOM = 9.81; //for display (converts readings into m/s^2)...used for visualizing only

/* CONSTANTS */
//Prefix to POST request:
const char PREFIX[] = "{\"wifiAccessPoints\": ["; //beginning of json body
const char SUFFIX[] = "]}"; //suffix to POST request
const char API_KEY[] = "AIzaSyAQ9SzqkHhV-Gjv-71LohsypXUH447GWX8"; //don't change this and don't share this

const uint8_t MAX_APS = 5;

char* SERVER = "googleapis.com";  // Server URL
const char* VOICE_SERVER = "speech.google.com";  //Server URL

uint32_t timer;

const uint16_t TIME_IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t TIME_OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char time_request_buffer[TIME_IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char time_response_buffer[TIME_OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response
char got_response_buffer[TIME_OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

const uint8_t GET = 0;
const uint8_t T_HOLD = 1;

// uint8_t scanning = 1;
uint8_t channel = 11; //network channel on 2.4 GHz
// byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.
// byte bssid[] = {0xD4, 0x20, 0xB0, 0xC4, 0xB5, 0x34};
//D4:20:B0:CC:B2:13
//D4:20:B0:C4:B5:33
/*char network[] = "EECS_Labs";
char password[] = "";*/

/*char network[] = "608_24G";
char password[] = "608g2020";*/

// char NETWORK[] = "MIT GUEST";
char NETWORK[] = "MIT";
char N_PASSWORD[] = "";

uint8_t time_state;  //used for remembering state
uint8_t previous_value;  //used for remembering previous button
uint32_t last_time; //used for timing
//uint32_t last_time_gotten; 
uint8_t getting_state; //used for remembering whether to get the time or calculate it
uint8_t onOff_state;

char alarm_time[15] = "";
uint8_t on_off;
uint16_t game_result;

uint8_t mode_state;
const uint8_t SET_MODE = 0;
const uint8_t PHOTO_MODE = 1;
const uint8_t LOCATION_MODE = 2;
const uint8_t CALENDAR_MODE = 3;
const uint8_t VOICE_MODE = 4;
const uint8_t RECAP_MODE = 5;
const uint8_t ALARM_MODE = 6;
uint8_t gotten_mode_state = 0;

HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
uint8_t alarm_sound = 2;
uint8_t played = 0;

/*const float ZOOM = 9.81; //for display (converts readings into m/s^2)...used for visualizing only
float acc_mag = 0;
float x, y, z;*/

uint8_t game_state;
const uint8_t TRIVIA_IDLE = 0;  //change if you'd like
const uint8_t Q_DISPLAY = 1;  //change if you'd like
const uint8_t USER_TRUE = 2;  //change if you'd like
const uint8_t USER_FALSE = 3;  //change if you'd like
const uint8_t GAME_OVER = 4;  //change if you'd like

const uint8_t BUTTON1 = 45;
const uint8_t BUTTON2 = 39;
const uint8_t BUTTON3 = 38;
const uint8_t BUTTON4 = 34;

const uint8_t SCREEN_HEIGHT = 160;
const uint8_t SCREEN_WIDTH = 128;
const uint8_t LOOP_PERIOD = 40;

float a, x, y;
uint16_t char_index = 0;
char output[400] = {0}; 
char numbers[20] = "0123456789";
char sound_numbers[20] = "123";
char mode_numbers[20] = "012345";
uint32_t scroll_timer;
const uint8_t scroll_threshold = 150;
const float angle_threshold = 0.4;

char question[1000];
char answer[10];

int score;
uint8_t correct_num;
uint8_t incorrect_num;

char TRUE[5] = "True";
char FALSE[6] = "False";

// photoresistor
int loop_timer;                 //used for loop timing
const int pin = 3;              //pin we use for analog inputs
int bright_inc = 0;             //auto-increasing value that goes from 0 to 4095 (approx) in steps of 100
int resolution = 12;
float photoresistor_value = 4096.0;
uint32_t photoresistor_timer;

/* Global variables*/
uint32_t time_since_sample;      // used for microsecond timing

WiFiClientSecure client; //global WiFiClient Secure object
WiFiClient client2; //global WiFiClient Secure object

const char USER[] = "ahan";

enum button_state {S0,S1,S2,S3,S4};

class Button{
  public:
  uint32_t S2_start_time;
  uint32_t button_change_time;    
  uint32_t debounce_duration;
  uint32_t long_press_duration;
  uint8_t pin;
  uint8_t flag;
  uint8_t button_pressed;
  button_state state; // This is public for the sake of convenience
  Button(int p) {
  flag = 0;  
    state = S0;
    pin = p;
    S2_start_time = millis(); //init
    button_change_time = millis(); //init
    debounce_duration = 10;
    long_press_duration = 1000;
    button_pressed = 0;
  }
  void read() {
    uint8_t button_val = digitalRead(pin);  
    button_pressed = !button_val; //invert button
  }
  int update() {
    read();
    flag = 0;
    if (state==S0) {
      if (button_pressed) {
        state = S1;
        button_change_time = millis();
      }
    } else if (state==S1) {
      // CODE HERE
      if (button_pressed && millis() - button_change_time >= debounce_duration) {
        state = S2;
        S2_start_time = millis();
      } else if (!button_pressed) {
        state = S0;
        button_change_time = millis();
      }
    } else if (state==S2) {
      // CODE HERE
      if (millis() - S2_start_time >= long_press_duration) {
        state = S3;
      } else if (!button_pressed) {
        state = S4;
        button_change_time = millis();
      }
    } else if (state==S3) {
      // CODE HERE
      if (!button_pressed) {
        state = S4;
        button_change_time = millis();
      }
    } else if (state==S4) {      	
      // CODE HERE
      if (button_pressed && millis() - S2_start_time < long_press_duration) {
        state = S2;
        button_change_time = millis();
      } else if (button_pressed && millis() - S2_start_time >= long_press_duration) {
        state = S3;
        button_change_time = millis();
      } else if (!button_pressed && millis() - button_change_time >= debounce_duration) {
        state = S0;
        if (millis() - S2_start_time < long_press_duration) {
          flag = 1;
        } else {
          flag = 2;
        }
        //button_change_time = millis();
      }      
    }
    return flag;
  }
};

Button button1(BUTTON1);
Button button2(BUTTON2);
Button button3(BUTTON3);
Button button4(BUTTON4);

//char*  SERVER = "googleapis.com";  // Server URL

void setup() {
  mySoftwareSerial.begin(9600, SERIAL_8N1, 18, 17);  // speed, type, RX, TX
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setCursor(0,0);
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  Serial.begin(115200); //begin serial comms
  
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
      uint8_t* cc = WiFi.BSSID(i);
      for (int k = 0; k < 6; k++) {
        Serial.print(*cc, HEX);
        if (k != 5) Serial.print(":");
        cc++;
      }
      Serial.println("");
    }
  }
  delay(100); //wait a bit (100 ms)

  //if using regular connection use line below:
  WiFi.begin(NETWORK, N_PASSWORD);
  //if using channel/mac specification for crowded bands use the following:
  // WiFi.begin(NETWORK, PASSWORD, channel, bssid);

  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(NETWORK);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }

  // imu set up  
  Wire.begin();
  delay(500); //pause to make sure comms get set up
  if (imu.setupIMU(1)) {
    Serial.println("IMU Connected!");
  } else {
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }

  // df player setup
  delay(1000);
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(myDFPlayer.readType(),HEX);
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  
  //----Set volume----
  myDFPlayer.volume(10);

  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  int delayms=100;

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);

  time_state = 0; //initialize to 0
  previous_value = 1; //initialize to 1
  getting_state = GET;
  onOff_state = 0;

  analogReadResolution(resolution);       // initialize the analog resolution for photoresistor

  game_state = TRIVIA_IDLE;
  mode_state = SET_MODE;
  score = 0;
  correct_num = 0;
  incorrect_num = 0;
  game_result = 0;
  on_off = 0;
  timer = millis();
  photoresistor_timer = millis();
  loggedIn = 0;
  
  sleep_time = millis();

  client.setCACert(CA_CERT); //set cert for https
  time_set = false;
}

void loop(){
  // Determine if screen should be lit or not
  imu.readAccelData(imu.accelCount);
  imu_x = ZOOM * imu.accelCount[0] * imu.aRes;
  imu_y = ZOOM * imu.accelCount[1] * imu.aRes;
  imu_z = ZOOM * imu.accelCount[2] * imu.aRes;
  acceleration = sqrt(imu_x*imu_x + imu_y*imu_y + imu_z*imu_z);

  // if (on_state == OFF){
  //   if (acceleration > 11){
  //     on_state = ON;
  //     sleep_time = millis();
  //   }
  // } else if (on_state == ON){
  //   if (millis() - sleep_time >= BLACKOUT_TIME && acceleration <= 11){
  //     on_state = OFF;
  //     tft.fillScreen(TFT_BLACK); //black out TFT Screen
  //   } else if (acceleration > 11){
  //     sleep_time = millis();
  //   }
  // }
  
  int bv4 = button4.update();
  int bv1 = button1.update();
  if (loggedIn) { 
    if (gotten_mode_state == 0) {
      handle_modes(bv4);
      if (bv4 == 2) {
        gotten_mode_state = 1;
        tft.fillScreen(TFT_BLACK);
        game_state = TRIVIA_IDLE;
        if (mode_state != RECAP_MODE) {
          alarm_sound = set_sound(y);
        }
        tft.fillScreen(TFT_BLACK);
      }
    } else {
      if (mode_state == SET_MODE) {
        set_alarm();
        mode_state = ALARM_MODE;
        game_state = TRIVIA_IDLE;
        //Serial.println(alarm_time);

      } else if (mode_state == PHOTO_MODE) {
        photoresistorAlarm();
        //Serial.println("photo");

      } else if (mode_state == LOCATION_MODE) {
        Serial.println("location");
        getDistanceTime(y);
        game_state = TRIVIA_IDLE;
        mode_state = ALARM_MODE;

      } else if (mode_state == CALENDAR_MODE) {
        Serial.println("calendar");
        getDistanceTimeCalendar(y);
        game_state = TRIVIA_IDLE;
        mode_state = ALARM_MODE;

      } else if (mode_state == VOICE_MODE) {
        // Serial.println("voice");
        if (!time_set) {
          set_voice_time();
        }
        else {
          game_state = TRIVIA_IDLE;
          mode_state = ALARM_MODE;
          tft.fillScreen(TFT_BLACK);
          Serial.println("voice to alarm");
        }
        // mode_state = ALARM_MODE;
      } else if (mode_state == RECAP_MODE) {
        display_recap();
        bv4 = button4.update();
        if (bv4 == 1) {
          gotten_mode_state = 0;
          tft.fillScreen(TFT_BLACK);
          Serial.println("bv4 update recap to choose modes");
        }
      } else if (mode_state == ALARM_MODE) {
        tell_time();
      }
    }
  } else {
    handle_login();    
  }
  
}

void getTime() {
  char host[] = "iesc-s3.mit.edu";
  memset(time_request_buffer, 0, TIME_IN_BUFFER_SIZE);
  memset(time_response_buffer, 0, TIME_OUT_BUFFER_SIZE);
  sprintf(time_request_buffer,"GET http://iesc-s3.mit.edu/esp32test/currenttime HTTP/1.1\r\n");
  strcat(time_request_buffer,"Host: iesc-s3.mit.edu\r\n"); //add more to the end
  strcat(time_request_buffer,"\r\n"); //add blank line!
  do_http_request(host, time_request_buffer, time_response_buffer, TIME_OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, 0);
}

void formatVoiceTime(char* voiceTime, char* americanTime) {
  // example: 10:30 p.m. (strlen = 10)
  // set AM/PM
  char AMorPM_voice[4] = "";
  slice(voiceTime, AMorPM_voice, strlen(voiceTime)-4, strlen(voiceTime)); // a.m. or p.m.

  char AMorPM[4] = "";
  if (!strcmp(AMorPM_voice, "a.m.")) {
    strcpy(AMorPM, " AM");
  } else if (!strcmp(AMorPM_voice, "p.m.")) {
    strcpy(AMorPM, " PM");
  }

  // set minutes
  char min_string[3] = "";
  if (strlen(voiceTime) == 9) { // 9:30 a.m.
    slice(voiceTime, min_string, 2, 4);
  } else if (strlen(voiceTime) == 10) { // 10:30 a.m.
    slice(voiceTime, min_string, 3, 5);
  }
  // slice(voiceTime, min_string, strlen(voiceTime)-7, strlen(voiceTime)-5); // 10:30 a.m.
  // Serial.println(min_string);

  // set hours
  char hr_colon_string[4] = "";
  if (strlen(voiceTime) == 9) { // 9:30 a.m.
    slice(voiceTime, hr_colon_string, 0, 2);
  } else if (strlen(voiceTime) == 10) { // 10:30 a.m.
    slice(voiceTime, hr_colon_string, 0, 3);
  }

  // combine strings
  memset(americanTime, 0, 15);
  strcpy(americanTime, hr_colon_string);
  strcat(americanTime, min_string);
  strcat(americanTime, AMorPM);
  Serial.println(americanTime);
}

void formatTime(char* militaryTime, char* americanTime) {
  char hourString[3] = "";
  slice(militaryTime, hourString, 0, 2); 

  int hour = atoi(hourString);
  char AMorPM[] = " AM";

  if(hour > 12){
    hour -= 12;
    strcpy(AMorPM, " PM");
  } else if(hour == 12) {
    strcpy(AMorPM, " PM");
  } else if (hour == 0) {
    hour += 12;    
  }
  sprintf(americanTime,"%d", hour); 

  char restOfTime[20] = "";
  slice(militaryTime, restOfTime, 2, strlen(militaryTime));
  strcat(americanTime, restOfTime);
  strcat(americanTime, AMorPM);
}

void removeColon(char* time, char* noColon) {
  char hourString[3] = "";
  slice(time, hourString, 0, 2); 

  char restOfTime[20] = "";
  slice(time, restOfTime, 3, strlen(time));

  strcpy(noColon, hourString); 
  strcat(noColon, " ");
  strcat(noColon, restOfTime);
}

void handle_modes(int bv4) {
  if (on_state == ON){
    if (bv4 == 1) {
      mode_state += 1;
      mode_state %= 6;
      sleep_time = millis();
    }
    tft.setTextSize(1);
    tft.setCursor(0, 0, 1);
    tft.println("                       ");
    tft.setCursor(5, 10, 1);
    tft.println("Please set the mode:  ");
    tft.println(" Mode 1: User input");
    tft.println(" Mode 2: Rise&Shine");
    tft.println(" Mode 3: Location   ");
    tft.println(" Mode 4: Calendar   ");
    tft.println(" Mode 5: Voice  ");
    tft.println(" Mode 6: Weekly recap");
    tft.println("                     ");
    
    tft.setCursor(5, 100, 1);
    if (mode_state >= 6) {
      mode_state = 0;
    }
    tft.printf("Current mode: %d\n", mode_state+1);
  }
}

void set_alarm() {
  if (on_state == ON){
    char mil_alarm[20] = "";
    set_time(y, mil_alarm);
    formatTime(mil_alarm, alarm_time);
    //Serial.println(mil_alarm);
    /*if (alarm_time[0] == '0') {
      slice(alarm_time, alarm_time, 1, strlen(alarm_time));
    }*/
    Serial.println(alarm_time);
    tft.fillScreen(TFT_BLACK);
  }
}


uint8_t value = digitalRead(BUTTON1);
uint8_t value2 = digitalRead(BUTTON2);
void tell_time() {
  value = digitalRead(BUTTON1);
  value2 = digitalRead(BUTTON2);
  previous_value = value; //remember for next time

  if (getting_state == GET) {
    getTime();
    memset(got_response_buffer, 0, TIME_OUT_BUFFER_SIZE);
    strcpy(got_response_buffer, time_response_buffer);
    last_time = millis();
    getting_state = T_HOLD;
  } else if (getting_state == T_HOLD) {
    char newTime[TIME_OUT_BUFFER_SIZE] = "";
    slice(got_response_buffer, newTime, 0, 11); 
    char hourString[3] = "";
    slice(got_response_buffer, hourString, 11, 13); 
    int hour = atoi(hourString);

    char minString[3] = "";
    slice(got_response_buffer, minString, 14, 16); 
    int min = atoi(minString);

    char secString[3] = "";
    slice(got_response_buffer, secString, 17, 19); 
    int sec = atoi(secString);

    char milsecString[8] = "";
    slice(got_response_buffer, milsecString, 20, 26); 
    int milsec = atoi(milsecString);
    
    uint32_t difference = millis() - last_time;
    milsec += difference*1000;
    sec += milsec/1000000;
    milsec = milsec%1000000;
    
    if (sec >= 60) {
      sec -= 60;
      min += 1;
      if (min >= 60) {
        min -= 60;
        hour += 1;
        if (hour == 25) {
          hour = 1;
        }
      }
    }
    
    char noDate[TIME_OUT_BUFFER_SIZE] = "";
    if (hour < 10) {
      sprintf(noDate,"0%d:", hour); 
    } else {
      sprintf(noDate,"%d:", hour);
    }

    char temp[TIME_OUT_BUFFER_SIZE] = "";
    if (min < 10) {
      sprintf(temp,"0%d:", min); 
      strcat(noDate, temp);
    } else {
      sprintf(temp,"%d:", min);
      strcat(noDate, temp);
    }

    if (sec < 10) {
      sprintf(temp,"0%d:", sec); 
      strcat(noDate, temp);
    } else {
      sprintf(temp,"%d:", sec);
      strcat(noDate, temp);
    }

    if (milsec < 100000) {
      sprintf(temp,"0%d:", milsec); 
      strcat(noDate, temp);
    } else {
      sprintf(temp,"%d:", milsec);
      strcat(noDate, temp);
    }

    strcat(newTime, noDate);
    strcpy(time_response_buffer, newTime);


    if (millis() - last_time >= 60000) {
      getting_state = GET;
    }

  }

  char milHourMin[TIME_OUT_BUFFER_SIZE] = "";
  slice(time_response_buffer, milHourMin, 11, 16);
  char USHourMin[TIME_OUT_BUFFER_SIZE] = "";
  formatTime(milHourMin, USHourMin);

  char milNoCol[TIME_OUT_BUFFER_SIZE] = "";
  removeColon(milHourMin, milNoCol);
  char USNoCol[TIME_OUT_BUFFER_SIZE] = "";
  formatTime(milNoCol, USNoCol);

  tft.setCursor(10, 140, 1);
  tft.setTextSize(2);

  char secString[3] = "";
  slice(time_response_buffer, secString, 17, 19); 
  int sec = atoi(secString);
  
  //Serial.println(USHourMin);
  if (compare(alarm_time, USHourMin)) {
    on_off = 1;
    on_state = ON;
  }

  if (on_off || on_state == ON){
    if (sec%2 == 0) {
      tft.println(USHourMin);
    } else {
      tft.println(USNoCol);
    }
  }
  
  int bv4 = button4.update();
  if(on_off) {
    if (game_result == 0) {
      // Serial.println(F("myDFPlayer.play(1)"));
      game_result = run_game();
    } else {
      on_off = 0;
      on_state = ON;
    }
  } else if (bv4 == 1) {
    gotten_mode_state = 0;
    tft.fillScreen(TFT_BLACK);
    time_set = false;
  }
}
