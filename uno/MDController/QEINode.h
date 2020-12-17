#ifndef QEINODE_H_
#define QEINODE_H_

#define IDD_MAX_NUM 4
#define IDD_DISENABLE_TIMER

#include <Arduino.h>
#include <IncrementalDecoderDriver.h>

#include "I2CNodeHandler.h"

class QEINodes
{
public:

    QEINodes(uint8_t sub_addr_begin, IDD *qei)
    : _sub_addr_begin(sub_addr_begin), _qei(qei)
    {}

    void begin(I2CNodeHandler *server)
    {
        for (int i = 0; i < _qei->get_slave_num(); i++) {
            if(i >= IDD_MAX_NUM) break;
            _node[i] = new QEINode_(_sub_addr_begin + i, i, _qei);
            server->add_node(_node[i]);
        }
    }

    float get_pps(uint8_t id)
    {
        return _node[id]->pps;
    }

private:
    class QEINode_ : public I2CSlaveNode
    {
    public:
        QEINode_(uint8_t sub_addr, uint8_t id, IDD *qei) 
        :I2CSlaveNode(sub_addr), _id(id), _qei(qei), _initTime(millis())
        {
            _prev_count = 0;
            _prevTime = 0;
        }


        /* data(4byte): | (int32_t) count| */
        virtual inline void _res_cb(uint8_t* data, uint8_t &len)
        {
            if (len >= 4) {
                int32_t count = data[0] | (int32_t)data[1] << 8 | (int32_t)data[2] << 16 | (int32_t)data[3] << 24;
                _qei->set_count(_id, count);
            }
        }

        /* data(8byte): | (uint32_t)now microSec| (int32_t)now count| */
        virtual inline void _req_cb(uint8_t* data, uint8_t &len)
        {
            len = 8;
            uint32_t nowTime = millis() - _initTime;
            int32_t count = _qei->get_count(_id);
            data[0] = nowTime       & 0xFF;
            data[1] = (nowTime >> 8)  & 0xFF;
            data[2] = (nowTime >> 16) & 0xFF;
            data[3] = (nowTime >> 24) & 0xFF;

            data[4] = count         & 0xFF;
            data[5] = (count >> 8)    & 0xFF;
            data[6] = (count >> 16)   & 0xFF;
            data[7] = (count >> 24)   & 0xFF;

            pps = (count - _prev_count) / ((nowTime - _prevTime) / 1000.0f);
            _prev_count = count;
            _prevTime = nowTime;
        }

        float pps;
    private:
        uint8_t _id;
        IDD *_qei;
        
        const uint32_t _initTime;
        int32_t _prev_count;
        uint32_t _prevTime;
    };
    
    uint8_t _sub_addr_begin;
    IDD *_qei;
    
    QEINode_ *_node[IDD_MAX_NUM];
};

#endif
