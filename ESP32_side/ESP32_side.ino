#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_camera.h"

// Wi-Fi and Flask server
const char* ssid = "WING";
const char* password = "abcd1234";
const char* serverFlask = "http://192.168.43.89:5000"; // Flask IP

// Pins
const int LED_PIN = 4;
const int ERROR_PIN = 33;
const int countdownSeconds = 5;

WiFiServer serverCam(80);

// AI-Thinker ESP32-CAM pins
#define PWDN_GPIO_NUM    32
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      21
#define Y4_GPIO_NUM      19
#define Y3_GPIO_NUM      18
#define Y2_GPIO_NUM       5
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

void flashError() {
  pinMode(ERROR_PIN, OUTPUT);
  while(true){
    digitalWrite(ERROR_PIN, HIGH);
    delay(250);
    digitalWrite(ERROR_PIN, LOW);
    delay(250);
  }
}

void sendHelloToFlask() {
  if(WiFi.status() != WL_CONNECTED) flashError();
  HTTPClient http;
  http.begin(String(serverFlask)+"/hello");
  int httpCode = http.POST("ESP32 Ready");
  http.end();
  if(httpCode>0) Serial.printf("📡 /hello sent, response: %d\n", httpCode);
  else flashError();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(ERROR_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  int attempts=0;
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
    attempts++;
    if(attempts>60) flashError();
  }
  Serial.println("\n✅ Connected: "+WiFi.localIP().toString());

  sendHelloToFlask();

  camera_config_t config;
  config.ledc_channel=LEDC_CHANNEL_0;
  config.ledc_timer=LEDC_TIMER_0;
  config.pin_d0=Y2_GPIO_NUM; config.pin_d1=Y3_GPIO_NUM;
  config.pin_d2=Y4_GPIO_NUM; config.pin_d3=Y5_GPIO_NUM;
  config.pin_d4=Y6_GPIO_NUM; config.pin_d5=Y7_GPIO_NUM;
  config.pin_d6=Y8_GPIO_NUM; config.pin_d7=Y9_GPIO_NUM;
  config.pin_xclk=XCLK_GPIO_NUM; config.pin_pclk=PCLK_GPIO_NUM;
  config.pin_vsync=VSYNC_GPIO_NUM; config.pin_href=HREF_GPIO_NUM;
  config.pin_sccb_sda=SIOD_GPIO_NUM; config.pin_sccb_scl=SIOC_GPIO_NUM;
  config.pin_pwdn=PWDN_GPIO_NUM; config.pin_reset=RESET_GPIO_NUM;
  config.xclk_freq_hz=20000000;
  config.pixel_format=PIXFORMAT_JPEG;
  config.frame_size=FRAMESIZE_UXGA; // 1600x1200
  config.jpeg_quality=10; // best quality
  config.fb_count=2;

  if(esp_camera_init(&config)!=ESP_OK) flashError();

  sensor_t * s = esp_camera_sensor_get();
  s->set_contrast(s,2);
  s->set_brightness(s,1);
  s->set_saturation(s,1);
  s->set_gainceiling(s,GAINCEILING_16X);
  s->set_exposure_ctrl(s,1);
  s->set_aec2(s,1);
  s->set_whitebal(s,1);

  serverCam.begin();
  Serial.println("🌐 Listening for trigger...");
}

void loop() {
  WiFiClient client = serverCam.available();
  if(client){
    String req = client.readStringUntil('\r');
    client.flush();

    if(req.indexOf("POST /trigger")>=0){
      Serial.println("✨ Trigger received! Countdown...");
      for(int i=countdownSeconds;i>0;i--){
        digitalWrite(LED_PIN,HIGH);
        delay(500);
        digitalWrite(LED_PIN,LOW);
        delay(500);
        Serial.println("⏳ "+String(i));
      }

      // Multi-frame capture
      camera_fb_t* fb;
      camera_fb_t* best_fb=nullptr;
      size_t max_len=0;
      for(int i=0;i<5;i++){
        fb = esp_camera_fb_get();
        if(fb){
          if(fb->len>max_len){
            if(best_fb) esp_camera_fb_return(best_fb);
            best_fb=fb;
            max_len=fb->len;
          } else esp_camera_fb_return(fb);
        }
      }

      if(!best_fb) flashError();

      // Send to Flask
      if(WiFi.status()!=WL_CONNECTED) flashError();
      HTTPClient http;
      http.begin(String(serverFlask)+"/status");
      http.addHeader("Content-Type","image/jpeg");
      int httpCode = http.POST(best_fb->buf,best_fb->len);
      http.end();
      Serial.printf("📡 Photo sent, response: %d\n",httpCode);
      esp_camera_fb_return(best_fb);
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("OK");
    client.stop();
  }
}
