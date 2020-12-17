/*以下のコードは、下記URLにあるmbedのプロジェクト例を改変しております。
 * https://os.mbed.com/handbook/Timer
*/

#include <Timer.h>

void setup()
{
  Serial.begin(9600);
  main_();
}

void loop()
{
}

Timer t;

int main_()
{
  t.start();
  Serial.print("Hello World!\n");
  wait(0.01); // as "delay(10)"
  t.stop();
  Serial.print("The time taken was ");
  Serial.print(t.read(), 6);
  Serial.print(" seconds\n");
}
