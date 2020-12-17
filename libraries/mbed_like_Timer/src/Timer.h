#ifndef GENERIC_TIMER_H
#define GENERIC_TIMER_H

#include <limits.h>

#if defined(ARDUINO)
#include <Arduino.h>
#else
#include <mbed.h>
#endif

#if defined(Arduino_h) && defined(__AVR__)

typedef uint64_t us_timestamp_t;

class Timer {
  public :
    Timer() :  _running(), _start(), _time(), _total_micros(micros()), _last_micros(micros())
    {
      reset();
    }
    ~Timer();

    void start();
    void stop();
    void reset();

    int32_t read_us();
    float read();
    int32_t read_ms();

    us_timestamp_t read_high_resolution_us();

    operator float();
  protected :
    us_timestamp_t _high_resolution_micros();

    us_timestamp_t _slicetime();

    int _running;
    us_timestamp_t _start, _time, _total_micros;
    volatile unsigned long _last_micros;
};


#define wait(s)     delay((uint32_t)(s * 1000))
#define wait_ms(ms) delay(ms)
#define wait_us(us) delayMicroseconds(us)

#elif defined(MBED_H)

#define delay(ms)             wait_ms(ms)
#define delayMicroseconds(us) wait_us(us)

#endif // defined(Arduino_h) && defined(__AVR__)

#endif // #ifndef GENERIC_TIMER_H
