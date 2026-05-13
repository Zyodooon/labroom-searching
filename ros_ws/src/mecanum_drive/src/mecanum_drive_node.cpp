#include "mecanum_drive/mecanum_drive_node.hpp"

#include <algorithm>
#include <array>
#include <functional>

namespace {
constexpr double Pi = 3.14159265358979323846;
constexpr std::size_t WheelCount = 4;
} // namespace

MecanumDriveNode::MecanumDriveNode() : Node("mecanum_drive_node") {
  declare_parameters();
  get_parameters();

  cmd_vel_subscription_ = this->create_subscription<geometry_msgs::msg::Twist>(
      "/cmd_vel", 10,
      std::bind(&MecanumDriveNode::cmd_vel_callback, this,
                std::placeholders::_1));

  wheel_rpm_publisher_ =
      this->create_publisher<std_msgs::msg::Float64MultiArray>(
          "/wheel_rpm_targets", 10);

  RCLCPP_INFO(this->get_logger(), "MecanumDriveNode has started.");
}

void MecanumDriveNode::declare_parameters() {
  this->declare_parameter<double>("wheel_radius", 0.05);
  this->declare_parameter<double>("wheel_base", 0.20);
  this->declare_parameter<double>("tread", 0.20);
  this->declare_parameter<double>("max_wheel_rpm", 330.0);
}

void MecanumDriveNode::get_parameters() {
  this->get_parameter("wheel_radius", wheel_radius_);
  this->get_parameter("wheel_base", wheel_base_);
  this->get_parameter("tread", tread_);
  this->get_parameter("max_wheel_rpm", max_wheel_rpm_);
}

void MecanumDriveNode::cmd_vel_callback(
    const geometry_msgs::msg::Twist::SharedPtr msg) {
  if (wheel_radius_ <= 0.0) {
    RCLCPP_ERROR_THROTTLE(this->get_logger(), *this->get_clock(), 2000,
                          "wheel_radius must be greater than zero.");
    return;
  }

  const double lxy_sum = wheel_base_ + tread_;
  const double vx = msg->linear.x;
  const double vy = msg->linear.y;
  const double wz = msg->angular.z;

  const double front_left_rad_per_sec =
      (vx - vy - lxy_sum * wz) / wheel_radius_;
  const double front_right_rad_per_sec =
      (vx + vy + lxy_sum * wz) / wheel_radius_;
  const double rear_left_rad_per_sec =
      (vx + vy - lxy_sum * wz) / wheel_radius_;
  const double rear_right_rad_per_sec =
      (vx - vy + lxy_sum * wz) / wheel_radius_;

  const std::array<double, WheelCount> wheel_rad_per_sec = {
      front_left_rad_per_sec,
      front_right_rad_per_sec,
      rear_left_rad_per_sec,
      rear_right_rad_per_sec,
  };

  std::array<double, WheelCount> wheel_rpm{};
  for (std::size_t index = 0; index < wheel_rpm.size(); ++index) {
    const double rpm = wheel_rad_per_sec[index] * 60.0 / (2 * Pi);
    wheel_rpm[index] = std::clamp(rpm, -max_wheel_rpm_, max_wheel_rpm_);
  }

  std_msgs::msg::Float64MultiArray rpm_msg;
  rpm_msg.data.assign(wheel_rpm.begin(), wheel_rpm.end());
  wheel_rpm_publisher_->publish(rpm_msg);
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MecanumDriveNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
