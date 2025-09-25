#pragma once

#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome {
namespace electrolux_eacm {

// Values for Electrolux EACM-12DR/N3
const uint8_t EACM_ADDRESS = 72;

// Temperature
const uint8_t EACM_TEMP_MIN = 16;          // Celsius
const uint8_t EACM_TEMP_MAX = 32;          // Celsius
const uint8_t EACM_TEMP_OFFSET_COOL = 16;  // Celsius

// Modes
const uint8_t EACM_MODE_COOL = 0b00010000;
const uint8_t EACM_MODE_DRY = 0b01000000;
const uint8_t EACM_MODE_FAN = 0b10000000;

// Fan Speed
const uint8_t EACM_FAN_HIGH = 0b1000;
const uint8_t EACM_FAN_MEDIUM = 0b0100;
const uint8_t EACM_FAN_LOW = 0b0010;

//
const uint8_t EACM_SWITCH_OFF = 0b00010010;
const uint8_t EACM_SWITCH_ON = 0b10000000;

// IR Transmission - similar to NEC1
const uint32_t EACM_IR_FREQUENCY = 38000;
const uint32_t EACM_HEADER_MARK = 9000;
const uint32_t EACM_HEADER_SPACE = 4500;
const uint32_t EACM_BIT_MARK = 465;
const uint32_t EACM_ONE_SPACE = 1750;
const uint32_t EACM_ZERO_SPACE = 670;

// State Frame size
const uint8_t EACM_STATE_FRAME_SIZE = 4;

class ElectroluxEACMClimate : public climate_ir::ClimateIR {
 public:
  ElectroluxEACMClimate()
      : climate_ir::ClimateIR(EACM_TEMP_MIN, EACM_TEMP_MAX, 1.0f, true, true,
                              {climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
                               climate::CLIMATE_FAN_HIGH}) {}

 protected:
  // Transmit via IR the state of this climate controller.
  void transmit_state() override;
  uint8_t operation_mode_();
  uint8_t fan_speed_();
  uint8_t temperature_();
  uint8_t on_off_();
  // Handle received IR Buffer
  bool on_receive(remote_base::RemoteReceiveData data) override;
  bool parse_state_frame_(const uint8_t frame[]);
};

}  // namespace electrolux_eacm
}  // namespace esphome
