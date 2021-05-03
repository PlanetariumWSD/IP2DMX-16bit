#ifndef Receiver_h
#define Receiver_h

#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 2
#include <ArduinoJson.h>
#include <Ethernet.h>

// v0_1_1 Pass Array back to main FAIL (Forced Integration into Main)--------------------------------------------
//--------------------------------------------------------------------------------

class Receiver {
 public:
  Receiver(const uint8_t ip[4], const uint8_t mac[6], const uint16_t port);
  JsonObject getJson();
  //JsonArray getJson();
  bool jsonIsAvailable();
};

#endif