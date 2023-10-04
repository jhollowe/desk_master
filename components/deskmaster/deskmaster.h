#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace deskmaster {
#define HEIGHT_MAX_DIFF 15
#define RX_INTERVAL_TIMEOUT 200

// TODO change to switch
#define PASSTHROUGH true

enum DMOperation : uint8_t {
  DM_OPERATION_IDLE = 0,
  DM_OPERATION_RAISING,
  DM_OPERATION_LOWERING,
};

const char *dm_operation_to_str(DMOperation op);

class DeskMaster: public Component, public sensor::Sensor, public uart::UARTDevice {
  public:
    float get_setup_priority() const override { return setup_priority::LATE; }
    void setup() override;
    void loop() override;
    void dump_config() override;

    // general settings setters
    void set_stopping_distance(int distance) { this->stopping_distance_ = distance; }
    void set_timeout(int timeout) { this->timeout_ = timeout; }
    void set_hw_uart(boolean hw_uart) { this->uart_rx_handler = hw_uart ? &DeskMaster::read_uart : &DeskMaster::read_uart_sw; }

    // Sensor setters
    void set_height_sensor(sensor::Sensor *sensor) { this->height_sensor_ = sensor; }

    // controller pin setters
    void set_up_pin(GPIOPin *pin) { this->up_pin_ = pin; }
    void set_down_pin(GPIOPin *pin) { this->down_pin_ = pin; }
    void set_request_pin(GPIOPin *pin) { this->request_pin_ = pin; }

    void move_to(int height);
    void stop();

    DMOperation current_operation{DM_OPERATION_IDLE};

  protected:
    void send_height(uint16_t height);
    void read_uart();
    void read_uart_sw();
    void passthrough_buttons();

    void (DeskMaster::*uart_rx_handler)(); // function pointer which is either read_uart or read_uart_sw

    // sensors
    sensor::Sensor *height_sensor_{nullptr};

    // controller pin
    GPIOPin *up_pin_{nullptr};
    GPIOPin *down_pin_{nullptr};
    GPIOPin *mode_pin_{nullptr};
    GPIOPin *preset_pin_{nullptr};
    GPIOPin *request_pin_{nullptr};

    // settings and state variables
    int stopping_distance_;
    int current_pos_{0};
    int target_pos_{-1};
    int timeout_{-1};
    uint64_t start_time_;
    uint64_t request_time_{0};

  private:
    void log_data(uint8_t* data);
};

}  // namespace deskmaster
}  // namespace esphome
