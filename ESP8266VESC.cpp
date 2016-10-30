#include "ESP8266VESC.hpp"
#include "vedder/crc.h"


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


#define PACKET_LENGTH_SHORT = 0x02;
#define PACKET_LENGTH_LONG = 0x03;

#define PACKET_TERMINATION_BYTE = 0x03;


ESP8266VESC::ESP8266VESC(SoftwareSerial &serial):
_serial(serial)
{

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

    _serial.write(packet[i], index);
}
