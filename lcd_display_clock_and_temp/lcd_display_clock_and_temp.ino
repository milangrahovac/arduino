// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include "dht.h"
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2

LiquidCrystal_I2C lcd(0x27, 16, 2);
dht DHT;
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup () {
  lcd.init();
  lcd.backlight();

#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif

  Serial.begin(9600);

  delay(3000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}


void TimeAndDate () {

    lcd.clear();

   for (int i=0; i <= 5; i++){

        DateTime now = rtc.now();
        
        char bufTime[50];
        sprintf(bufTime, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
        char bufDate[50];
        sprintf(bufDate, "%02d/%02d/%04d", now.day(), now.month(), now.year());
    
        
        lcd.setCursor(4,0);
        lcd.print(bufTime);

        lcd.setCursor(0,1);
        lcd.print(daysOfTheWeek[now.dayOfTheWeek()] + String("   ") +  bufDate);
        
        delay(1000);

   }
  
}


void temperature () {
  DHT.read11(DHTPIN);
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print(String("Temp:  ") + round(DHT.temperature) + String(" ") + (char) 223 + String("C"));
  lcd.setCursor(1,1);
  lcd.print(String("Humidity: ") + round(DHT.humidity) + String(" %"));
  delay(5000);
}



void loop () {
  TimeAndDate();
  temperature();
}