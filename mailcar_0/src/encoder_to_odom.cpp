//reference to : https://github.com/ros/ros_tutorials/blob/hydro-devel/turtlesim/tutorials/teleop_turtle_key.cpp

#include <ros/ros.h>
//#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/PointStamped.h>
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
//#include <std_msgs/Int8.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <math.h>

#define WHEEL_BASE 0.234 ///21cm
#define DISTANCE_PER_TICK 0.0098 //0.98cm per tick
#define PI 3.1415926
/*
class TeleopCar
{
public:
  TeleopCar();
  void keyLoop();

private:
  ros::NodeHandle nh_;
  std_msgs::Int8 velocity;
  ros::Publisher twist_pub_;
  
};
*/

class EncoderOdom
{
public:
  EncoderOdom();
  ros::Subscriber encoder_sub_;
  void readEncoder(const geometry_msgs::PointStamped& encoder);
  tf::TransformBroadcaster odom_broad_;

private:
  ros::NodeHandle nh_;
  geometry_msgs::TransformStamped t_;
  std::string base_link;
  std::string odom;
  double x_pose;
  double y_pose;
  double theta;
};

/*
TeleopCar::TeleopCar()
{
  velocity.data = 0;
  twist_pub_ = nh_.advertise<std_msgs::Int8>("car/cmd_vel", 1);
}
*/


EncoderOdom::EncoderOdom()
{
  puts("running encoder to odom:");
  base_link = "/base_link";
  odom = "/odom";
  encoder_sub_ = nh_.subscribe("/car/encoder",100,&EncoderOdom::readEncoder, this);
  t_.header.seq = 0;
  t_.header.frame_id = odom;
  t_.header.stamp.sec = 0;
  t_.header.stamp.nsec = 0;
  t_.child_frame_id = base_link;
  t_.transform.translation.x = 0;
  t_.transform.translation.y = 0;
  t_.transform.translation.z = 0;
  t_.transform.rotation = tf::createQuaternionMsgFromYaw(0);
  x_pose = 0;
  y_pose = 0;
  theta = 0;
  odom_broad_.sendTransform(t_);
}

int kfd = 0;
struct termios cooked, raw;

void quit(int sig)
{
  tcsetattr(kfd, TCSANOW, &cooked);
  ros::shutdown();
  exit(0);
}

/*
int main(int argc, char** argv)
{
  ros::init(argc, argv, "teleop_car");
  TeleopCar teleop_car;

  signal(SIGINT,quit);

  teleop_car.keyLoop();
  
  return(0);
}
*/

int main(int argc, char** argv)
{
  ros::init(argc, argv, "encoder_tf_odom");
  EncoderOdom encoder_tf_odom;

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


void EncoderOdom::readEncoder(const geometry_msgs::PointStamped& encoder)
{
  double delta_distance;
  double delta_theta;
  delta_distance = (encoder.point.x + encoder.point.y)*DISTANCE_PER_TICK/2;
  delta_theta = (encoder.point.x - encoder.point.y)*DISTANCE_PER_TICK/WHEEL_BASE;
  theta += delta_theta;
  theta -= (double)((int)(theta/2/PI))*2*PI;
  x_pose += delta_distance * cos(theta);
  y_pose += delta_distance * sin(theta);
  t_.header.seq = encoder.header.seq;
  t_.header.stamp = ros::Time::now();
  t_.transform.translation.x = x_pose;
  t_.transform.translation.y = y_pose;
  t_.transform.rotation = tf::createQuaternionMsgFromYaw (theta);
  odom_broad_.sendTransform(t_);
  ros::spinOnce();
}

