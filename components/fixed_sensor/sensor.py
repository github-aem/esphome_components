import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, UNIT_EMPTY, ICON_EMPTY

DEPENDENCIES = []
AUTO_LOAD = ["sensor"]

fixed_sensor_ns = cg.esphome_ns.namespace("fixed_sensor")
FixedSensor = fixed_sensor_ns.class_("FixedSensor", sensor.Sensor, cg.PollingComponent)

CONFIG_SCHEMA = sensor.sensor_schema(
    FixedSensor,
    unit_of_measurement=UNIT_EMPTY,
    icon=ICON_EMPTY,
    accuracy_decimals=0,
).extend(cv.polling_component_schema("60s"))

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)