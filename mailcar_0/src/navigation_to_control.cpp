//reference to : https://github.com/ros/ros_tutorials/blob/hydro-devel/turtlesim/tutorials/teleop_turtle_key.cpp

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int8.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>

#define KEYCODE_R 0x43
#define KEYCODE_L 0x44
#define KEYCODE_U 0x41
#define KEYCODE_D 0x42
#define KEYCODE_Q 0x71
#define KEYCODE_W 0x77

class CarAutoController
{
public:
  CarAutoController();
  void readVelocity(const geometry_msgs::Twist& velocity_twist);
  ros::Publisher twist_pub_;
  ros::Subscriber twist_sub_;

private:
  ros::NodeHandle nh_;
  std_msgs::Int8 velocity;

};


CarAutoController::CarAutoController()
{
  puts("running navigation_to_control:");
  puts("******************************");
  twist_sub_ = nh_.subscribe("/cmd_vel",100,&CarAutoController::readVelocity, this);
  velocity.data = 0;
  twist_pub_ = nh_.advertise<std_msgs::Int8>("car/cmd_vel",1);
}


int kfd = 0;
struct termios cooked, raw;

void quit(int sig)
{
  tcsetattr(kfd, TCSANOW, &cooked);
  ros::shutdown();
  exit(0);
}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "navigation_to_control");
  CarAutoController navigation_to_control;

  // get the console in raw mode
  tcgetattr(kfd, &cooked);
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~ (ICANON | ECHO);
  // Setting a new line, then end of file
  raw.c_cc[VEOL] = 1;
  raw.c_cc[VEOF] = 2;
  tcsetattr(kfd, TCSANOW, &raw);


  signal(SIGINT,quit);

  ros::spin();
  
  return(0);
}


void CarAutoController::readVelocity(const geometry_msgs::Twist& velocity_twist)
{
  ROS_DEBUG("value: 0x%02f\n", velocity_twist.angular.z);
  if (velocity_twist.linear.x > 0.01 )
  {
    velocity.data = 3;
  }
  else
  {
    if (velocity_twist.linear.y > 0.01)
    {
      velocity.data = 1;
    }
    else if (velocity_twist.linear.y < -0.01)
    {
      velocity.data = 2;
    }
    else
    {
      velocity.data = 4;
    }
  }





  // if ( (velocity_twist.angular.z < -0.5) || (velocity_twist.angular.z > 0.5) )
  // {
  //   velocity.data = 1;//left
  // }
  // else
  // {
  //   ROS_DEBUG("value: 0x%02f\n", velocity_twist.linear.x);
  //   ROS_DEBUG("value: 0x%02f\n", velocity_twist.linear.y);
  //   if ( (velocity_twist.linear.x < 0.1) && (velocity_twist.linear.y < 0.1) )
  //   {
  //     velocity.data = 4;//stop
  //     //TODO wait , velocity.data = 5;//for push
  //   }
  //   else
  //   {
  //     if ( (velocity_twist.linear.y < -0.1) || (velocity_twist.linear.y > 0.1) )
  //     {
  //       velocity.data = 2;//right
  //     }
  //     else
  //     {
  //       velocity.data = 3;//go straight
  //     }
  //   }
  // }
  twist_pub_.publish(velocity);
  ros::spinOnce();
}
