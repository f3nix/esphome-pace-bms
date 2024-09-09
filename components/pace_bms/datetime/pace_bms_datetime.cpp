#include "pace_bms_datetime.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.datetime";

/*
* wire up all the lambda callbacks
*/
void PaceBmsDatetime::setup() {
	if (this->system_date_and_time_datetime_ != nullptr) {
		this->parent_->register_system_datetime_callback([this](PaceBmsV25::DateTime & dt) {
			this->system_date_and_time_ = dt;
			this->system_date_and_time_seen_ = true;
			ESP_LOGV(TAG, "'bms_date_and_time': Publishing state due to update from the hardware: %04i:%02i:%02i %02i:%02i:%02i", dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second);
			this->system_date_and_time_datetime_->set_datetime(dt);
			this->system_date_and_time_datetime_->publish_state();
		});
	}
	if (this->system_date_and_time_datetime_ != nullptr) {
		this->system_date_and_time_datetime_->add_on_control_callback([this](const datetime::DateTimeCall& value) {
			if (!this->system_date_and_time_seen_) {
				ESP_LOGE(TAG, "bms_date_and_time cannot be set because the BMS hasn't responded to a get system time request");
				return;
			}
			ESP_LOGD(TAG, "Setting system_date_and_time user selected value %04i:%02i:%02i %02i:%02i:%02i", value.get_year().value(), value.get_month().value(), value.get_day().value(), value.get_hour().value(), value.get_minute().value(), value.get_second().value());
			this->system_date_and_time_.Year = value.get_year().value();
			this->system_date_and_time_.Month = value.get_month().value();
			this->system_date_and_time_.Day = value.get_day().value();
			this->system_date_and_time_.Hour = value.get_hour().value();
			this->system_date_and_time_.Minute = value.get_minute().value();
			this->system_date_and_time_.Second = value.get_second().value();
			this->parent_->set_system_datetime(this->system_date_and_time_);
		});
	}
}

void PaceBmsDatetime::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_datetime:");
	LOG_DATETIME_DATETIME("  ", "System Date and Time", this->system_date_and_time_datetime_);
}

}  // namespace pace_bms
}  // namespace esphome





