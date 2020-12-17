#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_


class I2CMaster
{
public:
    virtual void begin() = 0;
    virtual void frequency(long hz) = 0;
    virtual int write(int address_7bit, const char *data, const int length, bool repeated = false) = 0;
    virtual int read(int address_7bit, char *data, const int length, bool repeated = false) = 0;
};

#endif // #ifndef I2C_MASTER_H_
