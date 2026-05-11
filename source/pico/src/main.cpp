// raspberry pi pico w - listener 1 (leds)
#include <ArduinoBLE.h>

const int ledPins[] = {12, 13, 14, 15};

void setup() {
  Serial.begin(9600);
  for (int i=0; i<4; i++) pinMode(ledPins[i], OUTPUT);
  
  if (!BLE.begin()) while (1);
  BLE.scan(true);
  Serial.println("looking for nano");
}

void loop() {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    if (peripheral.hasLocalName() && peripheral.localName() == "Nano") {
      if (peripheral.hasManufacturerData()) {
        int len = peripheral.manufacturerDataLength();
        unsigned char data[len];
        peripheral.manufacturerData(data, len);
        
        int tempInt = (data[0] << 8) | data[1];
        float temp = tempInt / 100.0;
        
        Serial.print("received: "); Serial.println(temp);
        controlLEDs(temp);
      }
    }
  }
}

void controlLEDs(float t) {
  for (int i=0; i<4; i++) digitalWrite(ledPins[i], LOW);
  if (t > 0)  digitalWrite(ledPins[0], HIGH);
  if (t > 10) digitalWrite(ledPins[1], HIGH);
  if (t > 20) digitalWrite(ledPins[2], HIGH);
  if (t > 30) digitalWrite(ledPins[3], HIGH);
}
