#include "HardwareIO.h"

endstop::endstop(int pin) {
  _pin = pin;
  pinMode(_pin, INPUT_PULLUP);
}

bool endstop::getstate() {
  return !digitalRead(_pin);    //Invertiert da schließer
}


usbport::usbport(int pwmpin) {
  _pwmpin = pwmpin;
}

void usbport::setpower(int pwm) {
  analogWrite(_pwmpin, pwm);
}


potentiometer::potentiometer(int pin) {
  _analogpin = pin;
}

int potentiometer::getstate() {
  int mappedValue = map(analogRead(_analogpin), 0, 1023, 0, 100);
  return mappedValue;
}


stepper::stepper(int spin, int dpin, int epin, int mspeed, int srange) {
  _steppin = spin;
  _dirpin = dpin;
  _enpin = epin;
  _maxspeed = mspeed;
  _rangespeed = srange;


  pinMode(_steppin, OUTPUT);
  pinMode(_dirpin, OUTPUT);
  pinMode(_enpin, OUTPUT);
}


void stepper::startmove() {
  digitalWrite(_enpin, true);
}
void stepper::stopmove() {
  digitalWrite(_enpin, false);
}


void stepper::changespeed(int moveval) {
  cli();
  //=(16*10^A2)/(B1*A1)-1

  int speed = map(moveval, 0, 100, 0, _rangespeed) * _maxspeed;
  OCR1A = speed;

  sei();
}


void stepper::switchdir(bool dir) {
  digitalWrite(_dirpin, dir);
}
