#ifndef INCREMENTAL_DECODER_DRIVER_H_
#define INCREMENTAL_DECODER_DRIVER_H_

#include <I2CMaster.h>
#include <Timer.h>

#include "HardWareI2CMaster.h"

// #define IDD_DISENABLE_TIMER  //タイマー関係を使わない場合用のマクロ   有効化するとget_ppsが禁止になる

class IncrementalDecoderDriver
{
  public :
    typedef enum { //drive_mode
      QEI_MODE = 1,
      ADC_MODE,
      MODE_NUM,
    } IDD_mode_t;

    IncrementalDecoderDriver(I2CMaster *master, const int max_slave_num = 12);
    ~IncrementalDecoderDriver();
    int begin();
    int get_slave_num();
    uint16_t get_adc(uint8_t id);
    int32_t get_count(uint8_t id);
    int set_count(uint8_t id, int32_t count);
    int set_count(int32_t count);
    int reset(uint8_t id);
    int reset();

    float get_pps(uint8_t id);  // Pulse Per Second

    IDD_mode_t change_mode(uint8_t id, IDD_mode_t mode);

  protected :
    int _search_address(char *address_list, const int max_slave_num = 12);
    
#ifndef IDD_DISENABLE_TIMER
    Timer *_tim;
#endif
    enum { //command
      WRITE_QEI = 1,
      CHANGE_MODE,
    };
    I2CMaster *_master;
    const int _max_slave_num;
    int _slave_num;
    char _addresses[128];

    int32_t *_last_count;
};

typedef IncrementalDecoderDriver IDD;

#endif
