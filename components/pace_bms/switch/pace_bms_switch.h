#pragma once

#include "esphome/core/component.h"

#include "esphome/components/pace_bms/pace_bms_component.h"

#include "pace_bms_switch_implementation.h"

#include "../pace_bms_protocol_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsSwitch : public Component {
public:
	void set_parent(PaceBms* parent) { parent_ = parent; }

	// current switch states are actually in the status information response, so subscribe to that in order to update switch state
	void set_buzzer_alarm_switch(PaceBmsSwitchImplementation* sw) { this->buzzer_alarm_switch_ = sw; }
	void set_led_alarm_switch(PaceBmsSwitchImplementation* sw) { this->led_alarm_switch_ = sw; }
	void set_charge_current_limiter_switch(PaceBmsSwitchImplementation* sw) { this->charge_current_limiter_switch_ = sw; }
	void set_charge_mosfet_switch(PaceBmsSwitchImplementation* sw) { this->charge_mosfet_switch_ = sw; }
	void set_discharge_mosfet_switch(PaceBmsSwitchImplementation* sw) { this->discharge_mosfet_switch_ = sw; }

	void setup() override;
	float get_setup_priority() const override { return setup_priority::DATA; };
	void dump_config() override;

protected:
	pace_bms::PaceBms* parent_;

	pace_bms::PaceBmsSwitchImplementation* buzzer_alarm_switch_{ nullptr };
	pace_bms::PaceBmsSwitchImplementation* led_alarm_switch_{ nullptr };
	pace_bms::PaceBmsSwitchImplementation* charge_current_limiter_switch_{ nullptr };
	pace_bms::PaceBmsSwitchImplementation* charge_mosfet_switch_{ nullptr };
	pace_bms::PaceBmsSwitchImplementation* discharge_mosfet_switch_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome

