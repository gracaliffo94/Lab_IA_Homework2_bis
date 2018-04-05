#include "ros/ros.h"
#include "tf/transform_listener.h"
#include "sensor_msgs/LaserScan.h"
#include "nav_msgs/Odometry.h"

class Transf{
	private:
	ros::NodeHandle n;
	ros::Subscriber laser_subscriber;
	ros::Subscriber odom_subscriber;
	nav_msgs::Odometry * last_odometry_message;
	tf::StampedTransform transform;
	tf::TransformListener listener;

	public:
	Transf(){
		laser_subscriber=n.subscribe("base_scan", 1000, &Transf::base_scanCallback,this);
		odom_subscriber=n.subscribe("odom",1000, &Transf::odom_Callback,this);
	}

	void odom_Callback(nav_msgs::Odometry msg){
		last_odometry_message=&msg;
	}

	void base_scanCallback(sensor_msgs::LaserScan msg){
		try{
			listener.waitForTransform("/base_laser_link", "/base_link", ros::Time(0), ros::Duration(10.0));
        	        listener.lookupTransform("/base_laser_link", "/base_link",ros::Time(0), transform);
		} catch (tf::TransformException ex) {
    			ROS_ERROR("%s",ex.what());
		}
		if(last_odometry_message==NULL) return;
		uint32_t seconds = transform.stamp_.sec;
		uint32_t useconds= transform.stamp_.nsec;
		double x = transform.getOrigin().getX() + last_odometry_message->pose.pose.position.x;
		double y = transform.getOrigin().getY() + last_odometry_message->pose.pose.position.y;
		double angle = transform.getRotation().getAngle() +
                    tf::Quaternion(last_odometry_message->pose.pose.orientation.x,
                    last_odometry_message->pose.pose.orientation.y,
                    last_odometry_message->pose.pose.orientation.z,
                    last_odometry_message->pose.pose.orientation.w).getAngle();
		ROS_INFO("Timestamp: %d,%d ,Position (X: %g, Y: %g,  Angle: %g) ", seconds,useconds,x,y,angle);
	}
};
int main(int argc,char** argv){
	ros::init(argc, argv, "my_tf_listener");
	Transf mylistener;
	ros::spin();
	return(0);
}
