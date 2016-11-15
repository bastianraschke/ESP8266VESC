#include <ESP8266VESC.h>
#include <SoftwareSerial.h>

SoftwareSerial softwareSerial = SoftwareSerial(14, 12); // ESP8266 (NodeMCU); RX (D5), TX (D6 / GPIO12)
ESP8266VESC esp8266VESC = ESP8266VESC(softwareSerial);

void setup()
{
    // Debug output
    Serial.begin(115200);
    delay(500);

    // Setup serial connection to VESC
    softwareSerial.begin(115200);
    delay(500);
}

void loop()
{
    Serial.println("Setting duty cycle to -1.0");
    esp8266VESC.setDutyCycle(-1.0f);
    delay(2000);

    Serial.println("Setting duty cycle to -0.5");
    esp8266VESC.setDutyCycle(-0.5f);
    delay(2000);

    Serial.println("Setting duty cycle to 0.0");
    esp8266VESC.setDutyCycle(0.0f);
    delay(2000);

    Serial.println("Setting duty cycle to 0.5");
    esp8266VESC.setDutyCycle(0.5f);
    delay(2000);

    Serial.println("Setting duty cycle to 1.0");
    esp8266VESC.setDutyCycle(1.0f);
    delay(2000);

    Serial.println("Setting duty cycle to 0.0");
    esp8266VESC.setDutyCycle(0.0f);
    delay(2000);

    Serial.println();
    delay(1000);
}
