//#define COMPILE_READIMU_CODE
#ifdef  COMPILE_READIMU_CODE


#include "ADXL345_I2C.h"
#include "mbed.h"
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Imu.h>
#define RATE 0.0001
#include "Driver.h"
#include <string.h>


ros::NodeHandle  nh;

std_msgs::String str_msg;
geometry_msgs::Twist imu_data2;
sensor_msgs::Imu imu_data;
ros::Publisher chatter("chatter", &str_msg);
ros::Publisher imu2("imu_test2", &imu_data2);
ros::Publisher imu("imu_test", &imu_data);


ADXL345_I2C accelerometer(p9, p10);
//Serial pc(USBTX, USBRX);
I2C i2c(p9, p10);
const int adrIMU3000 = 0xD0;//8-bit write

int readingsAcc[3] = {0, 0, 0};
int gyroreadings[3] = {0,0,0};

void measureAngular();
void gyroInit();


char hello[13] = "hello world!";
char imu_data_id[] = "/imu_data_id";


int main() {

    nh.initNode();
    nh.advertise(chatter);
  //  advertise imu
    nh.advertise(imu2);
    nh.advertise(imu);
    

    // pc.baud(9600);

     //pc.printf("Starting ADXL345 test...\n");
     wait(.001);
    // pc.printf("Device ID is: 0x%02x\n", accelerometer.getDeviceID());
     wait(.001);
    
     // These are here to test whether any of the initialization fails. It will print the failure
     if (accelerometer.setPowerControl(0x00)){
        // pc.printf("didn't intitialize power control\n"); 
         return 0;  }
     //Full resolution, +/-16g, 4mg/LSB.
     wait(.001);
     
     if(accelerometer.setDataFormatControl(0x0B)){
       // pc.printf("didn't set data format\n");
        return 0;  }
     wait(.001);
     
     //3.2kHz data rate.
     if(accelerometer.setDataRate(ADXL345_3200HZ)){
        //pc.printf("didn't set data rate\n");
        return 0;    }
     wait(.001);
     
     //Measurement mode.
     
     if(accelerometer.setPowerControl(MeasurementMode)) {
      //  pc.printf("didn't set the power control to measurement\n"); 
        return 0;   } 
 
     gyroInit();
     

  
      imu_data.header.frame_id = imu_data_id;
      imu_data.orientation_covariance[0] = 0.0;
      imu_data.angular_velocity_covariance[0] = 0.0;
      imu_data.linear_acceleration_covariance[0] = 0.0;
      for (int i = 1; i<9; i++)
      {
          imu_data.orientation_covariance[i] = 0.0;
          imu_data.angular_velocity_covariance[i] = 0.0;
          imu_data.linear_acceleration_covariance[i] = 0.0;
      }
     
     
     
     
     while (1) {
     
     
        wait(0.1);
        str_msg.data = hello;
        chatter.publish( &str_msg );
        
        measureAngular();
        accelerometer.getOutput(readingsAcc);    
        
        imu_data2.angular.x = (float)(gyroreadings[0]+90)*0.00013315805;//*250/32768/360*2*3.1415926;
        imu_data2.angular.y = (float)gyroreadings[1]*0.00013315805;
        imu_data2.angular.z = (float)(gyroreadings[2]+10)*0.00013315805;        
        imu_data2.linear.x = (float)readingsAcc[0]*9.8/256;
        imu_data2.linear.y = (float)readingsAcc[1]*9.8/256;
        imu_data2.linear.z = (float)readingsAcc[2]*9.8/256;    
        
        imu_data.angular_velocity.x     = (float)(gyroreadings[0]+90)*0.00013315805;
        imu_data.angular_velocity.y     = (float)gyroreadings[1]*0.00013315805;
        imu_data.angular_velocity.z     = (float)(gyroreadings[2]+10)*0.00013315805;
        imu_data.linear_acceleration.x  = (float)readingsAcc[0]*9.8/256;
        imu_data.linear_acceleration.y  = (float)readingsAcc[1]*9.8/256;
        imu_data.linear_acceleration.z  = (float)readingsAcc[2]*9.8/256; 
        
        imu_data.header.stamp = nh.now();
        imu.publish( &imu_data );
        imu2.publish( &imu_data2 );
        
        
        
        
        nh.spinOnce();
     }
 
 }
 


void gyroInit()
{

    i2c.frequency(4000); //set 400kHz

 //   pc.baud(9600);

    char PWR_M[2] = {0x3E, 0x80};
    i2c.write(adrIMU3000, PWR_M, 2, true);
    char SMPL[2] = {0x15, 0x00};
    i2c.write(adrIMU3000, SMPL, 2, true);
    char DLPF[2] = {0x16, 0x18};
    i2c.write(adrIMU3000, DLPF, 2, true);
    char INT_C[2] = {0x17, 0x05};
    i2c.write(adrIMU3000, INT_C, 2, true);
    char PWR_M2[2] = {0x3E, 0x00};
    i2c.write(adrIMU3000, PWR_M2, 2, true);

   //tick.attach_us(&measure, interval);

}

void measureAngular()
{
    char adrIMU3000_y[1];
    char readIMU3000[8];

    short int read_x_2byte = 0;
    short int read_y_2byte = 0;
    short int read_z_2byte = 0;

    adrIMU3000_y[0] = 0x1D; //register GYRO_XOUT_H

    i2c.write(adrIMU3000, adrIMU3000_y , 1);
    i2c.read(adrIMU3000, readIMU3000, 8);

    read_x_2byte = ((readIMU3000[0] << 8) + readIMU3000[1]);
    read_y_2byte = ((readIMU3000[2] << 8) + readIMU3000[3]);
    read_z_2byte = ((readIMU3000[4] << 8) + readIMU3000[5]);

    gyroreadings[0] = read_x_2byte;
    gyroreadings[1] = read_y_2byte;
    gyroreadings[2] = read_z_2byte;
    //pc.printf("%d, %d, %d\r\n", read_x_2byte, read_y_2byte, read_z_2byte);

}

#endif