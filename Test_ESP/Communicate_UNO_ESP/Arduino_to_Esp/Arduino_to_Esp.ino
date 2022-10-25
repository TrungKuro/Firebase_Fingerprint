String text;

void setup()
{
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.print("Allo");
}

void loop()
{
  while(Serial.available())
  {
    text = Serial.readString();
    if(text == "OK")
    {
      Serial.print("Allo");
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  }
  delay(1000);
}
