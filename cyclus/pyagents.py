"""A collection of agents written in Python"""
from __future__ import unicode_literals, print_function

from cyclus.agents import Region, Institution, Facility
from cyclus import typesystem as ts

class NullRegion(Region):
    """A simple do nothing region."""
