#include <Lighting.h>

Node::Node(uint8_t i) : i(i){};

void Node::update() {
  uint16_t value = fade.update();

  DmxSimple.write(2 * i, value % 255);      // fine channel
  DmxSimple.write(2 * i - 1, value / 255);  // coarse channel
};

Lighting::Lighting() : nodes{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20} {
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  DmxSimple.usePin(4);
  DmxSimple.maxChannel(40);
};

Node Lighting::getNode(uint8_t i) {
  return nodes[i - 1];
};

void Lighting::update() {
  for (uint8_t i = 1; i <= 20; i++)
    getNode(i).fade.update();
};