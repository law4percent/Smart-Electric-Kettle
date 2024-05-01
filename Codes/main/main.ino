#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <RTClib.h>

#include "MyWebApp.h"

RTC_DS3231 rtc;

// char daysOfWeek[7][12] = {
//   "Sunday",
//   "Monday",
//   "Tuesday",
//   "Wednesday",
//   "Thursday",
//   "Friday",
//   "Saturday"
// };

const char *ssid = "Lenlen 2.4G";
const char *password = "PLDTWIFI@Lenlen2023";

WebServer server(80);

int getStartHour = 0;
int getStartMinute = 0;
int getEndHour = 0;
int getEndMinute = 0;
String getStartMeridiem = "--";
String getEndMeridiem = "--";
String getAlarmDate = "--/--/--";

// Pin Used
const byte LedIndicator = 2;
const byte Relay = 13;

bool RTCnotFound = false;
bool StartBoiling = false;
bool inProcessAlarm = false;

void setup() {
  Serial.begin(115200);

  pinMode(Relay, OUTPUT);
  pinMode(LedIndicator, OUTPUT);

  digitalWrite(Relay, 0);

  Serial.printf("Connecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
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

  if (nowHour >= 12) {
    nowHour = nowHour - 12;
    if (nowHour == 0) {
      nowHour = 12;
    }
    nowMeridiem = "PM";
  } else {
    nowMeridiem = "AM";
  }

  // Print all data
  Serial.printf("Current Date: %d/%d/%d\n"
                "Current Time: %d:%d:%d %s\n\n",
                nowMonth, nowDay, nowYear, nowHour, nowMinute, nowSecond, nowMeridiem);

  Serial.printf("Alarm Date: %s\n"
                "Start Time: %d:%d %s\n"
                "End Time: %d:%d %s\n\n",
                getAlarmDate, getStartHour, getStartMinute, getStartMeridiem, getEndHour, getEndMinute, getEndMeridiem);

  if ((getStartHour == nowHour) and (getStartMinute == nowMinute) and !(strcmp(getStartMeridiem.c_str(), nowMeridiem.c_str()))) {
    StartBoiling = true;
  }

  if (StartBoiling and (getEndHour == nowHour) and (getEndMinute == nowMinute) and !(strcmp(getEndMeridiem.c_str(), nowMeridiem.c_str()))) {
    ResetData();
  }

  digitalWrite(Relay, StartBoiling);
  Serial.printf("Boiling: %d\n\n", StartBoiling);
  Serial.println(F("---------------------------------------"));
  delay(5000);
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

  if (init_getStartHour == init_getEndHour and init_getStartMinute == init_getEndMinute and (strcmp(init_getStartMeridiem.c_str(), init_getEndMeridiem.c_str())) == 0) {
    const char errorMessageB[] = R"---(
        <h3>Time must not the same!</h3>
        <p>Your input start time and end time are the same value.</p>
    )---";
    
    String errorMsg = "";
    errorMsg += errorMessageA;
    errorMsg += errorMessageB;
    errorMsg += errorMessageC;
    
    server.send(200, "text/html", errorMsg); // must not same value
    saveNewData = false;
  } else if (init_getStartHour > init_getEndHour and (strcmp(init_getStartMeridiem.c_str(), init_getEndMeridiem.c_str())) == 0) {
    const char errorMessageB[] = R"---(
        <h3>Start time is before end time!</h3>
        <p>Start hour must not before end hour.</p>
    )---";
    
    String errorMsg = "";
    errorMsg += errorMessageA;
    errorMsg += errorMessageB;
    errorMsg += errorMessageC;
    
    server.send(200, "text/html", errorMsg); // start hour must not before end hour
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
    
    server.send(200, "text/html", errorMsg); // start minute must not before end minute
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
    
    server.send(200, "text/html", errorMsg); // must not more than one hour
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
  }
}

void handleCancel() {
  ResetData();
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
  homePage += body_script_F;

  server.send(200, "text/html", homePage);
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
}

bool CalculateTimeDuration(int sHour, int sMin, String sMeridiem, int eHour, int eMin, String eMeridiem) {
  int hourDiff = eHour - sHour;
  int minDiff = eMin - sMin;

  // Adjust for AM/PM difference
  if ((strcmp(sMeridiem.c_str(), eMeridiem.c_str())) != 0) {
    hourDiff += 12;
  }

  if (hourDiff > 1 || (hourDiff == 1 && minDiff > 0)) {
    return false;
  } else {
    return true;
  }
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