#include "Driver.h"
#include "mbed.h"

#define RATE 0.001


Driver::Driver(PinName left_motor,PinName right_motor,PinName left_encoder,PinName right_encoder):m_left(left_motor, p6, p5),
    m_right(right_motor, p7, p8),
    encoder_left(left_encoder,0),
    encoder_right(right_encoder,1),
    controller_left(0.2, 0.1, 0.1, RATE),
    controller_right(0.2, 0.1, 0.1, RATE),
    controller_diff(0.1, 0.02, 0.2, RATE)
{
    controller_left.setInputLimits(0.0, 500);
    controller_left.setOutputLimits(0.30, 0.9);
    controller_left.setBias(0.0); 
    controller_left.setMode(AUTO_MODE);

    controller_right.setInputLimits(0, 500);
    controller_right.setOutputLimits(0.30, 0.9);
    controller_right.setBias(0.00);
    controller_right.setMode(AUTO_MODE);
    controller_diff.setSetPoint(0);


    controller_diff.setInputLimits(-20, 20);
    controller_diff.setOutputLimits(-0.2,0.2);
    controller_diff.setBias(0);
    controller_diff.setMode(AUTO_MODE);
    m_left.speed(0);
    m_right.speed(0);


}
void Driver::forward(float speed)
{


    //int diff = encoder_left.readTotal() - encoder_right.readTotal();
    //controller_diff.setProcessValue(diff);
    
    /*
    if (controller_diff.compute()>0) {
        controller_left.setSetPoint(speed);
        controller_right.setSetPoint(speed - controller_diff.compute());
    } else {
        controller_left.setSetPoint(speed + controller_diff.compute());
        controller_right.setSetPoint(speed);
    }
    */
    controller_left.setSetPoint(speed);
    controller_right.setSetPoint(speed);
    controller_left.setProcessValue(encoder_left.getSpeed());
    controller_right.setProcessValue(encoder_right.getSpeed());
    m_right.speed(controller_right.compute());
    m_left.speed(controller_left.compute());
    
    /*
    if (controller_diff.compute()>0) {
        m_right.speed(controller_right.compute() - controller_diff.compute()/2);
        m_left.speed(controller_left.compute());
    } else {
        m_left.speed(controller_left.compute() + controller_diff.compute()/2);
        m_right.speed(controller_right.compute());
    }
    */

}
void Driver::stop(void)
{
    m_right.speed(0);
    m_left.speed(0);
    encoder_right.reset();
    encoder_left.reset();
    controller_left.reset();
    controller_right.reset();
    controller_diff.reset();

}
/**
 * direction == 0 turn left
 * direction == 1 turn right
 */

void Driver::turn(int direction, float degree)
{

    int pulseCount = degree * 36 / 90;   // 90 degree is 36 pulses;


    if (direction == 0) {
        int base = encoder_left.readTotal()-encoder_right.readTotal();
        while((encoder_left.readTotal()-encoder_right.readTotal()) < (pulseCount+base)) {
            m_left.speed(0.3);
            //printf("%d\r\n",encoder_left.readTotal());
        }
    } else {
        int base = encoder_right.readTotal()-encoder_left.readTotal();
        while((encoder_right.readTotal()-encoder_left.readTotal())<(pulseCount+base)) {
            m_right.speed(0.3);
            //printf("%d\r\n",encoder_right.readTotal());
        }
    }
    m_left.speed(0);
    m_right.speed(0);
    encoder_right.reset();
    encoder_left.reset();
}

void Driver::turnLeft()
{
    controller_right.setSetPoint(20);
    
    controller_right.setProcessValue(encoder_right.getSpeed());
    
    
    m_right.speed(controller_right.compute());
    
       

}

void Driver::turnRight()
{
    controller_left.setSetPoint(20);
    
    controller_left.setProcessValue(encoder_left.getSpeed());
    
    
    m_left.speed(controller_left.compute());
}

unsigned long Driver::getLeftTicks()
{
    return encoder_left.getPulse();
}

unsigned long Driver::getRightTicks()
{
    return encoder_right.getPulse();
}
