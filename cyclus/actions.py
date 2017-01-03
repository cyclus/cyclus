from __future__ import unicode_literals, print_function
import time
import json
"""Cyclus actions.

This file cannot be Cythonized due to current errors in Cython async
handling. -- scopatz 2017-01-02, for more information see.
see https://github.com/cython/cython/issues/1573
"""

from functools import wraps
from collections.abc import Set, Sequence

from cyclus.lazyasd import lazyobject
from cyclus.system import asyncio


def action(f):
    """Decorator for declaring async functions as actions."""
    @wraps(f)
    def dec(*args, **kwargs):
        async def bound():
            print("args", args)
            print("kw", kwargs)
            rtn = await f(*args, **kwargs)
            print("return", rtn)
            return rtn
        bound.__name__ = f.__name__
        bound.__qualname__ = f.__name__
        return bound
    return dec


@action
async def echo(state, s):
    """Simple asyncronous echo."""
    print(s)
    params = json.dumps({"s": s})
    data = json.dumps(s)
    message = ('{"event":"echo",'
               '"params":' + params + ','
               '"data":' + data + '}'
               )
    print("echo message", message)
    await state.send_queue.put(message)
    print("put echo message")
    return "returning from echo with " + message


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
async def send_registry_action():
    await send_registry()


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
async def send_table(state, table, conds=None, orient='split'):
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
    df = state.memory_backend.query(table, conds=conds)
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
    await state.send_queue.put(message)


@action
async def sleep(state, n):
    """Asynchronously sleeps for n seconds."""
    await asyncio.sleep(n)


