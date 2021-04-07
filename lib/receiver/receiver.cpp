#include <Receiver.h>

bool Receiver::jsonIsAvailable() {
  return true;
};

JsonArray Receiver::getJson() {
  DynamicJsonDocument doc(1024);

  /**
   * copy(JSON.stringify([json array here]).replaceAll('"', '\\"'));
   * this is useful for creating test data
  */
  char json[] = "[{\"node\":1,\"val\":1000,\"dur\":3000,\"ramp\":16,\"loop\":2}]";

  deserializeJson(doc, json);

  serializeJsonPretty(doc, Serial);

  JsonArray array = doc.as<JsonArray>();

  return array;
};

Receiver::Receiver(const uint8_t ip[4], const uint8_t mac[6], const uint16_t port) {
  return;
};

// this class needs to be defined here
// this class needs to give the json the correct default values (https://arduinojson.org/v6/api/jsonvariant/or/)
// it also needs to give the json the correct types (the above does this automatically)
// furthermore, it will ideally prune off unnecessary properties with a filter (https://arduinojson.org/news/2020/03/22/version-6-15-0/)
// you'll need to loop through and write code that looks like
// JsonObject object : objects
// object["ramp"] = object["ramp"] | (uint8_t)1
// return objects.as<JsonArrayConst>(); (or maybe objects.to<JsonArrayConst>();??)
// I don't know if const-ifying the array const-ifies all constituent elements
