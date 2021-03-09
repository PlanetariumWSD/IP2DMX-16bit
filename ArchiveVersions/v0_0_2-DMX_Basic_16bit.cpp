#include <Arduino.h>
#include <DmxSimple.h>

  /* Simple 16 bit DMX loop to ramp up brightness, 65279 is maximum 16bit DMX value. */

void setup() {
  //dmx library configuration
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);                                  // configure shield as master
  DmxSimple.usePin(4);                                    // initialize communication with shield on pin 4
  DmxSimple.maxChannel(40);                               // 40 channels in total
}

void loop() {
  
  //Reset to zero and wait
  DmxSimple.write(1, 0);
  DmxSimple.write(2, 0);
  delay(2000);           

  for (unsigned int brightness = 0; brightness <= 65279; brightness++) {
    DmxSimple.write(1, brightness / 255);                 // coarse channel (no remainder)
    DmxSimple.write(2, brightness % 255);                 // fine channel (produces remainder of division)
    //delay(1);                                             // 1 ms delay makes ~65.279 second fade up
  }
  delay(2000); 
}