"""A collection of agents written in Python"""
from __future__ import unicode_literals, print_function

from cyclus.agents import Region, Institution, Facility
from cyclus import typesystem as ts

class NullRegion(Region):
    """A simple do nothing region."""


class NullInst(Institution):
    """An instition that owns facilities in the simulation but exhibits
    null behavior. No parameters are given when using the null institution.
    """

class Sink(Facility):
    """This sink facility accepts specified amount of commodity."""
    in_commods = ts.VectorString(
        doc="commodities that the sink facility accepts.",
        tooltip="input commodities for the sink",
        uilabel="List of Input Commodities",
        uitype=["oneormore", "incommodity"],
        )
    recipe = ts.String(
        tooltip="input/request recipe name",
        doc="Name of recipe to request. If empty, sink requests material no
            "particular composition.",
        default="",
        uilabel="Input Recipe",
        uitype="recipe",
        )
    max_inv_size = ts.Double(
        default=1e299,
        doc="total maximum inventory size of sink facility",
        uilabel= "Maximum Inventory",
        tooltip="sink maximum inventory size",
        )
    capacity = ts.Double(
        doc="capacity the sink facility can accept at each time step",
        uilabel="Maximum Throughput",
        tooltip="sink capacity",
        default=100.0,
        )
    inventory = ts.ResourceBuff(capacity='max_inv_size')
