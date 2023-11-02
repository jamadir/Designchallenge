#include <Arduino.h>

class endstop {
private:
  int _pin;

public:
  endstop(int pin);
  bool getstate();
};


class usbport {
private:
  int _pwmpin;
  int _dutycycle;

public:
  usbport(int pwmpin);
  void setpower(int pwm);
};


class potentiometer {
private:
  int _analogpin;
  int _percentage;

public:
  potentiometer(int analogpin);
  int getstate();
};


class stepper {
private:
  int _steppin;
  int _dirpin;
  int _enpin;
  int _maxspeed;
  int _rangespeed;

public:
  stepper(int steppin, int dirpin, int enpin, int avspeed, int srange);
  void startmove();
  void stopmove();
  void switchdir(bool dir);
  void changespeed(int movespeed);
};
