//#define COMPILE_PUBSUB_CODE_ROSSERIAL
#ifdef  COMPILE_PUBSUB_CODE_ROSSERIAL


#include "mbed.h"
#define RATE 0.0001
#include "Driver.h"


Driver driver(p21,p22,p17,p18);  //PinName left_motor,PinName right_motor,PinName left_encoder,PinName right_encoder
int main()
{
    driver.stop();
    while(1) {
        wait(3);
        driver.turn(0,90);
    }
}
#endif