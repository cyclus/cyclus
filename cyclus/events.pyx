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
        STATE.action_queue.put(action(*args))
    while 'pause' in STATE.tasks or not STATE.action_queue.empty():
        time.sleep(STATE.frequency)


async def action_consumer():
    staged_tasks = []
    while True:
        while not STATE.action_queue.empty():
            action = STATE.action_queue.get()
            print("getting", action)
            action_task = asyncio.ensure_future(action())
            staged_tasks.append(action_task)
        else:
            if len(staged_tasks) > 0:
                await asyncio.wait(staged_tasks)
                staged_tasks.clear()
        await asyncio.sleep(STATE.frequency)


def action(f):
    """Decorator for declaring async functions as actions."""
    @wraps
    def dec(*args, **kwargs):
        async def bound():
            rtn = await f(*args, **kwargs)
            return rtn
        return bound
    return dec


@action
async def echo(s):
    """Simple asyncronous echo."""
    print(s)


@action
async def pause():
    """Asynchronous pause."""
    task = await asyncio.sleep(1e100)
    STATE.tasks['pause'] = task


@action
async def unpause():
    """Cancels and removes the pause action."""
    pause = STATE.tasks.pop('pause', None)
    pause.cancel()


def ensure_tables(tables):
    """Ensures that the input is a set of strings suitable for use as
    table names.
    """
    if isinstance(tables, Set):
        pass
    elif isinstance(tables, str):
        tables = frozenset([tables])
    elif isinstance(tables, Sequence):
        tables = frozenset(tables)
    else:
        raise ValueError('cannot register tables because it has the wrong '
                         'type: {}'.format(type(tables)))
    return tables


async def send_registry():
    """Sends the current value of the registry of the in-memory backend."""
    reg = list(STATE.memory_backend.registry)
    data = json.dumps(reg)
    message = ('{"event":"registry",'
               '"params":{},'
               '"data":' + data + '}'
               )
    await STATE.send_queue.put(message)


@action
async def register_tables(tables):
    """Add table names to the in-memory backend registry. The lone
    argument here may either be a str (single table), or a set or sequence
    of strings (many tables) to add.
    """
    tables = ensure_tables(tables)
    curr = STATE.memory_backend.registry
    STATE.memory_backend.registry = curr | tables
    await send_registry()


@action
async def deregister_tables(tables):
    """Remove table names to the in-memory backend registry. The lone
    argument here may either be a str (single table), or a set or sequence
    of strings (many tables) to add.
    """
    tables = ensure_tables(tables)
    curr = STATE.memory_backend.registry
    STATE.memory_backend.registry = curr - tables
    await send_registry()


@action
async def send_table(table, conds=None, orient='split'):
    """Sends all table data in JSON format.

    Parameters
    ----------
    table : str
        The name of the table to send
    conds : list of str or None, optional
        The query conditions for the table. See the queryable backend for
        more information.
    orient : str, optional
        The orientation of the JSON representation of the data. See
        the pandas.DataFrame.to_json() method documentation for more
        information.

    Notes
    -----
    This action adds a message to the send queue with the following
    form::

        {"event": "tabledata",
         "params": {...}
         "data": {...}
         }

    """
    df = STATE.memory_backend.query(table, conds=conds)
    if df is None:
        data = '"{} is not available."'.format(table)
    else:
        data = df.to_json(default_handler=str, orient=orient)
    params = {'table': table, 'conds': conds, 'orient': orient}
    params = json.dumps(params)
    message = ('{"event":"tabledata",'
               '"params":' + params + ','
               '"data":' + data + '}'
               )
    await STATE.send_queue.put(message)


@action
async def sleep(n):
    """Asynchronously sleeps for n seconds."""
    await asyncio.sleep(n)
