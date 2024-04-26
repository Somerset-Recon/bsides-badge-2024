#define SCAN 5
#define PAPER 4
#define ROCK 3
#define SCISSORS 2
#define UP SCAN
#define DOWN PAPER
#define LEFT ROCK
#define RIGHT SCISSORS
#define INT32_MAX 2147483647

#include <HardwareSerial.h>
HardwareSerial SerialSAO(1);

#include <WString.h>

bool exitState = false;
bool authenticated = false;


const byte numChars = 32;
char receivedChars[numChars];
bool newData = false;

const String flag = "FLAG{D3C0MP1L3_W1Z@RD}"; //requires dumping firmware to read


