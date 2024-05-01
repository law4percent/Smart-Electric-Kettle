#include <RTClib.h>

RTC_DS3231 rtc;

char daysOfWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

const byte Relay = 13;

void setup () {
  Serial.begin(115200);

  if (! rtc.begin()) {
    Serial.println("RTC module is NOT found");
    Serial.flush();
    while (1);
  }

  // Uncomment this for maunally setting the time
  // yy, mm, dd, hr, min, sec
  // remember the hour is 24 hour format
  // rtc.adjust(DateTime(2024, 5, 1, 1, 45, 00));
  // This is from your Computer Time
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(Relay, OUTPUT);
  digitalWrite(Relay, 0);
}

void loop() {
  DateTime now = rtc.now(); // Get current date/time from RTC
  
  // Store values in variables
  int year = now.year();
  int month = now.month();
  int day = now.day();
  int dayOfWeek = now.dayOfTheWeek();
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();
  
  // Print values to serial monitor
  Serial.print("ESP32 RTC Date Time: ");
  Serial.print(year);
  Serial.print('/');
  Serial.print(month);
  Serial.print('/');
  Serial.print(day);
  Serial.print(" (");
  Serial.print(daysOfWeek[dayOfWeek]);
  Serial.print(") ");
  Serial.print(hour);
  Serial.print(':');
  Serial.print(minute);
  Serial.print(':');
  Serial.println(second);
  
  delay(1000); // Delay for 1 second
}
