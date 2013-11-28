MyLidar-2013
============
This is the source code of ESE519 MyLidar project.

In order to connect the beagleboard to the AirPennNet-Guest and reconnect whennever the connection is lost, run the command "nohup ./every-30-seconds.sh &" after login.

To control the car wirelessly from host, add the command to export ROS_IP and export ROS_MASTER_URI at the end of .bashrc, run the command "source ~./bashrc" on the client and host seperately. Then the host could run roscore and ssh to client to run ros node as a slave.

Currently under constructing.
