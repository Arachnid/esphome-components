#include "4in8out.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m5stack {

enum FourIn8OutRegisters {
  FOURIN8OUT_INPUT0 = 0x10,
  FOURIN8OUT_OUTPUT0 = 0x20,
  FOURIN8OUT_FIRMWARE_VERSION = 0xFE,
  FOURIN8OUT_ADDRESS = 0xFF,
};

static const char *const TAG = "m5stack-FourIn8Out";

void FourIn8OutComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up m5stack FourIn8Out...");
  // Test to see if device exists
  uint8_t value;
  if (!this->read_register_(FOURIN8OUT_ADDRESS, &value)) {
    ESP_LOGE(TAG, "FourIn8Out not available under 0x%02X", this->address_);
    this->mark_failed();
    return;
  }

  // Read current register states
  for(uint i = 0; i < 4; i++) {
    this->read_register_(FOURIN8OUT_INPUT0 + i, &this->inputs[i]);
  }
  for(uint i = 0; i < 8; i++) {
    this->read_register_(FOURIN8OUT_OUTPUT0 + i, &this->outputs[i]);
  }

  ESP_LOGD(TAG, "Initialization complete. Warning: %d, Error: %d", this->status_has_warning(),
           this->status_has_error());
}

void FourIn8OutComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "FourIn8Out:");
  LOG_I2C_DEVICE(this)
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with FourIn8Out failed!");
  }
}

bool FourIn8OutComponent::digital_read(uint8_t pin) {
  uint8_t value = 0;
  if(pin < 4) {
    this->read_register_(FOURIN8OUT_INPUT0 + pin, &value);
  } else {
    this->read_register(FOURIN8OUT_OUTPUT0 + (pin - 4), &value);
  }
  return value;
}

void FourIn8OutComponent::digital_write(uint8_t pin, bool value) {
  if(pin < 4) {
    ESP_LOGE("Cannot write to a FourIn8Out input pin");
  } else {
    this->write_register_(FOURIN8OUT_OUTPUT0 + (pin - 4), value);
  }
}

void FourIn8OutComponent::pin_mode(uint8_t pin, gpio::Flags flags) {
  if (flags == gpio::FLAG_INPUT && pin >= 4) {
    ESP_LOGE("FourIn8Out pin %d is output only", pin);
  } else if(flags == gpio::FLAG_OUTPUT && pin < 4) {
    ESP_LOGE("FourIn8Out pin %d is input only", pin);
  }
}

bool FourIn8OutComponent::read_register_(uint8_t reg, uint8_t *value) {
  if (this->is_failed()) {
    ESP_LOGD(TAG, "Device marked failed");
    return false;
  }

  if ((this->last_error_ = this->read_register(reg, value, 1, true)) != esphome::i2c::ERROR_OK) {
    this->status_set_warning();
    ESP_LOGE(TAG, "read_register_(): I2C I/O error: %d", (int) this->last_error_);
    return false;
  }

  this->status_clear_warning();
  return true;
}

bool FourIn8OutComponent::write_register_(uint8_t reg, uint8_t value) {
  if (this->is_failed()) {
    ESP_LOGD(TAG, "Device marked failed");
    return false;
  }

  if ((this->last_error_ = this->write_register(reg, &value, 1, true)) != esphome::i2c::ERROR_OK) {
    this->status_set_warning();
    ESP_LOGE(TAG, "write_register_(): I2C I/O error: %d", (int) this->last_error_);
    return false;
  }

  this->status_clear_warning();
  return true;
}

float FourIn8OutComponent::get_setup_priority() const { return setup_priority::BUS; }

void FourIn8OutGPIOPin::setup() { pin_mode(flags_); }
void FourIn8OutGPIOPin::pin_mode(gpio::Flags flags) { this->parent_->pin_mode(this->pin_, flags); }
bool FourIn8OutGPIOPin::digital_read() { return this->parent_->digital_read(this->pin_) != this->inverted_; }
void FourIn8OutGPIOPin::digital_write(bool value) { this->parent_->digital_write(this->pin_, value != this->inverted_); }
std::string FourIn8OutGPIOPin::dump_summary() const {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%u via FourIn8Out", pin_);
  return buffer;
}

}  // namespace m5stack
}  // namespace esphome