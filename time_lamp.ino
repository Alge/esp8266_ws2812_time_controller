#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <time.h>
#include "FastLED.h"

const char* ssid = "Tardis";
const char* password = "algeinternet";

#define NUM_LEDS 30
#define DATA_PIN 5

CRGB leds[NUM_LEDS];

int timezone = 1 * 3600;
int dst = 0;

byte dayhour = 0;
byte dayminute = 0;

byte nighthour = 0;
byte nightminute = 0;

byte dayr = 0;
byte dayg = 0;
byte dayb = 0;

byte nightr = 0;
byte nightg = 0;
byte nightb = 0;

boolean day;

ESP8266WebServer server(80);

void printState(){
  Serial.println("Current state:");
  Serial.print("time day");
  Serial.print(dayhour);
  Serial.print(":");
  Serial.println(dayminute);
  Serial.print("time night");  
  Serial.print(nighthour);
  Serial.print(":");
  Serial.println(nightminute);
  Serial.print("Color day");
  Serial.print(dayr);
  Serial.print(":");
  Serial.print(dayg);
  Serial.print(":");
  Serial.println(dayb);  
  Serial.print("Color night");
  Serial.print(nightr);
  Serial.print(":");
  Serial.print(nightg);
  Serial.print(":");
  Serial.println(nightb);  
}

void saveState(){
    byte* p = (byte*) &dayhour;
    EEPROM.write(1, *p);

    p = (byte*) &dayminute;
    EEPROM.write(2, *p);

    p = (byte*) &nighthour;
    EEPROM.write(3, *p);

    p = (byte*) &nightminute;
    EEPROM.write(4, *p);

    p = (byte*) &dayr;
    EEPROM.write(5, *p);

    p = (byte*) &dayg;
    EEPROM.write(6, *p);

    p = (byte*) &dayb;
    EEPROM.write(7, *p);

    p = (byte*) &nightr;
    EEPROM.write(8, *p);

    p = (byte*) &nightg;
    EEPROM.write(9, *p);

    p = (byte*) &nightb;
    EEPROM.write(10, *p);
    
    EEPROM.commit();
}

void getState(){
  dayhour = EEPROM.read(1);
  dayminute = EEPROM.read(2);
  nighthour = EEPROM.read(3);
  nightminute = EEPROM.read(4);
  dayr = EEPROM.read(5);
  dayg = EEPROM.read(6);
  dayb = EEPROM.read(7);
  nightr = EEPROM.read(8);
  nightg = EEPROM.read(9);
  nightb = EEPROM.read(10);
}

void handleRoot() {

  if (server.args() > 0){
    Serial.println("got data!");
    
       for ( uint8_t i = 0; i < server.args(); i++ ) {
        Serial.print(server.argName(i)); // Display the argument
        Serial.print(" : ");
        Serial.println(server.arg(i));

        if (server.argName(i) == "day_hour") {
          dayhour = (byte)server.arg(i).toInt();
        }
        else if (server.argName(i) == "day_minute") {
          dayminute = (byte)server.arg(i).toInt();
        }
        else if (server.argName(i) == "night_hour") {
         nighthour = (byte)server.arg(i).toInt();
        }
        else if (server.argName(i) == "night_minute") {
          nightminute = (byte)server.arg(i).toInt();
        }
        else if (server.argName(i) == "night_r") {
          nightr = (byte)server.arg(i).toInt();
        }
        else if (server.argName(i) == "night_g") {
          nightg = (byte)server.arg(i).toInt();
        }
        else if (server.argName(i) == "night_b") {
          nightb = (byte)server.arg(i).toInt();
        }
        else if (server.argName(i) == "day_r") {
          dayr = (byte)server.arg(i).toInt();
        }
        else if (server.argName(i) == "day_g") {
          dayg = (byte)server.arg(i).toInt();
        }
        else if (server.argName(i) == "day_b") {
          dayb = (byte)server.arg(i).toInt();
        }

       }

    Serial.println("Old state:");
    printState();
    saveState();
    Serial.println("New state:");
    getState();
    printState();
  }

  
  
  String html = "<html><head><title>ESP8266 ws2812</title><head><body><h1>Esp8266 ws2812 time controller</h1><form><label></label>night hour<input type='text' name='night_hour'><br/><label>night minute</label><input type='text' name='night_minute'><br/><label>day hour</label><input type='text' name='day_hour'><br/><label>day minute</label><input type='text' name='day_minute'><br/><label>night r</label><input type='text' name='night_r'><br/><label>night g</label><input type='text' name='night_g'><br/><label>night b</label><input type='text' name='night_b'><br/><label>day r</label><input type='text' name='day_r'><br/><label>day g</label><input type='text' name='day_g'><br/><label>day b</label><input type='text' name='day_b'><br/><input type='submit' value='Update'/></form></body></html>";
  server.send(200, "text/html", html);
}


void eeWriteByte(int pos, byte val) {
    byte* p = (byte*) &val;
    EEPROM.write(pos, *p);
    EEPROM.commit();
}
byte eeGetChar(int pos) {
  byte val;
  byte* p = &val;
  *p      = EEPROM.read(pos);
  return val;
}

void eeWriteInt(int pos, int val) {
    byte* p = (byte*) &val;
    EEPROM.write(pos, *p);
    EEPROM.write(pos + 1, *(p + 1));
    EEPROM.write(pos + 2, *(p + 2));
    EEPROM.write(pos + 3, *(p + 3));
    EEPROM.commit();
}

int eeGetInt(int pos) {
  int val;
  byte* p = (byte*) &val;
  *p        = EEPROM.read(pos);
  *(p + 1)  = EEPROM.read(pos + 1);
  *(p + 2)  = EEPROM.read(pos + 2);
  *(p + 3)  = EEPROM.read(pos + 3);
  return val;
}

int timestamp = 1337;

void setup(void){

  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);

  for(int i = 0; i<NUM_LEDS; i++){
    leds[i] = CRGB(100,100,100);  
  }
  FastLED.show();
  
  Serial.begin(115200);

  EEPROM.begin(1024);


  getState();

  printState();
  
 
  WiFi.begin(ssid, password);
  Serial.println("");

  int counter = 0;
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    counter++;
    if(counter >= 1000){
      Serial.println("Failed connecting to wifi, resetting");
      ESP.reset();  
    }
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

 
  server.begin();
  Serial.println("HTTP server started");

  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  Serial.println("\nWaiting for Internet time");
  while(!time(nullptr)){
     Serial.print("*");
     delay(1000);
  }
  Serial.println("\nTime response....OK"); 
}

long last_update = 0;

void loop(void){

  if (WiFi.status() != 3)
  {
    Serial.println("Wifi not connected, rebooting in 60 seconds");
    delay(60000);
    ESP.reset();
  }
  
  server.handleClient();
  if (millis()> last_update+10000 || millis()< last_update){
      last_update = millis();
      time_t now = time(nullptr);
      
      struct tm* p_tm = localtime(&now);

      Serial.print(p_tm->tm_mday);
      Serial.print("/");
      Serial.print(p_tm->tm_mon + 1);
      Serial.print("/");
      Serial.print(p_tm->tm_year + 1900);
      
      Serial.print(" ");
      
      Serial.print(p_tm->tm_hour);
      Serial.print(":");
      Serial.print(p_tm->tm_min);
      Serial.print(":");
      Serial.println(p_tm->tm_sec);
      

      day = false;
      
      if((byte)p_tm->tm_hour >= dayhour){
        day = true;
      }
      if((byte)p_tm->tm_hour == dayhour && (byte)p_tm->tm_min >= dayminute){
        day = true;
      }

      if((byte)p_tm->tm_hour >= nighthour){
        day = false;
      }
      if((byte)p_tm->tm_hour == nighthour && (byte)p_tm->tm_min >= nightminute){
        day = false;
      }
      
      if(day){
          for(int i = 0; i<NUM_LEDS; i++){
            leds[i] = CRGB(dayg,dayr,dayb);  
          }
          Serial.println("it is day");
      }
      else{
        for(int i = 0; i<NUM_LEDS; i++){
            leds[i] = CRGB(nightg,nightr,nightb);  
        }
        Serial.println("it is night");
      }
      
      FastLED.show();
      
  }
}
