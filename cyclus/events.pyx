from __future__ import unicode_literals, print_function
import time
from collections.abc import Set, Sequence

from cyclus.lazyasd import lazyobject
from cyclus.system import asyncio, QUEUE

# The default amount of time algorithms should sleep for.
FREQUENCY = 0.001

# A list of actions that should be added to the queue at the start of
# each timestep, when the loop() is called.
REPEATING_ACTIONS = []

# A SimState instance representing the current simuation.
STATE = None


def loop():
    """Adds tasks to the queue"""
    if STATE is None:
        return
    for action in REPEATING_ACTIONS:
        print("putting", action)
        if callable(action):
            args = ()
        else:
            action, args = action[0], action[1:]
        QUEUE.put(action(*args))
    while 'pause' in STATE.tasks or not QUEUE.empty():
        time.sleep(FREQUENCY)


async def action_consumer():
    staged_tasks = []
    while True:
        while not QUEUE.empty():
            action = QUEUE.get()
            print("getting", action)
            action_task = asyncio.ensure_future(action())
            staged_tasks.append(action_task)
        else:
            if len(staged_tasks) > 0:
                await asyncio.wait(staged_tasks)
                staged_tasks.clear()
        await asyncio.sleep(FREQUENCY)


def action(f):
    """Decorator for declaring async functions as actions."""
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


@action
async def register_tables(tables):
    """Add table names to the in-memory backend registry. The lone
    argument here may either be a str (single table), or a set or sequence
    of strings (many tables) to add.
    """
    tables = ensure_tables(tables)
    curr = STATE.memory_backend.registry
    STATE.memory_backend.registry = curr | tables


@action
async def deregister_tables(tables):
    """Remove table names to the in-memory backend registry. The lone
    argument here may either be a str (single table), or a set or sequence
    of strings (many tables) to add.
    """
    tables = ensure_tables(tables)
    curr = STATE.memory_backend.registry
    STATE.memory_backend.registry = curr - tables


@action
async def send_table(table):
    """Sends all table data in JSON format."""
    #sock = STATE.sock
    #if sock is None:
    #    address = (host, port)
    #    sock = curio.socket.socket(curio.socket.AF_INET, curio.socket.SOCK_STREAM)
    #    sock.setsockopt(curio.socket.SOL_SOCKET, curio.socket.SO_REUSEADDR, 1)
    #    sock.bind(address)
    #    sock.listen(5)
    #    STATE.sock = sock
    #print('Server listening at', address)
    df = STATE.memory_backend.query(table)
    if df is None:
        data = '"{} is not available."'.format(table)
    else:
        data = df.to_json()
    print("Sending data: " + data)
    #put_task = asyncio.ensure_future(STATE.send_queue.put(data))
    #await asyncio.wait([put_task])
    STATE.send_queue.put_nowait(data)
    print("Sent data: " + data)
    #await STATE.send_queue.join()
    print("sending queue size: ", STATE.send_queue.qsize())
    #async with sock:
    #    client, addr = await sock.accept()
    #    await client.sendall(data)


@action
async def sleep(n=FREQUENCY):
    """Asynchronously sleeps for n seconds."""
    await asyncio.sleep(n)
