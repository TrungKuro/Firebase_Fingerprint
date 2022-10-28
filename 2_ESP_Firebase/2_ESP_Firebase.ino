#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

/* -------------- DEBUG (uncomment to open the Debug function) ------------- */

// #define ENABLE_DEBUG

#if defined(ENABLE_DEBUG)
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

/* ----------------- Replace with your network credentials ----------------- */

// #define WIFI_SSID "Truong Ngoc"
// #define WIFI_PASSWORD "NGOC5G65@382N%#1974"
#define WIFI_SSID "HshopLTK"
#define WIFI_PASSWORD "HshopLTK@2311"

/* -------------------------------- FIREBASE ------------------------------- */

#define FIREBASE_HOST "nodemcu-c6e6d-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "1sn5lmTJxEsz4Nk3bEptleriM0dSrlSdyd3qNWCm"

FirebaseData firebaseData;

/* ------------------------------- NTP CLIENT ------------------------------ */

/* Set offset time in seconds to adjust for your timezone
**
** VietNam is UTC+07:00
** For UTC +7.00 : +7 * 60 * 60: +25200
*/
#define UTC_OFFSET_IN_SECONDS 25200

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_IN_SECONDS);

/* -------------------------------- VARIABLE ------------------------------- */

String text = "";

long st; // Ordinal numbers

/* Convert time in a day to unit second (s)
**
** 00:00:00 = 0
** 23:59:59 = 23*3600 + 59*60 + 59 = 86399
*/
long numberOpen, numberClose;

/* Firebase data structure of each user */
struct dataFirebase
{
  String DATE = "";
  String ID = "";
  String NOTE = "";
  String TIME = "";
};

/* ------------------------------------------------------------------------- */
/*                                  FUNCTION                                 */
/* ------------------------------------------------------------------------- */

/* Get value of "Total" from Firebase */
void getValueTotal()
{
  if (Firebase.getString(firebaseData, F("/ID/Total")))
  {
    text = firebaseData.stringData();
    st = text.toInt();
    DEBUG_PRINT(F("Total: "));
    DEBUG_PRINTLN(text);
  }
  else
  {
    DEBUG_PRINT(F("Error: "));
    DEBUG_PRINTLN(firebaseData.errorReason());
  }
}

/* Get value of "Open" from Firebase */
void getValueOpen()
{
  if (Firebase.getString(firebaseData, F("/Open")))
  {
    text = firebaseData.stringData();
    numberOpen = text.toInt();
    DEBUG_PRINT(F("Open: "));
    DEBUG_PRINTLN(text);
  }
  else
  {
    DEBUG_PRINT(F("Error: "));
    DEBUG_PRINTLN(firebaseData.errorReason());
  }
}

/* Get value of "Close" from Firebase */
void getValueClose()
{
  if (Firebase.getString(firebaseData, F("/Close")))
  {
    text = firebaseData.stringData();
    numberClose = text.toInt();
    DEBUG_PRINT(F("Close: "));
    DEBUG_PRINTLN(text);
  }
  else
  {
    DEBUG_PRINT(F("Error: "));
    DEBUG_PRINTLN(firebaseData.errorReason());
  }
}

/* Set some value into Firebase */
void setValue(String path, String data)
{
  if (Firebase.setString(firebaseData, path, data))
  {
    DEBUG_PRINT(path);
    DEBUG_PRINT(F(" : "));
    DEBUG_PRINTLN(data);
  }
  else
  {
    DEBUG_PRINT(F("Error: "));
    DEBUG_PRINTLN(firebaseData.errorReason());
  }
}

/* ------------------------------------------------------------------------- */
/*                                    MAIN                                   */
/* ------------------------------------------------------------------------- */

void setup()
{
  /* Serial (UNO)*/
  Serial.begin(115200);

  /* Connect to Wifi */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  DEBUG_PRINT(F("connecting"));
  while (WiFi.status() != WL_CONNECTED)
  {
    DEBUG_PRINT(F("."));
    delay(500);
  }
  DEBUG_PRINTLN();
  DEBUG_PRINT(F("connected: "));
  DEBUG_PRINTLN(WiFi.localIP());

  /* Initialize a NTPClient to get time */
  timeClient.begin();

  /* Initialize Firebase */
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  /* Wait for UNO's confirmation response */
  Serial.print("Done");
  while (1)
  {
    while (Serial.available() == 0)
    {
    }
    text = Serial.readString();
    if (text == "OK") // To know ESP had connected with Firebase!
    {
      break;
    }
  }
}

void loop()
{
  /* Wait for UNO to send data */
  while (Serial.available() == 0)
  {
  }
  text = Serial.readString();
  if (text[0] == '#') // Correct command "#ID"
  {
    /* Update time now! */
    timeClient.update();

    /* Date, ID, Note, Time */
    dataFirebase user;

    /* Get ID */
    user.ID = text;

    /* Get TIME */
    user.TIME = timeClient.getFormattedTime();

    /* Get a time structure */
    time_t epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);

    /* Get DATE */
    int monthDay = ptm->tm_mday;
    int currentMonth = ptm->tm_mon + 1;
    int currentYear = ptm->tm_year + 1900;
    user.DATE = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);

    /* Update the latest "Open" and "Close" */
    getValueOpen();
    getValueClose();

    /* Calculate if this person is late or not? */
    long note = timeClient.getHours() * 3600 + timeClient.getMinutes() * 60 + timeClient.getSeconds();
    if (numberOpen < numberClose)
    {
      if (note > numberOpen && note < numberClose)
      {
        user.NOTE = "Late";
      }
    }
    else if (numberOpen > numberClose)
    {
      if (note > numberClose && note < numberOpen)
      {
        user.NOTE = "Late";
      }
    }

    /* Step 1: Get value now of "Total" from Firebase
    ** Step 2: Then increase by 1
    ** Step 3: Create a new path branch
    ** Step 4: Update value for "Date" on Firebase
    ** Step 5: Update value for "ID" on Firebase
    ** Step 6: Update value for "Note" on Firebase
    ** Step 7: Update value for "Time" on Firebase
    ** Step 8: Update new value to "Total" on Firebase
    */
    getValueTotal();
    st++;
    text = "/user/" + String(st) + "/";
    setValue(String(text + F("Date")), user.DATE);
    setValue(String(text + F("ID")), user.ID);
    setValue(String(text + F("Note")), user.NOTE);
    setValue(String(text + F("Time")), user.TIME);
    setValue(F("/ID/Total"), String(st));
  }
}
