"""Cyclus actions.

This file cannot be Cythonized due to current errors in Cython async
handling. -- scopatz 2017-01-02, for more information see.
see https://github.com/cython/cython/issues/1573
"""
from __future__ import unicode_literals, print_function
import time
import json
from functools import wraps
from collections.abc import Set, Sequence

from cyclus.system import asyncio
from cyclus import lib
from cyclus.jsoncpp import FastWriter


def action(f):
    """Decorator for declaring async functions as actions."""
    @wraps(f)
    def dec(*args, **kwargs):
        async def bound():
            rtn = await f(*args, **kwargs)
            return rtn
        bound.__name__ = f.__name__
        bound.__qualname__ = f.__name__
        return bound
    return dec


async def send_message(state, event, params=None, data='null'):
    """Formats and puts a message on the send queue.

    Parameters
    ----------
    state : SimState
        The state to put the message in
    event : str
        The name of event to send.
    params : dict or None, optional
        A params dict that the event was called with. This will be
        converted to JSON.
    data : str, optional
        The payload to send, this should already be in JSON form.
    """
    params = json.dumps(params)
    message = ('{{"event":"{event}",'
               '"params":{params},'
               '"data":{data}}}'
               ).format(event=event, params=params, data=data)
    await state.send_queue.put(message)


@action
async def echo(state, s):
    """Simple asyncronous echo."""
    print(s)
    await send_message(state, "echo", params={"s": s}, data=json.dumps(s))


@action
async def pause(state):
    """Asynchronous pause."""
    task = asyncio.ensure_future(asyncio.sleep(1e100))
    state.tasks['pause'] = task
    await asyncio.wait([task])


@action
async def unpause(state):
    """Cancels and removes the pause action."""
    pause = state.tasks.pop('pause', None)
    if pause is not None:
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


async def send_registry(state):
    """Sends the current value of the registry of the in-memory backend."""
    reg = sorted(state.memory_backend.registry)
    data = json.dumps(reg)
    await send_message(state, "registry", data=data)


@action
async def send_registry_action(state):
    await send_registry(state)


@action
async def register_tables(state, tables):
    """Add table names to the in-memory backend registry. The lone
    argument here may either be a str (single table), or a set or sequence
    of strings (many tables) to add.
    """
    tables = ensure_tables(tables)
    curr = state.memory_backend.registry
    state.memory_backend.registry = curr | tables
    await send_registry()


@action
async def deregister_tables(state, tables):
    """Remove table names to the in-memory backend registry. The lone
    argument here may either be a str (single table), or a set or sequence
    of strings (many tables) to add.
    """
    tables = ensure_tables(tables)
    curr = state.memory_backend.registry
    state.memory_backend.registry = curr - tables
    await send_registry()


@action
async def send_table_names(state):
    """Send the table names from the file-system backend."""
    names = sorted(state.file_backend.tables)
    data = json.dumps(names)
    await send_message(state, "table_names", data=data)


def table_data_as_json(state, table, conds, orient):
    """Obtains table data as a JSON string."""
    df = state.memory_backend.query(table, conds=conds)
    if df is None:
        data = '"{} is not available."'.format(table)
    else:
        data = df.to_json(default_handler=str, orient=orient)
    return data


@action
async def send_table_data(state, table, conds=None, orient='split'):
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
    """
    # get the table data in another thread, just in case.
    task = state.loop.run_in_executor(state.executor, table_data_as_json,
                                      state, table, conds, orient)
    await asyncio.wait([task])
    data = task.result()
    params = {'table': table, 'conds': conds, 'orient': orient}
    await send_message(state, "table_data", params=params, data=data)


@action
async def sleep(state, n):
    """Asynchronously sleeps for n seconds."""
    await asyncio.sleep(n)


@action
async def load(state):
    """Loads the simulation from the input file and starts executing it.
    """
    # load sim in another thread, just in case.
    task = state.loop.run_in_executor(state.executor, state.load)
    await asyncio.wait([task])
    params = {'status': 'ok'}
    await send_message(state, "loaded", params=params)


def agent_annotations_as_json(state, spec):
    """Returns the agent annotations as a JSON string."""
    if state.si is None:
        rec = lib.Recorder()
        ti = lib.Timer()
        ctx = lib.Context(ti, rec)
    else:
        ctx = state.si.context
    agent = lib.DynamicModule.make(ctx, spec)
    anno = agent.annotations
    writer = FastWriter()
    data = writer.write(anno)
    ctx.del_agent(agent)
    return data


@action
async def agent_annotations(state, spec):
    """Returns agent annotations for a given spec.

    Parameters
    ----------
    spec : str
        The agent specification, e.g. ":agents:NullRegion"
    """
    task = state.loop.run_in_executor(state.executor, agent_annotations_as_json,
                                      state, spec)
    await asyncio.wait([task])
    data = task.result()
    params = {'spec': spec}
    await send_message(state, "agent_annotations", params=params, data=data)


@action
async def shutdown(state, when="empty"):
    """Shuts down the server.

    Parameters
    ----------
    when : str, optional
        When to shutdown the server. Options are "empty" for when the action
        queue is empty, and "now" for right now. Default is empty.
    """
    if when == 'empty':
        await asyncio.sleep(0.1)
        while state.action_queue.qsize() > 1:
            await asyncio.sleep(0.001)
    state.loop.call_soon_threadsafe(state.loop.stop)
