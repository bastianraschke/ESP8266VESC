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
        Serial.println("Temperature Mosfet #1 = " + String(vescValues.temperatureMosfet1) + " degree");
        Serial.println("Temperature Mosfet #2 = " + String(vescValues.temperatureMosfet2) + " degree");
        Serial.println("Temperature Mosfet #3 = " + String(vescValues.temperatureMosfet3) + " degree");
        Serial.println("Temperature Mosfet #4 = " + String(vescValues.temperatureMosfet4) + " degree");
        Serial.println("Temperature Mosfet #5 = " + String(vescValues.temperatureMosfet5) + " degree");
        Serial.println("Temperature Mosfet #6 = " + String(vescValues.temperatureMosfet6) + " degree");
        Serial.println("Temperature PCB = " + String(vescValues.temperaturePCB) + " degree");

        Serial.println("Average motor current = " + String(vescValues.avgMotorCurrent) + "A");
        Serial.println("Average battery current = " + String(vescValues.avgInputCurrent) + "A");
        Serial.println("Duty cycle = " + String(vescValues.dutyCycleNow) + "%");
        
        Serial.println("rpm = " + String(vescValues.rpm) + "rpm");
        Serial.println("Battery voltage = " + String(vescValues.inputVoltage) + "V");
        
        Serial.println("Drawn energy (mAh) = " + String(vescValues.ampHours) + "mAh");
        Serial.println("Charged energy (mAh) = " + String(vescValues.ampHoursCharged) + "mAh");

        Serial.println("Drawn energy (Wh) = " + String(vescValues.wattHours) + "Wh");
        Serial.println("Charged energy (Wh) = " + String(vescValues.wattHoursCharged) + "Wh");

        Serial.println("tachometer = " + String(vescValues.tachometer));
        Serial.println("tachometerAbs = " + String(vescValues.tachometerAbs));
    }
    else
    {
        Serial.println("The VESC values could not be read!");
    }

    Serial.println();
    delay(200);
}
