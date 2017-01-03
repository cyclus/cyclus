"""An asyncronous cyclus server that provides as JSON API over websockets.

The webserver has a number of 'events' that it may send or recieve. These
in turn affect how a cyclus simulation runs.

The server, which operates both asynchronously and in parallel, has three
top-level tasks which it manages:

* The cyclus simulation object, run on a separate thread,
* A action consumer, which executes actions that have been queued either
  by the cyclus simulation or the user,
* A websockets server that sends and recieves JSON-formatted events from
  the client.

For purposes of this document, the following terminology is used:

**event:** A JSON object / dictionary that contains behaviour instructions.

**message:** The string form of an event.

**action:** A delayed asynchronous coroutine function that carries out the
behaviour specified in a cooresponding event. These do the actual work of
the event system.

**repeating action:** An action coroutine function (or event name) or a list
of the  coroutine function and arguments that is added to the action queue
each time step of the simulation. This enables pausing each time step,
streaming table data, etc.

**task:** A future object that resulsts from calling an action. See
asyncio for more details.


Events
======
Events are JSON-formatted strings that represent JSON-objects (dicts) at
their top-most level. All events must have an "event" key whose value is
the string name that distinguishes the event from all other kinds of events.

Often times, events may have parameters that they send/recieve. These live
in the "params" key as a JSON object / dict.

Events maybe conceptually divided into server-sent and client-sent events.

Server Events
-------------
Server-sent event are those that the server sends to the client. These are
often repsonse to requests for data about the state of the simulation.
They typically contain a "data" key which holds data about the simulation
state. They may also have a "success" key, whose value is true/false, that
specifies whether the data was able to be computed.

**registry:** The in-memory backend registy value in its current form::

    {"event": "registry",
     "params": null,
     "data": ["table0", "table1", ...]
    }

**table_names:** The current file system backend table names::

    {"event": "table_names",
     "params": null,
     "data": ["table0", "table1", ...]
    }

Client Events
-------------
Client events are often requests originating from users. They may either
express a request for behaviour (pause the simulation, restart, etc.) or
a request for data. These events may or may not have additional parameters,
depending on the type of request.

**registry_request:** A simple reqest for the in-memory backend regsitry::

    {"event": "registry_request"}

**table_names_request:** A simple reqest for the table names present in the
file system backend::

    {"event": "table_names_request"}

"""
import json
from argparse import ArgumentParser

import cyclus.events
from cyclus.system import asyncio, concurrent_futures, websockets
from cyclus.simstate import SimState
from cyclus.actions import sleep
from cyclus.events import EVENT_ACTIONS


def make_parser():
    """Makes the argument parser for the cyclus server."""
    p = ArgumentParser("cyclus", description="Cyclus Server CLI")
    p.add_argument('-o', '--output-path', dest='output_path',
                   default=None, help='output path')
    p.add_argument('input_file', help='path to input file')
    return p


async def run_sim(state, loop, executor):
    """Runs a cyclus simulation in an executor, which should be on another
    thread.
    """
    run_task = loop.run_in_executor(executor, state.run)
    state.tasks['run'] = run_task
    await asyncio.wait([run_task])


async def action_consumer(state):
    staged_tasks = []
    while True:
        while not state.action_queue.empty():
            action = state.action_queue.get()
            print("getting", action)
            #action_task = asyncio.ensure_future(action())
            action_obj = action()
            action_obj.__name__ = action_obj.__qualname__ = action.__name__
            action_task = asyncio.ensure_future(action_obj)
            print("action task")
            staged_tasks.append(action_task)
            print("task appended")
        else:
            #print("len(staged_tasks)", len(staged_tasks))
            if len(staged_tasks) > 0:
                print("awaiting staged tasks.", len(staged_tasks))
                await asyncio.wait(staged_tasks)
                print("print clearing")
                staged_tasks.clear()
        #print("sleeping")
        await asyncio.sleep(state.frequency)


async def get_send_data():
    """Asynchronously grabs the next data to send from the queue."""
    state = cyclus.events.STATE
    print("awaiting sending data")
    data = await state.send_queue.get()
    print("sending data", data)
    return data


async def queue_message_action(message):
    state = cyclus.events.STATE
    print("got message", message)
    event = json.loads(message)
    print("found event", event)
    params = event.get("params", {})
    print("found params", params)
    kind = event["event"]
    print("print kind", kind)
    action = EVENT_ACTIONS[kind]
    print("looked up action", action)
    state.action_queue.put(action(state, **params))
    print("added action to queue")


async def websocket_handler(websocket, path):
    """Sends and recieves data via a websocket."""
    while True:
        recv_task = asyncio.ensure_future(websocket.recv())
        send_task = asyncio.ensure_future(get_send_data())
        done, pending = await asyncio.wait([recv_task, send_task],
                                           return_when=asyncio.FIRST_COMPLETED)
        # handle incoming
        print("len(done)", len(done))
        print("len(pending)", len(pending))
        if recv_task in done:
            print("~~~ got incoming message")
            message = recv_task.result()
            print("prepping to parse message", message)
            await queue_message_action(message)
            print("queued the prepped message")
        else:
            recv_task.cancel()
        # handle sending of data
        if send_task in done:
            message = send_task.result()
            print("sending message", message)
            await websocket.send(message)
            print("message sent")
        else:
            send_task.cancel()


def main(args=None):
    """Main cyclus server entry point."""
    p = make_parser()
    ns = p.parse_args(args=args)
    cyclus.events.STATE = state = SimState(input_file=ns.input_file,
                                           output_path=ns.output_path,
                                           memory_backend=True)
    state.load()

    import logging
    logging.basicConfig(level=logging.DEBUG)
    logger = logging.getLogger('websockets.server')
    logger.setLevel(logging.ERROR)
    logger.addHandler(logging.StreamHandler())

    #QUEUE.put(register_tables("Compositions"))
    state.repeating_actions.append([sleep, 1])
    #state.repeating_actions.append([send_table, "TimeSeriesPower"])
    executor = concurrent_futures.ThreadPoolExecutor(max_workers=16)
    loop = asyncio.get_event_loop()
    loop.set_debug(True)
    server = websockets.serve(websocket_handler, 'localhost', 4242)
    try:
        loop.run_until_complete(asyncio.gather(
            asyncio.ensure_future(run_sim(state, loop, executor)),
            asyncio.ensure_future(action_consumer(state)),
            asyncio.ensure_future(server),
            ))
    finally:
        loop.close()


if __name__ == '__main__':
    main()
