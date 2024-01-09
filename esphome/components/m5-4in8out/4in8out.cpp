#include "4in8out.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m5stack {

enum 4In8OutRegisters {
  4IN8OUT_INPUT0 = 0x10,
  4IN8OUT_OUTPUT0 = 0x20,
  4IN8OUT_FIRMWARE_VERSION = 0xFE,
  4IN8OUT_ADDRESS = 0xFF,
};

static const char *const TAG = "m5stack-4in8out";

void 4In8OutComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up m5stack 4in8out...");
  // Test to see if device exists
  uint8_t value;
  if (!this->read_register_(4IN8OUT_ADDRESS, &value)) {
    ESP_LOGE(TAG, "4in8out not available under 0x%02X", this->address_);
    this->mark_failed();
    return;
  }

  // Read current register states
  for(uint i = 0; i < 4; i++) {
    this->read_register_(4IN8OUT_INPUT0 + i, &this->inputs[i]);
  }
  for(uint i = 0; i < 8; i++) {
    this->read_register_(4IN8OUT_OUTPUT0 + i, &this->outputs[i]);
  }

  ESP_LOGD(TAG, "Initialization complete. Warning: %d, Error: %d", this->status_has_warning(),
           this->status_has_error());
}

void 4In8OutComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "4in8out:");
  LOG_I2C_DEVICE(this)
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with 4in8out failed!");
  }
}

bool 4In8OutComponent::digital_read(uint8_t pin) {
  uint8_t value = 0;
  if(pin < 4) {
    this->read_register_(4IN8OUT_INPUT0 + pin, &value);
  } else {
    this->read_register(4IN8OUT_OUTPUT0 + (pin - 4), &value);
  }
  return value;
}

void 4In8OutComponent::digital_write(uint8_t pin, bool value) {
  if(pin < 4) {
    ESP_LOGE("Cannot write to a 4in8out input pin");
  } else {
    this->write_register_(4IN8OUT_OUTPUT0 + (pin - 4), value);
  }
}

void 4In8OutComponent::pin_mode(uint8_t pin, gpio::Flags flags) {
  if (flags == gpio::FLAG_INPUT && pin >= 4) {
    ESP_LOGE("4in8out pin %d is output only", pin);
  } else if(flags == gpio::FLAG_OUTPUT && pin < 4) {
    ESP_LOGE("4in8out pin %d is input only", pin);
  }
}

bool 4In8OutComponent::read_register_(uint8_t reg, uint8_t *value) {
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

bool 4In8OutComponent::write_register_(uint8_t reg, uint8_t value) {
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

float 4In8OutComponent::get_setup_priority() const { return setup_priority::BUS; }

void 4In8OutGPIOPin::setup() { pin_mode(flags_); }
void 4In8OutGPIOPin::pin_mode(gpio::Flags flags) { this->parent_->pin_mode(this->pin_, flags); }
bool 4In8OutGPIOPin::digital_read() { return this->parent_->digital_read(this->pin_) != this->inverted_; }
void 4In8OutGPIOPin::digital_write(bool value) { this->parent_->digital_write(this->pin_, value != this->inverted_); }
std::string 4In8OutGPIOPin::dump_summary() const {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%u via 4in8out", pin_);
  return buffer;
}

}  // namespace m5stack
}  // namespace esphome