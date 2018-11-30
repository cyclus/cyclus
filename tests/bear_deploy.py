"""Basic archetypes for testing the deployment of python facilities from python institutions"""
import random

from cyclus import lib
import cyclus.typesystem as ts
from cyclus.agents import Institution, Facility


class NOInst(Institution):
    """
    This institution deploys facilities based on demand curves using
    Non Optimizing (NO) methods.
    """

    prototypes = ts.VectorString(
        doc = "A list of prototypes that the institution will draw upon to fit" +
              "the demand curve",
        tooltip = "List of prototypes the institution can use to meet demand",
        uilabel = "Prototypes",
        uitype = "oneOrMore"
    )

    growth_rate = ts.Double(
        doc = "This value represents the growth rate that the institution is " +
              "attempting to meet.",
        tooltip = "Growth rate of growth commodity",
        uilabel = "Growth Rate"
    )

    growth_commod = ts.String(
        doc = "The commodity this institution will be monitoring for demand growth. " +
              "The default value of this field is electric power.",
        tooltip = "Growth commodity",
        uilabel = "Growth Commodity"
    )

    initial_demand = ts.Double(
        doc = "The initial power of the facility",
        tooltip = "Initital demand",
        uilabel = "Initial demand"
    )

    #The supply of a commodity
    commodity_supply = {}

    def enter_notify(self):
        super().enter_notify()
        lib.TIME_SERIES_LISTENERS[self.growth_commod].append(self.extract_supply)
    
    def tick(self):
        """
        #Method for the deployment of facilities.
        """
        time = self.context.time
        if time is 0:
            return
        print(self.commodity_supply[time-1], self.demand_calc(time))
        if self.commodity_supply[time-1] < self.demand_calc(time):
            proto = random.choice(self.prototypes)
            print("New fac: " + proto)
            print(self.kind)
            self.context.schedule_build(self, proto)


    def extract_supply(self, agent, time, value, commod):
        """
        Gather information on the available supply of a commodity over the
        lifetime of the simulation.
        """
        if time in self.commodity_supply:
            self.commodity_supply[time] += value
        else:
            self.commodity_supply[time] = value


    def demand_calc(self, time):
        """
        Calculate the electrical demand at a given timestep (time).
        Parameters
        ----------
        time : int
            The timestep that the demand will be calculated at.
        """
        timestep = self.context.dt / 86400 / 28
        demand = self.initial_demand * ((1.0+self.growth_rate)**(time/timestep))
        return demand


class DemandFac(Facility):
    """
    This institution deploys facilities based on demand curves using
    Non Optimizing (NO) methods.
    """

    production_rate_min = ts.Double(
        doc = "The minimum rate at which this facility produces it's commodity. ",
        tooltip = "The minimum rate at which this facility produces its product.",
        uilabel = "Min Production"
    )


    production_rate_max = ts.Double(
        doc = "The maximum rate at which this facility produces it's commodity.",
        tooltip = "The maximum rate at which this facility produces its product.",
        uilabel = "Max Production"
    )

    commodity = ts.String(
        doc = "",
        tooltip = "",
        uilabel = ""
    )

    def tick(self):
        rate = random.uniform(self.production_rate_min, self.production_rate_max)
        print("Agent {0} {1} {2}".format(self.id, self.production_rate_min, rate))
        lib.record_time_series(self.commodity, self, rate)
