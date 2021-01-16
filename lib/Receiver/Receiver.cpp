#include <Arduino.h>
#include <Ethernet.h>
#include <Receiver.h>

Receiver::Receiver(byte ip[4], byte mac[6], unsigned short port)
    : server(port) {
  Ethernet.begin(mac, ip);
  server.begin();
};

void Receiver::handleClientJson(EthernetClient client, DynamicJsonDocument doc) {
  char serialJson[] = {};
  int i = 0;
  while (client.connected()) {
    if (client.available() > 0) {
      serialJson[i++] = client.read();
    }
  }
  deserializeJson(doc, serialJson, 1024);
};

void Receiver::getJson(DynamicJsonDocument doc) {
  EthernetClient client = server.available();
  if (client)
    handleClientJson(client, doc);
  else
    deserializeJson(doc, "null", 1024);
};
