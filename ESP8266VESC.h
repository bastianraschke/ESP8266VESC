#ifndef __ESP8266VESC_H__
#define __ESP8266VESC_H__

#include <Stream.h>

#define PACKET_LENGTH_SHORT 0x02;
#define PACKET_LENGTH_LONG 0x03;

#define PACKET_TERMINATION_BYTE 0x03;


class ESP8266VESC
{
public:

    ESP8266VESC(Stream &serial);

    /**
     * Set duty cycle value from -1.0 (full throttle backwards) to 1.0 (full throttle forwards).
     *
     */
    void setDutyCycle(float dutyValue);

    /**
     * Set current value in ampere.
     *
     */
    void setCurrent(float currentInAmpere);

    /**
     * Set current value for breaking in ampere.
     *
     */
    void setCurrentBrake(float currentInAmpere);

    /**
     * Set current RPM value in rounds per minute.
     *
     */
    void setRPM(int32_t rpmValue);

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

    void _sendPacket(uint8_t* payload, uint16_t length);
};

#endif
