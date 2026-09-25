#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

#define FP_RX 16
#define FP_TX 17
#define RED_LED 26
#define GREEN_LED 27
#define SD_CS 5

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* scriptURL = "https://script.google.com/macros/s/YOUR_WEBAPP_ID/exec";

HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

unsigned long ledOffTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  turnOffLEDs();

  mySerial.begin(57600, SERIAL_8N1, FP_RX, FP_TX);
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Sensor OK! Capacity: " + String(finger.capacity));
  } else {
    Serial.println("Sensor not found!");
    while(1);
  }

  SD.begin(SD_CS);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi connected");

  configTime(0, 0, "pool.ntp.org");
  Serial.println("\nType 'enroll' to register");
}

void loop() {
  turnOffLEDsIfNeeded();

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "enroll") enrollFingerprint();
  }

  int id = getFingerprintID();
  if (id >= 0) successAction(id);
  else if (id == -1) failAction();

  delay(50);
}

void turnOffLEDs() {
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  ledOffTime = 0;
}

void turnOffLEDsIfNeeded() {
  if (ledOffTime > 0 && millis() > ledOffTime) turnOffLEDs();
}

int getFingerprintID() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) return -2;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -2;
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) return finger.fingerID;
  return -1;
}

void successAction(int id) {
  if (alreadySignedToday(id)) {
    digitalWrite(RED_LED, HIGH);
    Serial.println("Already signed in today!");
    ledOffTime = millis() + 2000;
    return;
  }

  digitalWrite(GREEN_LED, HIGH);
  String name = getNameFromSD(id);
  String ts = getTimestamp();
  Serial.printf("Success: %s (ID %d)\n", name.c_str(), id);
  logToSD(id, name, ts, "Present");
  sendToGoogleSheets(id, name, ts, "Present");
  ledOffTime = millis() + 2000;
}

bool alreadySignedToday(int id) {
  File file = SD.open("/attendance.csv");
  if (!file) return false;
  String today = getTimestamp().substring(0, 10);
  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (line.indexOf(String(id)) != -1 && line.indexOf(today) != -1) {
      file.close();
      return true;
    }
  }
  file.close();
  return false;
}

void failAction() {
  digitalWrite(RED_LED, HIGH);
  Serial.println("No match");
  ledOffTime = millis() + 1500;
}

void enrollFingerprint() {
  Serial.println("=== New User Enrollment ===");
  Serial.println("Enter ID (1-64):");
  while (!Serial.available()) delay(10);
  int id = Serial.parseInt();
  if (id < 1 || id > 64) {
    Serial.println("Invalid ID!");
    return;
  }

  Serial.print("Enrolling ID #"); Serial.println(id);
  Serial.println("Place finger...");
  while (finger.getImage() != FINGERPRINT_OK) delay(50);
  finger.image2Tz(1);

  Serial.println("Remove finger");
  delay(2000);
  Serial.println("Place same finger again...");
  while (finger.getImage() != FINGERPRINT_OK) delay(50);
  finger.image2Tz(2);

  if (finger.createModel() == FINGERPRINT_OK && finger.storeModel(id) == FINGERPRINT_OK) {
    Serial.println("Fingerprint OK!");
    Serial.println("Enter Full Name and press Enter:");
    while (!Serial.available()) delay(10);
    String name = Serial.readStringUntil('\n');
    name.trim();
    if (name.length() == 0) name = "User_" + String(id);
    saveUserToSD(id, name);
    Serial.println("Saved: " + name);
    digitalWrite(GREEN_LED, HIGH);
    delay(1000);
    turnOffLEDs();
  } else {
    Serial.println("Enrollment Failed!");
    digitalWrite(RED_LED, HIGH);
    delay(1000);
    turnOffLEDs();
  }
}

String getTimestamp() {
  time_t now;
  time(&now);
  char buf[30];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
  return String(buf);
}

void logToSD(int id, String name, String ts, String status) {
  File file = SD.open("/attendance.csv", FILE_APPEND);
  if (file) {
    file.printf("%s,%d,%s,%s\n", ts.c_str(), id, name.c_str(), status.c_str());
    file.close();
  }
}

void saveUserToSD(int id, String name) {
  File file = SD.open("/users.csv", FILE_APPEND);
  if (file) {
    file.printf("%d,%s\n", id, name.c_str());
    file.close();
  }
}

String getNameFromSD(int id) {
  File file = SD.open("/users.csv");
  if (!file) return "Unknown";
  while (file.available()) {
    String line = file.readStringUntil('\n');
    int comma = line.indexOf(',');
    if (comma > 0 && line.substring(0, comma).toInt() == id) {
      String n = line.substring(comma + 1);
      file.close();
      return n;
    }
  }
  file.close();
  return "Unknown";
}

void sendToGoogleSheets(int id, String name, String ts, String status) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(scriptURL);
    http.addHeader("Content-Type", "application/json");
    String json = "{\"id\":" + String(id) + ",\"name\":\"" + name + "\",\"timestamp\":\"" + ts + "\",\"status\":\"" + status + "\"}";
    http.POST(json);
    http.end();
  }
}