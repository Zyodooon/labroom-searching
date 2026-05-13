#include "joy_driver/joy_driver_node.hpp"

#include <cmath>
#include <functional>

JoyDriverNode::JoyDriverNode() : Node("joy_driver_node") {
  declare_parameters();
  get_parameters();

  joy_subscriber_ = this->create_subscription<sensor_msgs::msg::Joy>(
      "/joy", 10,
      std::bind(&JoyDriverNode::joy_callback, this, std::placeholders::_1));

  cmd_vel_publisher_ =
      this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

  RCLCPP_INFO(this->get_logger(), "JoyDriverNode has started.");
}

void JoyDriverNode::declare_parameters() {
  this->declare_parameter<int>("axis_linear_x", 1);
  this->declare_parameter<int>("axis_linear_y", 0);
  this->declare_parameter<int>("axis_trigger_l2", 7);
  this->declare_parameter<int>("axis_trigger_r2", 8);
  this->declare_parameter<double>("deadzone_linear", 0.05);
  this->declare_parameter<double>("deadzone_angular", 0.05);
  this->declare_parameter<double>("max_linear_x", 1.0);
  this->declare_parameter<double>("max_linear_y", 1.0);
  this->declare_parameter<double>("max_angular_z", 1.0);
}

void JoyDriverNode::get_parameters() {
  this->get_parameter("axis_linear_x", axis_linear_x_);
  this->get_parameter("axis_linear_y", axis_linear_y_);
  this->get_parameter("axis_trigger_l2", axis_trigger_l2_);
  this->get_parameter("axis_trigger_r2", axis_trigger_r2_);
  this->get_parameter("deadzone_linear", deadzone_linear_);
  this->get_parameter("deadzone_angular", deadzone_angular_);
  this->get_parameter("max_linear_x", max_linear_x_);
  this->get_parameter("max_linear_y", max_linear_y_);
  this->get_parameter("max_angular_z", max_angular_z_);
}

double JoyDriverNode::apply_deadzone(double value, double deadzone) const {
  return std::abs(value) < deadzone ? 0.0 : value;
}

double JoyDriverNode::remap_trigger_to_0_1(double value) const {
  return (1.0 - value) * 0.5;
}

void JoyDriverNode::joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg) {
  // check invalid axis
  const int axis_count = static_cast<int>(msg->axes.size());
  if (axis_linear_x_ < 0 || axis_linear_x_ >= axis_count ||
      axis_linear_y_ < 0 || axis_linear_y_ >= axis_count ||
      axis_trigger_l2_ < 0 || axis_trigger_l2_ >= axis_count ||
      axis_trigger_r2_ < 0 || axis_trigger_r2_ >= axis_count) {
    RCLCPP_WARN_THROTTLE(
        this->get_logger(), *this->get_clock(), 2000,
        "Configured joystick axes are out of range. axes size=%d", axis_count);
    return;
  }

  // Twist型の変数作成
  geometry_msgs::msg::Twist cmd_vel_msg;

  // msg_vel_msgに値を入れていく
  cmd_vel_msg.linear.x =
      apply_deadzone(msg->axes[axis_linear_x_], deadzone_linear_) *
      max_linear_x_;
  cmd_vel_msg.linear.y =
      apply_deadzone(msg->axes[axis_linear_y_], deadzone_linear_) *
      max_linear_y_;
  const double l2 = remap_trigger_to_0_1(msg->axes[axis_trigger_l2_]);
  const double r2 = remap_trigger_to_0_1(msg->axes[axis_trigger_r2_]);
  cmd_vel_msg.angular.z =
      apply_deadzone(r2 - l2, deadzone_angular_) * max_angular_z_;

  cmd_vel_publisher_->publish(cmd_vel_msg);
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<JoyDriverNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
