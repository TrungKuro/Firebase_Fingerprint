#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

// Set these to run example.
#define FIREBASE_HOST "nodemcu-c6e6d-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "1sn5lmTJxEsz4Nk3bEptleriM0dSrlSdyd3qNWCm"
#define WIFI_SSID "HshopLTK"
#define WIFI_PASSWORD "HshopLTK@2311"

const int LED_1_SLOT = D2;
void setup()
{
  // Debug console
  Serial.begin(9600);

  pinMode(LED_1_SLOT, OUTPUT);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  if (Firebase.failed())
  {
    Serial.println(Firebase.error());
  }
  else
  {
    Serial.println("Firebase Connected");
  }
}

void loop()
{
  String path = "/LED/";
  FirebaseObject object = Firebase.get(path);
  String LED_1 = object.getString("status");
  Serial.println("LED_1 : " + LED_1);

  if (LED_1 == "ON")
  {
    digitalWrite(LED_1_SLOT, HIGH);
  }
  else if (LED_1 == "OFF")
  {
    digitalWrite(LED_1_SLOT, LOW);
  }
}
