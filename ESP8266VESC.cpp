#include "ESP8266VESC.h"

// The C files must be included specially for C++
extern "C"
{
    #include "crc.h"
    #include "buffer.h"
    #include "datatypes.h"
}


ESP8266VESC::ESP8266VESC(Stream &serial):
_serial(serial)
{

}

void ESP8266VESC::setDutyCycle(float dutyValue)
{
    int32_t index = 0;

    // Length: 1 byte packet ID and 4 bytes for float (32 bit)
    uint8_t payload[5] = {0};

    payload[index++] = COMM_SET_DUTY;

    float dutyScale = 100000.0f;
    buffer_append_float32(payload, dutyValue, dutyScale, &index);

    _sendPacket(payload, sizeof(payload));
}

void ESP8266VESC::setCurrent(float currentInAmpere)
{
    int32_t index = 0;

    // Length: 1 byte packet ID and 4 bytes for integer (32 bit)
    uint8_t payload[5] = {0};

    payload[index++] = COMM_SET_CURRENT;

    int32_t currentInMilliampere = (int32_t) (currentInAmpere * 1000);
    buffer_append_int32(payload, currentInMilliampere, &index);

    _sendPacket(payload, sizeof(payload));
}

void ESP8266VESC::setCurrentBrake(float currentInAmpere)
{
    int32_t index = 0;

    // Length: 1 byte packet ID and 4 bytes for integer (32 bit)
    uint8_t payload[5] = {0};

    payload[index++] = COMM_SET_CURRENT_BRAKE;

    int32_t currentInMilliampere = (int32_t) (currentInAmpere * 1000);
    buffer_append_int32(payload, currentInMilliampere, &index);

    _sendPacket(payload, sizeof(payload));
}

void ESP8266VESC::setRPM(int32_t rpmValue)
{
    int32_t index = 0;

    // Length: 1 byte packet ID and 4 bytes for integer (32 bit)
    uint8_t payload[5] = {0};

    payload[index++] = COMM_SET_RPM;

    buffer_append_int32(payload, rpmValue, &index);

    _sendPacket(payload, sizeof(payload));
}

void ESP8266VESC::releaseEngine()
{
    // Set no engine current means idling
    setCurrent(0.0f);
}

void ESP8266VESC::fullBreaking()
{
    // Set duty cycle to 0.0 means full breaking
    setDutyCycle(0.0f);
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
