#include <HardWareI2CMaster.h>
#include <IncrementalDecoderDriver.h>

IDD master(new HardWareI2CMaster);

void setup() {
  Serial.begin(9600);
  while (!master.begin()){}  //スレーブが立ち上がるまで待機
  
  Serial.print("found total ");
  Serial.println(master.get_slave_num()); //接続されたデバイスの数を表示
  delay(1000);
  master.reset();
}

void loop() {
  //全てのデバイスに対して処理をする例
  for (int i = 0; i < master.get_slave_num(); i++) {
    master.set_count(i, millis()); //i番目のポートにパルス数の代わりにシステム時間を書き込んでみる
    Serial.print(i);
    Serial.print(" : ");
    Serial.print(master.get_count(i)); //i番目のポートのパルス数を取得。今回の場合さっきのシステム時間が返ってくる。
    Serial.print("\t");
  }
  Serial.print("\n");
}
