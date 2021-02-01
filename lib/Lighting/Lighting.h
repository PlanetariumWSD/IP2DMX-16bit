#ifndef Lighting_h
#define Lighting_h

#include <Arduino.h>
#include <DmxSimple.h>
#include <Ramp.h>

class Node {
 private:
  uint8_t i;

 public:
  ramp fade;
  Node(uint8_t i);
  void update();
};

class Lighting {
 public:
  Lighting();
  Node getNode(uint8_t i);
  void update();

 private:
  Node nodes[20];
};

#endif