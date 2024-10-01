import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import (
    CONF_ID,
    ENTITY_CATEGORY_CONFIG,
)
from . import HBridgeComponent, M5Stack_ns

DEPENDENCIES = ["m5-hbridge"]

CONF_HBRIDGE_ID = "hbridge_id"
CONF_DIRECTION = "direction"
CONF_SELECTS = [
    "Stop",
    "Forward",
    "Back",
]

HBridgeDirection = M5Stack_ns.class_(
    "HBridgeDirection", cg.Component
)

CONFIG_SCHEMA = select.select_schema(HBridgeDirection, entity_category=ENTITY_CATEGORY_CONFIG).extend({
    cv.GenerateID(CONF_HBRIDGE_ID): cv.use_id(HBridgeComponent),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_HBRIDGE_ID])
    sel = await select.new_select(config, options=[CONF_SELECTS])
    await cg.register_parented(sel, config[CONF_HBRIDGE_ID])
    return sel
