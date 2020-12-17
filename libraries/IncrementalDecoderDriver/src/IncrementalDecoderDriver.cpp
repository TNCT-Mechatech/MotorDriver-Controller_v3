#include "IncrementalDecoderDriver.h"

IncrementalDecoderDriver::IncrementalDecoderDriver(I2CMaster *master, const int max_slave_num) :
  _master(master), _max_slave_num(max_slave_num), _addresses(), _last_count(new int32_t[max_slave_num])
{}

IncrementalDecoderDriver::~IncrementalDecoderDriver()
{
#ifndef IDD_DISENABLE_TIMER
  delete _tim;
#endif
  delete _last_count;
}

int IncrementalDecoderDriver::begin()
{
  _slave_num = _search_address(_addresses, _max_slave_num);
#ifndef IDD_DISENABLE_TIMER
  _tim = new Timer[_slave_num];
  for (int i = 0; i < _slave_num; i++)
    _tim[i].start();
#endif

  return _slave_num;
}

int IncrementalDecoderDriver::get_slave_num()
{
  return  _slave_num;
}

uint16_t IncrementalDecoderDriver::get_adc(uint8_t id)
{
  uint8_t buffer[5] = {};
  if (id < _slave_num && !_master->read(_addresses[id], (char *)buffer, 5))
  {
    if (buffer[0] == ADC_MODE){
      return buffer[1] | ((uint16_t)buffer[2] << 8);
    }else
      return 0;
  }
  else
    return 0;
}

int32_t IncrementalDecoderDriver::get_count(uint8_t id)
{
  uint8_t buffer[5] = {};
  if (id < _slave_num && !_master->read(_addresses[id], (char *)buffer, 5)) {
    if (buffer[0] == QEI_MODE)
      return buffer[1] | (int32_t)buffer[2] << 8 | (int32_t)buffer[3] << 16 | (int32_t)buffer[4] << 24;
    else 
      return 0;
  } else
    return 0;
}

int IncrementalDecoderDriver::set_count(uint8_t id, int32_t count)
{
  uint8_t buffer[5] = {
    WRITE_QEI,
    count       & 0xFF,
    count >> 8  & 0xFF,
    count >> 16 & 0xFF,
    count >> 24 & 0xFF
  };
  int error = _master->write(_addresses[id], (char *)buffer, 5);
  return error;
}

int IncrementalDecoderDriver::set_count(int32_t count)
{
  uint8_t buffer[5] = {
    WRITE_QEI,
    count       & 0xFF,
    count >> 8  & 0xFF,
    count >> 16 & 0xFF,
    count >> 24 & 0xFF};
  int error = 0;
  for (int i = 0; i < _slave_num; i++) {
    error |= _master->write(_addresses[i], (char *)buffer, 5);
  }
  return error;
}

int IncrementalDecoderDriver::reset(uint8_t id)
{
  if (id < _slave_num) {
    uint8_t data[5] = {WRITE_QEI, };
    return _master->write(_addresses[id], (char *)data, 5);
  } else
    return -1;
}

int IncrementalDecoderDriver::reset()
{
  int error = 0;
  for (int i = 0; i < _slave_num; i++) {
    error |= reset(i);
  }
  return error;
}

int IncrementalDecoderDriver::_search_address(char *address_list, const int max_slave_num)
{
  int slave_num = 0;
  for (int i = 1; i < 127; i++) {
    if (_master->write(i, NULL, 0) == 0) {
      *address_list = i;
      slave_num++;
      ++address_list;
      if (slave_num == max_slave_num)
        return slave_num;
    }
  }
  return slave_num;
}

#ifndef IDD_DISENABLE_TIMER

float IncrementalDecoderDriver::get_pps(uint8_t id)
{
  if (id < _slave_num) {
    int32_t current_count = get_count(id);
    float pps = (float)(current_count - _last_count[id]) / _tim[id].read();
    _tim[id].reset();
    _last_count[id] = current_count;
    return pps;
  }else
    return -1;
}

#else

float IncrementalDecoderDriver::get_pps(uint8_t id)
{
  id; return 0;
}

#endif

IncrementalDecoderDriver::IDD_mode_t IncrementalDecoderDriver::change_mode(uint8_t id, IDD_mode_t mode)
{
  if(id < _slave_num) {
    uint8_t data[5] =
    {
      CHANGE_MODE,
      mode,
    };
    _master->write(_addresses[id], (char *)data, 5);
    data[0] = data[1] = 0;
    _master->read(_addresses[id], (char *)data, 5);
    return (IDD_mode_t)data[0];
  }
}
