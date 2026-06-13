#include <Servo.h>

// Pico W GPIO pins for servo control (PWM capable)
#define SERVO_PINKIE 16
#define SERVO_RING 17
#define SERVO_MIDDLE 18
#define SERVO_POINT 19
#define SERVO_THUMB 20

class Hand {
private:
  Servo pinkie;
  Servo ring;
  Servo middle;
  Servo point;
  Servo thumb;

public:
  Hand();
  ~Hand();
  bool MiddleFinger();
  bool OpenPalm();
  bool Fist();
  bool Like();
  bool Point();
};

Hand::Hand() {
  pinkie.attach(SERVO_PINKIE);
  pinkie.write(180);

  ring.attach(SERVO_RING);
  ring.write(180);

  middle.attach(SERVO_MIDDLE);
  middle.write(180);

  point.attach(SERVO_POINT);
  point.write(180);

  thumb.attach(SERVO_THUMB);
  thumb.write(180);

  // initial configuration, palm open essentially.
}

Hand::~Hand() {
  // detach servos
  pinkie.detach();
  ring.detach();
  middle.detach();
  point.detach();
  thumb.detach();
}

bool Hand::MiddleFinger() {
  pinkie.attach(SERVO_PINKIE);
  pinkie.write(0);

  ring.attach(SERVO_RING);
  ring.write(0);

  middle.attach(SERVO_MIDDLE);
  middle.write(180);

  point.attach(SERVO_POINT);
  point.write(0);

  thumb.attach(SERVO_THUMB);
  thumb.write(0);
  return true;
}

bool Hand::OpenPalm() {
  pinkie.attach(SERVO_PINKIE);
  pinkie.write(180);

  ring.attach(SERVO_RING);
  ring.write(180);

  middle.attach(SERVO_MIDDLE);
  middle.write(180);

  point.attach(SERVO_POINT);
  point.write(180);

  thumb.attach(SERVO_THUMB);
  thumb.write(180);
  return true;
}

bool Hand::Fist() {
  pinkie.attach(SERVO_PINKIE);
  pinkie.write(0);

  ring.attach(SERVO_RING);
  ring.write(0);

  middle.attach(SERVO_MIDDLE);
  middle.write(0);

  point.attach(SERVO_POINT);
  point.write(0);

  thumb.attach(SERVO_THUMB);
  thumb.write(0);
  return true;
}

bool Hand::Like() {
  pinkie.attach(SERVO_PINKIE);
  pinkie.write(0);

  ring.attach(SERVO_RING);
  ring.write(0);

  middle.attach(SERVO_MIDDLE);
  middle.write(0);

  point.attach(SERVO_POINT);
  point.write(0);

  thumb.attach(SERVO_THUMB);
  thumb.write(180);
  return true;
}

bool Hand::Point() {
  pinkie.attach(SERVO_PINKIE);
  pinkie.write(0);

  ring.attach(SERVO_RING);
  ring.write(0);

  middle.attach(SERVO_MIDDLE);
  middle.write(0);

  point.attach(SERVO_POINT);
  point.write(180);

  thumb.attach(SERVO_THUMB);
  thumb.write(0);
  return true;
}
