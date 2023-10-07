#include "HardwareIO.h"

endstop::endstop(int pin){
  pin = pin;
  pinMode(pin, INPUT);
}

bool endstop::getstate(){
  return digitalRead(pin);
}


usbport::usbport(int pwmpin) {
  pwmpin = pwmpin;
}

void usbport::setpower(int pwm) {
  analogWrite(pwmpin, pwm);
}


potentiometer::potentiometer(int pin) {
  analogpin = pin;
}

int potentiometer::getstate() {
  int mappedValue = map(analogRead(analogpin), 0, 1023, 0, 100);
  return mappedValue;
}


stepper::stepper(int spin, int dpin, int epin) {
  steppin = spin;
  dirpin = dpin;
  enpin = epin;

  bool movedir = false;
  bool enabled = true;
  int movespeed = 10;

  pinMode(steppin, OUTPUT);
  pinMode(dirpin, OUTPUT);
  pinMode(enpin, OUTPUT);
}


void stepper::startmove() {
  digitalWrite(enpin, true);
}
void stepper::stopmove() {
  digitalWrite(enpin, false);
}


void stepper::changespeed(int moveval) {

  cli();

  int c = 31;
  int c2;
  while (c >= 0) {

  }
  sei();
}


void stepper::switchdir(bool dir) {
  movedir = dir;
  digitalWrite(dirpin, movedir);
}
