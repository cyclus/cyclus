"""Tools for representing and driving the simulation."""
from __future__ import print_function, unicode_literals
import os
import queue
import atexit
import sys

#from cyclus.system import curio
from cyclus.lib import (DynamicModule, Env, version, load_string_from_file,
    Recorder, Timer, Context, set_warn_limit, discover_specs, XMLParser,
    discover_specs_in_cyclus_path, discover_metadata_in_cyclus_path, Logger,
    set_warn_limit, set_warn_as_error, xml_to_json, json_to_xml,
    Hdf5Back, SqliteBack, InfileTree, SimInit, XMLFileLoader, XMLFlatLoader)
from cyclus.memback import MemBack


def get_schema_path(flat_schema=False, schema_path=None):
    """Gets the schema path based on the given schema path an the flatness."""
    if flat_schema:
        path = Env.rng_schema(True)
    elif schema_path is not None:
        path = schema_path
    else:
        path = Env.rng_schema(False)
    return path


_DM_REGISTERED = False


def ensure_close_dynamic_modules():
    """Ensures that the dynamic module library is closed at the end of
    the process.
    """
    global _DM_REGISTERED
    if _DM_REGISTERED:
        return
    _dynamic_module = DynamicModule()
    atexit.register(_dynamic_module.close_all)
    _DM_REGISTERED = True


class SimState(object):
    """Class that represents and drives the simulation state.

    Parameters
    ----------
    input_file : str, optional
        The path to input file or a raw string of the input file.
    input_format : str, optional
        The format that the input file should be read in as, if it is a raw string.
    output_path : str or None, optional
        The path to the file system database, default (if None) is based
        on the input file path.
    memory_backend : MemBack, bool, or None, optional
        An in-memory backend, if specified.
    registry : set, bool, or None, optional
        The initial registry to start the in-memory backend with. Defaults
        is True, which stores all of the tables.
    schema_path : str or None, optional:
        The path to the cyclus master schema.
    flat_schema : bool, optional
        Whether or not to use the flat master simulation schema.
    frequency : int or float, optional
        The amount of time [sec] to sleep for in tight loops, default 1 ms.
    repeating_actions : list or None, optional
        A list of actions that should be added to the queue at the start of
        each timestep when the cyclus.events.loop() is called. If this is
        None, a new empty list is instantiated.
    heartbeat_frequency : float or int, optional
        The frequency with which to send heartbeat events.
    debug : bool, optional
        Whether the simulation should provide debugging information.

    Attributes
    ----------
    rec : Recorder or None
        A recorder instance, available after load.
    file_backend : FullBackend or None
        A file system based backend that is attached to the output path.
    si : SimInit or None
        The main simulation initializer object that then can be used
        to drive the simulation.
    loop : event loop or None
        The Python event loop the current simulation is running under.
    executor : Executor or None,
        The Python Executor that the simulation should use when running as
        a server. This is typically a thread pool.
    tasks : dict
        A str-keyed dictionary mapping to current tasks that may be
        shared among many actions.
    send_queue : asyncio.Queue or None
        A queue of data to send from the server to the client.
    action_queue : queue.Queue or None
        A queue for pending actions to be loaded to popped from.
    monitor_queue : queue.Queue or None
        A queue for pending actions for monitoring tasks.
    """

    def __init__(self, input_file=None, input_format=None, output_path=None,
                 memory_backend=False, registry=True, schema_path=None,
                 flat_schema=False, frequency=0.001, repeating_actions=None,
                 heartbeat_frequency=5, debug=False, print_ms = False):
        ensure_close_dynamic_modules()
        self.input_file = input_file
        self.input_format = input_format
        if output_path is None:
            if input_format is None or input_format == "none":
                base, _ = os.path.splitext(os.path.basename(input_file))
            else:
                base = "cyclus"
            output_path = base + '.h5'
        self.output_path = output_path
        self.memory_backend = memory_backend
        self._registry = registry
        self.flat_schema = flat_schema
        self.schema_path = schema_path
        self.print_ms = print_ms
        self.frequency = frequency
        self.repeating_actions = [] if repeating_actions is None \
                                    else repeating_actions
        self.heartbeat_frequency = heartbeat_frequency
        self.debug = debug
        self.rec = self.file_backend = self.si = None
        self.tasks = {}
        self._send_queue = self._action_queue = self._monitor_queue = None
        self.loop = self.executor = None

    def __del__(self):
        self.rec.flush()

    def load(self):
        """Loads the simulation."""
        Env.set_nuc_data_path()
        self.rec = rec = Recorder()
        self._load_backends()
        self._load_schema_path()
        self._load_input_file()
        self.si = SimInit(rec, self.file_backend)

    def _load_backends(self):
        """setup database backends"""
        # load the file based backend
        output_path = self.output_path
        _, ext = os.path.splitext(output_path)
        if ext == '.h5':
            self.file_backend = Hdf5Back(output_path)
        elif ext == '.sqlite':
            self.file_backend = SqliteBack(output_path)
        else:
            raise RuntimeError('Backend extension type not recognised, ' +
                               output_path)
        # load the memory based backend
        memory_backend = self.memory_backend
        if memory_backend is not None:
            if isinstance(memory_backend, MemBack):
                pass
            elif memory_backend:
                memory_backend = MemBack(registry=self._registry,
                                         fallback=self.file_backend)
                self.memory_backend = memory_backend
            else:
                self.memory_backend = None
        # register with the recorder
        self.rec.register_backend(self.file_backend)
        if self.memory_backend is not None:
            self.rec.register_backend(self.memory_backend)

    def _load_schema_path(self):
        """find schema type"""
        parser = XMLParser(filename=self.input_file, format=self.input_format)
        tree = InfileTree.from_parser(parser)
        schema_type = tree.optional_query("/simulation/schematype", "")
        if schema_type == "flat" and not self.flat_schema:
            print("flat schema tag detected - switching to flat input schema",
                  file=sys.stderr)
            self.flat_schema = True
        self.schema_path = get_schema_path(flat_schema=self.flat_schema,
                                           schema_path=self.schema_path)

    def _load_input_file(self):
        """Loads the input file for the simulation"""
        if self.flat_schema:
            loader = XMLFlatLoader(self.rec, self.file_backend,
                                   self.schema_path, self.input_file,
                                   self.input_format, self.print_ms)
        else:
            loader = XMLFileLoader(self.rec, self.file_backend,
                                   self.schema_path, self.input_file,
                                   self.input_format, self.print_ms)
        loader.load_sim()

    def run(self):
        """Starts running the simulation."""
        self.si.timer.run_sim()
        self.rec.flush()

    @property
    def send_queue(self):
        """A queue for sending data over the TCP server. This is
        not instantiated until it is first accessed.
        """
        if self._send_queue is None:
            from cyclus.system import asyncio
            self._send_queue = asyncio.Queue()
        return self._send_queue

    @property
    def action_queue(self):
        """A queue for pending actions."""
        if self._action_queue is None:
            self._action_queue = queue.Queue()
        return self._action_queue

    @property
    def monitor_queue(self):
        """A queue for pending monitoring actions."""
        if self._monitor_queue is None:
            self._monitor_queue = queue.Queue()
        return self._monitor_queue
