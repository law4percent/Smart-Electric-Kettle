#define Serial_Debug
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <RTClib.h>
#include <TM1637Display.h>

IPAddress staticIP(192, 168, 1, 25);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

#include "MyWebApp.h"

RTC_DS3231 rtc;

const char *ssid = "Hello World";
const char *password = "123456789";

WebServer server(80);

int getStartHour = 0;
int getStartMinute = 0;
int getEndHour = 0;
int getEndMinute = 0;
String getStartMeridiem = "--";
String getEndMeridiem = "--";
String getAlarmDate = "--/--/--";
bool RTCnotFound = false;
bool StartBoiling = false;
bool inProcessAlarm = false;

unsigned long startTime = 0;

// Pin Used
const byte LedIndicator = 2;
const byte buzzer = 12;
const byte ClockPin = 27;
const byte DataPin = 14;
const byte Relay = 13;

int getRemainingMins = 0;
bool TimeFire = false;
int prevMin = 0;
bool firstCheck = true;
String nowDate;
String nowTime;

TM1637Display display(ClockPin, DataPin);

void setup() {
  Serial.begin(115200);

  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  display.setBrightness(0x0c);
  display.setSegments(data);

  pinMode(Relay, OUTPUT);
  pinMode(LedIndicator, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(Relay, 0);
  Buzzer(1);
  delay(1000);
  Buzzer(0);

  Serial.printf("Connecting to %s\n", ssid);
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.printf("\nWiFi connected.\n IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);
  server.on("/request_cancel", HTTP_POST, handleCancel);

  if (!rtc.begin()) {
    digitalWrite(LedIndicator, 1);
    Serial.println(F("RTC module is NOT found!"));
    Serial.flush();
    RTCnotFound = true;
    while (1)
      ;
  }
  digitalWrite(LedIndicator, 0);
  display.setBrightness(0);  // Turn off
  display.clear();
}

void loop() {
  server.handleClient();
  DateTime now = rtc.now();

  int nowYear = now.year();
  int nowMonth = now.month();
  int nowDay = now.day();
  // int nowDayOfWeek = now.dayOfTheWeek();
  int nowHour = now.hour();
  int nowMinute = now.minute();
  int nowSecond = now.second();
  String nowMeridiem;
  bool DayAlarm = false;

  if (nowDate.equals(getAlarmDate)) {
    DayAlarm = true;
  }

  if (nowHour >= 12) {
    nowHour = nowHour - 12;
    if (nowHour == 0) {
      nowHour = 12;
    }
    nowMeridiem = "PM";
  } else {
    nowMeridiem = "AM";
  }

  nowDate = intDateConvertToString(nowYear, nowMonth, nowDay);
  nowTime = intTimeConvertToString(nowHour, nowMinute, nowMeridiem);

  if (DayAlarm and
      getStartHour == nowHour and
      getStartMinute == nowMinute and
      !(strcmp(getStartMeridiem.c_str(), nowMeridiem.c_str())) and
      !TimeFire) {
    StartBoiling = true;
    Buzzer(1);
    startTime = millis();
    TimeFire = true;
    DisplayTime(getRemainingMins - 1, (59 - nowSecond));
  }

  if (DayAlarm and
      StartBoiling and 
      getEndHour == nowHour and
      getEndMinute == nowMinute and
      !(strcmp(getEndMeridiem.c_str(), nowMeridiem.c_str())) and
    TimeFire) {
    ResetData();
    Buzzer(1);
    startTime = millis();
    delay(3000);
  }

  if (getRemainingMins >= 0 and TimeFire) {
    int nowSEC = (59 - nowSecond);
    int currMin = nowMinute;

    if (firstCheck) {
      firstCheck = false;
      prevMin = currMin;
    } else {
      if (getRemainingMins > 0 && currMin != prevMin) {
        getRemainingMins--;
        prevMin = currMin;
      }
    }
    display.setBrightness(7);  // Turn on
    DisplayTime(getRemainingMins, nowSEC);
  }

  if (startTime != 0 && millis() - startTime >= 1000) {
    startTime = 0;
    Buzzer(0);
  }

  // Print all data
#ifdef Serial_Debug
  Serial.printf("Current Date: %s\n"
                "Current Time: %d:%d:%d %s\n\n",
                nowDate, nowHour, nowMinute, nowSecond, nowMeridiem);

  Serial.printf("Alarm Date: %s\n"
                "Start Time: %d:%d %s\n"
                "End Time: %d:%d %s\n"
                "Minutes Duration: %d:%d\n\n",
                getAlarmDate, getStartHour, getStartMinute, getStartMeridiem, getEndHour, getEndMinute, getEndMeridiem, getRemainingMins,
                (TimeFire) ? (59 - nowSecond) : 59);
  Serial.printf("Boiling: %d\n\n", StartBoiling);
  Serial.println(F("---------------------------------------"));
#endif

  digitalWrite(Relay, StartBoiling);
  delay(500);
}

void handleRoot() {
  if (RTCnotFound) {
    const char errorMessageB[] = R"---(
        <h3>RTC module not found!</h3>
        <p>Try to check the components or restart the system.</p>
    )---";

    String errorMsg = "";
    errorMsg += errorMessageA;
    errorMsg += errorMessageB;
    errorMsg += errorMessageC;

    server.send(200, "text/html", errorMsg);
  } else {
    String homePage = "";
    homePage += head_CSSstyle;
    homePage += body_form_layout_A;
    homePage += body_activeContainer_B;
    homePage += body_activeContainer_C;

    homePage += "<td>";
    homePage += getAlarmDate;
    homePage += "</td>";

    homePage += "<td>";
    homePage += String(getStartHour);
    homePage += ":";
    if (getStartMinute < 10) {
      homePage += "0";
      homePage += String(getStartMinute);
    } else {
      homePage += String(getStartMinute);
    }
    homePage += " ";
    homePage += getStartMeridiem;
    homePage += "</td>";

    homePage += "<td>";
    homePage += String(getEndHour);
    homePage += ":";
    if (getEndMinute < 10) {
      homePage += "0";
      homePage += String(getEndMinute);
    } else {
      homePage += String(getEndMinute);
    }
    homePage += " ";
    homePage += getEndMeridiem;
    homePage += "</td>";

    if (inProcessAlarm) {
      homePage += body_activeContainer_D_a;
    } else {
      homePage += body_activeContainer_D_b;
    }

    homePage += body_activeContainer_E;
    homePage += "RTC: ";
    homePage += nowDate;
    homePage += " ";
    homePage += nowTime;
    homePage += body_activeContainer_F;
    homePage += body_script_F;

    server.send(200, "text/html", homePage);
  }
}

void handleSubmit() {
  Serial.println(F("New Client"));

  String init_getAlarmDate = server.arg("date");

  String startHour = server.arg("startHour");
  String startMinute = server.arg("startMinute");
  String sMeridiem = server.arg("sMeridiem");

  String endHour = server.arg("endHour");
  String endMinute = server.arg("endMinute");
  String eMeridiem = server.arg("eMeridiem");

  int init_getStartHour = startHour.toInt();
  int init_getStartMinute = startMinute.toInt();
  String init_getStartMeridiem = ToUpperCase(sMeridiem);

  int init_getEndHour = endHour.toInt();
  int init_getEndMinute = endMinute.toInt();
  String init_getEndMeridiem = ToUpperCase(eMeridiem);

  bool saveNewData;

  if (init_getStartHour == init_getEndHour and init_getStartMinute == init_getEndMinute and !(strcmp(init_getStartMeridiem.c_str(), init_getEndMeridiem.c_str()))) {
    const char errorMessageB[] = R"---(
        <h3>Time must not the same!</h3>
        <p>Your input start time and end time are the same value.</p>
    )---";

    String errorMsg = "";
    errorMsg += errorMessageA;
    errorMsg += errorMessageB;
    errorMsg += errorMessageC;

    server.send(200, "text/html", errorMsg);  // must not same value
    saveNewData = false;
  } else if (init_getStartHour > init_getEndHour && !(strcmp(init_getStartMeridiem.c_str(), init_getEndMeridiem.c_str()))
             && init_getStartHour != 12 && init_getEndHour != 1) {
    const char errorMessageB[] = R"---(
        <h3>Start time is before end time!</h3>
        <p>Start hour must not before end hour.</p>
    )---";

    String errorMsg = "";
    errorMsg += errorMessageA;
    errorMsg += errorMessageB;
    errorMsg += errorMessageC;

    server.send(200, "text/html", errorMsg);  // start hour must not before end hour
    saveNewData = false;
  } else if (init_getStartHour == init_getEndHour and init_getStartMinute > init_getEndMinute and (strcmp(init_getStartMeridiem.c_str(), init_getEndMeridiem.c_str())) == 0) {
    const char errorMessageB[] = R"---(
        <h3>Start time is before end time!</h3>
        <p>Start minute must not before end minute.</p>
    )---";

    String errorMsg = "";
    errorMsg += errorMessageA;
    errorMsg += errorMessageB;
    errorMsg += errorMessageC;

    server.send(200, "text/html", errorMsg);  // start minute must not before end minute
    saveNewData = false;
  } else if (!CalculateTimeDuration(init_getStartHour, init_getStartMinute, init_getStartMeridiem, init_getEndHour, init_getEndMinute, init_getEndMeridiem)) {
    const char errorMessageB[] = R"---(
        <h3>Time input is more than hour!</h3>
        <p>Time input must not more than one hour.</p>
    )---";

    String errorMsg = "";
    errorMsg += errorMessageA;
    errorMsg += errorMessageB;
    errorMsg += errorMessageC;

    server.send(200, "text/html", errorMsg);  // must not more than one hour
    saveNewData = false;
  } else {
    server.send(200, "text/html", successMessage);
    inProcessAlarm = true;
    saveNewData = true;
  }

  if (saveNewData) {
    getAlarmDate = server.arg("date");
    getStartHour = startHour.toInt();
    getStartMinute = startMinute.toInt();
    getStartMeridiem = ToUpperCase(sMeridiem);

    getEndHour = endHour.toInt();
    getEndMinute = endMinute.toInt();
    getEndMeridiem = ToUpperCase(eMeridiem);
    StartBoiling = false;
    firstCheck = true;
  }
}

void handleCancel() {
  ResetData();

  server.send(200, "text/html", cancellationSuccessMessage);
}

void ResetData() {
  StartBoiling = false;
  inProcessAlarm = false;

  getAlarmDate = "--/--/--";

  getStartHour = 0;
  getStartMinute = 0;
  getStartMeridiem = "--";

  getEndHour = 0;
  getEndMinute = 0;
  getEndMeridiem = "--";

  TimeFire = false;
  getRemainingMins = 0;
  prevMin = 0;
  display.setBrightness(0);  // Turn off
  display.clear();
  firstCheck = true;
}

bool CalculateTimeDuration(int sHour, int sMin, String sMeridiem, int eHour, int eMin, String eMeridiem) {
  int sHr_24format = convertTo24Hours(sHour, sMeridiem);
  int eHr_24format = convertTo24Hours(eHour, eMeridiem);
  int Hr = eHr_24format - sHr_24format;  // hour format
  int Min = eMin - sMin;

  if (eHr_24format == 0 && sHr_24format == 23) {
    Hr = 24 - sHr_24format;
  }

  if (Hr == 1 and Min < 0) {
    Min = Min + 60;
  } else if ((Hr == 1 && Min != 0) || (Hr > 1 || Hr < 0) || (Hr == 1 && Min > 0)) {
    return false;
  }

#ifdef Serial_Debug
  Serial.print(F("sHr_24format: "));
  Serial.println(sHr_24format);
  Serial.print(F("eHr_24format: "));
  Serial.println(eHr_24format);
  Serial.print(F("Min: "));
  Serial.println(Min);
#endif

  if (Hr == 1 && Min == 0) {
    getRemainingMins = (Min + 60) - 1;
  } else if (Hr == 0) {
    getRemainingMins = Min - 1;
  } else {
    getRemainingMins = Min - 1;
  }
  return true;
}

String ToUpperCase(String rawData) {
  String lowerStr = "";

  for (int i = 0; i < rawData.length(); i++) {
    char c = rawData.charAt(i);
    if (isLowerCase(c)) {
      c = toUpperCase(c);
    }
    lowerStr += c;
  }

  return lowerStr;
}

void Buzzer(const bool state) {
  if (state) {
    tone(buzzer, 3000);
  } else {
    noTone(buzzer);
  }
}

void DisplayTime(const int mins, const int secs) {
  display.showNumberDecEx(mins, 0b01000000, true, 2, 0);
  display.showNumberDecEx(secs, 0, true, 2, 2);
}

int convertTo24Hours(int Hour, String Meridiem) {
  if (Hour == 12 && !(strcmp(Meridiem.c_str(), "AM"))) {
    return 0;
  } else if (Hour <= 11 && !(strcmp(Meridiem.c_str(), "AM"))) {
    return Hour;
  } else if (Hour == 12 && !(strcmp(Meridiem.c_str(), "PM"))) {
    return Hour;
  } else if (Hour <= 11 && !(strcmp(Meridiem.c_str(), "PM"))) {
    return Hour + 12;
  }
}

String intDateConvertToString(int year, int month, int day) {
  String CurrentDate = "";

  CurrentDate += String(year);
  CurrentDate += "-";

  if (month < 10) {
    CurrentDate += "0";
  }
  CurrentDate += String(month);
  CurrentDate += "-";

  if (day < 10) {
    CurrentDate += "0";
  }
  CurrentDate += String(day);
  return CurrentDate;
}

String intTimeConvertToString(int hour, int min, String meridiem) {
  String CurrentTime = "";
  
  CurrentTime += String(hour);
  CurrentTime += ":";
  CurrentTime += String(min);
  CurrentTime += " ";
  CurrentTime += meridiem;
  return CurrentTime;
}
