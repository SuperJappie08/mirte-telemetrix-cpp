#pragma once

#include <mirte_telemetrix_cpp/parsers/sensors/sonar_data.hpp>

class EncoderData : public SensorData
{
public:
  pin_t pinA = (pin_t)-1;
  pin_t pinB = (pin_t)-1;

  EncoderData(
    std::shared_ptr<Parser> parser, std::shared_ptr<Mirte_Board> board, std::string name,
    std::map<std::string, rclcpp::ParameterValue> parameters);

  bool check();

  static std::string get_sensor_class() { return "encoder"; }
};