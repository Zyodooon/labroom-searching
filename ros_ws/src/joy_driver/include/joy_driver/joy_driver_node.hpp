#pragma once

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"

class JoyDriverNode : public rclcpp::Node {
public:
  JoyDriverNode();

private:
  void declare_parameters();
  void get_parameters();
  void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg);
  double apply_deadzone(double value, double deadzone) const;
  double remap_trigger_to_0_1(double value) const;

  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_subscriber_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_publisher_;

  // ros parameter
  int axis_linear_x_;
  int axis_linear_y_;
  int axis_trigger_l2_;
  int axis_trigger_r2_;
  double deadzone_linear_;
  double deadzone_angular_;
  double max_linear_x_;
  double max_linear_y_;
  double max_angular_z_;
};
