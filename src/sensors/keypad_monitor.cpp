#include <memory>

#include <rclcpp/rclcpp.hpp>

#include <mirte_telemetrix_cpp/sensors/keypad_monitor.hpp>

#include <mirte_msgs/msg/keypad.hpp>
#include <mirte_msgs/srv/get_keypad.hpp>

std::vector<std::shared_ptr<KeypadMonitor>> KeypadMonitor::get_keypad_monitors(
    std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
    std::shared_ptr<Mirte_Board> board, std::shared_ptr<Parser> parser) {
  std::vector<std::shared_ptr<KeypadMonitor>> sensors;
  auto keypads = Keypad_data::parse_keypad_data(parser, board);
  for (auto keypad : keypads) {
    sensors.push_back(std::make_shared<KeypadMonitor>(nh, tmx, board, keypad));
    std::cout << "add keypad" << keypad->name << std::endl;
  }
  return sensors;
  // TODO: schedule periodic publishing
}

KeypadMonitor::KeypadMonitor(std::shared_ptr<rclcpp::Node> nh,
                             std::shared_ptr<TMX> tmx,
                             std::shared_ptr<Mirte_Board> board,
                             std::shared_ptr<Keypad_data> keypad_data)
    : Mirte_Sensor(nh, tmx, board, {keypad_data->pin}, keypad_data->name) {
  this->keypad_data = keypad_data;
  keypad_pub = nh->create_publisher<mirte_msgs::msg::Keypad>(
      "keypad/" + keypad_data->name, 1);
  keypad_pressed_pub = nh->create_publisher<mirte_msgs::msg::Keypad>(
      "keypad/" + keypad_data->name + "_pressed", 1);
  keypad_service = nh->create_service<mirte_msgs::srv::GetKeypad>(
      keypad_data->name + "_get",
      std::bind(&KeypadMonitor::keypad_callback, this, std::placeholders::_1,
                std::placeholders::_2));
  tmx->setPinMode(keypad_data->pin, TMX::PIN_MODES::ANALOG_INPUT, true, 0);
  tmx->add_analog_callback(keypad_data->pin, [this](auto pin, auto value) {
    this->callback(value);
  });
}

void KeypadMonitor::callback(uint16_t value) {
  this->value = value;
  this->publish();
}

bool KeypadMonitor::keypad_callback(
    const std::shared_ptr<mirte_msgs::srv::GetKeypad::Request> req,
    std::shared_ptr<mirte_msgs::srv::GetKeypad::Response> res) {
  res->data = this->last_debounced_key;
  return true;
}

void KeypadMonitor::publish() {
  mirte_msgs::msg::Keypad msg;

  std::string key = "";
  auto maxValue = std::pow(2, board->get_adc_bits());
  auto scale = 4096.0 / maxValue;
  // RCLCPP_INFO(nh->get_logger(), "%d", this->value);
  if (this->value < 70 / scale) {
    key = "left";
  } else if (this->value < 230 / scale) {
    key = "up";
  } else if (this->value < 410 / scale) {
    key = "down";
  } else if (this->value < 620 / scale) {
    key = "right";
  } else if (this->value < 880 / scale) {

    key = "enter";
  }
  // # Do some debouncing
  if (this->last_key != key) {
    this->last_debounce_time = nh->now().seconds();
  }

  std::string debounced_key = "";
  if (nh->now().seconds() - this->last_debounce_time > 0.1) {
    debounced_key = key;
  }

  // # Publish the last debounced key
  mirte_msgs::msg::Keypad keypad;
  keypad.key = debounced_key;
  this->keypad_pub->publish(keypad);

  // # check if we need to send a pressed message
  if (this->last_debounced_key != "" &&
      this->last_debounced_key != debounced_key) // TODO: check this
  {
    mirte_msgs::msg::Keypad pressed;
    // pressed.header = this->get_header();
    pressed.key = this->last_debounced_key;
    this->keypad_pressed_pub->publish(pressed);
  }
  this->last_key = key;
  this->last_debounced_key = debounced_key;
}