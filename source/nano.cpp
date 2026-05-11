// arduino nano 33 ble - beacon (broadcaster)
#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>

const char* deviceName = "Nano";

void setup() {
  Serial.begin(9600);
  HTS.begin();
  if (!BLE.begin()) while (1);

  BLE.setLocalName(deviceName);
  BLE.advertise();
  Serial.println("nano active");
}

void loop() {
  float temp = HTS.readTemperature();
  int tempInt = (int)(temp * 100); 
  
  unsigned char data[2];
  data[0] = (tempInt >> 8) & 0xFF;
  data[1] = tempInt & 0xFF;

  BLE.stopAdvertise();
  BLE.setManufacturerData(data, 2);
  BLE.advertise();
  
  Serial.print("broadcasting: "); Serial.println(temp);
  delay(500);
}