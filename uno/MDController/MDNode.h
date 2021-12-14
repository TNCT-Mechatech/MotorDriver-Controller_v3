#ifndef MDNODE_H_
#define MDNODE_H_

#include <Arduino.h>
#include <MotorDriver.h>

#include "I2CNodeHandler.h"

class MDNode : public I2CSlaveNode
{

    /* data(3byte): | duty(255 ~ 0)| direction(bool(0 or other))| deadtime(255ms~0ms)| */
    virtual inline void _res_cb(uint8_t* data, uint8_t &len)
    {
        if (len >= 3) {
            _pwm = ((uint8_t)data[0]) * ((data[1])  ? 1 : -1);
            _deadtime_ms = data[2];
            _md->deadtimeDrive(_deadtime_ms, _pwm);
        }
    }

    /* data(3byte): | duty(255 ~ 0)| direction(bool(0 or other))| deadtime(255ms~0ms)| */
    virtual inline void _req_cb(uint8_t* data, uint8_t &len)
    {
        len = 3;
        data[0] = abs(_pwm) & 0xFF;
        data[1] = _pwm >= 0;
        data[2] = _deadtime_ms;
    }

public:
    MDNode(uint8_t sub_addr, MotorDriver *md, uint8_t deadtime_ms = 0)
    : I2CSlaveNode(sub_addr), _md(md), _deadtime_ms(deadtime_ms), _pwm(0)
    {}

    void begin(I2CNodeHandler *server)
    {
        server->add_node(this);
    }

    int16_t _pwm;
private:
    MotorDriver *_md;
    
    uint8_t _deadtime_ms;
};

#endif
