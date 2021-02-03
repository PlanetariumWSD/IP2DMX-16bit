#ifndef Receiver_h
#define Receiver_h

#include <Arduino.h>
#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 2
#include <ArduinoJson.h>
#include <Ethernet.h>

class Receiver {
 private:
  EthernetServer server;

 public:
  Receiver(uint8_t ip[4], uint8_t mac[6], uint16_t port);
  JsonArray getJson();
  bool clientIsConnected();
};

#endif