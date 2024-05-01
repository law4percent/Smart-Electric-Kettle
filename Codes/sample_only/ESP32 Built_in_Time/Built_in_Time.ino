#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid = "YourNetworkSSID";
const char* password = "YourNetworkPassword";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize NTP client
  timeClient.begin();
}

void loop() {
  timeClient.update();
  timeClient.getDay();
  timeClient.getHours();
  timeClient.getMinutes();
  timeClient.getSeconds();
  timeClient.getHours();
  timeClient.

  // Print local time
  Serial.print("Local time: ");
  Serial.println(timeClient.getFormattedTime());

  delay(1000); // Wait for 1 second
}