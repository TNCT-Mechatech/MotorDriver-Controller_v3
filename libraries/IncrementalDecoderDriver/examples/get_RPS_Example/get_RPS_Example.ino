#include <IncrementalDecoderDriver.h>
#include <HardWareI2CMaster.h>

IDD master(new HardWareI2CMaster);

const int32_t pulse_per_rev = 2048 * 4; //エンコーダの周パルス数 x 4倍

void setup()
{
    Serial.begin(9600);
    while(!master.begin()){}    //スレーブが立ち上がるまで待機
    master.change_mode(0, IDD::QEI_MODE); //動作モードをQEIモードに指定
}

void loop()
{
    //0ポートに繋がれたエンコーダのパルス毎秒を測定、一周分のパルスで割ってrpsを求める。
    float rps = master.get_pps(0) / pulse_per_rev;
    Serial.print("master :\t");
    Serial.print(rps, 6);
    Serial.println(" r/sec");
}