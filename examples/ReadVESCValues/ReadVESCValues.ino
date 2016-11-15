#include <ESP8266VESC.h>
#include <SoftwareSerial.h>

SoftwareSerial softwareSerial = SoftwareSerial(14, 12); // ESP8266 (NodeMCU): RX (D5), TX (D6 / GPIO12)
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
    VESCValues vescValues;

    if (esp8266VESC.getVESCValues(vescValues) == true)
    {
        Serial.print("temperatureMosfet1 = "); Serial.println(vescValues.temperatureMosfet1);
        Serial.print("temperatureMosfet2 = "); Serial.println(vescValues.temperatureMosfet2);
        Serial.print("temperatureMosfet3 = "); Serial.println(vescValues.temperatureMosfet3);
        Serial.print("temperatureMosfet4 = "); Serial.println(vescValues.temperatureMosfet4);
        Serial.print("temperatureMosfet5 = "); Serial.println(vescValues.temperatureMosfet5);
        Serial.print("temperatureMosfet6 = "); Serial.println(vescValues.temperatureMosfet6);
        Serial.print("temperaturePCB = "); Serial.println(vescValues.temperaturePCB);
        Serial.print("avgMotorCurrent = "); Serial.println(vescValues.avgMotorCurrent);
        Serial.print("avgInputCurrent = "); Serial.println(vescValues.avgInputCurrent);
        Serial.print("dutyCycleNow = "); Serial.println(vescValues.dutyCycleNow);
        Serial.print("rpm = "); Serial.println(vescValues.rpm);
        Serial.print("inputVoltage = "); Serial.println(vescValues.inputVoltage);
        Serial.print("ampHours = "); Serial.println(vescValues.ampHours);
        Serial.print("ampHoursCharged = "); Serial.println(vescValues.ampHoursCharged);
        Serial.print("wattHours = "); Serial.println(vescValues.wattHours);
        Serial.print("wattHoursCharged = "); Serial.println(vescValues.wattHoursCharged);
        Serial.print("tachometer = "); Serial.println(vescValues.tachometer);
        Serial.print("tachometerAbs = "); Serial.println(vescValues.tachometerAbs);
    }
    else
    {
        Serial.println("The VESC values could not be read!");
    }

    Serial.println();
    delay(1000);
}
