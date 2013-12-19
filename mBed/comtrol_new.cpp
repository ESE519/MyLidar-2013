#define COMPILE_CONTROL_WITH_ENCODER_TOPIC
#ifdef  COMPILE_CONTROL_WITH_ENCODER_TOPIC
#include "mbed.h"
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <std_msgs/Int8.h>
#include <geometry_msgs/PointStamped.h>
#include "Driver.h"
#include <string.h>

bool pushout = false;

Motor motor(p26,p9,p10);

ros::NodeHandle  nh;

int command = 0;
int old_command = 1;

DigitalIn pushBtn(p14);
DigitalIn pullBtn(p13);
Driver driver(p23,p24 ,p17,p18);  //left_motor,right_motor,left_encoder,right_encoder


Ticker ticker_movement;
Ticker ticker_odmn;
Ticker ticker_spin;
Timer timer;


//receive command from beagleboard
void velocityControl(const std_msgs::Int8& velocity_msg)
{
    if(velocity_msg.data == 3) {
        command = 1;
    } else if (velocity_msg.data == 4) {
        //back or stop
        command = 0;
    } else if(velocity_msg.data == 2) {
        //right
        command = 2;
    } else if (velocity_msg.data == 1) {
        //left
        command = 3;
    } else if (velocity_msg.data == 5) {
        command = 5;
    } else if (velocity_msg.data == 6) {
        command = 6;
    } else {
    }

}

ros::Subscriber<std_msgs::Int8> velocity("car/cmd_vel", velocityControl);

char odom_frame[] = "/odom";

geometry_msgs::PointStamped encoder_odom;

ros::Publisher encoder_topic("/car/encoder", &encoder_odom);
unsigned long last_left_ticks = 0;
unsigned long last_right_ticks = 0;
unsigned long left_ticks = 0;
unsigned long right_ticks = 0;
unsigned long seq = 0;



void getEncoderTicks()
{
    left_ticks = driver.getLeftTicks();
    right_ticks = driver.getRightTicks();
    encoder_odom.point.x = left_ticks - last_left_ticks;
    encoder_odom.point.y = right_ticks - last_right_ticks;
    last_left_ticks = left_ticks;
    last_right_ticks = right_ticks;//0.0098m per tick
    encoder_odom.point.z = 0;
    encoder_odom.header.stamp = nh.now();
    encoder_odom.header.seq = seq;
    encoder_odom.header.frame_id = odom_frame;
    encoder_topic.publish ( &encoder_odom );
}



void forward()
{
    driver.forward(80);
}
void left(){
    driver.turnLeft();
}
void right(){
driver.turnRight();  
}
void spin()
{
    nh.spinOnce();
}

void motorStop()
{
    motor.speed(0);

}

void motorPull()
{
    motor.pull();
}



int main()
{
    nh.initNode();   
    nh.advertise(encoder_topic);
    nh.subscribe(velocity);
    ticker_odmn.attach_us(&getEncoderTicks,549000);
    ticker_spin.attach_us(&spin,1234);


    while (1) {
        if (command != old_command ) {
            switch (command) {
                // move forward
                case 1:
                    driver.stop();
                    ticker_movement.attach_us(&forward,1000);
                    old_command = 1;
                    motor.speed(0);
                    pushout = false;
                    break;
                // stop
                case 0:
                    driver.stop();
                    pushout = false;
                    old_command = 0;
                    motor.speed(0);
                    ticker_movement.detach();
                    break;
                //turn left
                case 2:
                    driver.stop();
                    ticker_movement.attach_us(&right,1000);
                    //driver.turnRight();
                    motor.speed(0);
                    pushout = false;
                    old_command = 2;
                    break;

                //turn right
                case 3:
                    driver.stop();
                    ticker_movement.attach_us(&left,1000);
                    old_command = 3;
                    motor.speed(0);
                    pushout = false;
                    break;

                //push out box
                case 5:
                    motor.push();
                    timer.start();
                    old_command = 5;
                    break;

                //pull back arm
                case 6:
                    driver.stop();
                    driver.turn(0,15);
                    old_command = 6;
                    pushout = false;
                default:
                    driver.stop();
                    break;
            }
            // constant command that should be taken care of
        } else {
            if (command == 0) {
                driver.stop();
                pushout = false;
                ticker_movement.detach();
                motor.speed(0);
            }

            else {
            }
            if (command == 5) {
                if (!pushout) {
                    // polling the limit switch, 
                    //either touch the limit switch or after 1.5s
                    if (pushBtn == 1 || timer.read_ms() > 1500) {   
                        motor.pull();
                        pushout = true;
                        timer.reset();
                        timer.start();
                    } else {
                    }
                } else {
                    if (pullBtn == 1 || timer.read_ms() > 1500) {
                        motor.speed(0);
                        pushout = false;
                        timer.stop();
                        timer.reset();
                    } else {
                    }
                }
            } else {
            }

        }
    }
}
#endif