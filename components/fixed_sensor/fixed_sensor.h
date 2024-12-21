#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace fixed_sensor {

class FixedSensor : public sensor::Sensor, public PollingComponent {
 public:
  FixedSensor() = default;

  void setup() override {
    // This will be called by App.setup()
  }
  
  void update() override {
    // This will be called every 60 seconds
    publish_state(10);
  }
};

}  // namespace fixed_sensor
}  // namespace esphome