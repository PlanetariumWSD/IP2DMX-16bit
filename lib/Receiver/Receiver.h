#ifndef Receiver_h
#define Receiver_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Ethernet.h>

class Receiver {
 private:
  EthernetServer server;
  void handleClientJson(EthernetClient client, DynamicJsonDocument doc);

 public:
  Receiver(uint8_t ip[4], uint8_t mac[6], uint16_t port);
  void getJson(DynamicJsonDocument doc);
};

#endif