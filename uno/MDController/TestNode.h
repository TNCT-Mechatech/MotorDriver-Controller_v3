#ifndef TESTNODE_H_
#define TESTNODE_H_

#include <Arduino.h>

#include "I2CNodeHandler.h"

class TestNode : public I2CSlaveNode
{
    virtual inline void _res_cb(uint8_t* data, uint8_t &len)
    {
        if(len < BUFFER_SIZE - 2) {
            Serial.println("Test message :" + String((char*)data));
        }
    }

    /* data(2byte): | pong(bool = 0xFF)| total nodes(0~255)| */
    virtual inline void _req_cb(uint8_t* data, uint8_t &len)
    {
        len = 3;
        data[0] = 0xFF;
        data[1] = _total_nodes;
    }

public:
    TestNode(uint8_t sub_addr)
    : I2CSlaveNode(sub_addr){}

    void begin(I2CNodeHandler *server)
    {
        server->add_node(this);
    }

    void set_total_nodes(int num)
    {
        _total_nodes = num;
    }

private:
    uint8_t _total_nodes;
};

#endif
