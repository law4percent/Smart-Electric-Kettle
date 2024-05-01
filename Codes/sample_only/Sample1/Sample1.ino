#include <WiFi.h>
#include <WebServer.h>
#include <WebDesign.h>

// const char *ssid = "your_wifi_ssid";
// const char *password = "your_wifi_password";
const char* ssid     = "Lenlen 2.4G";
const char* password = "PLDTWIFI@Lenlen2023";

WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Define server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);

  // Start server
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", details_form);
}

void handleSubmit() {
  String fname = server.arg("fname");
  String mname = server.arg("mname");
  String lname = server.arg("lname");

  Serial.print("First Name: ");
  Serial.println(fname);
  Serial.print("Middle Name: ");
  Serial.println(mname);
  Serial.print("Family Name: ");
  Serial.println(lname);

  server.send(200, "text/plain", "Received data successfully!");
}
