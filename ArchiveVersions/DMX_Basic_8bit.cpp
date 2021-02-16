#include <Arduino.h>
#include <DmxSimple.h>

  /* Simple DMX loop to ramp up brightness */

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
    delay(500);   

  for (int brightness = 0; brightness <= 255; brightness++) {
    DmxSimple.write(1, brightness);                       // Update DMX channel 1 to new brightness
    delay(10);                                            // Small delay to slow down the ramping
  }
}