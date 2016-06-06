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
double getCurrent() {
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

#define PACKET_SIZE 1+2*sizeof(float)+2
char measuring_data[PACKET_SIZE+1];

char* getDataString() {
  measuring_data[0] = (char)DEVICE_INDEX;
  float voltage = getVoltage();
  float current = getCurrent();
  *((float*)&measuring_data[1]) = voltage;
  *((float*)&measuring_data[1+sizeof(float)]) = current;
  measuring_data[PACKET_SIZE-2] = 0;
  measuring_data[PACKET_SIZE-1] = 0;
  for(int i=0; i<PACKET_SIZE; i++) {
    measuring_data[PACKET_SIZE-1] += measuring_data[i];
  }
  measuring_data[PACKET_SIZE] = 0;
  return measuring_data;
}

// Function, which gets executed when Arduino wakes up
void wakeUpNow()
{
  String byteRead = mySerial.readString();

  // If main controller is calling this controller
  // to get its data
  if(byteRead == String(DEVICE_INDEX)) {
    String res = getDataString();
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
