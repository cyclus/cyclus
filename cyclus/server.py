"""An asyncronous cyclus server that provides as JSON API over websockets.

The webserver has a number of 'events' that it may send or recieve. These
in turn affect how a cyclus simulation runs.

The server, which operates both asynchronously and in parallel, has five
top-level tasks which it manages:

* The cyclus simulation object, run on a separate thread,
* A action consumer, which executes actions that have been queued either
  by the cyclus simulation or the user,
* A websockets server that sends and recieves JSON-formatted events from
  the client.
* A heartbeat that sends special events every so often.
* A monitor for presenting data about certain other future actions.

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

**task:** A future object that results from calling an action. See
asyncio for more details.


Events
======
Events are JSON-formatted strings that represent JSON-objects (dicts) at
their top-most level. All events must have an "event" key whose value is
the string name that distinguishes the event from all other kinds of events.

Often times, events may have parameters that they send/recieve. These live
in the "params" key as a JSON object / dict.

Events may be conceptually divided into server-sent, client-sent events, and
bidirectional events which are sent by either the client or the server.

Server Events
-------------
Server-sent events are those that the server sends to the client. These are
often repsonse to requests for data about the state of the simulation.
They typically contain a "data" key which holds data about the simulation
state. They may also have a "success" key, whose value is true/false, that
specifies whether the data was able to be computed.

**heartbeat:** A simple event the lets the client know that the server is
still alive. The data value is the approximate time of the next heartbeat
in seconds::

    {"event": "heartbeat", "data": val}

**loaded:** A simple message that says that a simulation has been loaded.

    {"event": "loaded",
     "params": {"status": "ok"},
     "data": null
    }

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

**deregister_tables:** Remove table names from the in-memory backend registry.
A registry event from the server will follow the completion of this event::

    {"event": "deregister_tables",
     "params": {"tables": ["table0", "table1", ...]}
     }

**load:** Loads the input file in the simulation and starts running the simulation::

    {"event": "load"}

**pause:** Pauses the simulation until it is unpaused::

    {"event": "pause"}

**register_tables:** Add table names to the in-memory backend registry.
A registry event from the server will follow the completion of this event::

    {"event": "register_tables",
     "params": {"tables": ["table0", "table1", ...]}
     }

**registry_request:** A simple reqest for the in-memory backend regsitry::

    {"event": "registry_request"}

**shutdown:** A reqest to shutdown the server::

    {"event": "shutdown",
     "params": {"when": "empty" or "now"}
     }

**table_names_request:** A simple reqest for the table names present in the
file system backend::

    {"event": "table_names_request"}

**unpause:** Unpauses the simulation by canceling the pause task::

    {"event": "unpause"}


Bidirectional Events
--------------------
These are events that may logically originate from either the client or the
server. Certian keys in the event may or not be present depending on the
sender, but the event name stays the same.

**agent_annotations:** This event requests and returns the agent annotations
for a given agent spec. If the data field is null, this is a request to the
server for annotation information. If the data field is a dict, it represents
the annotations for the spec::

    {"event": "agent_annotations",
     "params": {"spec": "<path>:<lib>:<name>"},
     "data": null or object
    }

**echo:** Echos back a single string parameter. When requesting an echo,
the data key need not be present::

    {"event": "table_names",
     "params": {"s": value},
     "data": value
    }

**sleep:** The requester instructs the reciever to sleep for n seconds::

    {"event": "sleep", "params": {"n": value}}

**table_data:** Data about a table with the conditions and other parameters
applied. If the client sends this event without the "data" key, the server
will respond with the requested table::

    {"event": "table_data",
     "params": {"table": "<name of table>",
                "conds": ["<list of condition lists, if any>"],
                "orient": "<orientation of JSON, see Pandas>"},
     "data": {"<keys>": "<values subject to orientation>"}
    }


Command Line Interface
======================
You may launch the cyclus server by running::

    $ python -m cyclus.server input.xml

Most of the arguments are relatively self-explanatory. However, the CLI here
also allows you to load initial and repeating actions. The syntax for this
is an event name followed by parameter tokens (which must contain an equals
sign)::

    $ python -m cyclus.server input.xml --repeating-actions sleep n=1

You may load many actions by repeating the name-params pattern:

    $ python -m cyclus.server input.xml --repeating-actions \
        sleep n=1 \
        table_data table="TimeSeriesPower"

Note that everything right of an equals sign in a parameter token is passed
to Python's eval() builtin function. It thererfore must be valid Python.
For string values, make sure that you properly escape quotation marks as
per the syntax of your shell language.
"""
from __future__ import print_function, unicode_literals
import sys
import json
import socket
import logging
from argparse import ArgumentParser, Action

import cyclus.events
from cyclus.system import asyncio, concurrent_futures, websockets
from cyclus.simstate import SimState
from cyclus.events import EVENT_ACTIONS, MONITOR_ACTIONS


async def run_sim(state, loop, executor):
    """Runs a cyclus simulation in an executor, which should be on another
    thread.
    """
    run_task = loop.run_in_executor(executor, state.run)
    state.tasks['run'] = run_task
    await asyncio.wait([run_task])


async def action_consumer(state):
    """The basic consumer of actions."""
    staged_tasks = []
    while True:
        while not state.action_queue.empty():
            action = state.action_queue.get()
            action_task = asyncio.ensure_future(action())
            staged_tasks.append(action_task)
        else:
            if len(staged_tasks) > 0:
                await asyncio.wait(staged_tasks)
                staged_tasks.clear()
        await asyncio.sleep(state.frequency)


async def action_monitor(state):
    """Consumes actions that have been scheduled for monitoring tasks,
    such as status reporting, sending signals, or canceling other task.
    These are awaited in the order recieved.
    """
    while True:
        while not state.monitor_queue.empty():
            action = state.monitor_queue.get()
            await action()
        await asyncio.sleep(max(state.frequency, 0.05))


async def get_send_data():
    """Asynchronously grabs the next data to send from the queue."""
    state = cyclus.events.STATE
    data = await state.send_queue.get()
    return data


async def queue_message_action(message):
    state = cyclus.events.STATE
    event = json.loads(message)
    params = event.get("params", {})
    kind = event["event"]
    if kind in EVENT_ACTIONS:
        action = EVENT_ACTIONS[kind]
        state.action_queue.put(action(state, **params))
    elif kind in MONITOR_ACTIONS:
        action = MONITOR_ACTIONS[kind]
        state.monitor_queue.put(action(state, **params))
    else:
        raise KeyError(kind + "action could not be found in either"
                       "EVENT_ACTIONS or MONITOR_ACTIONS.")


async def websocket_handler(websocket, path):
    """Sends and recieves data via a websocket."""
    while True:
        recv_task = asyncio.ensure_future(websocket.recv())
        send_task = asyncio.ensure_future(get_send_data())
        done, pending = await asyncio.wait([recv_task, send_task],
                                           return_when=asyncio.FIRST_COMPLETED)
        # handle incoming
        if recv_task in done:
            message = recv_task.result()
            await queue_message_action(message)
        else:
            recv_task.cancel()
        # handle sending of data
        if send_task in done:
            message = send_task.result()
            await websocket.send(message)
        else:
            send_task.cancel()


async def heartbeat(state):
    """This sends a heartbeat event to the client with a nominal period.
    This occurs outside of the normal action-consumer event system. The
    client is then able to detect the lack of a heartbeat and know that the
    server has been disconected.
    """
    message_template = '{{"event": "heartbeat", "data": {f}}}'
    f = state.heartbeat_frequency
    message = message_template.format(f=f)
    while True:
        if state.heartbeat_frequency != f:
            f = state.heartbeat_frequency
            message = message_template.format(f=f)
        await state.send_queue.put(message)
        await asyncio.sleep(f)


class EventCLIAction(Action):
    """A basic class for parsing action specs on the command line."""

    def __init__(self, option_strings, dest, nargs='+', **kwargs):
        super().__init__(option_strings, dest, nargs=nargs, **kwargs)

    def __call__(self, parser, ns, values, option_string=None):
        specs = []
        for tok in values:
            if '=' not in tok:
                # must have new action name
                specs.append((tok, {}))
            else:
                params = specs[-1][1]
                name, _, s = tok.partition('=')
                ctx = {}  # isolate teh evaluation context
                val = eval(s, ctx, ctx)
                params[name] = val
        setattr(ns, self.dest, specs)


def make_parser():
    """Makes the argument parser for the cyclus server."""
    p = ArgumentParser("cyclus", description="Cyclus Server CLI")
    p.add_argument('-o', '--output-path', dest='output_path',
                   default=None, help='output path')
    p.add_argument('--debug', action='store_true', default=False,
                   dest='debug', help="runs the server in debug mode.")
    p.add_argument('--host', dest='host', default='localhost',
                   help='hostname to run the server on')
    p.add_argument('-p', '--port', dest='port', type=int, default=4242,
                   help='port to run the server on')
    p.add_argument('-n', '--nthreads', type=int, dest='nthreads', default=16,
                   help='Maximum number of thread workers to run with.')
    p.add_argument('-r', '--repeating-actions', action=EventCLIAction,
                   dest='repeating_actions', default=(),
                   help='list of repeating actions')
    p.add_argument('-i', '--initial-actions', action=EventCLIAction,
                   dest='initial_actions', default=(),
                   help='list of initial actions to queue')
    p.add_argument('input_file', nargs= '?', default='<no-input-file>', help='path to input file')
    return p


def _start_debug(loop):
    logging.basicConfig(level=logging.DEBUG)
    logger = logging.getLogger('websockets.server')
    logger.setLevel(logging.ERROR)
    logger.addHandler(logging.StreamHandler())
    loop.set_debug(True)


def _find_open_port(host, port, debug=False):
    found = False
    while not found:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.bind((host, port))
        except socket.error as e:
            if debug:
                msg = '[cyclus-server] port {} not available, trying port {}'
                print(msg.format(port, port+1), file=sys.stderr)
            if e.errno == 98:
                port += 1
                continue
            else:
                raise
        finally:
            s.close()
        found = True
    return port


def main(args=None):
    """Main cyclus server entry point."""
    p = make_parser()
    ns = p.parse_args(args=args)
    cyclus.events.STATE = state = SimState(input_file=ns.input_file,
                                           output_path=ns.output_path,
                                           memory_backend=True,
                                           debug=ns.debug)
    # load initial and repeating actions
    for kind, params in ns.initial_actions:
        if kind in EVENT_ACTIONS:
            action = EVENT_ACTIONS[kind]
        else:
            action = MONITOR_ACTIONS[kind]
        # place all initial actions in action queue, even if it is a monitor action.
        # this enables shutdown to happen after all actions when issues from command line.
        state.action_queue.put(action(state, **params))
    state.repeating_actions.extend(ns.repeating_actions)
    # start up tasks
    executor = concurrent_futures.ThreadPoolExecutor(max_workers=ns.nthreads)
    state.executor = executor
    loop = state.loop = asyncio.get_event_loop()
    if ns.debug:
        _start_debug(loop)
    open_port = _find_open_port(ns.host, ns.port, debug=ns.debug)
    if open_port != ns.port:
        msg = "port {} already bound, next available port is {}"
        print(msg.format(ns.port, open_port), file=sys.stderr)
        ns.port = open_port
    if ns.debug:
        print("initilizing websockets at ws://{}:{}".format(ns.host, ns.port))
    server = websockets.serve(websocket_handler, ns.host, ns.port)
    print("serving cyclus at http://{}:{}".format(ns.host, ns.port))
    # run the loop!
    try:
        loop.run_until_complete(asyncio.gather(
            asyncio.ensure_future(run_sim(state, loop, executor)),
            asyncio.ensure_future(action_consumer(state)),
            asyncio.ensure_future(action_monitor(state)),
            asyncio.ensure_future(heartbeat(state)),
            asyncio.ensure_future(server),
            ))
    finally:
        if not loop.is_closed():
            loop.close()


if __name__ == '__main__':
    main()
