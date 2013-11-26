#include "mbed.h"
#include "Motor.h"
#include "IncrementalEncoder.h"
#define RATE 0.0001
#include "PID.h"
class Driver
{
    public:
        /** Create an incremental encoder interface.  Increments counter at every rise and fall signal 
         *
         * @param pin -- the pin to which a digital pulsetrain is sent
         */
        Driver(PinName left_motor,PinName right_motor,PinName left_encoder,PinName right_encoder);

        /** Get ticks since last call
         *
         * @returns the number of ticks since the last call to this method
         */        
        void forward(float speed);
        void stop();
        /** Get total tick count since last reset
         *
         * @returns total ticks since the last reset or instantiation
         */
        void turn(int direction, float degree);
        //void turn(int direction, float degree);
        /** Reset the tick counter
         *
         */

    private:
        Motor m_left; // pwm, fwd, rev
        Motor m_right; // pwm, fwd, rev
        IncrementalEncoder encoder_left;
        IncrementalEncoder encoder_right;
        PID controller_left;
        PID controller_right;
        PID controller_diff;
};