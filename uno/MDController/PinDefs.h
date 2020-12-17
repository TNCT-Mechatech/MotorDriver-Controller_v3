#ifndef PIN_DEFS_H_
#define PIN_DEFS_H_

#include <Arduino.h>

//ピン定義はArduino UNOに準拠

// SoftwareI2CMaster's assignment    ここは、エンコーダを読むやつとの通信を行うI2Cバス
#define SCL_PIN     5
#define SCL_PORT    PORTD
#define SDA_PIN     6
#define SDA_PORT    PORTD
#define I2C_PULLUP  1               //内部でプルアップしてるから要らない。

// MD's assignment
#define MD0_PWM     3
#define MD0_DIR     2

#define MD1_PWM     9
#define MD1_DIR     4

#define MD2_PWM     10
#define MD2_DIR     7

#define MD3_PWM     11
#define MD3_DIR     8

//Controller Pins
//コンフィグモードに移行するかどうか
#define CONF_M_PIN  13              //内部プルアップしており、起動時GND短絡でコンフィグモード

#define LED_BUILTIN 12              //動作の表示に使用

//おっとここは未実装区間だ。定義のみおいておく
#define DBG_M_SW    A0              //手動で動作チェックするときに使うスイッチ押した回数でモータ切り替え
#define DBG_VOL     A1              //出力調整用の可変抵抗
#define DBG_D_SW    A2              //出力方向変更用のトグルスイッチ

//以下 一応書いとく

//コンフィグと状態表示用
//UART_TXD          1
//UART_RXD          0

//メインの通信を行うI2Cバス
//HARDWARE_I2C_SDA  A4
//HARDWARE_I2C_SCL  A5

#endif
/* Pinmap layout
 *  
 *                      -------------
 *             ~RESET  1|           |28 HARDWARE_I2C_SCL
 *           UART_RXD   |           |   HARDWARE_I2C_SDA
 *           UART_TXD   |           |   
 *            MD0_DIR   |           |   DBG_D_SW
 *            MD0_PWM   |           |   DBG_VOL
 *            MD1_DIR   |           |   DBG_M_SW
 *                VCC   |           |   GND
 *                GND   |           |   AREF
 *        OSC1(16MHz)   |           |   AVCC
 *        OSC2(16MHz)   |           |   CONF_M_PIN
 *            SCL_PIN   |           |   LED_BUILTIN
 *            SDA_PIN   |           |   MD3_PWM
 *            MD2_DIR   |           |   MD2_PWM
 *            MD3_DIR 14|           |15 MD1_PWM
 *                      -------------
 *               ATmega-328p for ArduinoUNO
 */
