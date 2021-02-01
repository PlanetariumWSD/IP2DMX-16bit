#include <Arduino.h>
#include <lighting.h>
#include <receiver.h>

uint8_t ip[4] = {1, 2, 3, 4};
uint8_t mac[6] = {1, 2, 3, 4, 5, 6};
uint16_t port = 8080;
Receiver receiver(ip, mac, port);
Lighting lighting;

void handleNodeCommands(JsonArray nodeCommands) {
  for (JsonObject nodeCommand : nodeCommands) {
    lighting
        .getNode(nodeCommand["node"].as<uint8_t>())
        .fade.go(
            nodeCommand["val"].as<uint16_t>(),
            nodeCommand["dur"].as<uint32_t>(),
            static_cast<ramp_mode>(nodeCommand["ramp"].as<uint8_t>()),
            static_cast<loop_mode>(nodeCommand["loop"].as<uint8_t>()));
    // TODO: ramp_mode needs to default to 1 if nonexistant, not 0
  }
};

void setup(){};

void loop() {
  if (receiver.clientIsConnected()) {
    JsonArray nodeCommands = receiver.getJson();
    handleNodeCommands(nodeCommands);
  }

  lighting.update();
};