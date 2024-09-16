#pragma once

#include <rclcpp/rclcpp.hpp>
#include <mirte_telemetrix_cpp/sensors/base_sensor.hpp>

#include <mirte_telemetrix_cpp/parsers/p_sensors.hpp>

#include <mirte_msgs/msg/keypad.hpp>
#include <mirte_msgs/srv/get_keypad.hpp>

class KeypadMonitor : public Mirte_Sensor {
public:
  KeypadMonitor(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
                std::shared_ptr<Mirte_Board> board,
                std::shared_ptr<Keypad_data> keypad_data);
  void publish();
  std::shared_ptr<Keypad_data> keypad_data;
  std::shared_ptr<rclcpp::Publisher<mirte_msgs::msg::Keypad>> keypad_pub;
  std::shared_ptr<rclcpp::Publisher<mirte_msgs::msg::Keypad>>
      keypad_pressed_pub;
  std::shared_ptr<rclcpp::Service<mirte_msgs::srv::GetKeypad>> keypad_service;
  bool keypad_callback(
      const std::shared_ptr<mirte_msgs::srv::GetKeypad::Request> req,
      std::shared_ptr<mirte_msgs::srv::GetKeypad::Response> res);
  void callback(uint16_t value);
  uint16_t value;
  std::string last_key;
  double last_debounce_time = nh->now().seconds();
  std::string last_debounced_key;

  static std::vector<std::shared_ptr<KeypadMonitor>> get_keypad_monitors(
      std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
      std::shared_ptr<Mirte_Board> board, std::shared_ptr<Parser> parser);
};