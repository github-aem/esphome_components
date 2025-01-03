import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

CONF_HOST = "host"
CONF_PORT = "port"
CONF_UPDATE_INTERVAL = "update_interval"

DEPENDENCIES = ['ethernet']
AUTO_LOAD = ['sensor']

# Create the namespace for our component
CODEOWNERS = ['@github-aem']
modbus_tcp_ns = cg.esphome_ns.namespace('modbus_tcp')
ModbusTcpClient = modbus_tcp_ns.class_('ModbusTcpClient', cg.Component)

# Define the configuration schema
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ModbusTcpClient),
    cv.Required(CONF_HOST): cv.string,
    cv.Optional(CONF_PORT, default=502): cv.port,
    cv.Optional(CONF_UPDATE_INTERVAL, default='5s'): cv.positive_time_period_milliseconds,
}).extend(cv.COMPONENT_SCHEMA)

# Define how to translate the configuration into code
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    cg.add(var.set_host(config[CONF_HOST]))
    cg.add(var.set_port(config[CONF_PORT]))
    cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))