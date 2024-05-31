"""A collection of agents written in Python"""
from __future__ import unicode_literals, print_function

from cyclus.agents import Region, Institution, Facility
from cyclus import typesystem as ts
from cyclus.lib import CY_LARGE_DOUBLE

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
        doc="Name of recipe to request. If empty, sink requests material no "
            "particular composition.",
        default="",
        uilabel="Input Recipe",
        uitype="recipe",
        )
    max_inv_size = ts.Double(
        default=CY_LARGE_DOUBLE(),
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
    inventory = ts.ResBufProduct()

    def get_material_requests(self):
        if len(self.recipe) == 0:
            comp = {}
        else:
            comp = self.context.get_recipe(self.recipe)
        mat = ts.Material.create_untracked(self.capacity, comp)
        port = {"commodities": [{c: mat} for c in self.in_commods],
                "constraints": self.capacity}
        return port

    def get_product_requests(self):
        prod = ts.Product.create_untracked(self.capacity, "")
        port = {"commodities": [{c: prod} for c in self.in_commods],
                "constraints": self.capacity}
        return port

    def accept_material_trades(self, responses):
        for mat in responses.values():
            self.inventory.push(mat)

    def accept_product_trades(self, responses):
        for prod in responses.values():
            self.inventory.push(prod)


class Source(Facility):
    """A minimum implementation source facility that provides a commodity with
    a given capacity.
    """
    commod = ts.String(
        doc="commodity that the source facility supplies",
        tooltip="source commodity",
        schematype="token",
        uilabel="Commodity",
        uitype="outcommodity",
        )
    recipe_name = ts.String(
        doc="Recipe name for source facility's commodity. "
            "If empty, source supplies material with requested compositions.",
        tooltip="commodity recipe name",
        schematype="token",
        default="",
        uilabel="Recipe",
        uitype="recipe",
        )
    capacity = ts.Double(
        doc="amount of commodity that can be supplied at each time step",
        uilabel="Maximum Throughput",
        tooltip="source capacity",
        )

    def build(self, parent):
        super(Source, self).build(parent)
        if self.lifetime >= 0:
            self.context.schedule_decom(self, self.exit_time)

    def get_material_bids(self, requests):
        reqs = requests.get(self.commod, None)
        if not reqs:
            return
        if len(self.recipe_name) == 0:
            bids = [req for req in reqs]
        else:
            recipe_comp = self.context.get_recipe(self.recipe_name)
            bids = []
            for req in reqs:
                qty = min(req.target.quantity, self.capacity)
                mat = ts.Material.create_untracked(qty, recipe_comp)
                bids.append({'request': req, 'offer': mat})
        return {'bids': bids, 'constraints': self.capacity}

    def get_material_trades(self, trades):
        responses = {}
        if len(self.recipe_name) == 0:
            for trade in trades:
                mat = ts.Material.create(self, trade.amt, trade.request.target.comp())
                responses[trade] = mat
        else:
            recipe_comp = self.context.get_recipe(self.recipe_name)
            for trade in trades:
                mat = ts.Material.create(self, trade.amt, recipe_comp)
                responses[trade] = mat
        return responses
