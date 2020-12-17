#ifndef CONFIG_H_
#define CONFIG_H_

#include <EEPROM.h>
#include <MotorDriver.h>

#define PIDCONF_LENGTH 16

#define ROM_I2C_ADDR_HEAD 0
#define ROM_PID_CONF_HEAD 1

#define CMD_MAX_BUFFER 16

//Config data
typedef union PIDConfType
{
  byte data[PIDCONF_LENGTH];
  typedef struct {
    float kp;
    float ki;
    float kd;
    uint16_t ppr4;
    uint16_t rps_range;
  } str_t;
  str_t str;

} pid_conf_t;

pid_conf_t *pid_conf[PID_MAX_NUM];

uint8_t i2c_addr = 0;
//end Config data

void sysconf_init();
void sysconf_print();
void delete_conf();
void uart_process();
void motor_test_process();

void config_mode(uint8_t mode_toggle_pin)
{
  pinMode(mode_toggle_pin, INPUT_PULLUP);
  pinMode(DBG_M_SW, INPUT_PULLUP);
  pinMode(DBG_D_SW, INPUT_PULLUP);
  pinMode(DBG_VOL, INPUT);
  
  if (digitalRead(mode_toggle_pin) == LOW) // short to GND
  {
    Serial.println(F("\n-----MDController config_mode----- (RESET on exit)"));
    sysconf_print();
    Serial.println(F("\nInput commands----"));
    Serial.println(F("RR\\n \t: EEPROM all reset."));
    Serial.println(F("a x\\n \t: x=new i2c address."));
    Serial.println(F("px X\\n \t: x=port id, X=new Kp gain."));
    Serial.println(F("ix X\\n \t: x=port id, X=new Ki gain."));
    Serial.println(F("dx X\\n \t: x=port id, X=new Kd gain."));
    Serial.println(F("rx X\\n \t: x=port id, X=new pulse per revolution."));
    Serial.println(F("sx X\\n \t: x=port id, X=new max rps range."));
    Serial.println(F("SS\\n \t: save settings."));
    Serial.println(F("\nend----"));

    while (1) { // config_mode loop
      static bool led;
      digitalWrite(LED_BUILTIN, led = !led);  //config mode
      delay(300);
      uart_process();
      motor_test_process();
    } // end config_mode loop
  }
}

void reset_all()
{
  Serial.println(F("EEPROM reseting of all. please wait!"));
  for (uint16_t i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.update(i, 0);
  }
  Serial.println(F("EEPROM is all reset success."));
}

void save_all()
{
  Serial.println(F("Saving your changes to EEPROM. please wait!"));
  EEPROM.update(ROM_I2C_ADDR_HEAD, i2c_addr);

  uint8_t head = ROM_PID_CONF_HEAD;
  for (int i = 0; i < PID_MAX_NUM; i++) {
    for (int j = 0; j < PIDCONF_LENGTH; j++) {
      EEPROM.update(head, pid_conf[i]->data[j]);
      head++;
    }
  }
  Serial.println(F("saved success!"));
}

void sysconf_print()
{
  Serial.println(F("Config data list----"));
  sysconf_init();
  for (int i = 0; i < PID_MAX_NUM; i++) {
    Serial.print(F("No."));
    Serial.println(i);
    Serial.print(F("\tKp gain:"));
    Serial.println(pid_conf[i]->str.kp, 4);
    Serial.print(F("\tKi gain:"));
    Serial.println(pid_conf[i]->str.ki, 4);
    Serial.print(F("\tKd gain:"));
    Serial.println(pid_conf[i]->str.kd, 4);
    Serial.print(F("\tppr:"));
    Serial.println(pid_conf[i]->str.ppr4 / 4);
    Serial.print(F("\trps max:"));
    Serial.println(pid_conf[i]->str.rps_range);
  }
  delete_conf();
  Serial.println(F("end----\n"));
}

void sysconf_init()
{
  for (int i =  0; i < PID_MAX_NUM; i++) {
    pid_conf[i] = new pid_conf_t();
  }

  i2c_addr = (uint8_t)EEPROM.read(ROM_I2C_ADDR_HEAD);
  uint8_t head = ROM_PID_CONF_HEAD;
  for (int i = 0; i < PID_MAX_NUM; i++) {
    for (int j = 0; j < PIDCONF_LENGTH; j++) {
      pid_conf[i]->data[j] = (byte)EEPROM.read(head);
      head++;
    }
  }
  Serial.print(F("I2C 7bit address = 0x"));
  Serial.println(i2c_addr, HEX);
}

void delete_conf()
{
  for (int i =  0; i < PID_MAX_NUM; i++) {
    delete pid_conf[i];
  }
}

void uart_process()
{
  if (Serial.available()) {
    char cmd = Serial.read();
    char data[CMD_MAX_BUFFER] = {};
    int i = 0;
    while (1) {
      while (!Serial.available()) {}
      char tmp = Serial.read();
      if (i == (CMD_MAX_BUFFER - 1) || tmp == '\n') {
        data[i] = '\0';
        break;
      }
      data[i] = tmp;
      i++;
    }
    uint8_t head = 0;
    switch (cmd) {
      case 'R' :
        if (data[0] == 'R')
          reset_all();
        else
          Serial.println(F("Incorrect number."));
        break;
      case 'a' :
        i2c_addr = atoi(data);
        Serial.print(F("Change i2c 7bit address->0x"));
        Serial.println(i2c_addr, HEX);
        break;
      case 'p' :
        head = atoi(&data[0]);
        if (head >= PID_MAX_NUM) {
          Serial.println(F("Incorrect number."));
          break;
        }
        pid_conf[head]->str.kp = atof(data + 1);
        Serial.print(F("Change pid Kp(")); Serial.print(head); Serial.print(F(")->"));
        Serial.println(pid_conf[head]->str.kp, 4);
        break;
      case 'i' :
        head = atoi(&data[0]);
        if (head >= PID_MAX_NUM) {
          Serial.println(F("Incorrect number."));
          break;
        }
        pid_conf[head]->str.ki = atof(data + 1);
        Serial.print(F("Change pid Ki(")); Serial.print(head); Serial.print(F(")->"));
        Serial.println(pid_conf[head]->str.ki, 4);
        break;
      case 'd' :
        head = atoi(&data[0]);
        if (head >= PID_MAX_NUM) {
          Serial.println(F("Incorrect number."));
          break;
        }
        pid_conf[head]->str.kd = atof(data + 1);
        Serial.print(F("Change pid Kd(")); Serial.print(head); Serial.print(F(")->"));
        Serial.println(pid_conf[head]->str.kd, 4);
        break;
      case 'r' :
        head = atoi(&data[0]);
        if (head >= PID_MAX_NUM) {
          Serial.println(F("Incorrect number."));
          break;
        }
        pid_conf[head]->str.ppr4 = atoi(data + 1) * 4;
        Serial.print(F("Change pid ppr(")); Serial.print(head); Serial.print(F(")->"));
        Serial.println(pid_conf[head]->str.ppr4 / 4);
        break;
      case 's' :
        head = atoi(&data[0]);
        if (head >= PID_MAX_NUM) {
          Serial.println(F("Incorrect number."));
          break;
        }
        pid_conf[head]->str.rps_range = atoi(data + 1);
        Serial.print(F("Change pid rps_range(")); Serial.print(head); Serial.print(F(")->"));
        Serial.println(pid_conf[head]->str.rps_range);
        break;
      case 'S' :
        if (data[0] == 'S')
          save_all();
        else
          Serial.println(F("Incorrect number."));
        break;
      default :
        Serial.println(F("Incorrect number."));
        break;
    };
  }
}

inline void blink_led()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}

void motor_test_process()
{
  uint8_t motor_pwm = analogRead(DBG_VOL);
  bool motor_dir = digitalRead(DBG_D_SW);
  static int mode = -1;
  static MotorDriver *md = new SignMagnitudeMD(MD0_DIR, MD0_PWM);
  if (!digitalRead(DBG_M_SW)) {
    mode++;
    if (mode >= MD_MAX_NUM)
      mode = -1;
    while (!digitalRead(DBG_M_SW)) {}
    Serial.print(F("MotorTest : "));
    switch (mode) {
      case -1 :
      default :
        md->drive(0);
        Serial.println(F("Off."));
        blink_led();
        break;
      case 0  :
        md->drive(0);
        md = new SignMagnitudeMD(MD0_DIR, MD0_PWM);
        Serial.println(F("Motor 0 active."));
        blink_led();
        blink_led();
        break;
      case 1  :
        md->drive(0);
        md = new SignMagnitudeMD(MD1_DIR, MD1_PWM);
        Serial.println(F("Motor 1 active."));
        blink_led();
        blink_led();
        blink_led();
        break;
      case 2  :
        md->drive(0);
        md = new SignMagnitudeMD(MD2_DIR, MD2_PWM);
        Serial.println(F("Motor 2 active."));
        blink_led();
        blink_led();
        blink_led();
        blink_led();
        break;
      case 3  :
        md->drive(0);
        md = new SignMagnitudeMD(MD3_DIR, MD3_PWM);
        Serial.println(F("Motor 3 active."));
        blink_led();
        blink_led();
        blink_led();
        blink_led();
        blink_led();
        break;
    };
  }

  if(mode != -1) {
    md->drive(motor_dir, motor_pwm);
    Serial.print(F("MotorTest : No."));
    Serial.print(mode);
    Serial.print(F("\tPWM "));
    Serial.print(motor_pwm);
    Serial.print(F("\tDIR "));
    Serial.println(motor_dir);
    
  }

}

#endif
