#ifndef __ESP8266VESC_H__
#define __ESP8266VESC_H__

#include <SoftwareSerial.h>



#define PACKET_LENGTH_SHORT 0x02;
#define PACKET_LENGTH_LONG 0x03;

#define PACKET_TERMINATION_BYTE 0x03;



class ESP8266VESC
{
public:
    ESP8266VESC(SoftwareSerial &serial);

    void setDuty(float dutyValue);
    void setCurrent(float currentInAmpere);
    void setCurrentBrake(float currentInAmpere);
    void setRPM(int32_t rpmValue);
    void releaseEngine();

private:
    SoftwareSerial &_serial;

    void _sendPacket(uint8_t* payload, uint16_t length);
};

#endif




