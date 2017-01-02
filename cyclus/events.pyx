from __future__ import unicode_literals, print_function
import time
import json
from functools import wraps
from collections.abc import Set, Sequence

from cyclus.lazyasd import lazyobject
from cyclus.system import asyncio

# A SimState instance representing the current simuation.
STATE = None


def loop():
    """Adds tasks to the queue"""
    if STATE is None:
        return
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


from cyclus.actions import echo, sleep

EVENT_ACTIONS = {
    "echo": echo,
    #"registry_request": send_registry_action,
    "sleep": sleep,
    }
