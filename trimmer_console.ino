#include "Joystick.h"

// #define DEBUG 1
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

class Lever {
  uint8_t pin;
  int prev;
  double norm_prev;
  double gain;

public:
  Lever(uint8_t p) {
    this->pin = p;
  }

  void init(double gain) {
    pinMode(pin, INPUT);
    this->prev = 0;
    this->norm_prev = 0.0;
    this->gain = gain;
  }

  double update(double t_delta) {
    int sensorValue = analogRead(pin);
    DEBUG_PRINT("sensorValue=");
    DEBUG_PRINTLN(sensorValue);

    if (abs(sensorValue - 512) <= 15) {
      sensorValue = 512;
    }
    double norm = (sensorValue - 512) / 410.0;
    int delta = (int)(gain * t_delta * (norm_prev + norm) / 2.0);
    norm_prev = norm;

    int v = prev + delta;
    
    if (v > 127) {
      v = 127;
    }
    if (v < -127) {
      v = - 127;
    }
    prev = v;

    return v;
  }
};

class Button {
  uint8_t pin;
  int prev;

public:
  Button(uint8_t p) {
    pin = p;
  }

  void init() {
    pinMode(pin, INPUT_PULLUP);
    prev = 0;
  }

  int update() {
    int b = 1 - digitalRead(pin);
    DEBUG_PRINT("buttonValue=");
    DEBUG_PRINTLN(b);

    prev = b;
    return b;
  }
};

Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  4, 0,                 // button, hat
  true, true, false,    // x, y, z
  false, false, false,  // rx, ry, rz
  false, false,         // rudder, throttler
  false, false, false   // accel, brake, steering
);

unsigned long t_prev;

Lever xLever(A0);
Lever yLever(A1);

Button b0(A2);
Button b1(A3);
Button b2(A4);
Button b3(A5);

void setup() {
  Joystick.setXAxisRange(-127, 127);
  Joystick.setYAxisRange(-127, 127);
  Joystick.begin(false);

  xLever.init(0.25);
  yLever.init(0.25);

  b0.init();
  b1.init();
  b2.init();
  b3.init();

  t_prev = millis();
}

void loop() {
  unsigned long t = millis();
  unsigned long t_delta;
  if (t >= t_prev) {
    t_delta = t - t_prev;
  } else {
    t_delta = 0xFFFFFFFF - t_prev + t;
  }
  DEBUG_PRINT("t=");
  DEBUG_PRINTLN(t);
  DEBUG_PRINT("t_prev=");
  DEBUG_PRINTLN(t_prev);
  DEBUG_PRINT("t_delta=");
  DEBUG_PRINTLN(t_delta);

  int x = xLever.update(t_delta);
  int y = yLever.update(t_delta);

  Joystick.setXAxis(x);
  Joystick.setXAxis(y);

  Joystick.setButton(0, b0.update());
  Joystick.setButton(1, b1.update());
  Joystick.setButton(2, b2.update());
  Joystick.setButton(3, b3.update());
  Joystick.sendState();

  t_prev = t;

  delay(10);
}
 