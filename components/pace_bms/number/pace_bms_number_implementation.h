#pragma once

#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace pace_bms {

class PaceBmsNumberImplementation : public Component, public number::Number {
 public:
  float get_setup_priority() const override;

  void add_on_control_callback(std::function<void(bool)>&& callback);

protected:
  void control(float value) override;

  CallbackManager<void(float)> control_callbacks_{};
};

}  // namespace pace_bms
}  // namespace esphome
