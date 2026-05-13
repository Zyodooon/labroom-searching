#include "ddsm_controller/ddsm_controller.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <thread>

namespace {
constexpr std::size_t DdsmFrameLength = 10;
constexpr std::size_t DdsmCrcInputLength = DdsmFrameLength - 1;
} // namespace

DDSMController::DDSMController() : rclcpp::Node("ddsm_controller_node") {
  declare_parameters();
  get_parameters();

  wheel_rpm_subscription_ =
      this->create_subscription<std_msgs::msg::Float64MultiArray>(
          "/wheel_rpm_targets", 10,
          std::bind(&DDSMController::velocity_callback, this,
                    std::placeholders::_1));
  motor_vel_publisher_ =
      this->create_publisher<std_msgs::msg::Float64MultiArray>(
          "/motor_vel_feedback", 10);

  setup_serial_port(port_name_, static_cast<unsigned int>(baud_rate_));

  if (enable_feedback_) {
    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(200),
        std::bind(&DDSMController::request_and_receive_feedback, this));
  }
}

void DDSMController::declare_parameters() {
  this->declare_parameter<std::string>("port_name", "/dev/ttyACM0");
  this->declare_parameter<int>("baud_rate", 115200);
  this->declare_parameter<std::vector<int64_t>>("motor_ids", {1, 2, 3, 4});
  this->declare_parameter<double>("max_rpm", 330.0);
  this->declare_parameter<bool>("enable_feedback", false);
}

void DDSMController::get_parameters() {
  port_name_ = this->get_parameter("port_name").as_string();
  baud_rate_ = this->get_parameter("baud_rate").as_int();
  motor_ids_ = this->get_parameter("motor_ids").as_integer_array();
  max_rpm_ = this->get_parameter("max_rpm").as_double();
  enable_feedback_ = this->get_parameter("enable_feedback").as_bool();
}

bool DDSMController::setup_serial_port(const std::string &port_name,
                                       unsigned int baud_rate) {
  try {
    serial_port_.open(port_name);
    if (!serial_port_.is_open()) {
      RCLCPP_ERROR(this->get_logger(), "Failed to open serial port: %s",
                   port_name.c_str());
      return false;
    }

    serial_port_.set_option(
        boost::asio::serial_port_base::baud_rate(baud_rate));
    serial_port_.set_option(boost::asio::serial_port_base::character_size(8));
    serial_port_.set_option(boost::asio::serial_port_base::parity(
        boost::asio::serial_port_base::parity::none));
    serial_port_.set_option(boost::asio::serial_port_base::stop_bits(
        boost::asio::serial_port_base::stop_bits::one));
    serial_port_.set_option(boost::asio::serial_port_base::flow_control(
        boost::asio::serial_port_base::flow_control::none));

    RCLCPP_INFO(this->get_logger(), "Serial port %s initialized successfully.",
                port_name.c_str());
    return true;
  } catch (const boost::system::system_error &e) {
    RCLCPP_ERROR(this->get_logger(), "Serial port setup failed: %s", e.what());
    return false;
  }
}

void DDSMController::velocity_callback(
    const std_msgs::msg::Float64MultiArray::SharedPtr msg) {
  if (msg->data.size() != motor_ids_.size()) {
    RCLCPP_WARN_THROTTLE(
        this->get_logger(), *this->get_clock(), 2000,
        "wheel_rpm_targets size %zu does not match motor_ids size %zu",
        msg->data.size(), motor_ids_.size());
    return;
  }

  if (!serial_port_.is_open()) {
    RCLCPP_WARN_THROTTLE(
        this->get_logger(), *this->get_clock(), 2000,
        "Serial port is not open. Skipping wheel RPM command.");
    return;
  }

  for (size_t index = 0; index < motor_ids_.size(); ++index) {
    const auto motor_id = static_cast<uint8_t>(motor_ids_[index]);
    const double clamped_rpm =
        std::clamp(msg->data[index], -max_rpm_, max_rpm_);
    if (send_velocity_command(motor_id, clamped_rpm)) {
      RCLCPP_INFO(this->get_logger(), "Sent wheel RPM %.2f to motor ID 0x%02X",
                  clamped_rpm, motor_id);
    }
  }
}

bool DDSMController::send_velocity_command(uint8_t motor_id,
                                           double target_rpm) {
  const auto command = create_velocity_command(motor_id, target_rpm);
  if (command.empty()) {
    return false;
  }

  try {
    boost::asio::write(serial_port_, boost::asio::buffer(command));
    return true;
  } catch (const boost::system::system_error &e) {
    RCLCPP_ERROR(this->get_logger(), "Boost Asio write failed for motor %u: %s",
                 motor_id, e.what());
    return false;
  }
}

int32_t
DDSMController::decode_velocity_feedback(const std::vector<uint8_t> &response) {
  if (response.size() < DdsmFrameLength) {
    RCLCPP_ERROR(this->get_logger(),
                 "Response too short to decode velocity feedback.");
    return 0;
  }

  const auto velocity = static_cast<int16_t>(
      (static_cast<uint16_t>(response[4]) << 8) |
      static_cast<uint16_t>(response[5]));
  return velocity;
}

bool DDSMController::request_motor_feedback(uint8_t motor_id,
                                            double &velocity_feedback) {
  std::vector<uint8_t> request = {motor_id, CMD_GET_STATUS_, 0x00,
                                  0x00,     0x00,            0x00,
                                  0x00,     0x00,            0x00};
  request.push_back(calc_crc8(request));

  try {
    boost::asio::write(serial_port_, boost::asio::buffer(request));
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  } catch (const boost::system::system_error &e) {
    RCLCPP_ERROR(this->get_logger(),
                 "Boost Asio write failed for feedback request: %s", e.what());
    return false;
  }

  try {
    std::vector<uint8_t> response_buffer(32);
    const size_t bytes_transferred =
        serial_port_.read_some(boost::asio::buffer(response_buffer));
    if (bytes_transferred == 0) {
      return false;
    }

    response_buffer.resize(bytes_transferred);
    velocity_feedback =
        static_cast<double>(decode_velocity_feedback(response_buffer));
    return true;
  } catch (const boost::system::system_error &e) {
    RCLCPP_ERROR(this->get_logger(),
                 "Boost Asio read failed for feedback request: %s", e.what());
    return false;
  }
}

void DDSMController::request_and_receive_feedback() {
  if (!serial_port_.is_open() || !enable_feedback_) {
    return;
  }

  std_msgs::msg::Float64MultiArray feedback_msg;
  feedback_msg.data.reserve(motor_ids_.size());

  for (const auto motor_id_value : motor_ids_) {
    double velocity_feedback = 0.0;
    const auto motor_id = static_cast<uint8_t>(motor_id_value);
    if (!request_motor_feedback(motor_id, velocity_feedback)) {
      velocity_feedback = 0.0;
    }
    feedback_msg.data.push_back(velocity_feedback);
  }

  motor_vel_publisher_->publish(feedback_msg);
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<DDSMController>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}

uint8_t DDSMController::calc_crc8(const std::vector<uint8_t> &data) {
  uint8_t crc = 0x00;
  const uint8_t poly = 0x8C;
  for (const auto byte : data) {
    crc ^= byte;
    for (int bit = 0; bit < 8; ++bit) {
      if ((crc & 0x01U) != 0U) {
        crc = static_cast<uint8_t>((crc >> 1U) ^ poly);
      } else {
        crc = static_cast<uint8_t>(crc >> 1U);
      }
    }
  }
  return crc;
}

std::vector<uint8_t>
DDSMController::create_velocity_command(uint8_t motor_id, double target_rpm) {
  const double bounded_rpm = std::clamp(target_rpm, -max_rpm_, max_rpm_);
  const int16_t velocity = static_cast<int16_t>(std::round(bounded_rpm));
  const auto velocity_bits = static_cast<uint16_t>(velocity);

  std::vector<uint8_t> frame;
  frame.reserve(DdsmFrameLength);
  frame.push_back(motor_id);
  frame.push_back(CMD_VELOCITY_);
  frame.push_back(static_cast<uint8_t>((velocity_bits >> 8) & 0xFF));
  frame.push_back(static_cast<uint8_t>(velocity_bits & 0xFF));
  frame.push_back(0x00);
  frame.push_back(0x00);
  frame.push_back(0x00);
  frame.push_back(0x00);
  frame.push_back(0x00);
  if (frame.size() != DdsmCrcInputLength) {
    RCLCPP_ERROR(this->get_logger(), "Invalid DDSM frame size before CRC: %zu",
                 frame.size());
    return {};
  }
  frame.push_back(calc_crc8(frame));
  return frame;
}
