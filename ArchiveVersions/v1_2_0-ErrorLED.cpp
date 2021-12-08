#include <Arduino.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <dmx.h>
#include <node.h>
#include <timer.h>
#define NUMBER_OF_NODES 20

// v1_2_0 Added Error LED
//--------------------------------------------------------------------------------

// Wall House Lights Switch durations
uint32_t neglectDuration = 10000;  // milliseconds
uint16_t neglectWarningDuration = 10000;
uint16_t neglectSafetyDuration = 10000;

IPAddress ip(1, 1, 1, 40);                          // Local Area Network Address Assigned
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  // Physcial Hardware Address
unsigned int localPort = 8888;                      // local port to listen on
EthernetUDP Udp;                                    // An EthernetUDP instance to let us send and receive packets over UDP

Node nodes[NUMBER_OF_NODES];
Dmx dmx;
const int cmdRcvLED = A2;  //LED pin to indicate Command Recieved
const int ErrorLED = A3;   //LED pin to indicate Command Error

bool receiver();

StaticJsonDocument<768> jsondoc;  // 508 max safe capacity of UDP packet. (Assistant says 657 min, 768)

// Wall House Lights Switch
const int WallSwitch = A0;
bool switchStatus = 0;
Timer timer;
uint8_t neglectStatus = 0;
void FloorLightsOn();
void FloorLightsOff();
void FloorLightsWhite();
void FloorLightsFlash();
const int relayInt = A1;  // Pin to activate relay that interupts Chromacove house lights.

void setup() {
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  timer.reset();
  pinMode(relayInt, OUTPUT);
  pinMode(cmdRcvLED, OUTPUT);
  pinMode(ErrorLED, OUTPUT);
};
// =================================================================================
void loop() {
  // ------ Light Switch Logic Conditions--------------
  //Analog Voltage 2.9V to 3.3V = 410
  // Switch turned ON  - > Analog Pin = 0 volts (Works regardless of neglect mode)
  if (analogRead(WallSwitch) <= 410 && !switchStatus) {
    // Turn floor whites on solo and start timer
    switchStatus = 1;
    neglectStatus = 0;
    digitalWrite(ErrorLED, LOW);
    timer.set(neglectDuration);
    FloorLightsOn();
    digitalWrite(relayInt, LOW);
  }
  // Switch turned OFF - > Analog = ~3.3 volts
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
  // Neglected Status Checking Logic and Actions
  if (timer.ended()) {
    if (neglectStatus == 0) {
      // Flash floor lights for warning duration and then shut house lights off.
      FloorLightsFlash();
      timer.set(neglectWarningDuration);
      neglectStatus = 1;
      digitalWrite(ErrorLED, HIGH);
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
      // Turn floor lights OFF
      FloorLightsOff();
      timer.reset();
    }
  }

  // ------ IP Command Logic ----------------
  if (receiver() == 1) {
    //digitalWrite(cmdRcv, HIGH);
    JsonArray targets = jsondoc.as<JsonArray>();
    for (JsonObject target : targets) {
      uint8_t nodeNumber = target["n"].as<uint8_t>();
      if (!target["r"].isNull()) nodes[nodeNumber - 1].rampMode = target["r"];
      if (!target["l"].isNull()) nodes[nodeNumber - 1].loopMode = target["l"];
      if (!target["v"].isNull()) {
        if (!target["d"].isNull())
          nodes[nodeNumber - 1].setTarget(target["v"], target["d"]);
        else
          nodes[nodeNumber - 1].setTarget(target["v"], 0);
      }
    }
    //digitalWrite(cmdRcv, LOW);
  }
  // ------ IP Command Logic END ----------------

  for (uint8_t i = 1; i <= NUMBER_OF_NODES; i++) {
    dmx.setBrightness(i, nodes[i - 1].getCurrentBrightness());
  }
};
// =================================================================================

bool receiver() {
  unsigned int packetSize = Udp.parsePacket();
  if (packetSize) {
    digitalWrite(cmdRcvLED, HIGH);
    if (packetSize > 502) {
      // TWO FLASHES - UDP Comm Packet too long
      digitalWrite(ErrorLED, HIGH);
      delay(200);
      digitalWrite(ErrorLED, LOW);
      delay(500);
      digitalWrite(ErrorLED, HIGH);
      delay(200);
      digitalWrite(ErrorLED, LOW);
      if (neglectStatus > 0) {
        delay(200);
        digitalWrite(ErrorLED, HIGH);
      }
      digitalWrite(cmdRcvLED, LOW);
      return 0;
    }
    DeserializationError error = deserializeJson(jsondoc, Udp);

    switch (error.code()) {
      case DeserializationError::Ok:
        break;
      case DeserializationError::InvalidInput:
        // ONE FLASH - CMD Invalid
        digitalWrite(ErrorLED, HIGH);
        delay(500);
        digitalWrite(ErrorLED, LOW);
        if (neglectStatus > 0) {
          delay(200);
          digitalWrite(ErrorLED, HIGH);
        }
        digitalWrite(cmdRcvLED, LOW);
        break;
      case DeserializationError::NoMemory:
        // THREE FLASHES - CMD caused a JSON memory error
        digitalWrite(ErrorLED, HIGH);
        delay(200);
        digitalWrite(ErrorLED, LOW);
        delay(500);
        digitalWrite(ErrorLED, HIGH);
        delay(200);
        digitalWrite(ErrorLED, LOW);
        delay(500);
        digitalWrite(ErrorLED, HIGH);
        delay(200);
        digitalWrite(ErrorLED, LOW);
        if (neglectStatus > 0) {
          delay(200);
          digitalWrite(ErrorLED, HIGH);
        }
        digitalWrite(cmdRcvLED, LOW);
        break;
      default:
        break;
    }

    if (!error) {
      digitalWrite(cmdRcvLED, LOW);
      return 1;
    } else {
      digitalWrite(cmdRcvLED, LOW);
      return 0;
    }
  }
  digitalWrite(cmdRcvLED, LOW);
  return 0;
};

void FloorLightsOff() {
  for (uint8_t i = 0; i < NUMBER_OF_NODES; i++) {
    nodes[i].rampMode = 1;
    nodes[i].loopMode = 0;
    nodes[i].setTarget(0, 15);
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
  nodes[3].setTarget(65535, 4);
  nodes[4].setTarget(0, 1);
  nodes[5].setTarget(0, 1);
  nodes[6].setTarget(0, 1);
  nodes[7].setTarget(0, 1);
  nodes[8].setTarget(65535, 4);
  nodes[9].setTarget(0, 1);
  nodes[10].setTarget(0, 1);
  nodes[11].setTarget(0, 1);
  nodes[12].setTarget(0, 1);
  nodes[13].setTarget(65535, 4);
  nodes[14].setTarget(0, 1);
  nodes[15].setTarget(0, 1);
  nodes[16].setTarget(0, 1);
  nodes[17].setTarget(0, 1);
  nodes[18].setTarget(65535, 4);
  nodes[19].setTarget(0, 1);
};

void FloorLightsFlash() {
  for (uint8_t i = 0; i < NUMBER_OF_NODES; i++) {
    nodes[i].rampMode = 1;
    nodes[i].loopMode = 0;
    nodes[i].setTarget(0, 15);
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
  nodes[3].setTarget(0, 6);
  nodes[8].setTarget(0, 6);
  nodes[13].setTarget(0, 6);
  nodes[18].setTarget(0, 6);
};