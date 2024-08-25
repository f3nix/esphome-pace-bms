#pragma once

#include <vector>

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#include "pace_bms_v25.h"
#include <functional>
#include <queue>

namespace esphome {
namespace pace_bms {

// this class encapsulates an instance of PaceBmsV25 (which handles protocol version 0x25) and injects the logging dependencies into it
// in the future, other protocol versions may be supported
class PaceBms : public PollingComponent, public uart::UARTDevice {
 public:
  void set_flow_control_pin(GPIOPin* flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
  void set_address(int address) { this->address_ = address; }
  void set_protocol_version(int protocol_version) { this->protocol_version_ = protocol_version; }
  void set_request_throttle(int request_throttle) { this->request_throttle_ = request_throttle; }
  void set_response_timeout(int response_timeout) { this->response_timeout_ = response_timeout; }

  void setup() override;
  void update() override;
  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;

  void register_analog_information_callback(std::function<void(PaceBmsV25::AnalogInformation&)> callback) { analog_information_callbacks_.push_back(callback); }
  void register_status_information_callback(std::function<void(PaceBmsV25::StatusInformation&)> callback) { status_information_callbacks_.push_back(callback); }
  void register_hardware_version_callback(std::function<void(std::string&)> callback) { hardware_version_callbacks_.push_back(callback); }
  void register_serial_number_callback(std::function<void(std::string &) > callback) { serial_number_callbacks_.push_back(callback); }

  enum SwitchType {
	  ST_BuzzerAlarm,
	  ST_LedAlarm,
	  ST_ChargeCurrentLimiter,
	  ST_ChargeMosfet,
	  ST_DischargeMosfet,
	  // todo: make this a select/option?
	  ST_ChargeCurrentLimiterCurrentLimitHighGear, // false = low, true = high
	  // todo: make this a button?
	  ST_Reboot,
  };
  void set_switch_state(SwitchType switch_type, bool state);

 protected:
  GPIOPin* flow_control_pin_{ nullptr };
  int address_{ 0 };
  int protocol_version_{ 0 };
  int request_throttle_{ 0 };
  int response_timeout_{ 0 };


  PaceBmsV25* pace_bms_v25_;
  static const uint8_t max_data_len_ = 150;
  uint8_t raw_data_[max_data_len_];
  uint8_t raw_data_index_{0};
  uint32_t last_transmit_{ 0 };
  uint32_t last_receive_{ 0 };
  bool request_outstanding_ = false;

  void send_next_request_frame_();
  void process_response_frame_(uint8_t* frame_bytes, uint8_t frame_length);

  void handle_analog_information_response(std::vector<uint8_t>& response);
  void handle_status_information_response(std::vector<uint8_t>& response);
  void handle_hardware_version_response(std::vector<uint8_t>& response);
  void handle_serial_number_response(std::vector<uint8_t>& response);
  void handle_write_switch_command_response(PaceBmsV25::SwitchCommand, std::vector<uint8_t>& response);
  void handle_write_mosfet_switch_command_response(PaceBmsV25::MosfetType type, PaceBmsV25::MosfetState state, std::vector<uint8_t>& response);

  // each item points to:
  //     a description such as "Analog Information" for logging
  //     a function that will generate the request frame (to avoid holding the memory prior to it being required)
  //     a function that will process the response frame and dispatch the results to any child sensors registered via the callback vectors
  struct command_item
  {
	  std::string description_;
	  std::function<void(std::vector<uint8_t>&)> create_request_frame_;
	  std::function<void(std::vector<uint8_t>&)> process_response_frame_;
  };
  // when the bus is clear:
  //     the next command_item will be popped
  //     the request frame generated and dispatched via command_item.create_request_frame_
  //     the expected response handler (command_item.process_response_frame_) will be assigned to next_response_handler_ to be called once a response frame arrives
  std::queue<command_item*> command_queue_;
  std::function<void(std::vector<uint8_t>&)> next_response_handler_ = nullptr;
  std::string last_request_description;

  // child sensor callbacks
  std::vector<std::function<void(PaceBmsV25::AnalogInformation&)>> analog_information_callbacks_;
  std::vector<std::function<void(PaceBmsV25::StatusInformation&)>> status_information_callbacks_;
  std::vector<std::function<void(std::string&)>> hardware_version_callbacks_;
  std::vector<std::function<void(std::string&)>> serial_number_callbacks_;
};
 
}  // namespace pace_bms
}  // namespace esphome
