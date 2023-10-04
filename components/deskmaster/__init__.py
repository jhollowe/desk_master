import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor, uart
from esphome.const import CONF_HEIGHT, CONF_ID, CONF_TIMEOUT, ICON_GAUGE

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor"]

dm_ns = cg.esphome_ns.namespace("deskmaster")

DM = dm_ns.class_("DeskMaster", cg.Component, uart.UARTDevice)

CONF_UP = "up"
CONF_DOWN = "down"
CONF_MODE = "mode"
CONF_PRESET = "preset"
CONF_REQUEST = "request"
CONF_STOPPING_DISTANCE = "stopping_distance"
CONF_HW_UART = "hw_uart"

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend(
    {
        # general settings
        cv.GenerateID(): cv.declare_id(DM),
        cv.Optional(CONF_HW_UART, default=True): cv.boolean,
        cv.Optional(CONF_STOPPING_DISTANCE, default=15): cv.positive_int,
        cv.Optional(CONF_TIMEOUT): cv.time_period,
        # Output (controller) pins
        cv.Optional(CONF_UP): pins.gpio_output_pin_schema,
        cv.Optional(CONF_DOWN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_REQUEST): pins.gpio_output_pin_schema,
        # Sensors
        cv.Optional(CONF_HEIGHT): sensor.sensor_schema(
            icon=ICON_GAUGE, accuracy_decimals=0, unit_of_measurement="mm"
        ),
        cv.Optional(CONF_STOPPING_DISTANCE, default=15): cv.positive_int,
        cv.Optional(CONF_TIMEOUT): cv.time_period,
    }
).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    # PINS
    if CONF_UP in config:
        pin = await cg.gpio_pin_expression(config[CONF_UP])
        cg.add(var.set_up_pin(pin))
    if CONF_DOWN in config:
        pin = await cg.gpio_pin_expression(config[CONF_DOWN])
        cg.add(var.set_down_pin(pin))
    if CONF_MODE in config:
        pin = await cg.gpio_pin_expression(config[CONF_MODE])
        cg.add(var.set_mode_pin(pin))
    if CONF_PRESET in config:
        pin = await cg.gpio_pin_expression(config[CONF_PRESET])
        cg.add(var.set_preset_pin(pin))
    if CONF_REQUEST in config:
        pin = await cg.gpio_pin_expression(config[CONF_REQUEST])
        cg.add(var.set_request_pin(pin))

    # SENSORS
    if CONF_HEIGHT in config:
        sens = await sensor.new_sensor(config[CONF_HEIGHT])
        cg.add(var.set_height_sensor(sens))

    # GENERAL SETTINGS
    cg.add(var.set_stopping_distance(config[CONF_STOPPING_DISTANCE]))
    if CONF_TIMEOUT in config:
        cg.add(var.set_timeout(config[CONF_TIMEOUT].total_milliseconds))
