#include "ultraSound.h"
#include "mbed.h"




ultraSound::ultraSound(PinName pinIn):speedConst(1.0), Pin(pinIn), timer()
{

}
int ultraSound::read()
{
    bool overflow = false;
    Pin.output();
    timer.start();
    Pin.write(1);
    wait_us(5);
    Pin.write(0);
    Pin.input();
    while(Pin.read() == 0);
    int start = timer.read_us();
    while(Pin.read() == 1) {
        if (timer.read_us() - start > 18500) {
            overflow = true;
            break;
        }
    }
    int stop = timer.read_us();
    timer.stop();

    if (overflow) {
        return 0;
    } else {
        return (stop-start) /58;
    }
}


