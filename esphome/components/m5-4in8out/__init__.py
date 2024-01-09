import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import i2c
from esphome.const import (
    CONF_ID,
    CONF_INPUT,
    CONF_NUMBER,
    CONF_MODE,
    CONF_INVERTED,
    CONF_OUTPUT,
    CONF_PULLUP,
)

CODEOWNERS = ["@arachnid"]
DEPENDENCIES = ["i2c"]
MULTI_CONF = True
FourIn8Out_ns = cg.esphome_ns.namespace("4in8out")

FourIn8OutComponent = FourIn8Out_ns.class_("4In8OutComponent", cg.Component, i2c.I2CDevice)
FourIn8OutGPIOPin = FourIn8Out_ns.class_(
    "4In8OutGPIOPin", cg.GPIOPin, cg.Parented.template(FourIn8OutComponent)
)

CONF_4In8Out = "4in8out"
CONFIG_SCHEMA = (
    cv.Schema({cv.Required(CONF_ID): cv.declare_id(FourIn8OutComponent)})
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x45))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)


def validate_mode(value):
    if not (value[CONF_INPUT] or value[CONF_OUTPUT]):
        raise cv.Invalid("Mode must be either input or output")
    if value[CONF_INPUT] and value[CONF_OUTPUT]:
        raise cv.Invalid("Mode must be either input or output")
    return value


FourIn8Out_PIN_SCHEMA = cv.All(
    {
        cv.GenerateID(): cv.declare_id(FourIn8OutGPIOPin),
        cv.Required(CONF_4In8Out): cv.use_id(FourIn8OutComponent),
        cv.Required(CONF_NUMBER): cv.int_range(min=0, max=12),
        cv.Optional(CONF_MODE, default={}): cv.All(
            {
                cv.Optional(CONF_INPUT, default=False): cv.boolean,
                cv.Optional(CONF_OUTPUT, default=False): cv.boolean,
            },
            validate_mode,
        ),
        cv.Optional(CONF_INVERTED, default=False): cv.boolean,
    }
)


@pins.PIN_SCHEMA_REGISTRY.register(CONF_4In8Out, FourIn8Out_PIN_SCHEMA)
async def FourIn8Out_pin_to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    parent = await cg.get_variable(config[CONF_4In8Out])

    cg.add(var.set_parent(parent))

    num = config[CONF_NUMBER]
    cg.add(var.set_pin(num))
    cg.add(var.set_inverted(config[CONF_INVERTED]))
    cg.add(var.set_flags(pins.gpio_flags_expr(config[CONF_MODE])))
    return var
