from cyclus.agents import Facility
from cyclus import lib

class DummyPowerRecorder(Facility):

    def tick(self):
        lib.record_time_series('POWER', self, 10.)
        print("TICK")


def echo_power(agent, time, value):
    print("The power is {0}".format(value))

lib.TIME_SERIES_LISTENERS["Power"].append(echo_power)
