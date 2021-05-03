#include <Receiver.h>

// v0_1_1 Pass Array back to main FAIL (Forced Integration into Main) -------------------------------------------
//--------------------------------------------------------------------------------

bool Receiver::jsonIsAvailable() {
  return true;
};

JsonObject Receiver::getJson() {
  StaticJsonDocument<100> doc;

  /**
   * copy(JSON.stringify([json array here]).replaceAll('"', '\\"'));
   * this is useful for creating test data
  */
  char json[] = "[{\"node\":1,\"val\":1000,\"dur\":3000,\"ramp\":16,\"loop\":2}]";

  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  serializeJsonPretty(doc, Serial);  //This works!

  //JsonObject array = doc.as<JsonObject>();
  // //JsonObjectConst array = doc.as<JsonObject>();
  // //  JsonArray array = doc.as<JsonArray>();
  // int fooNode = array["node"];
  // Serial.println(fooNode);
  // int fooVal = array["val"];
  // Serial.println(fooVal);
  // int fooDur = array["dur"];
  // Serial.println(fooDur);

  return doc.as<JsonObject>();
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
