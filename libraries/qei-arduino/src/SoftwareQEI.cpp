/*
 * SoftwareQEI.cpp
 *
 * Created: 2017/03/19 11:37:56
 *  Author: Na-x4 <Na-x4@outlook.com>
 */

#include "SoftwareQEI.h"

#ifdef ARDUINO_ARCH_AVR
#if defined(digitalPinToPCICRbit) && defined(digitalPinToPCICR) && defined(digitalPinToPCMSK)

#include <assert.h>

extern "C" {
#include "utility/PCIntQEI.h"
};

SoftwareQEI::SoftwareQEI(uint8_t pina, uint8_t pinb)
 : pina_(pina), pinb_(pinb), count_(0) {
    assert(digitalPinToPort(pina_) == digitalPinToPort(pinb_));
    pinMode(pina_, INPUT_PULLUP);
    pinMode(pinb_, INPUT_PULLUP);
    qeiCh_ = pcint_addQEICh(pina_, pinb_, &count_);
}

SoftwareQEI::~SoftwareQEI() {
    pcint_removeQEICh(pina_, pinb_, qeiCh_);
}

long SoftwareQEI::read() {
    return count_;
}

void SoftwareQEI::write(long value) {
    count_ = value;
}


#else
#warning "PCINT macros for this board are not defined."
#endif

#else
#warning "SoftwareQEI only supports boards with an AVR processor."
#endif
