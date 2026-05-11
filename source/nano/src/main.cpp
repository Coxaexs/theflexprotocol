// arduino nano 33 ble - beacon (broadcaster)
#include <ArduinoBLE.h>

const char* deviceName = "Nano";
const int EMG_PIN_A3 = A3;
const int EMG_PIN_A4 = A4;

// Gesture enum
enum Gesture {
  OPEN = 0,
  CLOSE = 1,
  PINCH = 2,
  POINT = 3,
  THUMBSUP = 4,
  RELAX = 5
};

// TinyML inference function (fake implementation)
Gesture runTinyMLInference(int emg_a3, int emg_a4) {
  // TODO: Replace with actual TinyML model inference
  // For now, since idk how this works just weird things it's wrong tho'
  int combined = emg_a3 + emg_a4;
  
  if (combined < 200) return RELAX;
  if (combined < 400) return OPEN;
  if (combined < 600) return PINCH;
  if (combined < 800) return POINT;
  if (combined < 1000) return CLOSE;
  return THUMBSUP;
}

void setup() {
  Serial.begin(9600);
  pinMode(EMG_PIN_A3, INPUT);
  pinMode(EMG_PIN_A4, INPUT);
  
  if (!BLE.begin()) while (1);

  BLE.setLocalName(deviceName);
  BLE.advertise();
  Serial.println("nano active");
}

void loop() {
  // Read EMG from both channels
  int emg_a3 = analogRead(EMG_PIN_A3);
  int emg_a4 = analogRead(EMG_PIN_A4);
  
  // Run TinyML inference
  Gesture recognizedGesture = runTinyMLInference(emg_a3, emg_a4);
  
  // Prepare BLE data: gesture as single byte
  unsigned char data[1];
  data[0] = (unsigned char)recognizedGesture;

  BLE.stopAdvertise();
  BLE.setManufacturerData(data, 1);
  BLE.advertise();
  
  Serial.print("A3: "); Serial.print(emg_a3);
  Serial.print(" | A4: "); Serial.print(emg_a4);
  Serial.print(" | Gesture: "); Serial.println(recognizedGesture);
  delay(500);
}
