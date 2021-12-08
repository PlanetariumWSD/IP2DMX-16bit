#include <Arduino.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <dmx.h>
#include <node.h>
#include <timer.h>
#define NUMBER_OF_NODES 20

// v1_2_1
// Switch Logic Voltage Change for ON position from trial showing unstability
// Changed Switch logic to add switch off during pre-Neglect for instant reset (All Off).
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
  // Voltage 410 = ~2.9 to 3.3 V
  // Switch turned ON  - > Analog Pin = 0 volts (Works regardless of neglect mode)
  if (analogRead(WallSwitch) <= 50 && !switchStatus) {
    // Turn floor whites on solo and start timer
    switchStatus = 1;
    neglectStatus = 0;
    digitalWrite(ErrorLED, LOW);
    FloorLightsOn();
    digitalWrite(relayInt, LOW);
    timer.set(neglectDuration);
  }
  // Switch turned OFF -> Analog ~3.3V
  if (analogRead(WallSwitch) > 410 && switchStatus && neglectStatus < 4) {
    // Turn all floor lights off and stop timer
    switchStatus = 0;
    neglectStatus = 0;
    digitalWrite(ErrorLED, LOW);
    FloorLightsOff();
    digitalWrite(relayInt, LOW);
    timer.reset();
  }
  // Switch turned OFF (in Full Neglect) Analog = ~3.3V
  if (analogRead(WallSwitch) > 410 && switchStatus && neglectStatus == 4) {
    // Set neglect = 0, Flash lights for 5 seconds,
    switchStatus = 0;
    neglectStatus = 5;  // Dome OFF with Floor Flashing (Human present! Tell of neglect)
    FloorLightsFlash();
    timer.set(neglectWarningDuration);
  }
  //--------Neglected Status Checking Logic and Actions  ------------
  // Neglect 0 = Normal mode (Not Neglected)
  if (timer.ended()) {
    if (neglectStatus == 0) {
      neglectStatus = 1;  // Dome ON with Floor Flashes (Warning)
      FloorLightsFlash();
      digitalWrite(ErrorLED, HIGH);  // Error LED ON during any neglect
      timer.set(neglectWarningDuration);
    } else if (neglectStatus == 1) {
      neglectStatus = 2;  // Dome OFF, Floor ON (for safety)
      FloorLightsOn();
      digitalWrite(relayInt, HIGH);  // Interupt Relay ON to shut off house lights
      timer.set(neglectSafetyDuration);
    } else if (neglectStatus == 2) {
      neglectStatus = 3;  // Dome OFF, Floor FLASH ( Warn any occupants it's about to get dark.)
      FloorLightsFlash();
      timer.set(neglectWarningDuration);
    } else if (neglectStatus == 3) {
      neglectStatus = 4;  // Dome OFF, Floor OFF
      FloorLightsOff();
      timer.reset();
    } else if (neglectStatus == 5) {
      neglectStatus = 0;  // Neglect 5  = Human flipped switch while in full neglect. RESET to Off.
      digitalWrite(ErrorLED, LOW);
      FloorLightsOff();
      digitalWrite(relayInt, LOW);
      timer.reset();
    }
  }

  // ------ IP UDP Command Logic ----------------
  // Check if a valid UDP packet has arrived (returns 1)
  if (receiver() == 1) {
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
    digitalWrite(cmdRcvLED, HIGH);  // Indicator on to show inbound comms
    if (packetSize > 502) {
      // TWO FLASHES - UDP Error- Comm Packet beyond safe capacity
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
        // ONE FLASH - JSON data structure error ( CMD Invalid )
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
        // THREE FLASHES - JSON document memory overflow
        // Should never happen as the 502 capacity check above but here in case.
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