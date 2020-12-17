#include "Timer.h"

#if defined(Arduino_h) && defined(__AVR__)

Timer::~Timer() {
  _running = 0;
}

void Timer::start() {
  if (!_running) {
    _start = _high_resolution_micros();
    _running = 1;
  }
}

void Timer::stop() {
  _time += _slicetime();
  _running = 0;
}

int32_t Timer::read_us() {
  return (int32_t)read_high_resolution_us();
}

float Timer::read() {
  return (float)read_high_resolution_us() / 1000000.0f;
}

int32_t Timer::read_ms() {
  return (int32_t)read_high_resolution_us() / 1000;
}

us_timestamp_t Timer::read_high_resolution_us() {
  us_timestamp_t time = _time + _slicetime();
  return time;
}

Timer::operator float() {
  return read();
}

void Timer::reset() {
  _start = _high_resolution_micros();
  _time = 0;
}

us_timestamp_t Timer::_slicetime() {
  us_timestamp_t ret = 0;
  if (_running) {
    ret = (us_timestamp_t)_high_resolution_micros() - _start;
  }
  return ret;
}

us_timestamp_t Timer::_high_resolution_micros() {
  uint32_t current_micros = micros();
  if (current_micros >= _last_micros)
    _total_micros += current_micros - _last_micros;
  else
    _total_micros += (UINT32_MAX - _last_micros) + current_micros;
  _last_micros = current_micros;
  return _total_micros;
}

#endif
