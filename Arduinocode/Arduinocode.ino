#include "HardwareIO.h"


// Definiere input pins auf der platine
#define usbpwmpin 3  //PWM steuerung für USB Port

#define endstop1pina 4  //Endstops für Fahrrad 1
#define endstop1pine 5
#define endstop2pina 6  //Endstops für Fahhrad 2
#define endstop2pine 7

#define bestaetigenpin 8  //Bestätigen button
#define blankpin 9

#define pot1pin 19  //Poti Speed
#define pot2pin 20  //Poti Pumpe

#define stepper1enpin 21  //Stepper enable
#define stepper2enpin 18
#define stepper1steppin 15  //Stepper step
#define stepper2steppin 16
#define stepper1dirpin 14  //Stepper dir
#define stepper2dirpin 10

#define Motorspeedrange 10    //mindestgeschwindigkeit = maxspeed/Motorspeedrange

#define Maxmotorspeedmm 10    //Geschwindigkeit Fahrrad in mm pro Sekunde 
#define Maxmotorspeed (16000000)/(3*Maxmotorspeedmm*64)-1    //=(16000000)/(Hz*64)-1   Resultierendes compare match register


bool toggle = false;
uint32_t myBitArray = 0;
int statecounter = 0;

endstop anfang1(endstop1pina);  //Endstops initialisieren
endstop ende1(endstop1pine);
endstop anfang2(endstop2pina);
endstop ende2(endstop2pine);

endstop bestaeigen(bestaetigenpin);  //bestätigen initialisieren

usbport usb(usbpwmpin);

potentiometer poti1(pot1pin);  //Potis initialisieren
potentiometer poti2(pot2pin);

stepper stepperm1(stepper1steppin, stepper1dirpin, stepper1enpin, Maxmotorspeed, Motorspeedrange);  //Stepper initialisieren
stepper stepperm2(stepper2steppin, stepper2dirpin, stepper2enpin, Maxmotorspeed, Motorspeedrange);



void setup() {


  if (statecounter == -1) {
    Serial.begin(115200);
  }


  //set timer1 interrupt at 200kHz   NotwendUig für Stepper geschwindigkeitssteuerung
  TCCR1A = 0;  // set entire TCCR1A register to 0
  TCCR1B = 0;  // same for TCCR1B
  TCNT1 = 0;   //initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 1249;  // = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 64 prescaler
  TCCR1B |= (1 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {  //timer0 interrupt 2kHz toggles pin 8  Stepper geschwindigkeitssteuerung

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

  int usbpower = map(analogRead(pot2pin), 0, 255, 0, 254);  //Poti.getstate returnt spannung des potis von 0 bis 100
  usb.setpower(usbpower);                                // setze usb pwm wert

  stepperm1.changespeed(analogRead(pot1pin));  //stepper.changespeed nimmt wert von 0 bis 100 als geschwindigkeit aus poti
  stepperm2.changespeed(analogRead(pot1pin));



  Serial.println(statecounter);
  switch (statecounter) {  //Start Schrittkette
    case 0:
      if (!digitalRead(endstop1pina)) {     //solange anfangstaster nicht gedrückt
        digitalWrite(stepper1dirpin, 0);  //direction ist false -> rückwärts
        digitalWrite(stepper1enpin, 1);
        //starte stepper bewegung
      } else {
        digitalWrite(stepper1enpin, 0);  //anhalten
      }
      if (!digitalRead(endstop2pina)) {
        digitalWrite(stepper2dirpin, 0);
        digitalWrite(stepper2enpin, 1);
      } else {
        digitalWrite(stepper2enpin, 0);
      }
      if (digitalRead(endstop1pina) and digitalRead(endstop1pina)) {  //wenn beide am anfang sind nächster schritt
        statecounter = 1;
      }
      break;


    case 1:

      //stepperm1.changespeed(poti2.getstate());  //Die geschwindigkeit wird permanent aktualisiert
      //stepperm2.changespeed(poti2.getstate());

      if (bestaeigen.getstate()) {  //bei bestätigung nächster Schritt
        statecounter = 2;
      }
      break;

    case 2:

      if (!digitalRead(endstop1pine)) {      //solange endtaster nicht gedrückt
        digitalWrite(stepper1dirpin, 1);  //direction ist true -> Vorwärts
        digitalWrite(stepper1enpin, 1);      //starte stepper bewegung
      } else {
        digitalWrite(stepper1enpin, 0);  //anhalten
      }
      if (!digitalRead(endstop2pine)) {
        digitalWrite(stepper2dirpin, 1);
        digitalWrite(stepper2enpin, 1);
      } else {
        digitalWrite(stepper2enpin, 0);
      }
      if (digitalRead(endstop1pine) and digitalRead(endstop2pine)) {  //beginne neu
        statecounter = 0;
      }
      break;

    case -1:  //Debugging outputs werden im seriellen Monitor dargestellt
      Serial.println("endstops:");

      Serial.print("   a1:\t");
      Serial.print(anfang1.getstate());
      Serial.print("   e1:\t");
      Serial.print(ende1.getstate());
      Serial.print("   a2:\t");
      Serial.print(anfang1.getstate());
      Serial.print("   e2:\t");
      Serial.println(ende1.getstate());

      Serial.print("Potis:");
      Serial.print("   P1:\t");
      Serial.print(poti1.getstate());
      Serial.print("   P2:\t");
      Serial.print(poti2.getstate());
      Serial.print("   Stp:\t");
      Serial.println(statecounter);


      stepperm1.switchdir(true);
      stepperm1.changespeed(50);
      stepperm1.startmove();

      delay(100);

      break;
  }
}
