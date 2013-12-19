//navigation_to_control
//this class is to transform the data from ROS topic /cmd_vel to control command of the car
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int8.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <ros/message_operations.h>
#include <std_msgs/Header.h>
#include <move_base_msgs/MoveBaseActionResult.h>

#define KEYCODE_R 0x43
#define KEYCODE_L 0x44
#define KEYCODE_U 0x41
#define KEYCODE_D 0x42
#define KEYCODE_Q 0x71
#define KEYCODE_W 0x77

class CarAutoController
{
  // subscriber twist_sub_ subscribes to topic /cmd_vel, getting control message from navigation
  // subscriver goal_status_sub_ subscrives to topic /move_base/result, getting navigation status
  // Publisher twist_pub_ publish the topic /car/cmd_vel, mBed will subscribe on this topic
public:
  CarAutoController();
  void readVelocity(const geometry_msgs::Twist& velocity_twist);
  void readGoalStat(const move_base_msgs::MoveBaseActionResult result);
  ros::Publisher twist_pub_;
  ros::Subscriber twist_sub_;
  ros::Subscriber goal_status_sub_;

private:
  ros::NodeHandle nh_;
  std_msgs::Int8 velocity;
  actionlib_msgs::GoalStatus localResult;
  std::string goal;

};


CarAutoController::CarAutoController()
{
  puts("running navigation_to_control:");
  puts("******************************");
  goal="/goalll";
  twist_sub_ = nh_.subscribe("/cmd_vel",100,&CarAutoController::readVelocity, this);
  goal_status_sub_ = nh_.subscribe("/move_base/result",100,&CarAutoController::readGoalStat, this);
  velocity.data = 0;
  localResult.goal_id.stamp.sec = 0;
  localResult.goal_id.stamp.nsec = 0;
  localResult.goal_id.id = goal;
  localResult.status = 0;
  localResult.text = goal;
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
  // here is the transformation.
  // this algorithm needs to be refined
  // if x direction has speed, the go straight
  // else 
  //      if z > threshold, turn left
  //      else if z < threshold, turn right
  //      else
  //            if z != 0, go straight. NEEDREFINE
  //            else: here [[0 0 0] [0 0 0]], wait for instruction
  //if result.status = 3, means destination is reached
  //     should wait for some time and push the box.

  if (velocity_twist.linear.x > 0.2 )
  {
    velocity.data = 3;
  }
  else
  {
    if (velocity_twist.angular.z > 0.4)
    {
      velocity.data = 1;
    }
    else if (velocity_twist.angular.z < -0.4)
    {
      velocity.data = 2;
    }
    else
    {
      if (velocity_twist.angular.z != 0)
        velocity.data = 3;
      else
        velocity.data = 4;
    }
  }

  if (localResult.status == 3) 
  {
    velocity.data = 5;
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



void CarAutoController::readGoalStat(const move_base_msgs::MoveBaseActionResult result)
{
  localResult.status = result.status.status;
  ros::spinOnce();
}