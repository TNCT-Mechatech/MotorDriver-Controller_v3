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
                _rps_in = 0;
                _rps_back = 0;
                _last_pwm_out = 0;
                _md->drive(0);
            } else {
              
                _rps_in = pps_in / _ppr2;//2逓倍で割る
                _rps_back = _qei->get_pps(_idd_id) / _ppr2;
                _last_pwm_out = FastPID::step(_rps_in, _rps_back);
                _md->drive(_last_pwm_out);
            }
        } else {
            FastPID::clear();
            _rps_in = 0;
            _rps_back = 0;
            _last_pwm_out = 0;
            _md->drive(0);
        }
    }

    /* data(6byte): | now target pps(float 32bit)| now duty(255 ~ 0)| direction(bool(0 or other))|*/
    virtual inline void _req_cb(uint8_t* data, uint8_t &len)
    {
        len = 6;
        float pps_in = _rps_in * _ppr2;//2逓倍を掛ける
        format::ftoa(pps_in, data);
        data[4] = (uint8_t)abs(_last_pwm_out) & 0xFF;
        data[5] = _last_pwm_out >= 0; 
    }

public:
    PIDNode(uint8_t sub_addr, double Kp, double Ki, double Kd, uint16_t ppr2)
    : I2CSlaveNode(sub_addr),
    FastPID(Kp, Ki, Kd, CONTROLL_FLQ), _ppr2(ppr2)
    {
        _rps_in = 0;
        _rps_back = 0;
        FastPID::setOutputRange(-240, 240);
    }

    void begin(I2CNodeHandler *server, MotorDriver *md, QEINodes *qei, uint8_t idd_id)
    {
      
        Serial.begin(9600);
        server->add_node(this);
        _md = md;
        _qei = qei;
        _idd_id = idd_id;
        _md->drive(0);
    }   

    int16_t _last_pwm_out;
private:
    MotorDriver *_md;
    QEINodes *_qei;  

    uint8_t _idd_id;

    float _interval;
    
    double _rps_back;
    double _rps_in;

    uint16_t _ppr2;
};

#endif
