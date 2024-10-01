import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import (
    CONF_ID,
    ENTITY_CATEGORY_CONFIG,
)
from . import HBridgeComponent, M5Stack_ns

DEPENDENCIES = ["i2c", "m5-hbridge"]

CONF_HBRIDGE_ID = "hbridge_id"
CONF_DIRECTION = "direction"
CONF_SELECTS = [
    "Stop",
    "Forward",
    "Reverse",
]

HBridgeDirection = M5Stack_ns.class_(
    "HBridgeDirection", cg.Component
)

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_HBRIDGE_ID): cv.use_id(HBridgeComponent),
    cv.Required(CONF_DIRECTION): select.select_schema(
        HBridgeDirection,
        entity_category=ENTITY_CATEGORY_CONFIG
    )
}


async def to_code(config):
    parent = await cg.get_variable(config[CONF_HBRIDGE_ID])
    if direction_config := config.get(CONF_DIRECTION):
        sel = await select.new_select(
            direction_config,
            options=[CONF_SELECTS],
        )
        await cg.register_parented(sel, config[CONF_HBRIDGE_ID])
        cg.add(parent.set_direction_select(sel))
