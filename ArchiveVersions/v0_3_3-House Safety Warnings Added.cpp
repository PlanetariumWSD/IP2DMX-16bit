#include <Arduino.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <dmx.h>
#include <node.h>
#include <timer.h>
#define NUMBER_OF_NODES 20

// v0_3_3 House Light Safety Warnings added (Needs device "node:0" CMDs)
//--------------------------------------------------------------------------------

Node nodes[NUMBER_OF_NODES];
Dmx dmx;

bool receiver(DynamicJsonDocument &);

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  // Physcial Hardware Address
IPAddress ip(1, 1, 1, 40);                          // Local Area Network Address Assigned
unsigned int localPort = 8888;                      // local port to listen on
EthernetUDP Udp;                                    // An EthernetUDP instance to let us send and receive packets over UDP

DynamicJsonDocument jsondoc(768);  // 508 max safe capacity of UDP packet. (Assistant says 657 min, 768)

// Wall House Lights Switch
const int WallSwitch = A0;
bool switchStatus = 0;
Timer timer;
uint8_t neglectStatus = 0;
uint32_t neglectDuration = 10000;  // milliseconds
uint16_t neglectWarningDuration = 10000;
uint16_t neglectSafetyDuration = 10000;
void FloorLightsOn();
void FloorLightsOff();
void FloorLightsWhite();
void FloorLightsFlash();
//const int relayInt = 11;  //Digital output pin to engage relay.
const int relayInt = A1;

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  timer.reset();
  pinMode(relayInt, OUTPUT);
};
// =================================================================================
void loop() {
  // ------ Light Switch Logic --------------
  // Switch turned ON -> Analog Pin = 0 volts (Works regardless of neglect mode)
  //Serial.println(analogRead(WallSwitch));
  //Serial.println();
  if (analogRead(WallSwitch) <= 410 && !switchStatus) {
    // Turn floor whites on solo and start timer
    switchStatus = 1;
    neglectStatus = 0;
    timer.set(neglectDuration);
    FloorLightsOn();
    digitalWrite(relayInt, LOW);
  }
  // Switch turned  OFF - > Analog = ~3.3 volts
  if (analogRead(WallSwitch) > 410 && switchStatus && neglectStatus == 0) {
    // Turn all floor lights off and stop timer
    switchStatus = 0;
    timer.reset();
    FloorLightsOff();
    digitalWrite(relayInt, LOW);
  }
  // Switch turned OFF (in Neglect) Analog = ~3.3 volts
  if (analogRead(WallSwitch) > 410 && switchStatus && neglectStatus > 0) {
    // Set neglect = 0, Flash lights for 5 seconds,
    switchStatus = 0;
    FloorLightsFlash();
    timer.set(neglectWarningDuration);
  }

  if (timer.ended()) {
    if (neglectStatus == 0) {
      // Flash floor lights for warning duration and then shut house lights off.
      FloorLightsFlash();
      timer.set(neglectWarningDuration);
      neglectStatus = 1;
    } else if (neglectStatus == 1) {
      // Interupt Relay ON to shut off house lights, leave floor lights on for safety.
      FloorLightsOn();
      digitalWrite(relayInt, HIGH);
      timer.set(neglectSafetyDuration);
      neglectStatus = 2;
    } else if (neglectStatus == 2) {
      // Warn any occupants it's about to get dark.
      FloorLightsFlash();
      timer.set(neglectWarningDuration);
      neglectStatus = 3;
    } else if (neglectStatus == 3) {
      // Turn Floor lights OFF
      FloorLightsOff();
      timer.reset();
    }
  }

  // ------ IP Command Logic ----------------
  if (receiver(jsondoc) == 1) {
    serializeJsonPretty(jsondoc, Serial);
    Serial.println();
    JsonArray targets = jsondoc.as<JsonArray>();
    for (JsonObject target : targets) {
      Serial.println(F("------"));
      Serial.print(F("NODE: "));
      uint8_t nodeNumber = target["n"].as<uint8_t>();
      Serial.println(nodeNumber);
      if (!target["r"].isNull()) nodes[nodeNumber - 1].rampMode = target["r"];
      if (!target["l"].isNull()) nodes[nodeNumber - 1].loopMode = target["l"];
      if (!target["v"].isNull()) {
        if (!target["d"].isNull())
          nodes[nodeNumber - 1].setTarget(target["v"], target["d"]);
        else
          nodes[nodeNumber - 1].setTarget(target["v"], 0);
      }
    }
  }
  // ------ IP Command Logic END ----------------

  for (uint8_t i = 1; i <= NUMBER_OF_NODES; i++) {
    dmx.setBrightness(i, nodes[i - 1].getCurrentBrightness());
  }
};
// =================================================================================

bool receiver(DynamicJsonDocument &doc) {
  unsigned int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("PacketSize: ");
    Serial.println(packetSize);
    if (packetSize > 502) {
      Serial.println("Udp Packet too large");
      return 0;
    }
    DeserializationError error = deserializeJson(doc, Udp);

    switch (error.code()) {
      case DeserializationError::Ok:
        Serial.print(F("Deserialization succeeded"));
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F("Invalid input!"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Not enough memory"));
        break;
      default:
        Serial.print(F("Deserialization failed"));
        break;
    }
    Serial.println();

    if (!error) {
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
};

void FloorLightsOff() {
  for (uint8_t i = 0; i < NUMBER_OF_NODES; i++) {
    nodes[i].rampMode = 1;
    nodes[i].loopMode = 0;
    nodes[i].setTarget(0, 2);
  }
};

void FloorLightsOn() {
  for (uint8_t i = 0; i < NUMBER_OF_NODES; i++) {
    nodes[i].rampMode = 1;
    nodes[i].loopMode = 0;
  }
  nodes[0].setTarget(0, 1);
  nodes[1].setTarget(0, 1);
  nodes[2].setTarget(0, 1);
  nodes[3].setTarget(65535, 1);
  nodes[4].setTarget(0, 1);
  nodes[5].setTarget(0, 1);
  nodes[6].setTarget(0, 1);
  nodes[7].setTarget(0, 1);
  nodes[8].setTarget(65535, 1);
  nodes[9].setTarget(0, 1);
  nodes[10].setTarget(0, 1);
  nodes[11].setTarget(0, 1);
  nodes[12].setTarget(0, 1);
  nodes[13].setTarget(65535, 1);
  nodes[14].setTarget(0, 1);
  nodes[15].setTarget(0, 1);
  nodes[16].setTarget(0, 1);
  nodes[17].setTarget(0, 1);
  nodes[18].setTarget(65535, 1);
  nodes[19].setTarget(0, 1);
};

void FloorLightsFlash() {
  // nodes[3].rampMode = 1;
  // nodes[3].loopMode = 0;
  // nodes[8].rampMode = 1;
  // nodes[8].loopMode = 0;
  // nodes[13].rampMode = 1;
  // nodes[13].loopMode = 0;
  // nodes[18].rampMode = 1;
  // nodes[18].loopMode = 0;
  for (uint8_t i = 0; i < NUMBER_OF_NODES; i++) {
    nodes[i].rampMode = 1;
    nodes[i].loopMode = 0;
    nodes[i].setTarget(0, 2);
  }
  nodes[3].setTarget(65535, 0);
  nodes[8].setTarget(65535, 0);
  nodes[13].setTarget(65535, 0);
  nodes[18].setTarget(65535, 0);
  nodes[3].rampMode = 16;
  nodes[3].loopMode = 2;
  nodes[8].rampMode = 16;
  nodes[8].loopMode = 2;
  nodes[13].rampMode = 16;
  nodes[13].loopMode = 2;
  nodes[18].rampMode = 16;
  nodes[18].loopMode = 2;
  nodes[3].setTarget(0, 1);
  nodes[8].setTarget(0, 1);
  nodes[13].setTarget(0, 1);
  nodes[18].setTarget(0, 1);
};