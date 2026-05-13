#pragma once

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

class MecanumDriveNode : public rclcpp::Node {
public:
  MecanumDriveNode();

private:
  void declare_parameters();
  void get_parameters();
  void cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg);

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr
      cmd_vel_subscription_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr
      wheel_rpm_publisher_;

  // ros param
  double wheel_radius_;
  double wheel_base_;
  double tread_;
  double max_wheel_rpm_{330.0};
};
