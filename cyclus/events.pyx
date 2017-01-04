from __future__ import unicode_literals, print_function
import time
import json
from functools import wraps
from collections.abc import Set, Sequence

import cyclus.system
from cyclus.lazyasd import lazyobject
from cyclus.system import asyncio

# A SimState instance representing the current simuation.
STATE = None


def loop():
    """Adds tasks to the queue"""
    if STATE is None:
        return
    STATE.rec.flush()
    for action in STATE.repeating_actions:
        print("putting", action)
        if callable(action):
            args = ()
        else:
            action, args = action[0], action[1:]
            if isinstance(action, str):
                action = EVENT_ACTIONS[action]
        STATE.action_queue.put(action(STATE, *args))
    while 'pause' in STATE.tasks or not STATE.action_queue.empty():
        time.sleep(STATE.frequency)


#
# Set up event actions dict
#

EVENT_ACTIONS = {}
MONITOR_ACTIONS = {}

if cyclus.system.PY_VERSION_TUPLE >= (3, 5, 0):
    from cyclus import actions
    EVENT_ACTIONS.update(
        echo=actions.echo,
        pause=actions.pause,
        registry_request=actions.send_registry_action,
        sleep=actions.sleep,
        table_data=actions.send_table_data,
        table_names_request=actions.send_table_names,
        )
    MONITOR_ACTIONS.update(
        unpause=actions.unpause,
        )
    del actions
