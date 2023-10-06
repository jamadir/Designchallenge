#include <Arduino.h>

class endstop {
private:
  int pin;

public:
  endstop(int pin);
  bool getstate();
};


class usbport {
private:
  int pwmpin;
  int dutycycle;

public:
  usbport(int pwmpin);
  void setpower(int pwm);
};


class potentiometer {
private:
  int analogpin;
  int percentage;

public:
  potentiometer(int analogpin);
  int getstate();
};


class stepper {
private:
  int steppin;
  int dirpin;
  int enpin;
  bool movedir = false;
  bool enabled = true;
  int movespeed = 10;

public:
  stepper(int steppin, int dirpin, int enpin);
  void startmove();
  void stopmove();
  void switchdir();
  void changespeed(int movespeed);
};
