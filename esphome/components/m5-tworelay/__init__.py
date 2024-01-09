import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import i2c
from esphome.const import (
    CONF_ID,
    CONF_NUMBER,
    CONF_MODE,
    CONF_INVERTED,
    CONF_OUTPUT,
)

CODEOWNERS = ["@arachnid"]
DEPENDENCIES = ["i2c"]
MULTI_CONF = True
M5Stack_ns = cg.esphome_ns.namespace("m5stack")

TwoRelayComponent = M5Stack_ns.class_("TwoRelayComponent", cg.Component, i2c.I2CDevice)
TwoRelayGPIOPin = M5Stack_ns.class_(
    "TwoRelayGPIOPin", cg.GPIOPin, cg.Parented.template(TwoRelayComponent)
)

CONF_TWORELAY = "tworelay"
CONFIG_SCHEMA = (
    cv.Schema({cv.Required(CONF_ID): cv.declare_id(TwoRelayComponent)})
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x26))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)


def validate_mode(value):
    if not (value[CONF_OUTPUT]):
        raise cv.Invalid("Mode must be output")
    return value


TWORELAY_PIN_SCHEMA = cv.All(
    {
        cv.GenerateID(): cv.declare_id(TwoRelayGPIOPin),
        cv.Required(CONF_TWORELAY): cv.use_id(TwoRelayComponent),
        cv.Required(CONF_NUMBER): cv.int_range(min=0, max=2),
        cv.Optional(CONF_MODE, default={}): cv.All(
            {
                cv.Optional(CONF_OUTPUT, default=False): cv.boolean,
            },
            validate_mode,
        ),
        cv.Optional(CONF_INVERTED, default=False): cv.boolean,
    }
)


@pins.PIN_SCHEMA_REGISTRY.register(CONF_TWORELAY, TWORELAY_PIN_SCHEMA)
async def TwoRelay_pin_to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    parent = await cg.get_variable(config[CONF_TWORELAY])

    cg.add(var.set_parent(parent))

    num = config[CONF_NUMBER]
    cg.add(var.set_pin(num))
    cg.add(var.set_inverted(config[CONF_INVERTED]))
    cg.add(var.set_flags(pins.gpio_flags_expr(config[CONF_MODE])))
    return var
