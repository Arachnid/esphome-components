import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import (
    CONF_ID,
)
from . import HBridgeComponent, M5Stack_ns

DEPENDENCIES = ["output", "m5-hbridge"]

CONF_HBRIDGE_ID = "hbridge_id"

HBridgeOutput = M5Stack_ns.class_(
    "HBridgeOutput", output.FloatOutput
)

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend(
    {
        cv.Required(CONF_ID): cv.declare_id(HBridgeOutput),
        cv.GenerateID(CONF_HBRIDGE_ID): cv.use_id(HBridgeComponent),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    parent = await cg.get_variable(config[CONF_HBRIDGE_ID])
    
    await cg.register_parented(var, config[CONF_HBRIDGE_ID])
    await output.register_output(var, config)
    return var
