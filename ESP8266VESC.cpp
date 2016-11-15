#include "ESP8266VESC.h"
#include <Arduino.h>

// The C files must be included specially for C++
extern "C"
{
    #include "crc.h"
    #include "buffer.h"
    #include "datatypes.h"
}


ESP8266VESC::ESP8266VESC(Stream &serial)
: _serial(serial)
{

}

void ESP8266VESC::setDutyCycle(const float dutyValue)
{
    int32_t index = 0;

    // Length: 1 byte packet ID and 4 bytes for float (32 bit)
    uint8_t payload[5] = {0};

    payload[index++] = COMM_SET_DUTY;

    float dutyScale = 100000.0f;
    buffer_append_float32(payload, dutyValue, dutyScale, &index);

    _sendPacket(payload, sizeof(payload));
}

void ESP8266VESC::setCurrent(const float currentInAmpere)
{
    int32_t index = 0;

    // Length: 1 byte packet ID and 4 bytes for integer (32 bit)
    uint8_t payload[5] = {0};

    payload[index++] = COMM_SET_CURRENT;

    int32_t currentInMilliampere = (int32_t) (currentInAmpere * 1000);
    buffer_append_int32(payload, currentInMilliampere, &index);

    _sendPacket(payload, sizeof(payload));
}

void ESP8266VESC::setCurrentBrake(const float currentInAmpere)
{
    int32_t index = 0;

    // Length: 1 byte packet ID and 4 bytes for integer (32 bit)
    uint8_t payload[5] = {0};

    payload[index++] = COMM_SET_CURRENT_BRAKE;

    int32_t currentInMilliampere = (int32_t) (currentInAmpere * 1000);
    buffer_append_int32(payload, currentInMilliampere, &index);

    _sendPacket(payload, sizeof(payload));
}

void ESP8266VESC::setRPM(const int32_t rpmValue)
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

bool ESP8266VESC::getVESCValues(VESCValues &vescValues)
{
    bool wasSuccessful = false;

    // Length: 1 byte packet ID
    uint8_t payload[1] = {0};
    payload[0] = COMM_GET_VALUES;

    // Send packet and wait some time to let the VESC process the request
    _sendPacket(payload, sizeof(payload));
    delay(100);

    uint8_t receivedPayload[256] = {0};
    uint16_t packetPayloadLength = _receivePacket(receivedPayload);

    // The packet has a length of 56 bytes minimum
    if (packetPayloadLength >= 56)
    {
        COMM_PACKET_ID receivedPacketID = (COMM_PACKET_ID) receivedPayload[0];
        int32_t index = 0;

        // Skip the packet ID (first bit)
        index++;

        if (receivedPacketID == COMM_GET_VALUES)
        {
            index = 14; // Skipped 14 bit
            vescValues.avgMotorCurrent = buffer_get_float32(receivedPayload, 100.0, &index);
            vescValues.avgInputCurrent = buffer_get_float32(receivedPayload, 100.0, &index);
            vescValues.dutyCycleNow = buffer_get_float16(receivedPayload, 1000.0, &index);
            vescValues.rpm = buffer_get_int32(receivedPayload, &index);
            vescValues.inpVoltage = buffer_get_float16(receivedPayload, 10.0, &index);
            vescValues.ampHours = buffer_get_float32(receivedPayload, 10000.0, &index);
            vescValues.ampHoursCharged = buffer_get_float32(receivedPayload, 10000.0, &index);
            index += 8; //Skip 9 bit
            vescValues.tachometer = buffer_get_int32(receivedPayload, &index);
            vescValues.tachometerAbs = buffer_get_int32(receivedPayload, &index);

            wasSuccessful = true;
        }
        else
        {
            Serial.println("The received packet is not a COMM_GET_VALUES packet!");
            wasSuccessful = false;
        }
    }
    else
    {
        Serial.println("The received packet is too short!");
        wasSuccessful = false;
    }

    return wasSuccessful;
}










uint16_t ESP8266VESC::_receivePacket(uint8_t payload[])
{
    bool packetWasReceived = false;
    bool packetPayloadWasUnpacked = false;

    uint8_t packet[PACKET_MAX_LENGTH] = {0};
    uint16_t index = 0;

    uint16_t packetLength = 0;
    uint16_t packetPayloadLength = 0;

    // Read bytes until available
    while (_serial.available())
    {
        packet[index] = _serial.read();

        // When the index reaches 2, three bytes are received:
        // packet[0]: start byte indicating packet length type
        // packet[1]: packet length (first part)
        // packet[2]: packet length (second part if packet lenth type is PACKET_LENGTH_IDENTIFICATION_BYTE_LONG)
        if (index == 2)
        {
            uint8_t packetLengthType = packet[0];

            switch(packetLengthType)
            {
                case PACKET_LENGTH_IDENTIFICATION_BYTE_SHORT:
                {
                    packetPayloadLength = packet[1];

                    // Start byte + packet length type byte + payload + 2 bytes (CRC) + termination byte
                    packetLength = 1 + 1 + packetPayloadLength + 2 + 1;
                }
                break;

                case PACKET_LENGTH_IDENTIFICATION_BYTE_LONG:
                {
                    // The length is splitted into 2 bytes
                    packetPayloadLength = (packet[1] << 8) | packet[2];

                    // Start byte + packet length type bytes + payload + 2 bytes (CRC) + termination byte
                    packetLength = 1 + 2 + packetPayloadLength + 2 + 1;
                }
                break;
            }
        }

        // Give up if the packet is longer than the maximum packet length
        if (index >= PACKET_MAX_LENGTH)
        {
            Serial.println("Packet is too long!");
            break;
        }

        uint8_t lastPacketByte = packet[packetLength - 1];

        // When the index reaches the last byte (index begins at 0, so length - 1), check if the termination byte is set correctly
        if (index == packetLength - 1 && lastPacketByte == PACKET_TERMINATION_BYTE)
        {
            packetWasReceived = true;
            break;
        }

        // If still not broke out of the loop, increment the byte index
        index++;
    }

    if (packetWasReceived == true)
    {
        Serial.print("packetLength = ");
        Serial.println(packetLength);

        Serial.print("packetPayloadLength = ");
        Serial.println(packetPayloadLength);

        // TODO handle bigger packets index 2 or 3 start depends onm type
        memcpy(payload, &packet[2], packetPayloadLength);

        uint16_t packetCRC = (packet[packetLength - 3] << 8) | packet[packetLength - 2];
        uint16_t calculatedPacketCRC = crc16(payload, packetPayloadLength);

        Serial.println("packet:");
        debugPrintArray(packet, packetLength);

        Serial.print("packetCRC = ");
        Serial.println(packetCRC);

        Serial.print("calculatedPacketCRC = ");
        Serial.println(calculatedPacketCRC);

        // Serial.println("payload:");
        // debugPrintArray(payload, packetPayloadLength);

        // Check if packet was received correctly
        if (packetCRC == calculatedPacketCRC)
        {
            packetPayloadWasUnpacked = true;
        }
        else
        {
            Serial.println("The CRC value is different!");
        }
    }

    if (packetWasReceived && packetPayloadWasUnpacked)
    {
        return packetPayloadLength;
    }
    else
    {
        return 0;
    }
}

void ESP8266VESC::_sendPacket(uint8_t payload[], uint16_t length)
{
    // Check if we got valid pointer and length is valid
    if ( !payload || length == 0 )
    {
        return ;
    }

    if ( length > PACKET_MAX_LENGTH )
    {
        Serial.println("The packet is larger than PACKET_MAX_LENGTH bytes!");
        return ;
    }

    uint8_t packet[PACKET_MAX_LENGTH] = {0};
    uint16_t index = 0;

    if (length <= 256)
    {
        packet[index++] = PACKET_LENGTH_IDENTIFICATION_BYTE_SHORT;
        packet[index++] = length;
    }
    else
    {
        packet[index++] = PACKET_LENGTH_IDENTIFICATION_BYTE_LONG;

        // The packet length is splitted up to 2 bytes
        packet[index++] = (uint8_t) (length >> 8);
        packet[index++] = (uint8_t) (length >> 0 & 0xFF);
    }

    // Copy payload to packet starting at index
    memcpy(&packet[index], payload, length);

    // Increment packet index by number of copied payload bytes
    index += length;

    // CRC16 checksum (2 bytes)
    uint16_t crc16Checksum = crc16(payload, length);
    packet[index++] = (uint8_t) (crc16Checksum >> 8);
    packet[index++] = (uint8_t) (crc16Checksum >> 0 & 0xFF);

    // Termination byte
    packet[index++] = PACKET_TERMINATION_BYTE;

    // Write packet until index
    _serial.write(packet, index);
}

void ESP8266VESC::debugPrintArray(uint8_t values[], int length)
{
    // Pointer sanity check
    if ( !values || length == 0 )
    {
        return ;
    }

    char buffer[16];

    for (int i = 0; i < length; i++)
    {
        sprintf(buffer, "%02X ", values[i]);
        Serial.print(buffer);
    }

    Serial.println();
}
