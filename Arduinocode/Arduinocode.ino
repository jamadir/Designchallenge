#include "HardwareIO.h"


// Definiere input pins auf der platine
#define usbpwmpin 3  //PWM steuerung für USB Port

#define endstop1pina 6  //Endstops für Fahrrad 1
#define endstop1pine 7
#define endstop2pina 8  //Endstops für Fahrrad 2
#define endstop2pine 9

#define bestaetigenpin 4  //Bestätigen button
#define blankpin 5

#define pot1pin 15  //Poti Speed
#define pot2pin 16  //Poti Pumpe


#define stepper1enpin 17    //Stepper enable Fahrrad 1
#define stepper1steppin 14  //Stepper step
#define stepper1dirpin 13   //Stepper dir

#define stepper2enpin 12    //Stepper enable Fahrrad 2
#define stepper2steppin 1  //Stepper step
#define stepper2dirpin 0   //Stepper dir


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

  //if (statecounter == -1) {
  //Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);


  //set timer1 interrupt at 200kHz   Notwendig für Stepper geschwindigkeitssteuerung
  //TCCR1A = 0;  // set entire TCCR1A register to 0
  //TCCR1B = 0;  // same for TCCR1B
  //TCNT1 = 0;   //initialize counter value to 0
  // set compare match register for 1hz increments
  //OCR1A = 1249;  // = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  //TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 64 prescaler
  //TCCR1B |= (1 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  //TIMSK1 |= (1 << OCIE1A);
}

//ISR(TIMER1_COMPA_vect) {  //timer0 interrupt 2kHz toggles pin 8  Stepper geschwindigkeitssteuerung

//if(bitRead(myBitArray,31)){
//digitalWrite(stepper1steppin, toggle);
//digitalWrite(stepper2steppin, toggle);
//toggle = !toggle;
//  myBitArray >> 1
//  bitSet(myBitArray, 0);
//}else{
//  myBitArray >> 1
//  bitClear(myBitArray, 0);
//}
//}


void loop() {

  int usbpower = map(poti1.getstate(), 0, 100, 0, 254);  //Poti.getstate returnt spannung des potis von 0 bis 100
  usb.setpower(usbpower);                                // setze usb pwm wert

  //stepperm1.changespeed(poti2.getstate());  //stepper.changespeed nimmt wert von 0 bis 100 als geschwindigkeit aus poti
  //stepperm2.changespeed(poti2.getstate());
  stepperm1.changespeed(1);  //stepper.changespeed nimmt wert von 0 bis 100 als geschwindigkeit aus poti
  stepperm2.changespeed(1);

  digitalWrite(stepper1steppin, 1);
  delay(map(poti1.getstate(),0,100,0,10));
  digitalWrite(stepper1steppin, 0);


  switch (statecounter) {  //Start Schrittkette

    case 0:
      digitalWrite(LED_BUILTIN, 0);
      if (!anfang1.getstate()) {     //solange anfangstaster nicht gedrückt
        stepperm1.switchdir(false);  //direction ist false -> rückwärts
        stepperm1.startmove();       //starte stepper bewegung
      } else {
        stepperm1.stopmove();  //anhalten
      }
      if (!anfang2.getstate()) {
        stepperm2.switchdir(false);
        stepperm2.startmove();
      } else {
        stepperm2.stopmove();
      }
      if (anfang1.getstate() and anfang2.getstate()) {  //wenn beide am anfang sind nächster schritt
        statecounter = 1;
        delay(2000);
      }
      break;


    case 1:

      //stepperm1.changespeed(poti2.getstate());  //Die geschwindigkeit wird permanent aktualisiert
      //stepperm2.changespeed(poti2.getstate());

      stepperm1.changespeed(100);  //Die geschwindigkeit wird permanent aktualisiert
      stepperm2.changespeed(100);


      if (bestaeigen.getstate()) {  //bei bestätigung nächster Schritt
        statecounter = 2;
      }
      break;

    case 2:
      digitalWrite(LED_BUILTIN, 1);
      if (!ende1.getstate()) {      //solange endtaster nicht gedrückt
        stepperm1.switchdir(true);  //direction ist true -> Vorwärts
        stepperm1.startmove();      //starte stepper bewegung
      } else {
        stepperm1.stopmove();  //anhalten
      }
      if (!ende2.getstate()) {
        stepperm2.switchdir(true);
        stepperm2.startmove();
      } else {
        stepperm2.stopmove();
      }
      if (ende1.getstate() and ende2.getstate()) {  //beginne neu
        statecounter = 0;
        delay(2000);
      }
      break;
  }

  //case -1:  Debugging outputs werden im seriellen Monitor dargestellt
  Serial.println("endstops:");
  Serial.print("   a1:\t");
  Serial.print(anfang1.getstate());
  Serial.print("   e1:\t");
  Serial.print(ende1.getstate());
  Serial.print("   a2:\t");
  Serial.print(anfang2.getstate());
  Serial.print("   e2:\t");
  Serial.println(ende2.getstate());

  Serial.print("Potis:");
  Serial.print("   P1:\t");
  Serial.print(poti1.getstate());
  Serial.print("   P2:\t");
  Serial.print(poti2.getstate());
  Serial.print("   Stp:\t");
  Serial.println(statecounter);

  //stepperm1.switchdir(true);
  //stepperm1.changespeed(50);
  //stepperm1.startmove();


  //delay(50);
}
