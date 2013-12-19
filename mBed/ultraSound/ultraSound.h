#include "mbed.h"

class ultraSound
{
    public:

        ultraSound(PinName pinIn);
 
        int read();
        

    private:
        float speedConst;
        DigitalInOut Pin;
        Timer timer;

};