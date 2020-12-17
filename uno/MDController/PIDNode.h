#ifndef PIDNODE_H_
#define PIDNODE_H_

#include <Arduino.h>

#include <FastPID.h>
#include <MotorDriver.h>

#define CONTROLL_FLQ    100  //Hz
#define OFFSET_RANGE    1   //pluse/sec
#define PWM_RANGE       255 //pulus minus


#include "QEINode.h"

#include "I2CNodeHandler.h"

namespace format 
{
    void ftoa(float input, uint8_t output[])
    {
        union{float f; uint8_t c[4];} tmp;
        tmp.f = input;
        int i = 3;
        do{
            output[i] = tmp.c[i];
        }while(i--);
    }

    float atof(uint8_t input[])
    {
        union{float f; uint8_t c[4];} tmp;
        int i = 3;
        do{
            tmp.c[i] = input[i];
        }while(i--);
    
        return tmp.f;
    }
};

class PIDNode : public I2CSlaveNode, public FastPID
{

    /* data(5byte): | pid stop(bool)| target pps(float 32bit)| */
    virtual inline void _res_cb(uint8_t* data, uint8_t &len)
    {
        if(len == 5){
            bool stop = (bool)data[0];
            float pps_in = format::atof(++data);
            if(stop) {
                FastPID::clear();
                _pps_in = 0;
                _pps_back = 0;
                _last_pwm_out = 0;
                _md->drive(0);
            } else {
                _pps_in = pps_in; 
                _pps_back = _qei->get_pps(_idd_id);
                _last_pwm_out = FastPID::step(_pps_in * _feedback_range, _pps_back * _feedback_range);
                _md->drive(_last_pwm_out);
            }
        } else {
            FastPID::clear();
            _pps_in = 0;
            _pps_back = 0;
            _last_pwm_out = 0;
            _md->drive(0);
        }
    }

    /* data(6byte): | now target pps(float 32bit)| now duty(255 ~ 0)| direction(bool(0 or other))|*/
    virtual inline void _req_cb(uint8_t* data, uint8_t &len)
    {
        len = 6;
        float pps_in = _pps_in;
        format::ftoa(pps_in, data);
        data[4] = (uint8_t)abs(_last_pwm_out) & 0xFF;
        data[5] = _last_pwm_out >= 0; 
    }

public:
    PIDNode(uint8_t sub_addr, double Kp, double Ki, double Kd, uint32_t feedback_range)
    : I2CSlaveNode(sub_addr),
    FastPID(Kp, Ki, Kd, CONTROLL_FLQ, 9/*bit*/, true), _feedback_range(feedback_range)
    {
        _pps_in = 0;
        _pps_back = 0;
    }

    void begin(I2CNodeHandler *server, MotorDriver *md, QEINodes *qei, uint8_t idd_id)
    {
        server->add_node(this);
        _md = md;
        _qei = qei;
        _idd_id = idd_id;
        _md->drive(0);
    }   

private:
    MotorDriver *_md;
    QEINodes *_qei;  

    uint8_t _idd_id;

    float _interval;
    
    int16_t _last_pwm_out;
    double _pps_back;
    double _pps_in;

    uint32_t _feedback_range;
};

#endif
