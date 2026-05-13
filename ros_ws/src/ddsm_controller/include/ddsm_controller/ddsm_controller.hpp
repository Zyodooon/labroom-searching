#pragma once

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#include <cstdint>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

class DDSMController : public rclcpp::Node {
public:
  DDSMController();

private:
  static constexpr uint8_t CMD_VELOCITY_ = 0x64;
  static constexpr uint8_t CMD_GET_STATUS_ = 0x74;

  boost::asio::io_context io_context_;
  boost::asio::serial_port serial_port_{io_context_};

  rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr
      wheel_rpm_subscription_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr
      motor_vel_publisher_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::string port_name_;
  int baud_rate_{115200};
  std::vector<int64_t> motor_ids_;
  double max_rpm_{330.0};
  bool enable_feedback_{false};

  void declare_parameters();
  void get_parameters();

  bool setup_serial_port(const std::string &port_name, unsigned int baud_rate);
  bool send_velocity_command(uint8_t motor_id, double target_rpm);
  bool request_motor_feedback(uint8_t motor_id, double &velocity_feedback);

  uint8_t calc_crc8(const std::vector<uint8_t> &data);
  std::vector<uint8_t> create_velocity_command(uint8_t motor_id,
                                               double target_rpm);
  int32_t decode_velocity_feedback(const std::vector<uint8_t> &response);
  void request_and_receive_feedback();

  void velocity_callback(const std_msgs::msg::Float64MultiArray::SharedPtr msg);
};
