#include <avr/sleep.h>
//TODO: Read from A0 the voltage of the battery(0 - 20.48V)

#define VOLTAGE_PIN 0
#define CURRENT_PIN 1
#define CURRENT_ENABLE_PIN 19

#define DEVICE_INDEX 0
SoftwareSerial dataSerial(2, 3);
int wakePin = 2;
int sleepStatus = 0;

double getVoltage() {
  double voltage;
  voltage = (double)analogRead(VOLTAGE_PIN)/1023.0;
  voltage *= 20.48;
  return voltage;
}

double getAmpers() {
  digitalWrite(CURRENT_ENABLE_PIN, HIGH);
  delay(50);
  double current = (double)analogRead(CURRENT_PIN)/1023.0;
  current *= 2.0;
  current -= 1.0;
  current *= 51.1;
  analogRead(CURRENT_PIN)
  digitalWrite(CURRENT_ENABLE_PIN, LOW);
  return current;
}

void wakeUpNow()
{
  String res = "no response";
  
  int byteRead = Serial.readString();
  if(byteRead == DEVICE_INDEX) {
    res = "voltage=" + String(getVoltage());
    res += "&ampers=" + String(getAmpers());
    res += "&battery_id=" + String(CURRENT_ENABLE_PIN);
  }

  dataSerial.write(res);
  sleep(100);
}

void setup()
{
  pinMode(CURRENT_ENABLE_PIN, OUTPUT);
  pinMode(wakePin, INPUT);
  dataSerial.begin(9600);
  attachInterrupt(0, wakeUpNow, CHANGE);
}
 
void sleepNow()
{
    sleep_enable();
    attachInterrupt(0, wakeUpNow, CHANGE);
    analogReference(INTERNAL);
    sleep_mode();
    sleep_disable();
    detachInterrupt(0);
 
}
 
void loop()
{
  Serial.println("Entering Sleep mode");
  delay(100);
  sleepNow();
}
