#include "2relay.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m5stack {

enum TwoRelayRegisters : uint8_t {
  TwoRelay_RELAY0 = 0x00,
  TwoRelay_FIRMWARE_VERSION = 0xFE,
  TwoRelay_ADDRESS = 0xFF,
};

static const char *const TAG = "m5stack-TwoRelay";

void TwoRelayComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up m5stack TwoRelay...");
  // Test to see if device exists
  uint8_t value;
  if (!this->read_register_(TwoRelay_ADDRESS, &value)) {
    ESP_LOGE(TAG, "TwoRelay not available under 0x%02X", this->address_);
    this->mark_failed();
    return;
  }

  ESP_LOGD(TAG, "Initialization complete. Warning: %d, Error: %d", this->status_has_warning(),
           this->status_has_error());
}

void TwoRelayComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "TwoRelay:");
  LOG_I2C_DEVICE(this)
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with TwoRelay failed!");
  }
}

bool TwoRelayComponent::digital_read(uint8_t pin) {
  uint8_t value = 0;
  this->read_register_(TwoRelay_RELAY0 + pin, &value);
  return value;
}

void TwoRelayComponent::digital_write(uint8_t pin, bool value) {
  this->write_register_(TwoRelay_RELAY0 + pin, value);
}

void TwoRelayComponent::pin_mode(uint8_t pin, gpio::Flags flags) {
}

bool TwoRelayComponent::read_register_(uint8_t reg, uint8_t *value) {
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

bool TwoRelayComponent::write_register_(uint8_t reg, uint8_t value) {
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

float TwoRelayComponent::get_setup_priority() const { return setup_priority::BUS; }

void TwoRelayGPIOPin::setup() { pin_mode(flags_); }
void TwoRelayGPIOPin::pin_mode(gpio::Flags flags) { this->parent_->pin_mode(this->pin_, flags); }
bool TwoRelayGPIOPin::digital_read() { return this->parent_->digital_read(this->pin_) != this->inverted_; }
void TwoRelayGPIOPin::digital_write(bool value) { this->parent_->digital_write(this->pin_, value != this->inverted_); }
std::string TwoRelayGPIOPin::dump_summary() const {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%u via TwoRelay", pin_);
  return buffer;
}

}  // namespace m5stack
}  // namespace esphome