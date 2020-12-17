#ifndef SOFTWARE_I2C_MASTER_H_
#define SOFTWARE_I2C_MASTER_H_

#include <Arduino.h>
#include "I2CMaster.h"

#include <SoftWire.h>

class SoftWareI2CMaster : public I2CMaster
{
public:
    SoftWareI2CMaster()
    {
        _master = new SoftWire();
        begin();
    }

    void end()
    {
        _master->end();
    }

    virtual void begin()
    {
        _master->begin();
    }

    virtual void frequency(long hz)
    {
        _master->setClock(hz);
    }
    virtual int write(int address_7bit, const char *data, const int length, bool repeated = false)
    {
        int address = address_7bit;
        _master->beginTransmission(address);
        _master->write(data, length);
        return _master->endTransmission(!repeated);
    }
    virtual int read(int address_7bit, char *data, const int length, bool repeated = false)
    {
        int address = address_7bit;
        _master->requestFrom(address, length, !repeated);
        if (_master->available() == length)
        {
            for (int i = 0; i < length; i++)
            {
                data[i] = _master->read();
            }
            return 0;
        }
        else
        {
            return 1; // got failure(nack)
        }
    }

private:
    SoftWire *_master;
};

#endif // #ifndef SOFTWARE_I2C_MASTER_H_
