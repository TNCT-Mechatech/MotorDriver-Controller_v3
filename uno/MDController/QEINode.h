#ifndef QEINODE_H_
#define QEINODE_H_

#define IDD_MAX_NUM 4
#define IDD_DISENABLE_TIMER

#include <Arduino.h>
#include <IncrementalDecoderDriver.h>

#include "I2CNodeHandler.h"
#include "Config.h"

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
      
        //Serial.println(_node[0]->pps/96);
        
        return _node[id]->pps;
    }

    void task(uint8_t id)
    {
        _node[id]->_task();
        //Serial.println(_node[0]->pps/96);
    }

private:
    class QEINode_ : public I2CSlaveNode
    {
    public:
        QEINode_(uint8_t sub_addr, uint8_t id, IDD *qei) 
        :I2CSlaveNode(sub_addr), _id(id), _qei(qei), _initTime(millis())
        {
            _count = 0;
            _prev_count = 0;
            _prevTime = _initTime;
            _nowTime = millis() - _initTime;
        }

        void _task()
        {
            static uint8_t diff2_Time = 100;
            _nowTime = millis() - _initTime;
            _count = _qei->get_count(_id) * (pid_conf[_id]->str.qei_dir?-1:1);
            diff1_Time += _nowTime - _prevTime;
            if(i == 9){
            diff2_Time = diff1_Time / 10;
            diff1_Time = 0;
            i = 0;
            }
            
            pps = (_count - _prev_count) / ((diff2_Time) / 1000.0f);
            _prev_count = _count;
            _prevTime = _nowTime;
            i++;
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
            data[0] = _nowTime & 0xFF;
            data[1] = (_nowTime >> 8) & 0xFF;
            data[2] = (_nowTime >> 16) & 0xFF;
            data[3] = (_nowTime >> 24) & 0xFF;

            data[4] = _count & 0xFF;
            data[5] = (_count >> 8) & 0xFF;
            data[6] = (_count >> 16) & 0xFF;
            data[7] = (_count >> 24) & 0xFF;
        }

        double pps;
    private:
        uint8_t _id;
        uint8_t i;
        uint8_t diff1_Time;
        IDD *_qei;

        const uint32_t _initTime;
        uint32_t _nowTime;
        int32_t _count;

        int32_t _prev_count;
        uint32_t _prevTime;
    };
    
    uint8_t _sub_addr_begin;
    IDD *_qei;
    
    QEINode_ *_node[IDD_MAX_NUM];
};

#endif
