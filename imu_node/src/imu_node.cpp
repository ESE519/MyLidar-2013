
//reference to : https://github.com/ros/ros_tutorials/blob/hydro-devel/turtlesim/tutorials/teleop_turtle_key.cpp

#include <ros/ros.h>
#include <geometry_msgs/TwistStamped.h>
#include <sensor_msgs/Imu.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <std_msgs/Time.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>

#define KEYCODE_R 0x43 
#define KEYCODE_L 0x44
#define KEYCODE_U 0x41
#define KEYCODE_D 0x42
#define KEYCODE_Q 0x71

class ImuNode
{
public:
 
  void get_imu_data(const geometry_msgs::TwistStamped& imu_msg); 
sensor_msgs::Imu imu_sensor_msg;
ros::Publisher imu_pub_;
   ros::Subscriber imu_sub;
    ros::NodeHandle nh_;
   void init(){
    imu_pub_ = nh_.advertise<sensor_msgs::Imu>("imu_data", 100);
  imu_sub = nh_.subscribe("imu_test", 100, &ImuNode::get_imu_data, this);
   }

};



int main(int argc, char** argv)
{
  ros::init(argc, argv, "imu_node");
  
  ImuNode node;
   node.init();

  ros::spin();
  return(0);
}

void ImuNode::get_imu_data(const geometry_msgs::TwistStamped& imu_msg)
{
   imu_sensor_msg.header.seq=imu_msg.header.seq;
  imu_sensor_msg.header.stamp = imu_msg.header.stamp;
  imu_sensor_msg.header.frame_id =imu_msg.header.frame_id;
  imu_sensor_msg.orientation.x = (double)0.0;
  imu_sensor_msg.orientation.y = (double)0.0;
  imu_sensor_msg.orientation.z = (double)0.0;
  imu_sensor_msg.orientation.w = (double)0.0;
  int i;

  for(i=0;i<9;i++)
 {
    if((i==0)||(i==4)||(i==8)) 
    {
       imu_sensor_msg.orientation_covariance[i] = (double)-1;
       imu_sensor_msg.angular_velocity_covariance[i] = (double)-1;
       imu_sensor_msg.linear_acceleration_covariance[i] = (double)-1;
    }
    else{
        imu_sensor_msg.orientation_covariance[i] = (double)0.0;
        imu_sensor_msg.angular_velocity_covariance[i] = (double)0.0;
        imu_sensor_msg.linear_acceleration_covariance[i] = (double)0.0;
}


 }
    imu_sensor_msg.angular_velocity.x = imu_msg.twist.angular.x;
   imu_sensor_msg.angular_velocity.y = imu_msg.twist.angular.y;
    
   imu_sensor_msg.angular_velocity.z = imu_msg.twist.angular.z;
  
   imu_sensor_msg.linear_acceleration.x = imu_msg.twist.linear.x;

   imu_sensor_msg.linear_acceleration.y = imu_msg.twist.linear.y;

   imu_sensor_msg.linear_acceleration.z = imu_msg.twist.linear.z;
   
imu_pub_.publish(imu_sensor_msg); 
ros::spinOnce();
return;
   }




