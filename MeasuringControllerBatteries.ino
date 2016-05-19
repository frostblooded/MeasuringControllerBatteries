#include <avr/sleep.h>
//TODO: Read from A0 the voltage of the battery(0 - 20.48V)

#define DEVICE_INDEX 0
SoftwareSerial dataSerial(2, 3);
int wakePin = 2;
int sleepStatus = 0;

double getVoltage() {
  // TODO: Implement
}

double getAmpers() {
  //TODO: Implement
}

void wakeUpNow()
{
  String res = "no response";
  
  int byteRead = Serial.readString();
  if(byteRead == DEVICE_INDEX) {
    res = "voltage" + String(getVoltage());
    res += "ampers" + String(getAmpers());
  }

  dataSerial.write(res);
  sleep(100);
SoftwareSerial dataSerial(2, 3);
}

void setup()
{
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
