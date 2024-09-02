#include "pace_bms.h"
#include "esphome/core/log.h"
#include <cinttypes>
#include <iomanip>
#include <sstream>
#include <functional>

namespace esphome {
namespace pace_bms { 

static const char* const TAG = "pace_bms";

// for the protocol implementation dependency injection only
static const char* const TAG_V25 = "pace_bms_v25";

void error_log_func(std::string message) {
    ESP_LOGE(TAG_V25, "%s", message.c_str());
}

void warning_log_func(std::string message) {
    ESP_LOGW(TAG_V25, "%s", message.c_str());
}

void info_log_func(std::string message) {
    ESP_LOGI(TAG_V25, "%s", message.c_str());
}

void debug_log_func(std::string message) {
    ESP_LOGD(TAG_V25, "%s", message.c_str());
}

void verbose_log_func(std::string message) {
    ESP_LOGV(TAG_V25, "%s", message.c_str());
}

void very_verbose_log_func(std::string message) {
    ESP_LOGVV(TAG_V25, "%s", message.c_str());
}


void PaceBms::setup() {
  if (this->protocol_version_ != 0x25) {
    this->status_set_error();
    ESP_LOGE(TAG, "Protocol version %02X is not supported", this->protocol_version_);
    return;
  }
  else {
    // the protocol en/decoder PaceBmsV25 is meant to be standalone with no dependencies - inject esphome logging function wrappers on construction
    this->pace_bms_v25_ = new PaceBmsV25(error_log_func, warning_log_func, info_log_func, debug_log_func, verbose_log_func, very_verbose_log_func);
  }

  if (this->flow_control_pin_ != nullptr) 
    this->flow_control_pin_->setup();
}

// fills up command_queue_ based on what was subscribed for by sensor instances via callbacks, for a full update cycle of BMS request/response pairs 
void PaceBms::update() {
  if (this->pace_bms_v25_ == nullptr)
    return;

 if (!command_queue_.empty()) {
    ESP_LOGW(TAG, "Commands still in queue on update(), skipping this cycle: Could not speak with the BMS fast enough: increase update_interval or reduce request_throttle.", command_queue_.size());
  }
  else {
    if (this->analog_information_callbacks_.size() > 0) {
      command_item* item = new command_item;
      item->description_ = std::string("read analog information");
      item->create_request_frame_ = std::bind(&PaceBmsV25::CreateReadAnalogInformationRequest, this->pace_bms_v25_, this->address_, std::placeholders::_1);
      item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_analog_information_response, this, std::placeholders::_1);
      command_queue_.push(item);
    }
    if (this->status_information_callbacks_.size() > 0) {
      command_item* item = new command_item;
      item->description_ = std::string("read status information");
      item->create_request_frame_ = std::bind(&PaceBmsV25::CreateReadStatusInformationRequest, this->pace_bms_v25_, this->address_, std::placeholders::_1);
      item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_status_information_response, this, std::placeholders::_1);
      command_queue_.push(item);
    }
    if (this->hardware_version_callbacks_.size() > 0) {
      command_item* item = new command_item;
      item->description_ = std::string("read hardware version");
      item->create_request_frame_ = std::bind(&PaceBmsV25::CreateReadHardwareVersionRequest, this->pace_bms_v25_, this->address_, std::placeholders::_1);
      item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_hardware_version_response, this, std::placeholders::_1);
      command_queue_.push(item);
    }
    if (this->serial_number_callbacks_.size() > 0) {
      command_item* item = new command_item;
      item->description_ = std::string("read serial number");
      item->create_request_frame_ = std::bind(&PaceBmsV25::CreateReadSerialNumberRequest, this->pace_bms_v25_, this->address_, std::placeholders::_1);
      item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_serial_number_response, this, std::placeholders::_1);
      command_queue_.push(item);
    }
    if (this->protocols_callbacks_.size() > 0) {
      command_item* item = new command_item;
      item->description_ = std::string("read protocols");
      item->create_request_frame_ = std::bind(&PaceBmsV25::CreateReadProtocolsRequest, this->pace_bms_v25_, this->address_, std::placeholders::_1);
      item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_read_protocols_response, this, std::placeholders::_1);
      command_queue_.push(item);
    }
    ESP_LOGV(TAG, "Update commands queued: %i", command_queue_.size());
  }
}

// incrementally process incoming bytes off the bus
//     eventually dispatching a full response to process_response_frame_ and then calling send_next_request_frame to continue poping the command_queue_
//     or just calling send_next_request_frame_ if previous request timed out and there are still commands in queue
void PaceBms::loop() {
  if (this->pace_bms_v25_ == nullptr)
    return;

  // if there is no request active, throw away any incoming data before proceeding
  if (this->request_outstanding_ == false &&
      this->available() != 0) {
    ESP_LOGV(TAG, "Throwing away incoming data because there is no request active");
    uint8_t byte;
    while (this->available() != 0) {
        this->read_byte(&byte);
    }
  }

  const uint32_t now = millis();

  // if no request is active, we are not throttled, and there are pending requests to send, do so
  if (this->request_outstanding_ == false &&
      now - this->last_transmit_ >= this->request_throttle_ &&
      this->command_queue_.size() > 0) {
    this->send_next_request_frame_();
    this->request_outstanding_ = true;
    this->last_transmit_ = now;
    this->last_receive_ = now;
    this->raw_data_index_ = 0;
  }

  // if a request is active but we have passed the response timeout period and no more data is available, abandon the request
  if (this->request_outstanding_ == true && 
      now - this->last_receive_ >= this->response_timeout_ &&
      this->available() == 0) {
    if (this->raw_data_index_ > 0) {
      std::string str(this->raw_data_, this->raw_data_ + this->raw_data_index_ + 1);
      ESP_LOGW(TAG, "Response frame timeout for request %s after %i ms, partial frame: %s", this->last_request_description.c_str(), now - this->last_receive_, str.c_str());
    }
    else {
      ESP_LOGW(TAG, "Response frame timeout for request %s after %i ms, no valid data received", this->last_request_description.c_str(), now - this->last_receive_);
    }
    request_outstanding_ = false;
    this->raw_data_index_ = 0;
    return;
  }

  // if no data or no request outstanding, nothing to do
  if (this->available() == 0 ||
      this->request_outstanding_ == false) {
    return;
  }

  // reset timer since we're actively receiving
  this->last_receive_ = now;

  while (this->available() != 0) {
    this->read_byte(&this->raw_data_[this->raw_data_index_]);

    // is the SOI marker present at byte 0?
    if (this->raw_data_index_ == 0 && this->raw_data_[this->raw_data_index_] != '~') {
      ESP_LOGV(TAG, "Response frame does not begin with '~', actual: 0x%02X = '%c'", this->raw_data_[this->raw_data_index_], this->raw_data_[this->raw_data_index_]);
      request_outstanding_ = false;
      this->raw_data_index_ = 0;
      return;
    }

    // is this the end of a frame? process it
    if (this->raw_data_[this->raw_data_index_] == '\r') {
      this->process_response_frame_(this->raw_data_, this->raw_data_index_ + 1);
      request_outstanding_ = false;
      this->raw_data_index_ = 0;
      return;
    }

    // did we run out of buffer before EOI?
    if (this->raw_data_index_ + 1 >= this->max_data_len_) {
      std::string str(this->raw_data_, this->raw_data_ + this->raw_data_index_ + 1);
      ESP_LOGV(TAG, "Response frame exceeds maximum supported length, last request was '%s', incomplete response frame: %s", this->last_request_description.c_str(), str.c_str());
      request_outstanding_ = false;
      this->raw_data_index_ = 0;
      return;
    }

    this->raw_data_index_++;
  }
}

// preferably we'll be setup after all child sensors have registered their callbacks via their own setup(), 
//     but this class still handle the case where they register late, a single update cycle will simply be missed in that case
float PaceBms::get_setup_priority() const { return setup_priority::LATE; }

void PaceBms::dump_config() {
    ESP_LOGCONFIG(TAG, "pace_bms:");
    LOG_PIN("  Flow Control Pin: ", this->flow_control_pin_);
    ESP_LOGCONFIG(TAG, "  Address: %i", this->address_);
    ESP_LOGCONFIG(TAG, "  Protocol Version: 0x%02X", this->protocol_version_);
    ESP_LOGCONFIG(TAG, "  Request Throttle (ms): %i", this->request_throttle_);
    ESP_LOGCONFIG(TAG, "  Response Timeout (ms): %i", this->response_timeout_);
    this->check_uart_settings(9600);
}


// pops the next item off of this->command_queue_, generates and dispatches a request frame, and sets up this->next_response_handler_
void PaceBms::send_next_request_frame_() {

    if (command_queue_.empty()) {
      ESP_LOGE(TAG, "command queue empty on send_next_request_frame");
      return;
    }
    PaceBms::command_item* command = command_queue_.front();
    command_queue_.pop();

    std::vector<uint8_t> request;
    command->create_request_frame_(request);

#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
    {
        std::string str(request.data(), request.data() + request.size());
        ESP_LOGVV(TAG, "Sending request frame: %s", str.c_str());
    }
#endif

    // process_response_frame_ will call this on the next frame received
    this->next_response_handler_ = command->process_response_frame_;
    // saved for logging
    this->last_request_description = command->description_;

    delete(command);

    ESP_LOGV(TAG, "Sending %s request", command->description_.c_str());
    if (this->flow_control_pin_ != nullptr)
        this->flow_control_pin_->digital_write(true);
    this->write_array(request.data(), request.size());
    // if flow control is required (rs485 does read+write on the same differential pair) then I don't see any other option than to block on flush(), even at this abysmal data rate
    // if using rs232, a flow control pin should not be assigned in yaml in order to avoid this block
    if (this->flow_control_pin_ != nullptr) {
        this->flush();
        this->flow_control_pin_->digital_write(false);
    }
}

// calls this->next_response_handler_ (set up from the previously dispatched command_queue_ item)
void PaceBms::process_response_frame_(uint8_t* frame_bytes, uint8_t frame_length) {
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
  {
    std::string str(frame_bytes, frame_bytes + frame_length);
    ESP_LOGVV(TAG, "Response frame received: %s", str.c_str());
  }
#endif

  std::vector<uint8_t> response(frame_bytes, frame_bytes + frame_length);

  if (next_response_handler_ != nullptr)
    next_response_handler_(response);
  else
    ESP_LOGE(TAG, "Response frame received but no response handler set");

  // this request/response pair is complete, any additional frames received will not be expected and should not be processed until the next command queue pop / send
  next_response_handler_ = nullptr;
}


void PaceBms::handle_analog_information_response(std::vector<uint8_t>& response) {
  ESP_LOGV(TAG, "Processing read analog information response");

  PaceBmsV25::AnalogInformation analog_information;
  bool result = this->pace_bms_v25_->ProcessReadAnalogInformationResponse(this->address_, response, analog_information);
  if (result == false) {
      ESP_LOGW(TAG, "BMS response did not indicate success for read analog information request");
      return;
  }

  // dispatch to any child sensor components that registered for a callback with us
  for (int i = 0; i < this->analog_information_callbacks_.size(); i++) {
    analog_information_callbacks_[i](analog_information);
  }
}

void PaceBms::handle_status_information_response(std::vector<uint8_t>& response) {
  ESP_LOGV(TAG, "Processing read status information response");

  PaceBmsV25::StatusInformation status_information;
  bool result = this->pace_bms_v25_->ProcessReadStatusInformationResponse(this->address_, response, status_information);
  if (result == false) {
      ESP_LOGW(TAG, "BMS response did not indicate success for read status information request");
      return;
  }

  // dispatch to any child sensor components that registered for a callback with us
  for (int i = 0; i < this->status_information_callbacks_.size(); i++) {
    status_information_callbacks_[i](status_information);
  }
}

void PaceBms::handle_hardware_version_response(std::vector<uint8_t>& response) {
  ESP_LOGV(TAG, "Processing read hardware version response");

  std::string hardware_version;
  bool result = this->pace_bms_v25_->ProcessReadHardwareVersionResponse(this->address_, response, hardware_version);
  if (result == false) {
      ESP_LOGW(TAG, "BMS response did not indicate success for read hardware version request");
      return;
  }

  // dispatch to any child sensor components that registered for a callback with us
  for (int i = 0; i < this->hardware_version_callbacks_.size(); i++) {
      hardware_version_callbacks_[i](hardware_version);
  }
}

void PaceBms::handle_serial_number_response(std::vector<uint8_t>& response) {
  ESP_LOGV(TAG, "Processing read serial number response");

  std::string serial_number;
  bool result = this->pace_bms_v25_->ProcessReadSerialNumberResponse(this->address_, response, serial_number);
  if (result == false) {
      ESP_LOGW(TAG, "BMS response did not indicate success for read serial number request");
      return;
  }

  // dispatch to any child sensor components that registered for a callback with us
  for (int i = 0; i < this->serial_number_callbacks_.size(); i++) {
    serial_number_callbacks_[i](serial_number);
  }
}

void PaceBms::handle_write_switch_command_response(PaceBmsV25::SwitchCommand switch_command, std::vector<uint8_t>& response) {
    ESP_LOGV(TAG, "Processing write switch command response");

    bool result = this->pace_bms_v25_->ProcessWriteSwitchCommandResponse(this->address_, switch_command, response);
    if (result == false) {
        ESP_LOGW(TAG, "BMS response did not indicate success for write switch command request");
        return;
    }
}

void PaceBms::handle_write_mosfet_switch_command_response(PaceBmsV25::MosfetType type, PaceBmsV25::MosfetState state, std::vector<uint8_t>& response) {
    ESP_LOGV(TAG, "Processing write switch command response");

    bool result = this->pace_bms_v25_->ProcessWriteMosfetSwitchCommandResponse(this->address_, type, state, response);
    if (result == false) {
        ESP_LOGW(TAG, "BMS response did not indicate success for write switch command request");
        return;
    }
}

void PaceBms::handle_write_shutdown_command_response(std::vector<uint8_t>& response) {
    ESP_LOGV(TAG, "Processing write shutdown command response");

    bool result = this->pace_bms_v25_->ProcessWriteShutdownCommandResponse(this->address_, response);
    if (result == false) {
        ESP_LOGW(TAG, "BMS response did not indicate success for write shutdown command request");
        return;
    }
}

void PaceBms::handle_read_protocols_response(std::vector<uint8_t>& response) {
    ESP_LOGV(TAG, "Processing read protocols response");

    PaceBmsV25::Protocols protocols;
    bool result = this->pace_bms_v25_->ProcessReadProtocolsResponse(this->address_, response, protocols);
    if (result == false) {
        ESP_LOGW(TAG, "BMS response did not indicate success for read protocols request");
        return;
    }
}

void PaceBms::handle_write_protocols_response(PaceBmsV25::Protocols protocols, std::vector<uint8_t>& response) {
    ESP_LOGV(TAG, "Processing write protocols response");

    bool result = this->pace_bms_v25_->ProcessWriteProtocolsResponse(this->address_, response);
    if (result == false) {
        ESP_LOGW(TAG, "BMS response did not indicate success for write protocols request");
        return;
    }
}

void PaceBms::handle_write_configuration_response(std::vector<uint8_t>& response) {
    ESP_LOGV(TAG, "Processing write configuration response");

    bool result = this->pace_bms_v25_->ProcessWriteConfigurationResponse(this->address_, response);
    if (result == false) {
        ESP_LOGW(TAG, "BMS response did not indicate success for write configuration request");
        return;
    }
}


void PaceBms::set_switch_state(PaceBmsV25::SwitchCommand state) {
  command_item* item = new command_item;

  // this is just to generate the text
  switch (state) {
    case PaceBmsV25::SC_DisableBuzzer:
    case PaceBmsV25::SC_EnableBuzzer:
      item->description_ = std::string("set buzzer alarm state ") + (state == PaceBmsV25::SC_EnableBuzzer ? "ON" : "OFF");
      break;
    case PaceBmsV25::SC_DisableLedWarning:
    case PaceBmsV25::SC_EnableLedWarning:
      item->description_ = std::string("set led alarm state ") + (state == PaceBmsV25::SC_EnableLedWarning ? "ON" : "OFF");
      break;
    case PaceBmsV25::SC_DisableChargeCurrentLimiter:
    case PaceBmsV25::SC_EnableChargeCurrentLimiter:
      item->description_ = std::string("set charge current limiter state ") + (state == PaceBmsV25::SC_EnableChargeCurrentLimiter ? "ON" : "OFF");
      break;
    case PaceBmsV25::SC_SetChargeCurrentLimiterCurrentLimitHighGear:
    case PaceBmsV25::SC_SetChargeCurrentLimiterCurrentLimitLowGear:
      item->description_ = std::string("set charge current limiter gear ") + (state == PaceBmsV25::SC_SetChargeCurrentLimiterCurrentLimitHighGear ? "high" : "low");
  }

  item->create_request_frame_ = std::bind(&PaceBmsV25::CreateWriteSwitchCommandRequest, this->pace_bms_v25_, this->address_, state, std::placeholders::_1);
  item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_write_switch_command_response, this, state, std::placeholders::_1);
  command_queue_.push(item);

  ESP_LOGV(TAG, "Update commands queued: %i", command_queue_.size());
}

void PaceBms::set_mosfet_state(PaceBmsV25::MosfetType type, PaceBmsV25::MosfetState state) {
  command_item* item = new command_item;

  // this is just to generate the text
  switch(type) {
    case PaceBmsV25::MT_Charge:
      item->description_ = std::string("set charge mosfet state ") + (state == PaceBmsV25::MS_Open ? "Open" : "Close");
      break;
    case PaceBmsV25::MT_Discharge:
      item->description_ = std::string("set discharge mosfet state ") + (state == PaceBmsV25::MS_Open ? "Open" : "Close");
      break;
  }

  item->create_request_frame_ = std::bind(&PaceBmsV25::CreateWriteMosfetSwitchCommandRequest, this->pace_bms_v25_, this->address_, type, state, std::placeholders::_1);
  item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_write_mosfet_switch_command_response, this, type, state, std::placeholders::_1);
  command_queue_.push(item);

  ESP_LOGV(TAG, "Update commands queued: %i", command_queue_.size());
}

void PaceBms::send_shutdown() {
  command_item* item = new command_item;
  item->description_ = std::string("send shutdown");
  item->create_request_frame_ = std::bind(&PaceBmsV25::CreateWriteShutdownCommandRequest, this->pace_bms_v25_, this->address_, std::placeholders::_1);
  item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_write_shutdown_command_response, this, std::placeholders::_1);
  command_queue_.push(item);

  ESP_LOGV(TAG, "Update commands queued: %i", command_queue_.size());
}

void PaceBms::set_protocols(PaceBmsV25::Protocols& protocols) {
    command_item* item = new command_item;
    item->description_ = std::string("setting protocols");
    item->create_request_frame_ = std::bind(&PaceBmsV25::CreateWriteProtocolsRequest, this->pace_bms_v25_, this->address_, protocols, std::placeholders::_1);
    item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_write_protocols_response, this, protocols, std::placeholders::_1);
    command_queue_.push(item);

    ESP_LOGV(TAG, "Update commands queued: %i", command_queue_.size());
}

void PaceBms::set_cell_over_voltage_configuration(PaceBmsV25::CellOverVoltageConfiguration& config) {
    command_item* item = new command_item;
    item->description_ = std::string("setting cell over voltage configuration");
    item->create_request_frame_ = std::bind(&PaceBmsV25::CreateWriteConfigurationRequest, this->pace_bms_v25_, this->address_, config, std::placeholders::_1);
    item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_write_configuration_response, this, std::placeholders::_1);
    command_queue_.push(item);

    ESP_LOGV(TAG, "Update commands queued: %i", command_queue_.size());
}

}  // namespace pace_bms
}  // namespace esphome
