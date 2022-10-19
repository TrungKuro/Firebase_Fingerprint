/* -------------------------------- LIBRARY -------------------------------- */

#include <Wire.h>
#include "OneButton.h"
#include "LiquidCrystal_I2C.h"
#include "Adafruit_Fingerprint.h"

/* -------------- DEBUG (uncomment to open the Debug function) ------------- */

// #define ENABLE_DEBUG

#if defined(ENABLE_DEBUG)
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

/* --------------------------------- BUTTON -------------------------------- */

#define PIN_BUTTON_MENU 6
#define PIN_BUTTON_ID 7
#define PIN_BUTTON_OK 8

/* Button is active LOW */
OneButton btnMenu(PIN_BUTTON_MENU, true);
OneButton btnID(PIN_BUTTON_ID, true);
OneButton btnOk(PIN_BUTTON_OK, true);

/* ---------------------------------- LCD ---------------------------------- */

LiquidCrystal_I2C lcd(0x27, 16, 2);

enum monitor
{
  FINGERPRINT_SCAN,
  CHOOSE_ID,
  ENROLL_FINGERPRINT,
  DELETE_FINGERPRINT,
  DELETE_ALL_FINGERPRINT
};
uint8_t menu = 0;
bool menuChange = true;

#define MIN_ID 1
#define MAX_ID 127
uint8_t id = 1; // ID # (from 1 to 127)

#define TIME_TO_VIEW 1500 // (ms)

#define TIME_AUTO_RETURN 7000 // (ms)
unsigned long lastEvent;

/* ------------------------------ FINGERPRINT ------------------------------ */

/**
 * Arduino UNO : FingerPrint
 * RX (2) ------ Tx (Yellow)
 * TX (3) ------ Rx (White)
 */
SoftwareSerial mySerial(2, 3); // RX, TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

/**
 * Note, the larger this value
 * The more it affects the ability to handle the Button
 */
#define SCAN_FREQ 30 // (ms)

volatile int fingerStatus = -1;

/* ---------------------------------- DOOR --------------------------------- */

#define PIN_DOOR 5
#define TIME_OPEN_DOOR 3000 // (ms)

/* --------------------------------- BUZZER -------------------------------- */

#define PIN_BUZZ 4

/* ------------------------------------------------------------------------- */
/*                                  FUNCTION                                 */
/* ------------------------------------------------------------------------- */

void printSelectedID()
{
  lcd.setCursor(0, 1);
  switch (menu)
  {
  case CHOOSE_ID:
    if (id / 10 == 0) // From 1 to 9
    {
      lcd.print("      #00");
    }
    else if (id / 10 >= 1 && id / 10 <= 9) // From 10 to 99
    {
      lcd.print("      #0");
    }
    else // From 100 to 127
    {
      lcd.print("      #");
    }
    lcd.print(id);
    lcd.print("      ");
    break;
  case ENROLL_FINGERPRINT:
  case DELETE_FINGERPRINT:
    if (id / 10 == 0) // From 1 to 9
    {
      lcd.print("    ID #00");
    }
    else if (id / 10 >= 1 && id / 10 <= 9) // From 10 to 99
    {
      lcd.print("    ID #0");
    }
    else // From 100 to 127
    {
      lcd.print("    ID #");
    }
    lcd.print(id);
    lcd.print("!    ");
  }
}

uint8_t deleteFingerprint(uint8_t id)
{
  uint8_t p = -1;

  p = finger.deleteModel(id);
  lcd.setCursor(0, 1);
  if (p == FINGERPRINT_OK) // Deleted success
  {
    DEBUG_PRINTLN(F("Deleted!"));
    lcd.print("    Success!    ");
  }
  else // Deleted failure
  {
    DEBUG_PRINT(F("Error!"));
    lcd.print("    Failure!    ");
  }

  return p;
}

uint8_t deleteAllFingerprint()
{
  uint8_t p = -1;

  p = finger.emptyDatabase();
  lcd.setCursor(0, 1);
  if (p == FINGERPRINT_OK) // Deleted success
  {
    DEBUG_PRINTLN(F("Deleted!"));
    lcd.print("all ID [Success]");
  }
  else // Deleted failure
  {
    DEBUG_PRINT(F("Error!"));
    lcd.print("all ID [Failure]");
  }

  return p;
}

uint8_t getFingerprintEnroll()
{
  /* ------------------------- STEP 1: wait finger ------------------------- */

  DEBUG_PRINT(F("Waiting for valid finger to enroll as #"));
  DEBUG_PRINTLN(id);
  lcd.setCursor(0, 0);
  lcd.print("Please put your ");
  lcd.setCursor(0, 1);
  lcd.print(" finger into... ");

  int p = -1;
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      DEBUG_PRINTLN(F("Image taken"));
      break;
    case FINGERPRINT_NOFINGER:
      DEBUG_PRINTLN(F("."));
      break;
    default:
      DEBUG_PRINTLN(F("Unknown error"));
    }
  }

  /* ---------------- STEP 2: image converted (OK success!) ---------------- */

  p = finger.image2Tz(1);
  switch (p)
  {
  case FINGERPRINT_OK:
    DEBUG_PRINTLN(F("Image converted"));
    break;
  default:
    DEBUG_PRINTLN(F("Unknown error"));
    lcd.setCursor(0, 0);
    lcd.print("     Error!     ");
    lcd.setCursor(0, 1);
    lcd.print("      [01]      ");
    delay(TIME_TO_VIEW);
    return p;
  }

  /* ------------------------ STEP 3: remove finger ------------------------ */

  DEBUG_PRINTLN(F("Remove finger"));

  lcd.setCursor(0, 0);
  lcd.print("Pls, remove your");
  lcd.setCursor(0, 1);
  lcd.print("  finger in 3s  ");

  delay(3000);

  p = 0;
  while (p != FINGERPRINT_NOFINGER)
  {
    p = finger.getImage();
  }

  DEBUG_PRINT(F("ID "));
  DEBUG_PRINTLN(id);

  /* ---------------------- STEP 4: wait finger again ---------------------- */

  DEBUG_PRINTLN(F("Place same finger again"));

  lcd.setCursor(0, 0);
  lcd.print(" Pls place same ");
  lcd.setCursor(0, 1);
  lcd.print("  finger again  ");

  p = -1;
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      DEBUG_PRINTLN(F("Image taken"));
      break;
    case FINGERPRINT_NOFINGER:
      DEBUG_PRINTLN(F("."));
      break;
    default:
      DEBUG_PRINTLN(F("Unknown error"));
      break;
    }
  }

  /* ---------------- STEP 5: image converted (OK success!) ---------------- */

  p = finger.image2Tz(2);
  switch (p)
  {
  case FINGERPRINT_OK:
    DEBUG_PRINTLN(F("Image converted"));
    break;
  default:
    DEBUG_PRINTLN(F("Unknown error"));
    lcd.setCursor(0, 0);
    lcd.print("     Error!     ");
    lcd.setCursor(0, 1);
    lcd.print("      [02]      ");
    delay(TIME_TO_VIEW);
    return p;
  }

  /* ------------------------ STEP 6: creating model ----------------------- */

  DEBUG_PRINT(F("Creating model for #"));
  DEBUG_PRINTLN(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK)
  {
    DEBUG_PRINTLN(F("Prints matched!"));
  }
  else
  {
    DEBUG_PRINTLN(F("Unknown error"));
    lcd.setCursor(0, 0);
    lcd.print("     Error!     ");
    lcd.setCursor(0, 1);
    lcd.print("      [03]      ");
    delay(TIME_TO_VIEW);
    return p;
  }

  DEBUG_PRINT(F("ID "));
  DEBUG_PRINTLN(id);

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK)
  {
    DEBUG_PRINTLN(F("Stored!"));
  }
  else
  {
    DEBUG_PRINTLN(F("Unknown error"));
    lcd.setCursor(0, 0);
    lcd.print("     Error!     ");
    lcd.setCursor(0, 1);
    lcd.print("      [04]      ");
    delay(TIME_TO_VIEW);
    return p;
  }

  lcd.setCursor(0, 0);
  lcd.print(" Enroll Success ");
  printSelectedID();
  delay(TIME_TO_VIEW);
  return true;
}

int getFingerprintIDez()
{
  uint8_t p = finger.getImage();
  if (p != 2)
  {
    DEBUG_PRINTLN(p);
  }
  if (p != FINGERPRINT_OK)
  {
    return -1;
  }

  p = finger.image2Tz();
  if (p != 2)
  {
    DEBUG_PRINTLN(p);
  }
  if (p != FINGERPRINT_OK)
  {
    return -1;
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
    return -2;
  }

  /* Found a match! */
  DEBUG_PRINT(F("Found ID #"));
  DEBUG_PRINT(finger.fingerID);
  DEBUG_PRINT(F(" with confidence of "));
  DEBUG_PRINTLN(finger.confidence);
  return finger.fingerID;
}

void handlingRelay()
{
  digitalWrite(PIN_DOOR, HIGH);
  delay(TIME_OPEN_DOOR);
  digitalWrite(PIN_DOOR, LOW);
}

/* ------------------------------------------------------------------------- */

void displayMonitor()
{
  if (menuChange)
  {
    menuChange = false;

    switch (menu)
    {
    case FINGERPRINT_SCAN:
      lcd.setCursor(0, 0);
      lcd.print(" Timekeeping... ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      break;
    case CHOOSE_ID:
      lcd.setCursor(0, 0);
      lcd.print("Please Choose ID");
      printSelectedID();
      break;
    case ENROLL_FINGERPRINT:
      lcd.setCursor(0, 0);
      lcd.print(" Confirm Enroll ");
      printSelectedID();
      break;
    case DELETE_FINGERPRINT:
      lcd.setCursor(0, 0);
      lcd.print(" Confirm Delete ");
      printSelectedID();
      break;
    case DELETE_ALL_FINGERPRINT:
      lcd.setCursor(0, 0);
      lcd.print(" Confirm Delete ");
      lcd.setCursor(0, 1);
      lcd.print("all ID [       ]");
    }
  }
}

/* ------------------------------------------------------------------------- */

void chooseMenu()
{
  menuChange = true;

  if (menu == FINGERPRINT_SCAN)
  {
    menu = CHOOSE_ID;
    lastEvent = millis(); // Turn on auto back HOME
  }
  else if (menu == CHOOSE_ID)
  {
    menu = FINGERPRINT_SCAN;
  }

  if (menu == ENROLL_FINGERPRINT)
  {
    menu = DELETE_FINGERPRINT;
    lastEvent = millis(); // Update the timer
  }
  else if (menu == DELETE_FINGERPRINT)
  {
    menu = DELETE_ALL_FINGERPRINT;
    lastEvent = millis(); // Update the timer
  }
  else if (menu == DELETE_ALL_FINGERPRINT)
  {
    menu = ENROLL_FINGERPRINT;
    lastEvent = millis(); // Update the timer
  }
}

void exitSetting()
{
  menuChange = true;
  menu = FINGERPRINT_SCAN;
}

void chooseID()
{
  if (menu == CHOOSE_ID)
  {
    id++;
    if (id > MAX_ID)
    {
      id = MIN_ID;
    }
    printSelectedID();
    lastEvent = millis(); // Update the timer
  }
}

void confirmID_enrollID()
{
  if (menu == CHOOSE_ID) // Choose this ID
  {
    menuChange = true;
    menu = ENROLL_FINGERPRINT;
    lastEvent = millis(); // Update the timer
  }
  else if (menu == ENROLL_FINGERPRINT) // Enroll Fingerprint with this ID
  {
    DEBUG_PRINT(F("Enrolling ID #"));
    DEBUG_PRINTLN(id);
    while (!getFingerprintEnroll())
      ;
  }
}

void deleteID()
{
  if (menu == DELETE_FINGERPRINT)
  {
    DEBUG_PRINT(F("Deleting ID #"));
    DEBUG_PRINTLN(id);
    deleteFingerprint(id); // Delete Fingerprint with this ID
    lastEvent = millis();  // Update the timer
  }
  else if (menu == DELETE_ALL_FINGERPRINT)
  {
    DEBUG_PRINTLN(F("Deleting all ID"));
    deleteAllFingerprint(); // Delete all ID of Fingerprint
    DEBUG_PRINTLN(F("Now database is empty :)"));
    lastEvent = millis(); // Update the timer
  }
}

/* ------------------------------------------------------------------------- */
/*                                    MAIN                                   */
/* ------------------------------------------------------------------------- */

void setup()
{
  /* Serial (ESP)*/
  Serial.begin(115200);

  /* Link function to be called on event of Button */
  btnMenu.attachClick(chooseMenu);
  btnMenu.attachLongPressStop(exitSetting);
  btnID.attachClick(chooseID);
  btnID.attachDuringLongPress(chooseID);
  btnOk.attachClick(confirmID_enrollID);
  btnOk.attachDoubleClick(deleteID);

  /* LCD */
  lcd.init();
  lcd.clear();
  lcd.backlight();

  /* FingerPrint */
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword())
  {
    DEBUG_PRINTLN(F("Found fingerprint sensor!"));
  }
  else
  {
    DEBUG_PRINTLN(F("Did not find fingerprint sensor :("));
    lcd.setCursor(0, 0); // C=1, R=1
    lcd.print("Not found sensor");
    lcd.setCursor(0, 1); // C=1, R=2
    lcd.print("  FingerPrint!  ");
    while (1)
    {
      delay(1);
    }
  }
  //
  DEBUG_PRINTLN(F("Reading sensor parameters"));
  finger.getParameters();
  DEBUG_PRINT(F("Status: 0x"));
  DEBUG_PRINTLN(finger.status_reg, HEX);
  DEBUG_PRINT(F("Sys ID: 0x"));
  DEBUG_PRINTLN(finger.system_id, HEX);
  DEBUG_PRINT(F("Capacity: "));
  DEBUG_PRINTLN(finger.capacity);
  DEBUG_PRINT(F("Security level: "));
  DEBUG_PRINTLN(finger.security_level);
  DEBUG_PRINT(F("Device address: "));
  DEBUG_PRINTLN(finger.device_addr, HEX);
  DEBUG_PRINT(F("Packet len: "));
  DEBUG_PRINTLN(finger.packet_len);
  DEBUG_PRINT(F("Baud rate: "));
  DEBUG_PRINTLN(finger.baud_rate);
  //
  finger.getTemplateCount();
  if (finger.templateCount == 0)
  {
    DEBUG_PRINT(F("Sensor doesn't contain any fingerprint data."));
  }
  else
  {
    DEBUG_PRINTLN(F("Waiting for valid finger..."));
    DEBUG_PRINT(F("Sensor contains "));
    DEBUG_PRINT(finger.templateCount);
    DEBUG_PRINTLN(F(" templates"));
  }

  /* Door (Relay) */
  pinMode(PIN_DOOR, OUTPUT);

  /* Buzzer */
  pinMode(PIN_BUZZ, OUTPUT);
}

void loop()
{
  /* Scan buttons */
  btnMenu.tick();
  btnID.tick();
  btnOk.tick();

  /* Display LCD */
  displayMonitor();

  /* Fingerprint processing */
  if (menu == FINGERPRINT_SCAN)
  {
    fingerStatus = getFingerprintIDez();
    delay(SCAN_FREQ); // Reduce Fingerprint scanning frequency

    if (fingerStatus != -1 && fingerStatus != -2)
    {
      lcd.setCursor(0, 0);
      lcd.print(" Found a valid! ");
      lcd.setCursor(0, 1);
      lcd.print("    ID # ");
      lcd.print(fingerStatus);
      lcd.print("      ");

      handlingRelay();

      menuChange = true;
      delay(TIME_TO_VIEW);
    }
    else if (fingerStatus == -2)
    {
      lcd.setCursor(0, 0);
      lcd.print("    _Invalid    ");
      lcd.setCursor(0, 1);
      lcd.print("  FingerPrint!  ");

      menuChange = true;
      delay(TIME_TO_VIEW);
    }
  }
  else // Automatically return to HOME screen
  {
    if (millis() - lastEvent >= TIME_AUTO_RETURN)
    {
      exitSetting();
    }
  }
}
