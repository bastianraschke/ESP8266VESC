#include "ESP8266VESC.hpp"
#include "vedder/crc.h"
#include "vedder/buffer.h"
#include "vedder/datatypes.h"


/*


    One Start byte (value 2 for short packets and 3 for long packets)
    One or two bytes specifying the packet length
    The payload of the packet
    Two bytes with a CRC checksum on the payload
    One stop byte (value 3)

First byte:

0x02 for payload length of 256 byte >> next byte is for the payload length

0x03 for >256 byte payload length >> next 2 byte for the payload length

The follwing 2 bytes after the payload are the checksum. (see crc.h)

The byte stream it terminated with a 0x03.

COMM_SET_DUTY,
COMM_SET_CURRENT,
COMM_SET_CURRENT_BRAKE,
COMM_SET_RPM,



*/





ESP8266VESC::ESP8266VESC(SoftwareSerial &serial):
_serial(serial)
{

}

void ESP8266VESC::setDuty(float dutyValue)
{
    uint8_t payload[5] = {0};
    int32_t index = 0;

    payload[index++] = COMM_SET_DUTY;

    double dutyScale = 100000.0;
    buffer_append_float32(payload, dutyValue, dutyScale, &index);

    _sendPacket(payload, sizeof(payload));
}

void ESP8266VESC::setCurrent(float currentInAmpere)
{
    uint8_t payload[5] = {0};
    int32_t index = 0;

    payload[index++] = COMM_SET_CURRENT;

    int32_t currentInMilliampere = (int32_t) (currentInAmpere * 1000);
    buffer_append_int32(payload, currentInMilliampere, &index);

    _sendPacket(payload, sizeof(payload));
}

void ESP8266VESC::setCurrentBrake(float currentInAmpere)
{
    uint8_t payload[5] = {0};
    int32_t index = 0;

    payload[index++] = COMM_SET_CURRENT_BRAKE;

    int32_t currentInMilliampere = (int32_t) (currentInAmpere * 1000);
    buffer_append_int32(payload, currentInMilliampere, &index);

    _sendPacket(payload, sizeof(payload));
}

void ESP8266VESC::releaseEngine()
{
    setCurrent(0.0f);
}

void ESP8266VESC::_sendPacket(uint8_t* payload, uint16_t length)
{
    // TODO: length for long packets
    uint8_t packet[256] = {0};
    int32_t index = 0;

    if (length <= 256)
    {
        packet[index++] = PACKET_LENGTH_SHORT;
        packet[index++] = length;
    }
    else
    {
        packet[index++] = PACKET_LENGTH_LONG;

        // The packet length is splitted up to 2 bytes
        packet[index++] = (uint8_t) (length >> 8);
        packet[index++] = (uint8_t) (length >> 0 & 0xFF);
    }

    // Copy payload to packet
    memcpy(&packet[index], payload, length);
    index += length;

    // CRC16 checksum (2 bytes)
    uint16_t crc16Checksum = crc16(payload, length);
    packet[index++] = (uint8_t) (crc16Checksum >> 8);
    packet[index++] = (uint8_t) (crc16Checksum >> 0 & 0xFF);

    // Termination byte
    packet[index++] = PACKET_TERMINATION_BYTE;

    _serial.write(packet, index);
}
