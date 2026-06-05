#include <ArduinoBLE.h>
#include <math.h>
#include <string.h>
#define THRESHOLD 100

const char *deviceName = "Nano";
const int EMG_PIN_A3 =
    A3; // I think these gotta be a0 and a1, maybe change it idk
const int EMG_PIN_A4 = A4;
const unsigned long DATASET_SAMPLE_INTERVAL_MS = 40;

// UUIDs, gotta match pico so dont touch
const char *PICO_SERVICE_UUID = "c3feed70-b50c-400a-836c-c8981beb0b1c";
const char *PICO_CMD_CHAR_UUID = "c3feed71-b50c-400a-836c-c8981beb0b1c";
const char *PICO_STATE_CHAR_UUID = "c3feed72-b50c-400a-836c-c8981beb0b1c";

struct EmgSample {
  unsigned long timestampMs;
  int a3;
  int a4;
};

bool datasetCaptureActive = false;
bool testMode = true; // make it false when actual EMG is connected, it now
                      // sends open and closed palm like very second
char serialCommandBuffer[48];
size_t serialCommandLength = 0;
unsigned long lastGestureSendTime = 0;

// Gesture enum
enum Gesture {
  OPEN = 0,
  CLOSE = 1,
  PINCH = 2,
  POINT = 3,
  THUMBSUP = 4,
  RELAX = 5
};

// BLE peripheral reference
BLEDevice picoDevice;
BLECharacteristic cmdCharacteristic;

// TinyML inference function
Gesture runTinyMLInference(int emg_a3, int emg_a4) {
  int combined = emg_a3 + emg_a4;

  if (combined < 200)
    return RELAX;
  if (combined < 400)
    return OPEN;
  if (combined < 600)
    return PINCH;
  if (combined < 800)
    return POINT;
  if (combined < 1000)
    return CLOSE;
  return THUMBSUP;
}
// yeah uhhh this is where actual ml is gonna be

const char *gestureToString(Gesture gesture) {
  switch (gesture) {
  case OPEN:
    return "open";
  case CLOSE:
    return "fist";
  case PINCH:
    return "pinch";
  case POINT:
    return "point";
  case THUMBSUP:
    return "like";
  case RELAX:
    return "relax";
  }
  return "unknown";
}

EmgSample getDatasetSample() {
  EmgSample sample;
  sample.timestampMs = millis();
  sample.a3 = analogRead(EMG_PIN_A3);
  sample.a4 = analogRead(EMG_PIN_A4);
  return sample;
}

void printDatasetSample(const EmgSample &sample) {
  Serial.print(sample.timestampMs);
  Serial.print(',');
  Serial.print(sample.a3);
  Serial.print(',');
  Serial.println(sample.a4);
}

void sendGestureCommand(Gesture gesture) {
  if (picoDevice && picoDevice.connected()) {
    const char *cmd = gestureToString(gesture);
    if (cmdCharacteristic.canWrite()) {
      if (cmdCharacteristic.writeValue(cmd)) {
        Serial.print("arduino: Sent command to Pico: ");
        Serial.println(cmd);
        Serial.flush();
      } else {
        Serial.print("arduino: Failed to write command: ");
        Serial.println(cmd);
        Serial.flush();
      }
    } else {
      Serial.println("arduino: Characteristic not writable");
      Serial.flush();
    }
  } else {
    Serial.println("arduino: Not connected, cannot send command");
    Serial.flush();
  }
}

void handleSerialCommand(const char *command) {
  if (strcmp(command, "BEGIN_CAPTURE") == 0) {
    datasetCaptureActive = true;
    Serial.println("BEGIN_CAPTURE");
    return;
  }

  if (strcmp(command, "END_CAPTURE") == 0) {
    datasetCaptureActive = false;
    Serial.println("END_CAPTURE");
    return;
  }
}

void pollSerialCommands() {
  while (Serial.available() > 0) {
    char incoming = static_cast<char>(Serial.read());

    if (incoming == '\r') {
      continue;
    }

    if (incoming == '\n') {
      serialCommandBuffer[serialCommandLength] = '\0';
      if (serialCommandLength > 0) {
        handleSerialCommand(serialCommandBuffer);
      }
      serialCommandLength = 0;
      continue;
    }

    if (serialCommandLength < sizeof(serialCommandBuffer) - 1) {
      serialCommandBuffer[serialCommandLength++] = incoming;
    }
  }
}
void connectToPico() {
  Serial.println("arduino: Attempting to connect to Pico W");
  // I missed the esp..
  Serial.flush();

  Serial.println("arduino: Scanning for Pico W...");
  Serial.flush();

  // FIXED: Changed scanForUUID to scanForUuid
  if (BLE.scanForUuid(PICO_SERVICE_UUID)) {
    Serial.println(
        "arduino: Scan started, looking for devices (5s timeout)...");
    Serial.flush();

    unsigned long startMillis = millis();
    BLEDevice peripheral;

    while (millis() - startMillis < 5000) {
      peripheral = BLE.available();
      if (peripheral) {
        break;
      }
      delay(10);
    }

    BLE.stopScan();

    if (peripheral) {
      Serial.print("arduino:  Found device:  woohoo");
      Serial.println(peripheral.address());
      Serial.flush();

      Serial.println("arduino: Scan stopped, attempting to connect...");
      Serial.flush();

      if (peripheral.connect()) {
        Serial.println(
            "arduino: Connected to Pico W! Discovering attributes...");
        Serial.flush();

        // Discover attributes
        if (peripheral.discoverAttributes()) {
          Serial.println("arduino: Attributes discovered");
          Serial.flush();

          // Get the command characteristic
          cmdCharacteristic = peripheral.characteristic(PICO_CMD_CHAR_UUID);

          if (cmdCharacteristic) {
            Serial.println("arduino:  Found command characteristic!");
            picoDevice = peripheral;
            Serial.println(
                "arduino:  Connection complete, ready to send gestures");
            Serial.flush();
          } else {
            Serial.println("arduino: Command characteristic not found");
            Serial.flush();
            peripheral.disconnect();
          }
        } else {
          Serial.println("arduino: Failed to discover attributes");
          Serial.flush();
          peripheral.disconnect();
        }
      } else {
        Serial.println("arduino: Failed to connect to Pico W");
        Serial.flush();
      }
    } else {
      Serial.println("arduino: No device found in scan");
      Serial.flush();
    }
  } else {
    Serial.println("arduino: Scan failed");
    Serial.flush();
  }
}

void setup() {
  // Initialize serial FIRST, no delays
  Serial.begin(115200);
  Serial.println("");
  Serial.println("arduino:  SETUP START  SETUP START SETUP START SETUP START");
  Serial.flush();

  // Wait for port to stabilize
  delay(2000);

  Serial.println("arduino: Serial initialized, starting hardware setup...");
  Serial.flush();

  analogReadResolution(16);
  pinMode(EMG_PIN_A3, INPUT);
  pinMode(EMG_PIN_A4, INPUT);
  Serial.println("arduino: EMG pins configured");
  Serial.flush();

  Serial.println("arduino: Initializing BLE");
  Serial.flush();

  if (!BLE.begin()) {
    Serial.println("[THECOREISDEADWEAREALLGONNADIE] Failed to initialize BLE!");
    Serial.flush();
    while (1)
      delay(100);
  }

  Serial.println("arduino: BLE initialized successfully");
  Serial.flush();

  BLE.setLocalName(deviceName);
  BLE.advertise();
  Serial.println(
      "arduino: BLE advertising started, ready to connect to Pico W");
  Serial.println(
      "arduino:  SETUP COMPLETE SETUP COMPLETE SETUP COMPLETE SETUP COMPLETE");
  Serial.flush();
}

void loop() {
  pollSerialCommands();

  if (datasetCaptureActive) {
    printDatasetSample(getDatasetSample());
    delay(DATASET_SAMPLE_INTERVAL_MS);
    return;
  }

  // Try to connect to Pico if not connected
  if (!picoDevice || !picoDevice.connected()) {
    static unsigned long lastConnectionAttempt = 0;
    unsigned long now = millis();

    // Only attempt connection every 3 seconds to avoid spam
    if (now - lastConnectionAttempt >= 3000) {
      Serial.println("\narduino: Connection status: NOT CONNECTED");
      connectToPico();
      lastConnectionAttempt = now;
    }
    delay(100);
    return;
  }

  // Connected! Show status periodically
  static unsigned long lastStatusPrint = 0;
  if (millis() - lastStatusPrint >= 5000) {
    Serial.println("arduino: Status: Connected to Pico W, sending gestures");
    lastStatusPrint = millis();
  }

  // Test mode: send a cycling gesture every second
  if (testMode) {
    unsigned long now = millis();
    if (now - lastGestureSendTime >= 1000) { // Send every 1000ms
      static int testGestureIndex = 0;
      Gesture testGestures[] = {OPEN, CLOSE};
      Gesture gesture = testGestures[testGestureIndex % 2];

      sendGestureCommand(gesture);
      testGestureIndex++;
      lastGestureSendTime = now;

      Serial.print("[TEST] Sent gesture: ");
      Serial.println(gestureToString(gesture));
      Serial.flush();
    }
    delay(50); // Small delay to avoid busy loop
    return;
  } else {

    // the actual xode and yes its commented out dont ask why
    /*
    // Read EMG from both channels
    int emg_a3 = analogRead(EMG_PIN_A3);
    int emg_a4 = analogRead(EMG_PIN_A4);

    // Run TinyML inference
    Gesture recognizedGesture = runTinyMLInference(emg_a3, emg_a4);

    // Send gesture command to Pico W
    sendGestureCommand(recognizedGesture);

    Serial.print("arduino: Gesture: ");
    Serial.print(gestureToString(recognizedGesture));
    Serial.print(" | A3: ");
    Serial.print(emg_a3);
    Serial.print(" | A4: ");
    Serial.println(emg_a4);

    delay(200);
    */
  }
}
