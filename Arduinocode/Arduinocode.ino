#include "HardwareIO.h"

#define usbpwmpin 3
#define endstop1pin 7
#define endstop2pin 9
#define bestaetigenpin 8
#define pot1pin 19
#define pot2pin 20
#define stepperenpin 18
#define stepper1steppin 15
#define stepper1dirpin 14
#define stepper2steppin 16
#define stepper2dirpin 10


bool toggle = false;
uint32_t myBitArray = 0;
int statecounter = 0;

endstop anfang1(endstop1pin);
endstop ende1(endstop2pin);
endstop anfang2(endstop1pin);
endstop ende2(endstop1pin);

endstop bestaeigen(bestaetigenpin);

usbport usb(usbpwmpin);

potentiometer poti1(pot1pin);
potentiometer poti2(pot2pin);

stepper stepperm1(stepper1steppin, stepper1dirpin, stepperenpin);
stepper stepperm2(stepper2steppin, stepper2dirpin, stepperenpin);



void setup() {
  Serial.begin(9600);

  //set timer0 interrupt at 2kHz
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0 = 0;

  OCR0A = 124;
  TCCR0A |= (1 << WGM01);
  TCCR0B |= (1 << CS01) | (1 << CS00);
  TIMSK0 |= (1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect) {  //timer0 interrupt 2kHz toggles pin 8

  //if(bitRead(myBitArray,31)){
  digitalWrite(stepper1steppin, toggle);
  digitalWrite(stepper2steppin, toggle);
  toggle = !toggle;
  //  myBitArray >> 1
  //  bitSet(myBitArray, 0);
  //}else{
  //  myBitArray >> 1
  //  bitClear(myBitArray, 0);
  //}
}


void loop() {

  int usbpower = map(poti1.getstate(), 0, 100, 0, 254);
  usb.setpower(usbpower);

  stepperm1.changespeed(poti2.getstate());
  stepperm2.changespeed(poti2.getstate());



  switch (statecounter) {
    case 0:
      if(!anfang1.getstate()){
        stepperm1.switchdir(false);
        stepperm1.startmove();
      }
      if(!anfang2.getstate()){
        stepperm2.switchdir(false);
        stepperm1.startmove();
      }else{
        if(anfang1.getstate()){
          statecounter = 1;
        }
      }

    case 1:
      if(bestaeigen.getstate()){
        statecounter = 2;
      }

    case 2:
      if(ende1.getstate()){
        stepperm1.stopmove();
      }
      if(ende2.getstate()){
        stepperm2.stopmove();
      }

  }
}
