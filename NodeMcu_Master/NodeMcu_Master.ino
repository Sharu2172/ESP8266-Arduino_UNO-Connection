#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include <Wire.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <BlynkSimpleEsp8266.h>

//on/off callbacks
void officeLightsOn();
void officeLightsOff();
void kitchenLightsOn();
void kitchenLightsOff();

//I am using ESP8266 EPS-12E GPIO16 and GPIO14
#define BLYNK_TEMPLATE_ID "TMPLZtMP56dv"
#define BLYNK_DEVICE_NAME "Simple LED Control"
#define BLYNK_AUTH_TOKEN "R36r64Ej66qmn_SwI6rOLAg1YbkzwbSQ"
#define BLYNK_PRINT Serial

const int relayPin2 = 4;
char auth[] = BLYNK_AUTH_TOKEN;
const char* ssid;
const char* pass;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *office = NULL;
Switch *kitchen = NULL;

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();
  digitalWrite(LED_BUILTIN, !value);
  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  
  // Initialise wifi connection
  WiFiManager wifiManager;
  // Uncomment and run it once, if you want to erase all the stored information
   wifiManager.resetSettings();
  // set custom ip for portal
  // wifiManager.setAPConfig(IPAddress(192,168,0,24), IPAddress(192,168,0,1), IPAddress(255,255,255,0));
  wifiManager.autoConnect("Sharath ESP8266");

  ssid = WiFi.SSID().c_str();
  pass = WiFi.psk().c_str();

  upnpBroadcastResponder.beginUdpMulticast();

  // Define your switches here. Max 14
  // Format: Alexa invocation name, local port no, on callback, off callback
  office = new Switch("office lights", 80, officeLightsOn, officeLightsOff);
  kitchen = new Switch("kitchen lights", 81, kitchenLightsOn, kitchenLightsOff);

  Serial.println("Adding switches upnp broadcast responder");
  upnpBroadcastResponder.addDevice(*office);
  upnpBroadcastResponder.addDevice(*kitchen);

  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  
//  Blynk.begin(auth,ssid,pass);
  // Setup a function to be called every second
//  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  upnpBroadcastResponder.serverLoop();
  kitchen->serverLoop();
  office->serverLoop();

  BUILTIN_LED_ON();
  kitchenLightsOn();
  delay(5000);
  BUILTIN_LED_OFF();
  kitchenLightsOff();
  delay(5000);
}

void BUILTIN_LED_ON()
{
  Serial.print("Switch 1 turn on ...");
  digitalWrite(LED_BUILTIN, LOW);
}

void BUILTIN_LED_OFF()
{
  Serial.print("Switch 1 turn off ...");
  digitalWrite(LED_BUILTIN, HIGH);
}

void kitchenLightsOn()
{
  Serial.print("Switch 2 turn on ...");
  Wire.beginTransmission(8);              /* begin with device address 8 */
  Wire.write("{\"gpio\":3,\"state\":1}"); /* sends hello string */
  Wire.endTransmission();                 /* stop transmitting */
}

void kitchenLightsOff()
{
  Serial.print("Switch 2 turn off ...");
  Wire.beginTransmission(8);              /* begin with device address 8 */
  Wire.write("{\"gpio\":3,\"state\":0}"); /* sends hello string */
  Wire.endTransmission();                 /* stop transmitting */
}
