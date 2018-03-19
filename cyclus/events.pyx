"""Events module for interfacing with the main time step loop of a Cyclus simulation."""
from __future__ import unicode_literals, print_function
from cpython.exc cimport PyErr_CheckSignals
import time
import json
from functools import wraps

import cyclus.system
from cyclus.system import asyncio

# A SimState instance representing the current simuation.
STATE = None


def loop():
    """Adds tasks to the queue"""
    if STATE is None:
        return
    PyErr_CheckSignals()
    STATE.rec.flush()
    for action in STATE.repeating_actions:
        if callable(action):
            args = ()
        else:
            action, params = action[0], action[1]
            if isinstance(action, str):
                action = EVENT_ACTIONS[action]
        STATE.action_queue.put(action(STATE, **params))
    while 'pause' in STATE.tasks or not STATE.action_queue.empty():
        PyErr_CheckSignals()
        time.sleep(STATE.frequency)


#
# Set up event actions dict
#

EVENT_ACTIONS = {}
MONITOR_ACTIONS = {}

if cyclus.system.PY_VERSION_TUPLE >= (3, 5, 0):
    from cyclus import actions
    EVENT_ACTIONS.update(
        agent_annotations=actions.agent_annotations,
        deregister_tables=actions.deregister_tables,
        echo=actions.echo,
        load=actions.load,
        pause=actions.pause,
        register_tables=actions.register_tables,
        registry_request=actions.send_registry_action,
        sleep=actions.sleep,
        table_data=actions.send_table_data,
        table_names_request=actions.send_table_names,
        )
    MONITOR_ACTIONS.update(
        shutdown=actions.shutdown,
        unpause=actions.unpause,
        )
    del actions
