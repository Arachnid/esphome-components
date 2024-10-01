#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/output/float_output.h"
#include <map>

namespace esphome {
namespace m5stack {

enum Direction: uint8_t {
    DIRECTION_Stop = 0,
    DIRECTION_Forward = 1,
    DIRECTION_Back = 2
}

static const std::map<std::string, Direction> DIRECTION_STRING_TO_ENUM{
    {"Stop", DIRECTION_Stop}, {"Forward", DIRECTION_Forward}, {"Back", DIRECTION_Back}};

class HBridgeComponent : public Component, public i2c::I2CDevice {
 public:
  HBridgeComponent() = default;

  /// Check i2c availability and setup masks
  void setup() override;

  /// Helper function to write speed
  bool set_speed(float value);
  void set_direction(Direction dir);

  float get_setup_priority() const override;

  void dump_config() override;

 protected:
  bool read_register_(uint8_t reg, uint8_t *value);
  bool write_register_(uint8_t reg, uint8_t value);

  /// Storage for last I2C error seen
  esphome::i2c::ErrorCode last_error_;

  uint8_t duty_;
  Direction dir_;
};

/// Helper class to expose an m5stack 4in8out pin as an internal GPIO pin.
class HBridgeOutput : public output::FloatOutput, public Parented<HBridgeComponent> {
 public:
 protected:
  void write_state(float state) override;
};

class HBridgeDirection : public Component, public select::Select, public Parented<HBridgeComponent> {
 public:
  HBridgeDirection() = default;

 protected:
  void control(const std:: string &value) override;
}

}  // namespace m5stack
}  // namespace esphome