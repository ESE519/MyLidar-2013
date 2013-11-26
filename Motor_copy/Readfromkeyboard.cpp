//#define COMPILE_PUBSUB_CODE_ROSSERIALa
#ifdef  COMPILE_PUBSUB_CODE_ROSSERIALa

/*
 * rosserial PubSub Example
 * Prints "hello world!" and toggles led
 */
#include "mbed.h"
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <geometry_msgs/Twist.h>
#define RATE 0.0001
#include "Driver.h"
#include <string.h>
ros::NodeHandle  nh;
int command = 0;
DigitalOut ledRight(LED4);
DigitalOut ledForward(LED2);
DigitalOut ledLeft(LED1);
Driver driver(p21,p22,p17,p18);  //PinName left_motor,PinName right_motor,PinName left_encoder,PinName right_encoder
/*
void messageCb( const std_msgs::Empty& toggle_msg)
{
    myled = !myled;   // blink the led
}
*/
void velocityControl(const geometry_msgs::Twist& velocity_msg)
{
    if(velocity_msg.linear.x > 0) {
        ledForward.write(1);
        command = 1;
    }
    else if (velocity_msg.linear.x < 0)
    {
        //back or stop
        ledForward.write(0);
        ledRight.write(0);
        ledLeft.write(0);
        command = 0;
    }else if(velocity_msg.angular.z < 0)
    {
        //right
        ledLeft.write(0);
        ledRight.write(1);
        command = 2;
    }
    else if (velocity_msg.angular.z > 0)
    {
        //left
        ledRight.write(0);
        ledLeft.write(1);
        command = 3;
    }else
    {
        command = 0;
    }
}

//ros::Subscriber<std_msgs::Empty> sub("toggle_led", messageCb );
ros::Subscriber<geometry_msgs::Twist> velocity("car/cmd_vel", velocityControl);



std_msgs::String str_msg;
ros::Publisher chatter("chatter", &str_msg);

char hello[13] = "hello world!";

                 int main()
{

    
    nh.initNode();
    nh.advertise(chatter);
    //nh.subscribe(sub);
    nh.subscribe(velocity);

    while (1) {
        str_msg.data = hello;
        chatter.publish( &str_msg );
        nh.spinOnce();
        //wait_ms(500);
        switch (command) {
            case 1:
                for (int i = 0; i < 100; i ++){
                driver.forward(30);
                }
                break;
            case 0:
                driver.stop();
                
                break;
            case 2:
                driver.turn(1,10);
                //command = 0;
                //ledRight.write(0);
                break;
            case 3:
                driver.turn(0,10);
                //command = 0;
                //ledLeft.write(0);
                break;        
            default:
                break;
        }
    }
}
#endif