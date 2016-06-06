#include <avr/sleep.h>
#include <SoftwareSerial.h>

#define VOLTAGE_PIN 0
#define CURRENT_PIN 1
#define CURRENT_ENABLE_PIN 19
#define DEVICE_INDEX 0

// Serial for communication with main controller
SoftwareSerial mySerial(2, 3);
int wakePin = 2;
int sleepStatus = 0;

// Function that checks battery voltage and returns it
double getVoltage() {
  double voltage;
  voltage = (double)analogRead(VOLTAGE_PIN)/1023.0;
  voltage *= 20.48;
  return voltage;
}

// Function that checks battery current and returns it
double getAmpers() {
  digitalWrite(CURRENT_ENABLE_PIN, HIGH);
  delay(50);
  double current = (double)analogRead(CURRENT_PIN)/1023.0;
  current *= 2.0;
  current -= 1.0;
  current *= 51.1;
  analogRead(CURRENT_PIN);
  digitalWrite(CURRENT_ENABLE_PIN, LOW);
  return current;
}

// Function, which gets executed when Arduino wakes up
void wakeUpNow()
{
  String res = "no response";
  
  String byteRead = mySerial.readString();

  // If main controller is calling this controller
  // to get its data
  if(byteRead == String(DEVICE_INDEX)) {
    res = "voltage=" + String(getVoltage());
    res += "&ampers=" + String(getAmpers());
    res += "&battery_id=" + String(CURRENT_ENABLE_PIN);
    mySerial.print(res);
    delay(100);
  }
}

void setup()
{
  pinMode(CURRENT_ENABLE_PIN, OUTPUT);
  pinMode(wakePin, INPUT); // Pin for waking up
  attachInterrupt(0, wakeUpNow, CHANGE); // Wake up in voltage change
  mySerial.begin(1200);
  analogReference(INTERNAL);
}

// Start sleeping
void sleepNow()
{
  sleep_enable();
  attachInterrupt(0, wakeUpNow, CHANGE);
  sleep_mode();
  sleep_disable();
  detachInterrupt(0);
}

// After waking up and handling wakeUpNow(), wait a bit and sleep again
void loop()
{
  delay(100);
  sleepNow();
}
