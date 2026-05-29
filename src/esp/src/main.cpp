// esp32 lilygo - listener 2 (display)
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <TFT_eSPI.h> 
#include "../../hand.hpp"

TFT_eSPI tft = TFT_eSPI(); 
BLEScan* pBLEScan;
Hand hand;

// EMG thresholds for gesture recognition
const int EMG_THRESHOLD_LOW = 200;
const int EMG_THRESHOLD_HIGH = 600;

enum Gesture {
  OPEN_PALM,
  FIST,
  POINT,
  LIKE,
  MIDDLE_FINGER
};

void updateScreen(const char* gesture, int a3, int a4);
Gesture recognizeGesture(int emg_a3, int emg_a4);
void performGesture(Gesture g);

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (advertisedDevice.getName() == "Nano") {
        if (advertisedDevice.haveManufacturerData()) {
          std::string data = advertisedDevice.getManufacturerData();
          // Parse EMG data: bytes 0-1 (A3), bytes 2-3 (A4)
          int emg_a3 = ((unsigned char)data[0] << 8) | (unsigned char)data[1];
          int emg_a4 = ((unsigned char)data[2] << 8) | (unsigned char)data[3];
          
          Gesture detectedGesture = recognizeGesture(emg_a3, emg_a4);
          performGesture(detectedGesture);
          
          const char* gestureName;
          switch(detectedGesture) {
            case FIST: gestureName = "FIST"; break;
            case POINT: gestureName = "POINT"; break;
            case LIKE: gestureName = "LIKE"; break;
            case MIDDLE_FINGER: gestureName = "MIDDLE"; break;
            default: gestureName = "OPEN"; break;
          }
          updateScreen(gestureName, emg_a3, emg_a4);
        }
      }
    }
};

Gesture recognizeGesture(int emg_a3, int emg_a4) {
  // Simple gesture recognition based on EMG thresholds
  bool a3_low = emg_a3 < EMG_THRESHOLD_LOW;
  bool a3_mid = emg_a3 >= EMG_THRESHOLD_LOW && emg_a3 < EMG_THRESHOLD_HIGH;
  bool a3_high = emg_a3 >= EMG_THRESHOLD_HIGH;
  
  bool a4_low = emg_a4 < EMG_THRESHOLD_LOW;
  bool a4_mid = emg_a4 >= EMG_THRESHOLD_LOW && emg_a4 < EMG_THRESHOLD_HIGH;
  bool a4_high = emg_a4 >= EMG_THRESHOLD_HIGH;
  
  // Gesture logic based on EMG combinations
  if (a3_low && a4_low) {
    return OPEN_PALM;
  } else if (a3_high && a4_high) {
    return FIST;
  } else if (a3_high && a4_low) {
    return POINT;
  } else if (a3_low && a4_high) {
    return LIKE;
  } else if (a3_high && a4_mid) {
    return MIDDLE_FINGER;
  }
  return OPEN_PALM;
}

void performGesture(Gesture g) {
  switch(g) {
    case OPEN_PALM:
      hand.OpenPalm();
      break;
    case FIST:
      hand.Fist();
      break;
    case POINT:
      hand.Point();
      break;
    case LIKE:
      hand.Like();
      break;
    case MIDDLE_FINGER:
      hand.MiddleFinger();
      break;
  }
}

void updateScreen(const char* gesture, int a3, int a4) {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 20);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(3);
  tft.println(gesture);
  
  tft.setCursor(10, 70);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.print("A3: ");
  tft.println(a3);
  
  tft.setCursor(10, 100);
  tft.print("A4: ");
  tft.println(a4);
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.println("scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(1, false);
  pBLEScan->clearResults();   
}
