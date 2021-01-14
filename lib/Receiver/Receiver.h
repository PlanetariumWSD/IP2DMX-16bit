#include <Arduino.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
// https://www.arduino.cc/en/Reference/IfEthernetClient

#ifndef Receiver_h
#define Receiver_h

class Receiver {
 private:
  EthernetServer server;
  void handleClientJson(EthernetClient client, DynamicJsonDocument doc);

 public:
  Receiver(byte ip[4], byte mac[6], unsigned short port);
  void getJson(DynamicJsonDocument doc);
};

#endif