// https://github.com/mobizt/Firebase-ESP8266/blob/master/README.md
// https://www.youtube.com/watch?v=9DNS_3Cu7NU   Exception 9 stack error
// https://www.youtube.com/watch?v=5FMQF5fGYrs

#include <Adafruit_NeoPixel.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>

// #define WIFI_SSID "Truong Ngoc"
// #define WIFI_PASSWORD "NGOC5G65@382N%#1974"
#define WIFI_SSID "HshopLTK"
#define WIFI_PASSWORD "HshopLTK@2311"

#define LedPin 16     // pin D0 as toggle pin
#define NUM_PIXELS 3  // number of rgb led
#define PIN_PIXELS 14 // pin D5
#define FIREBASE_HOST "nodemcu-c6e6d-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "1sn5lmTJxEsz4Nk3bEptleriM0dSrlSdyd3qNWCm"

FirebaseData firebaseData;

String getValueRed;
String getValueGreen;
String getValueBlue;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800);

void setup()
{
  pinMode(LedPin, OUTPUT);
  pixels.begin();
  Serial.begin(9600);

  // connect to wifi
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
  Firebase.reconnectWiFi(true);
}
void loop()
{
  /* ----------------------------------------------------------------------- */
  /*              ESP requests Firebase to respond value for ESP             */
  /* ----------------------------------------------------------------------- */

  if (Firebase.getString(firebaseData, "/redValue"))
  {
    getValueRed = firebaseData.stringData();
    Serial.print("red:");
    Serial.print(getValueRed);
  }
  else
  {
    Serial.print("Error in getInt, ");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.getString(firebaseData, "/greenValue"))
  {
    getValueGreen = firebaseData.stringData();
    Serial.print("\tgreen:");
    Serial.print(getValueGreen);
  }
  else
  {
    Serial.print("Error in getInt, ");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.getString(firebaseData, "/blueValue"))
  {
    getValueBlue = firebaseData.stringData();
    Serial.print("\tblue:");
    Serial.println(getValueBlue);
  }
  else
  {
    Serial.print("Error in getInt, ");
    Serial.println(firebaseData.errorReason());
  }

  /* ----------------------------------------------------------------------- */
  /*                      ESP control device (NeoPixel)                      */
  /* ----------------------------------------------------------------------- */

  for (int i = 0; i < NUM_PIXELS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(getValueRed.toInt(), getValueGreen.toInt(), getValueBlue.toInt()));
  }
  pixels.show();
}
