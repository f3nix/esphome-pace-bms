#pragma once

#include "esphome/core/component.h"

#include "pace_bms_datetime_implementation.h"
#include "esphome/components/pace_bms/pace_bms_component.h"

namespace esphome {
namespace pace_bms {

class PaceBmsDatetime : public Component {
public:
	void set_parent(PaceBms* parent) { parent_ = parent; }

	void set_system_date_and_time_datetime(PaceBmsDatetimeImplementation* datetime) { this->system_date_and_time_datetime_ = datetime; }

	void setup() override;
	float get_setup_priority() const { return setup_priority::DATA; }
	void dump_config() override;

protected:
	pace_bms::PaceBms* parent_;

	PaceBmsProtocolV25::DateTime system_date_and_time_;
	bool system_date_and_time_seen_{ false };
	pace_bms::PaceBmsDatetimeImplementation* system_date_and_time_datetime_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome



