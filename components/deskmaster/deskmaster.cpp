#include "deskmaster.h"
#include "esphome/core/log.h"

namespace esphome {
namespace deskmaster {

static const char *TAG = "deskmaster";

const char *dM_operation_to_str(DMOperation op) {
  switch (op) {
    case DM_OPERATION_IDLE:
      return "IDLE";
    case DM_OPERATION_RAISING:
      return "RAISING";
    case DM_OPERATION_LOWERING:
      return "LOWERING";
    default:
      return "UNKNOWN";
  }
}

void DeskMaster::setup() {
  if (this->up_pin_ != nullptr)
    this->up_pin_->digital_write(false);
  if (this->down_pin_ != nullptr)
    this->down_pin_->digital_write(false);
  if (this->request_pin_ != nullptr) {
    this->request_pin_->digital_write(true);
    this->request_time_ = millis();
  }
  // if (this->hw_serial_ != nullptr )
  if (false) // TODO fix this
    this->uart_rx_handler = &DeskMaster::read_uart;
  else
    this->uart_rx_handler = &DeskMaster::read_uart_sw;

  // TODO bounce the request pin (if present) to get the current height
}

void DeskMaster::loop() {

  // call the correct UART receive handler based on if this is using HW or SW UART
  // SW provides limited functionality since it is less reliable and more has to be done to sanitize the data
  (this->*uart_rx_handler)();

  if (this->target_pos_ >= 0) {
    if (abs(this->target_pos_ - this->current_pos_) < this->stopping_distance_)
      this->stop();
    if ((this->timeout_ >= 0) && (millis() - this->start_time_ >= this->timeout_))
      this->stop();
  }

  if ((this->request_time_ > 0) && (millis() - this->request_time_ >= 100)) {
    this->request_pin_->digital_write(false);
    this->request_time_ = 0;
  }
}

void DeskMaster::dump_config() {
  ESP_LOGCONFIG(TAG, "DeskMaster desk:");
  LOG_SENSOR("", "Height", this->height_sensor_);
  LOG_PIN("Up pin: ", this->up_pin_);
  LOG_PIN("Down pin: ", this->down_pin_);
  LOG_PIN("Request pin: ", this->request_pin_);
}

void DeskMaster::move_to(int target_pos) {
  if (abs(target_pos - this->current_pos_) < this->stopping_distance_)
    return;
  if (target_pos > this->current_pos_) {
    if (this->up_pin_ == nullptr)
      return;
    this->up_pin_->digital_write(true);
    this->current_operation = DM_OPERATION_RAISING;
  } else {
    if (this->down_pin_ == nullptr)
      return;
    this->down_pin_->digital_write(true);
    this->current_operation = DM_OPERATION_LOWERING;
  }
  this->target_pos_ = target_pos;
  if (this->timeout_ >= 0)
    this->start_time_ = millis();
}

void DeskMaster::stop() {
  this->target_pos_ = -1;
  if (this->up_pin_ != nullptr)
    this->up_pin_->digital_write(false);
  if (this->down_pin_ != nullptr)
    this->down_pin_->digital_write(false);
  // TODO clear preset2 button
  this->current_operation = DM_OPERATION_IDLE;
}

void DeskMaster::send_height(uint16_t height){
  // TODO fix compiler warning
  uint8_t data[] = {1,1,(height & 0xff00) >> 8, height & 0xff};
  this->write_array(data, 4);
}

void DeskMaster::read_uart(){
  // TODO implement a hardware-only read handler
  this->read_uart_sw();
  return;

  static uint8_t data_index = 0; // value will remain between 0 and 3 inclusive
  static uint8_t uart_data[] = {0,0,0,0};

  while (this->available()) {
    this->read_byte(&(uart_data[data_index]));

    if (PASSTHROUGH) {
      this->write_byte(uart_data[data_index]);
    }

    // TODO implement a hardware-only read handler

    data_index = (data_index + 1) % 4;
  }
}

/// @brief Reads height data from the UART using the ESP8266's software UART.
// This is done to support pin configuration which do not use the HW UART's pins.
// The SW UART supports 5v RX while the HW UART requires 3.3v RX
// Since the SW UART is less reliable, it only supports reading the height
void DeskMaster::read_uart_sw(){
  static uint8_t data_index = 0; // value will remain between 0 and 3 inclusive
  static uint8_t high_byte;

  while (this->available()) {
    uint8_t curr;
    // read the next byte from UART and place in the current index
    this->read_byte(&curr);

    if (PASSTHROUGH) {
      this->write_byte(curr);
    }

    // ESP_LOGD(TAG, "%d: 0b" BYTE_TO_BINARY_PATTERN " (0x%02X)", data_index, BYTE_TO_BINARY(uart_data[data_index]), uart_data[data_index]);

    // switch needed to help validate and align the data to the 4-byte data packet
    // since SW UART can miss some bits/bytes
    switch (data_index) {
      case 0:
      case 1:
        if (curr == 1)
          data_index++;
        else
          data_index = 0;
        break;
      case 2:
        high_byte = curr;
        data_index = 3;
        break;
      case 3:
        uint16_t height = (high_byte << 8) + curr;
        // only update the height if it has changed
        if (this->current_pos_ != height) {
          // filter out erroneous heights (too large of a delta to be possible)
          if (this->current_pos_ == 0 || (this->current_pos_ - height < HEIGHT_MAX_DIFF && height - this->current_pos_ < HEIGHT_MAX_DIFF)){
            this->current_pos_ = height;
            if (this->height_sensor_ != nullptr)
              this->height_sensor_->publish_state(height);
          } else {
            ESP_LOGD(TAG, "bad height of %d, ignoring", height);
          }
        }
        data_index = 0;
        break;
    }
  }
}

}  // namespace deskmaster
}  // namespace esphome
