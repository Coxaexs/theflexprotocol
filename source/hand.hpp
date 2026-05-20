#include "external/src/ESP32Servo.h" 

//GPIO pins defined for Servos.
#define SERVO_PINKIE 37
#define SERVO_RING 36
#define SERVO_MIDDLE 35
#define SERVO_POINT 34
#define SERVO_THUMB 33

class Hand
{
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

Hand::Hand()
{
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

    //initial configuration, palm open essentially.
}

Hand::~Hand()
{
    // detach servos
    pinkie.detach();
    ring.detach();
    middle.detach();
    point.detach();
    thumb.detach();
}

bool Hand::MiddleFinger()
{
    pinkie.attach(SERVO_PINKIE);
    pinkie.write(90);

    ring.attach(SERVO_RING);
    ring.write(90);

    middle.attach(SERVO_MIDDLE);
    middle.write(180);

    point.attach(SERVO_POINT);
    point.write(90);

    thumb.attach(SERVO_THUMB);
    thumb.write(90);
    return true;
}

bool Hand::OpenPalm()
{
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
}

bool Hand::Fist()
{
    pinkie.attach(SERVO_PINKIE);
    pinkie.write(90);

    ring.attach(SERVO_RING);
    ring.write(90);

    middle.attach(SERVO_MIDDLE);
    middle.write(90);

    point.attach(SERVO_POINT);
    point.write(90);

    thumb.attach(SERVO_THUMB);
    thumb.write(90);
}
bool Hand::Like()
{
    pinkie.attach(SERVO_PINKIE);
    pinkie.write(90);

    ring.attach(SERVO_RING);
    ring.write(90);

    middle.attach(SERVO_MIDDLE);
    middle.write(90);

    point.attach(SERVO_POINT);
    point.write(90);

    thumb.attach(SERVO_THUMB);
    thumb.write(180);
}
bool Hand::Point()
{
    pinkie.attach(SERVO_PINKIE);
    pinkie.write(90);

    ring.attach(SERVO_RING);
    ring.write(90);

    middle.attach(SERVO_MIDDLE);
    middle.write(90);

    point.attach(SERVO_POINT);
    point.write(180);

    thumb.attach(SERVO_THUMB);
    thumb.write(90);
}
