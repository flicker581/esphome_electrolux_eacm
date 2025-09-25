import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.const import CONF_ID

AUTO_LOAD = ["climate_ir"]

eacm_ns = cg.esphome_ns.namespace("electrolux_eacm")
EACMClimate = eacm_ns.class_("ElectroluxEACMClimate", climate_ir.ClimateIR)

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(EACMClimate)

async def to_code(config):
    await climate_ir.new_climate_ir(config)
