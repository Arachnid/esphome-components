import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import i2c
from esphome.const import (
    CONF_ID,
)

AUTO_LOAD = ["output", "select"]
CODEOWNERS = ["@arachnid"]
DEPENDENCIES = ["i2c"]
MULTI_CONF = True
M5Stack_ns = cg.esphome_ns.namespace("m5stack")

HBridgeComponent = M5Stack_ns.class_("HBridgeComponent", cg.Component, i2c.I2CDevice)

CONF_HBRIDGE = "hbridge"
CONFIG_SCHEMA = (
    cv.Schema({cv.Required(CONF_ID): cv.declare_id(HBridgeComponent)})
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x20))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config) 
    return var
