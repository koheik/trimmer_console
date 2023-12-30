#include "Joystick.h"

// #define DEBUG 1
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  2, 0,                 // button, hat
  true, false, false,    // x, y, z
  false, false, false,  // rx, ry, rz
  false, false,         // rudder, throttler
  false, false, false   // accel, brake, steering
);

unsigned long t_prev;
int sensorPinX;
int sensorPinB1;
int sensorPinB2;
int x_prev;
double x_norm_prev;
double x_gain;
int b1_prev;
int b2_prev;

void setup() {
  Joystick.setXAxisRange(-127, 127);
  Joystick.begin(false);

  sensorPinX = A0;
  sensorPinB1 = A2;
  sensorPinB2 = A3;

  pinMode(sensorPinX, INPUT);
  pinMode(sensorPinB1, INPUT);
  pinMode(sensorPinB2, INPUT);

  t_prev = millis();
  x_prev = 0;
  x_norm_prev = 0.0;
  x_gain = 0.25;

  b1_prev = 0;
  b2_prev = 0;
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

  int sensorValueX = analogRead(sensorPinX);
  DEBUG_PRINT("sensorValueX=");
  DEBUG_PRINTLN(sensorValueX);

  if (abs(sensorValueX - 512) <= 3) {
    sensorValueX = 512;
  }
  double x_norm = (sensorValueX - 512) / 410.0;

  DEBUG_PRINT("x_norm=");
  DEBUG_PRINTLN(x_norm);

  int x_delta = (int)(x_gain * t_delta * (x_norm_prev + x_norm) / 2.0);
  int x = x_prev + x_delta;
  if (x > 127) {
    x = 127;
  }
  if (x < -127) {
    x = - 127;
  }
  DEBUG_PRINT("x=");
  DEBUG_PRINTLN(x);

  Joystick.setXAxis(x);

  int b1 = digitalRead(sensorPinB1);
  int b2 = digitalRead(sensorPinB2);

  DEBUG_PRINT("b1=");
  DEBUG_PRINTLN(b1);

  DEBUG_PRINT("b2=");
  DEBUG_PRINTLN(b2);

  if (b1 != b1_prev) {
    Joystick.setButton(0, b1);
  }

  if (b2 != b2_prev) {
    Joystick.setButton(1, b2);
  }

  Joystick.sendState();

  t_prev = t;
  x_prev = x;
  x_norm_prev = x_norm;

  delay(10);
}
