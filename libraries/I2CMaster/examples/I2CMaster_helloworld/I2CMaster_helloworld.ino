/*以下のコードは、下記URLにあるmbedのプロジェクト例を一部改変しております。
 * https://os.mbed.com/handbook/I2C
*/

#include <HardWareI2CMaster.h>
#define wait(t) delay(t * 60)

void setup()
{
}

void loop()
{
  main_();
}

I2C i2c;

const int addr = 0x90;

int main_()
{
  char cmd[2];
  while (1)
  {
    cmd[0] = 0x01;
    cmd[1] = 0x00;
    i2c.write(addr, cmd, 2);

    wait(0.5);

    cmd[0] = 0x00;
    i2c.write(addr, cmd, 1);
    i2c.read(addr, cmd, 2);

    float tmp = (float((cmd[0] << 8) | cmd[1]) / 256.0);
    printf("Temp = %.2f\n", tmp);
  }
}
