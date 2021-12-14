#define MD_MAX_NUM  4
#define PID_MAX_NUM MD_MAX_NUM

#define IDD_MAX_NUM_DEFAULT 4

#define INR16MAX    32767

//#define I2C_FREQ 100000
#define I2C_FREQ 9600

#include "PinDefs.h"

#include <MotorDriver.h>
#include <SignMagnitudeMD.h>
#include <I2CMaster.h>
#include <HardWareI2CMaster.h>
#include <SoftWareI2CMaster.h>
#include <IncrementalDecoderDriver.h>

#include "Config.h"

#include "I2CNodeHandler.h"
//#include "TestNode.h"
#include "MDNode.h"
#include "QEINode.h"
#include "PIDNode.h"

MotorDriver *md[MD_MAX_NUM];

I2CMaster *soft_i2c = new SoftWareI2CMaster();
uint8_t IDD_num = IDD_MAX_NUM_DEFAULT;
IDD soft_qei(soft_i2c, IDD_num);

I2CNodeHandler server;

//TestNode testnode(0x00);
#define MD_SUBADD  0x10
MDNode *nmd[MD_MAX_NUM];

#define QEI_SUBADD  0x20
QEINodes nqei(QEI_SUBADD, &soft_qei);

#define PID_SUBADD  0x30
PIDNode *npid[PID_MAX_NUM];


void IDD_setup();
void md_setup();
void server_init();
void server_begin();
void sys_setup();


void setup()
{
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  delay(100);
  config_mode(CONF_M_PIN);
  sys_setup();
}

void loop()
{
  static uint32_t last_time = millis();
  static bool led;
  while (1){
    if (millis() - last_time >= 700)
    {                                        //wait 700ms
      digitalWrite(LED_BUILTIN, led = !led); //normal mode
      last_time = millis();
    }
    for (int i = 0; i < IDD_num; i++)
      nqei.task(i);
      delay(5);
  }
}


void md_setup()
{
  TCCR1B = TCCR1B & 0xf8 | 0x02; // Pin9,Pin10 PWM 3921.16Hz
  TCCR2B = TCCR2B & 0xf8 | 0x02; // Pin3,Pin11 PWM 3921.16Hz
  md[0] = new SignMagnitudeMD(MD0_DIR, MD0_PWM);
  md[1] = new SignMagnitudeMD(MD1_DIR, MD1_PWM);
  md[2] = new SignMagnitudeMD(MD2_DIR, MD2_PWM);
  md[3] = new SignMagnitudeMD(MD3_DIR, MD3_PWM);
}

void IDD_setup()
{
  while (!soft_qei.begin()) { //スレーブが立ち上がるまで待機
    Serial.println(F("Wait connection for IncrementalDecoders..."));
    delay(200);
  }
  for (int i = 0; i < soft_qei.get_slave_num(); i++) {
    soft_qei.change_mode(i, IDD::QEI_MODE); //動作モードをQEIモードに指定
  }

  IDD_num = soft_qei.get_slave_num();
  Serial.print(F("Found IncrementalDecoders:"));
  Serial.print(IDD_num);
  Serial.print(F("/"));
  Serial.println(IDD_MAX_NUM_DEFAULT);
  if (IDD_num != IDD_MAX_NUM_DEFAULT) Serial.println(F("Warning:Insufficient number of IncrementalDecoder."));
}

void server_init()
{
  for (int i = 0; i < MD_MAX_NUM; i++) {
    nmd[i] = new MDNode(MD_SUBADD | i, md[i], 0);
    if (i < IDD_num) {
      npid[i] = new PIDNode(PID_SUBADD | i,
                            pid_conf[i]->str.kp, pid_conf[i]->str.ki, pid_conf[i]->str.kd,
                            pid_conf[i]->str.ppr2);
    }
  }
}

void server_begin()
{
  //testnode.begin(&server);
  nqei.begin(&server);
  for (int i = 0; i < MD_MAX_NUM; i++) {
    nmd[i]->begin(&server);
    if (i < IDD_num) {
      npid[i]->begin(&server, md[i], &nqei, i);
    }
  }
  

  //testnode.set_total_nodes(server.get_node_num());
  server.begin(i2c_addr, I2C_FREQ);
}

void sys_setup()
{
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(F("\n-----MDController-----"));
  Serial.println(F("booting..."));
  sysconf_init();
  md_setup();
  IDD_setup();
  delay(20);
  server_init();
  delete_conf();
  server_begin();
  digitalWrite(LED_BUILTIN, LOW);
  //Serial.println(F("Ping pong node sub address is 0x00"));
  Serial.print(F("md node(0x1X) total = "));
  Serial.println(MD_MAX_NUM);
  Serial.print(F("qei node(0x2X) total = "));
  Serial.println(IDD_num);
  Serial.print(F("pid node(0x3X) total = "));
  Serial.println(IDD_num);
  Serial.print(F("Server start running."));
  Serial.println(F("\n"));
}

/*
#include "PinDefs.h"
#include <HardWareI2CMaster.h>
#include <SoftWareI2CMaster.h>
#include <I2CMaster.h>
#include <IncrementalDecoderDriver.h>

I2CMaster *soft_i2c = new SoftWareI2CMaster();

//I2CMaster *hard_i2c = new HardWareI2CMaster();

IDD master(soft_i2c);
//IDD master(hard_i2c);

void setup() {
  Serial.begin(9600);
  soft_i2c->begin();
  while (!master.begin()){
    Serial.println("AAAA");
    } //スレーブが立ち上がるまで待機
    
  master.change_mode(0, IDD::QEI_MODE); //0ポートの動作モードをQEIモードに指定
  master.change_mode(1, IDD::QEI_MODE); //1ポートの動作モードをQEIモードに指定
  master.change_mode(2, IDD::QEI_MODE); //2ポートの動作モードをQEIモードに指定
  master.change_mode(3, IDD::QEI_MODE); //3ポートの動作モードをQEIモードに指定
}

void loop() {
  int32_t count;
  int32_t pps;
  //0~3ポートに繋がれたエンコーダのパルスカウント値を取得
  for(int i = 0;i < 4;i++){
    count = master.get_count(i);
    pps = master.get_pps(i);
    Serial.print(i);
    Serial.print("->count :\t");
    Serial.print(count);
    Serial.print(",");
    Serial.print("pps:\t");
    Serial.print(pps);
    Serial.print("\t");
  }
  Serial.print("\n");
}
*/
