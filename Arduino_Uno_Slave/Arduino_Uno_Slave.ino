#include <Wire.h>
#include <ArduinoJson.h>


void setup() {
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Serial.begin(115200);           /* start serial for debug */
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(3,OUTPUT);
}

void loop() {
  delay(100);
}

void processCall(String command) {
  JsonObject jsonBuffer;
  DynamicJsonDocument doc(1024);
  auto error = deserializeJson(doc, command);

  if (!error) {
    int gpio = doc["gpio"];
    Serial.println(gpio);
    int state = doc["state"];
    Serial.println(state);

    //set GPIO state
    digitalWrite(LED_BUILTIN,!state);
    digitalWrite(gpio, state);
  } else {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }
}


// function that executes when data is received from master
void receiveEvent(int howMany) {
  String data = "";
  while (0 < Wire.available()) {
    char c = Wire.read();      /* receive byte as a character */
    data += c;

  }
  Serial.println(data);           /* print the request data */
  processCall(data);         /* to newline */
}
