#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI display = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

#include <NTPClient.h>
#include <WiFi.h> // for WiFi shield
#include <WiFiUdp.h>
#include <Preferences.h>

#include "font.h"
#include "config.h"

WiFiUDP ntpUDP;
Preferences preferences;

int offset = 0;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800000, 60000);

void display_text(String text){
  display.setTextColor(TFT_WHITE);  // Adding a black background colour erases previous text automatically
  display.setFreeFont(FF48);
  //display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(text, 64, 50);
  //display.display();
}

void setup(){
  preferences.begin("NTP", true);
  offset = preferences.getInt("offset", 3);
  preferences.end();

  timeClient.setTimeOffset(offset*3600);

  pinMode(0,INPUT);
  digitalWrite(0,HIGH);

  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high

  display.init();
  display.setRotation(1);
  display.fillScreen(TFT_BLACK);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    display_text("CONN");
    delay ( 500 );
  }

  //display.clear();
  display.fillScreen(TFT_BLACK);
  display_text(ssid);
  
  timeClient.begin();

  while (!timeClient.forceUpdate()){
    delay(10);
  }

  display.fillScreen(TFT_BLACK);
}

char buffer[5];

void loop() {
  timeClient.update();

  if (digitalRead(0) == 0){
    offset += 1;
    if (offset > 14)
      offset = -11;

    timeClient.setTimeOffset(offset*3600);

    preferences.begin("NTP", false);
    preferences.putInt("offset", offset);
    preferences.end();
  }
  
  //display.clear();
  //display.fillScreen(TFT_BLACK);
  //display second bar
  if (timeClient.getSeconds() == 0)
  {
    display.fillScreen(TFT_BLACK);
  }
  
  display.fillRect(1, 0, 240*timeClient.getSeconds()/59, 3, TFT_WHITE);

  //display time
  sprintf(buffer, "%02d:%02d", timeClient.getHours(), timeClient.getMinutes());
  display_text(buffer);

  delay(1000);
}
