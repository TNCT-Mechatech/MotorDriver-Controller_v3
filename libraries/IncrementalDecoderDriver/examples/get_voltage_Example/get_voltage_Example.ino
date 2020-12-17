#include <HardWareI2CMaster.h>
#include <IncrementalDecoderDriver.h>

#define ADC_10T_MAX 1023
//ADC_10T_MAX : ArduinoのADCの分解能

#define V_REF 5.0f
//V_REF : 駆動電圧は5v

IDD master(new HardWareI2CMaster);

void setup() {
  Serial.begin(9600);
  while (!master.begin()){} //スレーブが立ち上がるまで待機

  master.change_mode(0, IDD::ADC_MODE); //0ポートの動作モードをADCモードに指定
}

void loop() {
  //0ポートに繋がれたボリュームから電圧を測定、実際の電圧に変換する
  float voltage = V_REF * ((float)master.get_adc(0) / ADC_10T_MAX);
  Serial.print("voltage :\t");
  Serial.print(voltage, 3);
  Serial.println(" V");
  Serial.print("\n");
}
