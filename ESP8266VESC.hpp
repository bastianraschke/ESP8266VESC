#include <SoftwareSerial.h>


#ifndef __ESP8266VESC_H__
#define __ESP8266VESC_H__

class ESP8266VESC
{
public:
    ESP8266VESC(SoftwareSerial &serial);

private:
    SoftwareSerial &_serial;

    void _sendPacket(uint8_t packetPayload[], int packetLength);
};

#endif
