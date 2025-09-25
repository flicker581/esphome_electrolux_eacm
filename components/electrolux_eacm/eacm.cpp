#include "eacm.h"
#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace electrolux_eacm {

static const char *const TAG = "electrolux_eacm.climate";

void ElectroluxEACMClimate::transmit_state() {
  uint8_t remote_state[EACM_STATE_FRAME_SIZE] = {0};
  remote_state[0] = EACM_ADDRESS;
  remote_state[1] = this->operation_mode_();
  remote_state[1] |= this->fan_speed_();
  remote_state[2] = this->on_off_();
  remote_state[3] = this->temperature_();

  auto transmit = this->transmitter_->transmit();
  auto *data = transmit.get_data();
  data->set_carrier_frequency(EACM_IR_FREQUENCY);

  data->mark(EACM_HEADER_MARK);
  data->space(EACM_HEADER_SPACE);
  for (unsigned char b : remote_state) {
    for (uint8_t mask = 1; mask > 0; mask <<= 1) {  // iterate through bit mask
      data->mark(EACM_BIT_MARK);
      bool bit = b & mask;
      data->space(bit ? EACM_ONE_SPACE : EACM_ZERO_SPACE);
    }
  }
  data->mark(EACM_BIT_MARK);
  data->space(0);

  transmit.perform();
}

uint8_t ElectroluxEACMClimate::operation_mode_() {
  uint8_t operating_mode;
  switch (this->mode) {
    case climate::CLIMATE_MODE_COOL:
      operating_mode = EACM_MODE_COOL;
      break;
    case climate::CLIMATE_MODE_DRY:
      operating_mode = EACM_MODE_DRY;
      break;
    case climate::CLIMATE_MODE_FAN_ONLY:
      operating_mode = EACM_MODE_FAN;
      break;
    case climate::CLIMATE_MODE_OFF:
    default:
      operating_mode = EACM_MODE_FAN;
      break;
  }
  return operating_mode;
}

uint8_t ElectroluxEACMClimate::fan_speed_() {
  uint8_t fan_speed;
  switch (this->fan_mode.value()) {
    case climate::CLIMATE_FAN_LOW:
      fan_speed = EACM_FAN_LOW;
      break;
    case climate::CLIMATE_FAN_MEDIUM:
      fan_speed = EACM_FAN_MEDIUM;
      break;
    case climate::CLIMATE_FAN_HIGH:
      fan_speed = EACM_FAN_HIGH;
      break;
    case climate::CLIMATE_FAN_AUTO:
    default:
      fan_speed = EACM_FAN_LOW;
  }
  return fan_speed;
}

uint8_t ElectroluxEACMClimate::on_off_() {
  uint8_t on_off;
  switch (this->mode) {
    case climate::CLIMATE_MODE_OFF:
      on_off = EACM_SWITCH_OFF;
      break;
    default:
      on_off = EACM_SWITCH_ON;
  }
  return on_off;
}

uint8_t ElectroluxEACMClimate::temperature_() {
  // Force special temperatures depending on the mode
  uint8_t temperature;
  temperature = (uint8_t) roundf(this->target_temperature) - EACM_TEMP_OFFSET_COOL;
  return temperature;
}

bool ElectroluxEACMClimate::parse_state_frame_(const uint8_t frame[]) {
  uint8_t on_off = frame[2];
  uint8_t mode = frame[1] & 0xF0;

  if (on_off & EACM_SWITCH_ON) {
    switch (mode) {
      case EACM_MODE_COOL:
        this->mode = climate::CLIMATE_MODE_COOL;
        break;
      case EACM_MODE_DRY:
        this->mode = climate::CLIMATE_MODE_DRY;
        break;
      case EACM_MODE_FAN:
        this->mode = climate::CLIMATE_MODE_FAN_ONLY;
        break;
    }
  } else {
    this->mode = climate::CLIMATE_MODE_OFF;
  }
  uint8_t temperature = frame[3];
  this->target_temperature = temperature + EACM_TEMP_OFFSET_COOL;
  uint8_t fan_mode = frame[1] & 0x0F;
  switch (fan_mode) {
    case EACM_FAN_LOW:
      this->fan_mode = climate::CLIMATE_FAN_LOW;
      break;
    case EACM_FAN_MEDIUM:
      this->fan_mode = climate::CLIMATE_FAN_MEDIUM;
      break;
    case EACM_FAN_HIGH:
      this->fan_mode = climate::CLIMATE_FAN_HIGH;
      break;
  }
  this->publish_state();
  return true;
}

bool ElectroluxEACMClimate::on_receive(remote_base::RemoteReceiveData data) {
  uint8_t state_frame[EACM_STATE_FRAME_SIZE] = {};
  if (!data.expect_item(EACM_HEADER_MARK, EACM_HEADER_SPACE)) {
    return false;
  }
  for (uint8_t pos = 0; pos < EACM_STATE_FRAME_SIZE; pos++) {
    uint8_t byte = 0;
    for (int8_t bit = 0; bit < 8; bit++) {
      if (data.expect_item(EACM_BIT_MARK, EACM_ONE_SPACE)) {
        byte |= 1 << bit;
      } else if (!data.expect_item(EACM_BIT_MARK, EACM_ZERO_SPACE)) {
        return false;
      }
    }
    state_frame[pos] = byte;
    if (pos == 0) {
      // frame header
      if (byte != EACM_ADDRESS) {
        return false;
      }
    }
  }
  return this->parse_state_frame_(state_frame);
}

}  // namespace electrolux_eacm
}  // namespace esphome
