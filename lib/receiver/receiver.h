#ifndef Receiver_h
#define Receiver_h

#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 2
#include <ArduinoJson.h>
#include <Ethernet.h>

class Receiver {
 public:
  Receiver(const uint8_t ip[4], const uint8_t mac[6], const uint16_t port);
  JsonArray getJson();
  bool jsonIsAvailable();
};

#endif