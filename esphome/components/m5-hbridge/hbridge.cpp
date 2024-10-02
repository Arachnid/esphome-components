#include "hbridge.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m5stack {

enum HBridgeRegisters : uint8_t {
  HBridge_DRIVER_CONFIG = 0x0,
  HBridge_VIN_ADC = 0x10,
  HBridge_VIN_ADC_12BIT = 0x20,
  HBridge_VIN_Current = 0x30,
  HBridge_FIRMWARE_VERSION = 0xFE,
  HBridge_ADDRESS = 0xFF,
};

static const char *const TAG = "m5stack-HBridge";

void HBridgeComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up m5stack HBridge...");
  // Test to see if device exists
  uint8_t value;
  if ((this->last_error_ = this->read_register(HBridge_ADDRESS, &value, 1, true)) != esphome::i2c::ERROR_OK) {
    ESP_LOGE(TAG, "HBridge not available under 0x%02X", this->address_);
    this->mark_failed();
    return;
  }

  ESP_LOGD(TAG, "Initialization complete. Warning: %d, Error: %d", this->status_has_warning(),
           this->status_has_error());
}

void HBridgeComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "HBridge:");
  LOG_I2C_DEVICE(this)
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with HBridge failed!");
  }
}

bool HBridgeComponent::set_speed(float value) {
    duty_ = (uint16_t)(value * 65535);
    return this->write_driver_config_();
}

bool HBridgeComponent::set_direction(Direction dir) {
    dir_ = dir;
    return this->write_driver_config_();
}

bool HBridgeComponent::set_frequency(uint16_t freq) {
    frequency_ = freq;
    return this->write_driver_config_();
}

bool HBridgeComponent::write_driver_config_() {
  if (this->is_failed()) {
    ESP_LOGD(TAG, "Device marked failed");
    return false;
  }

  ESP_LOGD(TAG, "Setting motor to %d %s", duty_, ENUM_TO_DIRECTION_STRING[dir_].c_str());

  uint8_t value[] = {
    dir_,
    0,
    (uint8_t)(duty_ % 256),
    (uint8_t)(duty_ / 256),
    (uint8_t)(frequency_ % 256),
    (uint8_t)(frequency_ / 256)
  };
  if ((this->last_error_ = this->write_register(HBridge_DRIVER_CONFIG, value, 6, true)) != esphome::i2c::ERROR_OK) {
    this->status_set_warning();
    ESP_LOGE(TAG, "write_register_(): I2C I/O error: %d", (int) this->last_error_);
    return false;
  }

  this->status_clear_warning();
  return true;
}

float HBridgeComponent::get_setup_priority() const { return setup_priority::BUS; }

void HBridgeOutput::write_state(float state) {
    this->parent_->set_speed(state);
}

void HBridgeDirection::control(const std::string &value) {
    this->publish_state(value);
    this->parent_->set_direction(DIRECTION_STRING_TO_ENUM.at(value));
}

}  // namespace m5stack
}  // namespace esphome