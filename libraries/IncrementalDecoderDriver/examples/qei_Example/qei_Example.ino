#include <HardWareI2CMaster.h>
#include <IncrementalDecoderDriver.h>

IDD master(new HardWareI2CMaster);

void setup() {
  Serial.begin(9600);
  while (!master.begin()){} //スレーブが立ち上がるまで待機

  master.change_mode(0, IDD::QEI_MODE); //0ポートの動作モードをQEIモードに指定
}

void loop() {
  //0ポートに繋がれたエンコーダのパルスカウント値を取得
  int32_t count = master.get_count(0);
  Serial.print("count :\t");
  Serial.print(count);
  Serial.print("\n");
}
