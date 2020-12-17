#define ADC_PIN A3
#define QEI_A_CH_PIN 1
#define QEI_B_CH_PIN 3
#define STATUS_PIN 4

#ifndef I2C_SLAVE_ADDRESS // I2C_SLAVE_ADDRESS is initialized with an external macro.
#define I2C_SLAVE_ADDRESS 0x00
#endif

#include <Wire.h>
#include <SoftwareQEI.h>

#define RQ_DATA_SIZE 5
#define RC_DATA_SIZE 5

SoftwareQEI *swqei;

enum { //command
  WRITE_QEI = 1,
  CHANGE_MODE,
};

enum { //drive_mode
  QEI_MODE = 1,
  ADC_MODE,
  MODE_NUM,
};

volatile uint8_t drive_mode = QEI_MODE;

void wire_irq_cb();
void wire_irc_cb(const int);

void cb_irc_event(const uint8_t cmd, uint8_t *data);
void pin_setup(uint8_t mode);
int32_t get_val(uint8_t mode);

void setup() {
  swqei = new SoftwareQEI(QEI_A_CH_PIN, QEI_B_CH_PIN);
  pinMode(QEI_A_CH_PIN, INPUT);
  pinMode(QEI_B_CH_PIN, INPUT);
  pinMode(STATUS_PIN, OUTPUT);
  delay(1);
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onRequest(wire_irq_cb);
  Wire.onReceive(wire_irc_cb);
}

void loop() {
  delay(800);
  digitalWrite(STATUS_PIN, LOW);
}

void wire_irq_cb() {
  uint8_t send_data[RQ_DATA_SIZE] = {};
  int32_t count = get_val(drive_mode);
  send_data[0] = drive_mode;
  send_data[1] = count       & 0xFF;
  send_data[2] = (count >> 8)  & 0xFF;
  send_data[3] = (count >> 16) & 0xFF;
  send_data[4] = (count >> 24) & 0xFF;

  Wire.write(send_data, RQ_DATA_SIZE);
  digitalWrite(STATUS_PIN, HIGH);
}

void wire_irc_cb(const int numByte) {
  if (numByte <= RC_DATA_SIZE) {
    uint8_t res_data[RC_DATA_SIZE] = {};
    for (int i = 0; i < RC_DATA_SIZE; i++) {
      res_data[i] = Wire.read();
    }
    cb_irc_event(res_data[0], res_data + 1);
  }
  digitalWrite(STATUS_PIN, HIGH);
}

void cb_irc_event(const uint8_t cmd, uint8_t *data) {
  switch (cmd) {
    case WRITE_QEI :
      if (swqei != NULL)
        swqei->write(data[0] | (int32_t)data[1] << 8 | (int32_t)data[2] << 16 | (int32_t)data[3] << 24);
      break;
    case CHANGE_MODE :
      if (data[0] < MODE_NUM) {
        drive_mode = data[0];
        pin_setup(drive_mode);
      }
      break;
  };
}

void pin_setup(uint8_t mode) {
  static uint8_t current_mode = drive_mode;
  if (mode != current_mode) {
    current_mode = mode;
    switch (mode) {
      case QEI_MODE :
        swqei = new SoftwareQEI(QEI_A_CH_PIN, QEI_B_CH_PIN);
        pinMode(QEI_A_CH_PIN, INPUT);
        pinMode(QEI_B_CH_PIN, INPUT);
        delay(1);
        break;
      case ADC_MODE :
        delete swqei;
        pinMode(ADC_PIN, INPUT);
        delay(1);
        break;
    };
  }
}

int32_t get_val(uint8_t mode) {
  switch (mode) {
    case QEI_MODE :
      return swqei->read();
      break;
    case ADC_MODE :
      return analogRead(ADC_PIN);
      break;
  };
  return 0;
}
