#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <DmxSimple.h>

/*=============================================================
   V5.1.3
   PROBABLY THE 8 BIT VERSION IN THE THEATER as of 01/25/2021
   DMX control of QTY(4) PX403 4 Channel LED Drivers
   Used in Planetarium of the Wausau School District
   RGBW Under-Step Theater Lighting
   Lead Programmer: Ben Edmondson with Chris Janssen

   IMPORTANT NOTE: Commands such as   Serial.write("[2J");
   Clear the Serial Screen in apps like PuTTY for serial debugging and sending IP commands.
   If you use the Arduino Serial comms window, you will not get readable results when debugging.
  ==============================================================

    Correct command IP packet Structure:  command(Char[4]), Val1(Int), Val2(Int), Val3(Int), Val4(Int), Val5(Int)

    Light Commands:

    ROW1, ROW2, ROW3, STRS, ALL
    Ligth Values (0-255): r, g, b, w, deltaT(s) time in seconds you want the fade to occur

    Utility Commands:

    TEST Val1   Repeats a light check test
    0-> Off
    1-> ROW1 RGBW 1 second interval
    2-> ROW2 RGBW 1 second interval
    3-> ROW3 RGBW 1 second interval
    4-> STRS RGBW 1 second interval
    5-> All Theater - Row1,2,3,Strs RGBW 1 second interval
    6-> All Theater - Row1,2,3,Strs RGBW 200  msecond interval

    DBUG Val1  Controls the amount of data to send to serial port for debugging.
    0-> Off
    1-> System speeds
    2-> Above plus Light Values
    3-> Above plus arrays and packet status

    CLCK Val1(ms) Sets the system refresh interval
    Without a hardware interput fades are calulated on assumed system refresh times. Finding and settig the correct refresh time after timing real theater fades will help set this default value properly.
    0 - 10,000 ms

    SCRN Val1(ms) Sets the debug serial data send refresh. Needed to stabilize screen to be readable
    0 - 10,000 ms
*/

//VARIABLE DECLARATIONS ===================================================================
//helper Define:
#define __isRGB__ ((A >= 0) && (A <= 255) && (B >= 0) && (B <= 255) && (C >= 0) && (C <= 255) && (D >= 0) && (D <= 255) && (E >= 0) && (E <= 255))
// For Percentage
//#define __isRGB__ ((A >= 0) && (A <= 100) && (B >= 0) && (B <= 100) && (C >= 0) && (C <= 100) && (D >= 0) && (D <= 100) && (E >= 0) && (E <= 100))

class channel {
  public:
    bool isChanging = 0;
    int startVal;
    int endVal;
    long long startTime;
    long long duration;
    float currentVal;
};

channel chan[19];

// DMX Outputs Global
const int MAX_LEN = 210; //max length of the deltaT command
float refreshRate = 33; //Light Update Cycle Time in milliseconds. This should be set just under the Native CPU time
unsigned long lastUpdate; //Clock time at which the next DMX refresh should occur
float packetContents[33];
bool chnChngFlg[33];
char *packetParsed[33]; //pointer array to read strtok values into. This is where the IP command is parsed into.

// Network Global
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //physical mac address
char packetBuffer[MAX_LEN]; //packet to hold incoming ip commands

//TCP Code
//byte ip[] = { 192, 168, 2, 246 }; // ip in lan
//EthernetServer server(8888);; //server port

//UDP Code
IPAddress ip(1, 1, 1, 40);
//IPAddress ip(1, 1, 1, 40);
unsigned int localPort = 8888;      // local port to listen on
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;


// Button pins on the arduino
const int b1Pin = A0;
bool b1Received = true;
bool whiteLock = 0;
//const int button2 = 8;
// bTwoPressed = false;

// Control Globals
int screenRefreshRate = 300; //in milliseconds
byte debugLevel = 0; // 0-> 'Clean mode' No diagnostic output/calcs,   1-> Native CPU Time,  2-> Add Light levels,   3->Add Array and Buffer Values
float nativeProcTime = 0.0; //in milliseconds
unsigned long oldTime;
float updateScreenTime = 0; //Clock time at which the next Screen refresh should occur
byte testLevel = 0;

// IP Parsed Commands (CMD A B C D E) normally-> (CMD R G B W dTime) but may be DEBUG 3, or SCREEN 200 etc..
char *CMD[] = {"NONE"};
int A, B, C, D;
float E;

//SUBFUNCTION DECLARATIONS ===================================================================
bool parse_packet();
void propChanger(int channel, int endVal, long long duration);
void multPropChanger(int startChan, int endChan, int countBy, int endVal, long long duration);
int getCurrentVal(int channel);
void debug();
void clearAndHome();
void nativeCalc();


// SETUP ==========================================================================
void setup() {


  // Ethernet library
  Ethernet.begin(mac, ip); //start ethernet

  //TCP only
  //server.begin();

  // UDP start
  Udp.begin(localPort);

  //dmx library configuration
  //Reindeer

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH); //configure shield as master
  DmxSimple.usePin(4); //initialize communication with shield on pin 4
  DmxSimple.maxChannel(18); //18 channels in total


  // Serial Port for debugging output
  // LEFT OUT FOR NORMAL OPERATION
  //Serial.begin(115200); //high baud rate for PuTTy to be able to clear serial monitor smoothly and show static values instead of scrolling values
}
// MAIN LOOPING CONTROLLER FUNCTION =================================================
//###################################################################################
//===================================================================================
void loop () {

  parse_packet();

  if (!whiteLock) {
    if (!b1Received) whiteLock = 1;
    for (int i = 3; i <= 18; i++) {
      if (chan[i].isChanging) {
        chan[i].currentVal = getCurrentVal(i);
        DmxSimple.write(i, chan[i].currentVal);
      }
    }
  }

  // lightswitch
  if (analogRead(b1Pin) > 410 && !b1Received) {
    b1Received = true;
    whiteLock = 0;
    multPropChanger(3, 18, 1, 0, 0);
  } else if (analogRead(b1Pin) <= 410 && b1Received) {
    b1Received = false;
    multPropChanger(6, 18, 4, 255, 0);
    multPropChanger(3, 5, 1, 0, 0);
    multPropChanger(7, 9, 1, 0, 0);
    multPropChanger(11, 13, 1, 0, 0);
    multPropChanger(15, 17, 1, 0, 0);
  }

  // Should debug info be sent to Comm Port
  if (debugLevel > 0) {
    nativeCalc();
    if (millis() >= updateScreenTime) { //if its time to refresh AKA updateScreenTime (next time to go) has been passed
      updateScreenTime = millis() + screenRefreshRate; //update next updateScreenTime
      debug();
    }
  }
}
//===================================================================================
//###################################################################################
// SUBFUNCTIONS =====================================================================

bool parse_packet() {

  // TCP #1 only->  EthernetClient client = server.available();   // Create a client connection
  // TCP #2 only->  if (client) { //right when the client connects

  int packetSize = Udp.parsePacket();    //UDP #1 version of above
  if (packetSize) {
    for (int i = 0; i <= MAX_LEN; i++) { //Get ready for new data, reset packetBuffer to all '\0'. This is the byte that strtok looks for to finish its last term
      packetBuffer[i] = '\0';
    }
    // UDP packet buffer fill
    Udp.read(packetBuffer, MAX_LEN);


    CMD[0] = strtok(packetBuffer, " "); //set the first one. " " is the deliminater


    bool Stop = 0;
    while (Stop == 0) {
      int currentChannel = atoi(strtok(NULL, " -"));
      if (currentChannel != NULL) {
        chan[currentChannel].endVal = atoi(strtok(NULL, ", "));
        chan[currentChannel].duration = atoi(strtok(NULL, " "));
        chan[currentChannel].startTime = millis();
        chan[currentChannel].isChanging = 1;
        chan[currentChannel].startVal = chan[currentChannel].currentVal;
      } else {
        Stop = 1;
      }
    }

    return 1;
  }
  else return 0;
}
// ================================================================================

void propChanger(int channel, int endVal, long long duration) {
  chan[channel].isChanging = 1;
  chan[channel].startTime = millis();
  chan[channel].startVal = chan[channel].currentVal;
  chan[channel].duration = duration;
  chan[channel].endVal = endVal;
}

void multPropChanger(int startChan, int endChan, int countBy, int endVal, long long duration) {
  for ( int current = startChan; current <= endChan; current += countBy) {
    propChanger(current, endVal, duration);
  }
}

// ================================================================================

int getCurrentVal(int channel) {
  float whatTheyShouldSee;
  int whatWeNeedToPrint;

  if ((millis() - chan[channel].startTime) <= (chan[channel].duration * 1000) && chan[channel].duration != 0 && chan[channel].currentVal != chan[channel].endVal) {
    whatTheyShouldSee = (millis() - chan[channel].startTime)  *  (chan[channel].endVal - chan[channel].startVal) / (chan[channel].duration * 1000)  +  chan[channel].startVal;
    //whatWeNeedToPrint = round(255 * pow(whatTheyShouldSee / 255, 2));
    whatWeNeedToPrint = whatTheyShouldSee;
  } else {
    whatWeNeedToPrint = chan[channel].endVal;
    chan[channel].isChanging = 0;
    chan[channel].endVal = 0;
    chan[channel].duration = 0;
  }
  return whatWeNeedToPrint;
}

// ================================================================================
//The next function is a bebug table printed through PuTTy.
//Clear current PuTTy Serial screen, then update info.

void debug() {
  clearAndHome();
  if (debugLevel >= 1) {
    Serial.print("Screen(ms): ");
    Serial.print(screenRefreshRate);
    Serial.print(" @ ");
    Serial.println(millis() - oldTime);
    oldTime = millis();
  }
  if (debugLevel >= 2) {
    Serial.print("Millis: ");
    Serial.println(millis());
    Serial.println();
    Serial.println("endVal: 3-18");
    for (int i = 3; i <= 18; i++) {
      Serial.print(chan[i].endVal);
      Serial.print(", ");
    }
    Serial.println("\n");
    Serial.println("currentValue: 3-18");
    for (int i = 3; i <= 18; i++) {
      Serial.print(chan[i].currentVal);
      Serial.print(", ");
    }
    Serial.println();
  }
  if (debugLevel >= 3) {
    Serial.println();
    Serial.println("isChanging: 3-18");
    for (int i = 3; i <= 18; i++) {
      Serial.print(chan[i].isChanging);
      Serial.print(", ");
    }
  }
}

// ================================================================================
void clearAndHome() {
  Serial.write(27); // ESC
  Serial.write("[2J"); // clear screen
  Serial.write(27); // ESC
  Serial.write("[H"); // cursor to home
}
// ================================================================================
unsigned long oldNativeTime;
void nativeCalc() {
  nativeProcTime = millis() - oldNativeTime;
  oldNativeTime = millis();
}

