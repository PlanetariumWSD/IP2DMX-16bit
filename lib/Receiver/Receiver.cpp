#include <Receiver.h>

Receiver::Receiver(uint8_t ip[4], uint8_t mac[6], uint16_t port) : server(port) {
  Ethernet.begin(mac, ip);
  server.begin();
};

JsonArray Receiver::getJson() {
  StaticJsonDocument<1024> json;

  EthernetClient client = server.available();
  if (!client) return json.to<JsonArray>();  // TODO: double check that this line is even necessary

  deserializeJson(json, client);
  client.stop();

  return json.as<JsonArray>();
};

bool Receiver::clientIsConnected() {
  return server.available();
}
