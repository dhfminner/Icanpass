/*
	This module is designed by Jonathan Falco.

	Credit to Jonathan Falco and this works with BC127.
	
 */
#include <SparkFunbc127.h> //All functions specific for BC127 Chip coded by Sparkfun
#include <SoftwareSerial.h> //<-- Should be included with Arduino Uno libraries.
 //^Can be tracked down on internet if needed.
//This code will probably only work on Arduino Uno.
// create virtual serial port 'cuz time-sharing with uploading code is a pain.
SoftwareSerial swPort(3,2); // Digital port 3 is RX, and port 2 is TX
//SoftwareSerial debugPort(5,4); //used for debugging
//Create a BC127 and attach software serial port to it.
BC127 BTModu(&swPort); //This is transmitting device
BC127 BTModu2(&Serial); //This is receiving device
// Aliases for the input and output pins we're going to use.
#define BUTTONPIN 7
void setup()
{ // Serial port configuration. Default speed for BC127 is 9600 baud.
Serial.begin(9600);
swPort.begin(9600);
//debugPort.begin(9600);
String buffer = "", buffer2 = "";
pinMode(BUTTONPIN, INPUT_PULLUP);
delay(220);//Wait for chips to boot
BTModu.stdCmd("POWER OFF"); //Turn off BCTX while user connects to the BCRX
//BC127-Rx Setup
SetBC127RX();

//BC127-Tx Setup
SetBC127TX();
//Connect to a device
BC127Connect();
}
String inBuffer = ""; //Arduino complains if this is included in the music streaming loop. It is fine here
String buffer = "";
int SetBC127RX() //Setup for the recieving chip
{
int BC127RXres = BC127::DEFAULT_ERR;
String bufferRX = "";
//Overwright existing settings to factory defaults.
BTModu2.exitDataMode(1000); //Exit Data Mode
//This functions keeps returning a timeout error, possibly due to device not being in data mode in the first place.
//Clear Device List
if(BTModu2.restore() == 1) //Restore
{
 BTModu2.writeConfig(); //If restore is successful, Write
}
BTModu2.reset(); //Reset
BTModu2.stdCmd("UNPAIR"); //Remove all previous connections
BTModu2.stdGetParam("AUTOCONN", &bufferRX);
if (!(bufferRX.endsWith("1")))
 { /*For all statements, second "get command" can be removed if memory low*/
 BTModu2.stdSetParam("AUTOCONN", "1"); //Autoconnect Enabled
 BTModu2.stdGetParam("AUTOCONN", &bufferRX);
 }
bufferRX = "";
BTModu2.stdGetParam("BLE_ROLE", &bufferRX);
if (!(bufferRX.endsWith("0")))
 {
 BTModu2.stdSetParam("BLE_ROLE", "0"); //BLE Protocol Disabled
 BTModu2.stdGetParam("BLE_ROLE", &bufferRX);
 }
bufferRX = "";
BTModu2.stdGetParam("DISCOVERABLE", &bufferRX);
if (!(bufferRX.endsWith("1 0")))
 {
 BTModu2.stdCmd("DISCOVERABLE ON"); //Discovey Enabled
 BTModu2.stdSetParam("DISCOVERABLE", "1 0");
 BTModu2.stdGetParam("DISCOVERABLE", &bufferRX);
 }
bufferRX = "";
BTModu2.stdGetParam("DEEP_SLEEP", &bufferRX);
if (!(bufferRX.endsWith("ON")))
 {
 BTModu2.stdSetParam("DEEP_SLEEP", "ON"); //Deep Sleep Permitted
 BTModu2.stdGetParam("DEEP_SLEEP", &bufferRX);
 }
bufferRX = "";
BTModu2.stdGetParam("ENABLE_HFP", &bufferRX);
if (!(bufferRX.endsWith("OFF")))
 { //Hands Free Profile Disabled
 BTModu2.stdSetParam("ENABLE_HFP", "OFF");
  BTModu2.stdGetParam("ENABLE_HFP", &bufferRX);
 }
bufferRX = "";
BTModu2.stdGetParam("CLASSIC_ROLE", &bufferRX);
if (!(bufferRX.endsWith("0")))
 {
 BTModu2.setClassicSink();
 BTModu2.stdGetParam("CLASSIC_ROLE", &bufferRX);
 }
bufferRX = "";
BTModu2.stdGetParam("AUDIO", &bufferRX);
if (!(bufferRX.endsWith("0")))
 { //Audio set to
 BTModu2.stdSetParam("AUDIO", "0");
 BTModu2.stdGetParam("AUDIO", &bufferRX);
 }
bufferRX = "";
BTModu2.stdCmd("STATUS"); //Status check for debugging
BTModu2.writeConfig(); //Write
BTModu2.reset(); //Reset
while (1) //Waiting for a device to connect to BCRX
{
 //Buffer incoming serial data to check if connected.
 if (Serial.available() > 0) buffer.concat((char)Serial.read());
 // Check if it's a full line from the serial port
 if (buffer.endsWith("\r"))
 {
 // If the buffer has a connection message, break out of the loop.
 if (buffer.startsWith("OPEN_OK")|buffer.startsWith("PAIR_OK")|buffer.startsWith("SUCCESS"))
 {
 if (buffer.startsWith("PAIR_OK")) //If pair successful
 {
 delay(500); //wait for connection to establish
 bufferRX = buffer.substring(8,20); //Capture device address
 BTModu2.stdSetParam("REMOTE_ADDR", bufferRX); //Set as autoconnect address
 bufferRX = "";
 BTModu2.stdGetParam("REMOTE_ADDR", &bufferRX); //for debugging
 bufferRX = "";
 buffer = "";
 //BTModu.stdCmd("POWER ON"); //turn on other chip so it's ready for connecting
 //Possibly not getting enough time to boot the device
 BC127RXres = BC127::SUCCESS;
 break; // Exit this while loop to the music streaming loop.
 }
 buffer = ""; //Reaching here means something has connected
 }
 buffer = ""; // Otherwise, clear the buffer and go back to waiting.
 }
}
if (BC127RXres != BC127::SUCCESS) return BC127RXres;
return BC127RXres;
}
int SetBC127TX() //Setup for the transmitting chip
{
int BC127TXres = BC127::DEFAULT_ERR; //Assume failure
String bufferTX = "";
BTModu.stdCmd("POWER ON");
delay(200); //Needed for Successful Bootup
//Overwright existing settings to factory defaults.
BTModu.exitDataMode(1000); //Exit Data Mode
//This functions keeps returning a timeout error, possibly due to device not being in data mode in the first place.
if (BTModu.restore() == 1) //Restore
BTModu.writeConfig(); //Write, if restore is successful
BTModu.reset(); //Reset:
BTModu.stdCmd("UNPAIR"); //Clear Device List
BTModu.stdGetParam("AUTOCONN", &bufferTX); //Check and Enable Autoconnect
if (!(bufferTX.endsWith("1")))
 { /*For all statements, second "get command" can be removed if memory low*/
 BTModu.stdSetParam("AUTOCONN", "1");
 BTModu.stdGetParam("AUTOCONN", &bufferTX);
 }
bufferTX = "";
BTModu.stdGetParam("BLE_ROLE", &bufferTX); //Check and Disable BLE Protocol
if (!(bufferTX.endsWith("0")))
 {
 BTModu.stdSetParam("BLE_ROLE", "0");
 BTModu.stdGetParam("BLE_ROLE", &bufferTX);
 }
bufferTX = "";
BTModu.stdGetParam("ENABLE_HFP", &bufferTX); //Check and Permit Deep Sleep
if (!(bufferTX.endsWith("OFF")))
 {
 BTModu.stdSetParam("ENABLE_HFP", "OFF");
 BTModu.stdGetParam("ENABLE_HFP", &bufferTX);
 }
bufferTX = "";
BTModu.stdGetParam("CLASSIC_ROLE", &bufferTX); //Check and Set Bluetooth as a Source
if (!(bufferTX.endsWith("1")))
{
 BTModu.setClassicSource();
 bufferTX = "";
  BTModu.stdGetParam("CLASSIC_ROLE", &bufferTX);
 if (!(bufferTX.endsWith("1")))
 {
 BC127TXres = BC127::INVALID_PARAM;
 }
 else
 {
 BC127TXres = BC127::SUCCESS; //This is the primary objective of this setup routine,
 } // thus Success is achieved only here
 }
 else if (bufferTX.endsWith("1"))
 {
 BC127TXres = BC127::SUCCESS; // and here
 }
bufferTX = "";
BTModu.stdGetParam("AUDIO", &bufferTX); //Check and set Analog Audio
if (!(bufferTX.endsWith("0")))
 {
 BTModu.stdSetParam("AUDIO", "0");
 BTModu.stdGetParam("AUDIO", &bufferTX);
 }
bufferTX = "";
BTModu.stdCmd("STATUS"); //Can be removed, mostly for debugging
BTModu.writeConfig(); //Commit changes to flash memory on chip
BTModu.reset();
if (BC127TXres != BC127::SUCCESS) return BC127TXres;
return BC127TXres;
}
int BC127Connect() //Scan and connect
{
int connectionResult = BC127::REMOTE_ERROR; //Initially assume failure
int pairRes = 0;
BTModu.inquiry(20); // Spend ~26 seconds seeking devices. t = 1.28 * input
String address;
for (byte i = 0; i < 5; i++)
{
 if (BTModu.getAddress(i, address) && (address != ""))
 {
 // Now, attempt to connect. Timeouts are enabled.
 //General commands for connecting to music streaming Bluetooth protocols
 if (BTModu.connect(address, BC127::A2DP) > 0)
 {
 connectionResult = BC127::SUCCESS;
 pairRes += 1;
 }
 if (BTModu.connect(address, BC127::AVRCP) > 0)
 {
 connectionResult = BC127::SUCCESS;
 pairRes += 1;
 }
 Serial.println(BTModu.stdCmd("LIST"));
 /*if (BTModu.connect(address, BC127::AVRCP) > 0) //Other Bluetooth profile connections
 pairRes += 1;
 if (BTModu.connect(address, BC127::SPP) > 0)
 pairRes += 1;
 if (BTModu.connect(address, BC127::HFP) > 0)
 pairRes += 1;
 if (BTModu.connect(address, BC127::PBAP) > 0)
 pairRes += 1;*/
 if (pairRes > 1) //Finished connection attempt, check if successful
 {
 Serial.println("pairRes is > 1");
 connectionResult = BC127::SUCCESS;
 break; //If it works, jump straight to streaming music
 }
 }
 buffer = ""; // Otherwise, clear the buffer and go back to waiting.
}
if (connectionResult != BC127::SUCCESS) return connectionResult;
return connectionResult;
}
void loop()
{
if (digitalRead(BUTTONPIN) == LOW)
 {
 BTModu.stdCmd("UNPAIR"); //Close any previous connections
 delay(500); //Giving it time to complete that operation
 BTModu.reset(); //This seems to help when already paired
 delay(100); //Probably unnessary but kept in because seems to help
 if (BC127Connect() == BC127::SUCCESS) //Initializing Device Search
 {
 delay(2000); //Connection succeeded!
 BTModu.stdCmd("MUSIC PLAY"); //Issue music stream command
 BTModu.stdCmd("TONE TE 400 V 64 TI 0 N C5 L 8 N R0 L 32 N E5 L 8 N R0 L 32 N G5 L 8 N R0 L 32 N B5 L 4 N R0 L 1 N C6 L
2 TN C6 L 8");
 //Victory tone, seems to only play out of the wired connection and not the Bluetooth
 }
 BTModu.stdCmd("MUSIC PLAY"); //Connection attempt failed. Back to waiting
 }
}
