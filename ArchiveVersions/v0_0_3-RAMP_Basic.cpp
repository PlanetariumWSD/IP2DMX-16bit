#include <Arduino.h>
#include <Ramp.h>  
// RAMP- Basic> Writes value to serial port ( or LED pin option )

//int ledPin = 5;

ramp myRamp;                                          // new ramp object (ramp<unsigned char> by default)

void setup() {
  //pinMode(ledPin, OUTPUT);                            // sets the pin as output
  
  Serial.begin(9600);                                 // begin Serial communication
  
  Serial.print("Value start at: ");
  myRamp.go(5,0);                                     // (Value, Duration)
  Serial.println(myRamp.getValue());                  // accessing start value
  
  Serial.println("Starting interpolation"); 
  myRamp.setGrain(1); 
  myRamp.go(80, 150, SINUSOIDAL_INOUT, FORTHANDBACK); // start interpolation (value to go to, duration)
  //myRamp.go(80, 150, LINEAR, FORTHANDBACK);         // Looks nice also
}

void loop() {

  //analogWrite(ledPin, myRamp.update());               // analogWrite values from 0 to 255

  Serial.print("Actual value is: ");        
  Serial.println(myRamp.update());                    // update() return the actual interpolation value
  //delay(100);                                       // only for debugging
}
