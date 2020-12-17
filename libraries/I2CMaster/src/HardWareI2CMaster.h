#ifndef HARDWARE_I2C_MASTER_H_
#define HARDWARE_I2C_MASTER_H_

#include <Arduino.h>
#include <Wire.h>


#include "I2CMaster.h"

class HardWareI2CMaster : protected TwoWire, public I2CMaster
{
public:

    HardWareI2CMaster(long hz = 100000)
    {
        begin();
        frequency(hz);
    }

    void end()
    {
        TwoWire::end();
    }

    virtual void begin()
    {
        TwoWire::begin();
    }

    virtual void frequency(int hz)
    {
        TwoWire::setClock(hz);
    }
    virtual int write(int address_7bit, const char* data, const int length, bool repeated = false)
    {
        int address = address_7bit;
        TwoWire::beginTransmission(address);
        TwoWire::write(data, length);
        return TwoWire::endTransmission(!repeated);   
    }
    virtual int read(int address_7bit, char* data, const int length, bool repeated = false)
    {
        int address = address_7bit;
        TwoWire::requestFrom(address, length, !repeated); 
        if(TwoWire::available() == length){
            for(int i = 0; i < length; i++){
                data[i] = TwoWire::read();
            }
            return 0;
        }else{
            return 1; // got failure(nack)
        }
    }
private :
};

#endif // #ifndef HARDWARE_I2C_MASTER_H_
