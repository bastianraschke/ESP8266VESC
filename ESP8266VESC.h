#ifndef __ESP8266VESC_H__
#define __ESP8266VESC_H__

#include <Stream.h>

#define PACKET_LENGTH_IDENTIFICATION_BYTE_SHORT 0x02
#define PACKET_LENGTH_IDENTIFICATION_BYTE_LONG 0x03
#define PACKET_TERMINATION_BYTE 0x03

#define PACKET_MAX_LENGTH 512


struct VESCValues {
    //7 Values int16_t not read(14 byte)
    float avgMotorCurrent;
    float avgInputCurrent;
    float dutyCycleNow;
    long rpm;
    float inpVoltage;
    float ampHours;
    float ampHoursCharged;
    //2 values int32_t not read (8 byte)
    long tachometer;
    long tachometerAbs;
};



class ESP8266VESC
{
public:

    ESP8266VESC(Stream &serial);

    /**
     * Set duty cycle value from -1.0 (full throttle backwards) to 1.0 (full throttle forwards).
     *
     */
    void setDutyCycle(const float dutyValue);

    /**
     * Set current value in ampere (e.g. 3.0 for 3A forwards or -3.0 for 3A backwards).
     *
     */
    void setCurrent(const float currentInAmpere);

    /**
     * Set current value for breaking in ampere.
     *
     */
    void setCurrentBrake(const float currentInAmpere);

    /**
     * Set current RPM value in rounds per minute.
     *
     */
    void setRPM(const int32_t rpmValue);

    /**
     * Enable idling of the engine.
     *
     */
    void releaseEngine();

    /**
     * Perform a full breaking of the engine.
     *
     */
    void fullBreaking();

private:
    Stream &_serial;
    Stream &_debugSerial;

    void _sendPacket(uint8_t payload[], uint16_t length);
    void _receivePacket(uint8_t payload[]);
};

#endif
