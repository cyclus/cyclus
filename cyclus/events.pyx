from __future__ import unicode_literals, print_function

from cyclus.lazyasd import lazyobject
from cyclus.system import curio, QUEUE


# A list of actions that should be added to the queue at the start of
# each timestep, when the loop() is called.
REPEATING_ACTIONS = []


def loop():
    """Adds tasks to the queue"""
    if curio is None:
        return
    for action in REPEATING_ACTIONS:
        QUEUE.put(action)
    QUEUE.join()


async def action_consumer():
    staged_tasks = []
    while True:
        while not QUEUE.empty():
            action = await QUEUE.get()
            action_task = await curio.spawn(action())
            staged_tasks.append(action_task)
            await QUEUE.task_done()
        else:
            for task in staged_tasks:
                await task.join()
            staged_tasks.clear()
        await curio.sleep(0.1)


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

