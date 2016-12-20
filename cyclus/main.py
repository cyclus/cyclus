"""Main Cyclus CLI entry point."""
from __future__ import unicode_literals, print_function
import os
import atexit
from argparse import ArgumentParser, Action

from cyclus.jsoncpp import CustomWriter
from cyclus.lib import (DynamicModule, Env, version, load_string_from_file,
    Recorder, Timer, Context, set_warn_limit, discover_specs,
    discover_specs_in_cyclus_path, discover_metadata_in_cyclus_path, Logger,
    set_warn_limit, set_warn_as_error)


# ensure that Cyclus dynamic modules are closed when Python exits.
_DYNAMIC_MODULE = DynamicModule()
atexit.register(_DYNAMIC_MODULE.close_all)


def set_schema_path(ns):
    """Sets the schema path on the namespace."""
    if ns.flat_schema:
        path = Env.rng_schema(True)
    elif ns.schema_path is not None:
        path = ns.schema_path
    else:
        path = Env.rng_schema(False)
    ns.schema_path = path


class ZeroArgAction(Action):
    """An action with no arguments."""

    def __init__(self, option_strings, dest, nargs=0, **kwargs):
        super(ZeroArgAction, self).__init__(option_strings, dest, nargs=nargs,
                                            **kwargs)

class CyclusVersion(ZeroArgAction):
    """Displays the cyclus version"""

    def __call__(self, parser, namespace, values, option_string=None):
        print(version(), end="")


class Restart(Action):
    """Restart action for cyclus. Not supported."""

    def __call__(self, parser, namespace, values, option_string=None):
        raise RuntimeError("Restart is experimental, not supported!")


class Schema(ZeroArgAction):
    """Displays cyclus schema"""

    def __call__(self, parser, ns, values, option_string=None):
        set_schema_path(ns)
        schema = load_string_from_file(ns.schema_path)
        print(schema)


def make_agent_ctx(spec):
    """Contsructs and agent and returns the agent and the context."""
    set_warn_limit(0)
    rec = Recorder()
    ti = Timer()
    ctx = Context(ti, rec)
    agent = DynamicModule.make(ctx, spec)
    return agent, ctx


class AgentSchema(Action):
    """Displays an agent schema"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.agent_schema = values
        agent, ctx = make_agent_ctx(ns.agent_schema)
        print(agent.schema.rstrip())
        ctx.del_agent(agent)


class AgentVersion(Action):
    """Displays an agent version"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.agent_version = values
        agent, ctx = make_agent_ctx(ns.agent_version)
        print(agent.version)
        ctx.del_agent(agent)


class AgentAnnotations(Action):
    """Displays an agent annotations"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.agent_annotations = values
        agent, ctx = make_agent_ctx(ns.agent_annotations)
        anno = agent.annotations
        writer = CustomWriter("{", "}", "[", "]", ": ", ", ", " ", 80)
        s = writer.write(anno)
        print(s.rstrip())
        ctx.del_agent(agent)


class AgentListing(Action):
    """Displays an agent listing"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.agent_listing = values
        path, library = ns.agent_listing.split(':')
        specs = discover_specs(path, library)
        s = '\n'.join(sorted(specs))
        print(s)


class AllAgentListing(ZeroArgAction):
    """Displays all known cyclus agents"""

    def __call__(self, parser, ns, values, option_string=None):
        specs = discover_specs_in_cyclus_path()
        s = '\n'.join(sorted(specs))
        print(s)


class Metadata(ZeroArgAction):
    """Displays all known cyclus agents"""

    def __call__(self, parser, ns, values, option_string=None):
        md = discover_metadata_in_cyclus_path()
        writer = CustomWriter("{", "}", "[", "]", ": ", ", ", " ", 80)
        s = writer.write(md)
        print(s.rstrip())


class NoAgent(ZeroArgAction):
    """Disables agent logging"""

    def __call__(self, parser, ns, values, option_string=None):
        Logger().no_agent = True


class NoMem(ZeroArgAction):
    """Disables mem logging"""

    def __call__(self, parser, ns, values, option_string=None):
        Logger().no_mem = True


class Verbosity(Action):
    """Sets versbosity level"""

    def __call__(self, parser, ns, values, option_string=None):
        logger = Logger()
        try:
            ns.verbosity = int(values)
        except ValueError:
            ns.verbosity = logger.to_log_level(values)
        logger.report_level = ns.verbosity


class WarnLimit(Action):
    """Sets warning limit"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.warn_limit = int(values)
        set_warn_limit(ns.warn_limit)


class WarnAsError(ZeroArgAction):
    """Sets warning to be errors"""

    def __call__(self, parser, ns, values, option_string=None):
        set_warn_as_error(True)


class CyclusPath(ZeroArgAction):
    """Prints the Cyclus Path"""

    def __call__(self, parser, ns, values, option_string=None):
        cp = Env().cyclus_path
        s = os.pathsep.join(cp)
        print(s)


class IncludePath(ZeroArgAction):
    """Prints the Cyclus Include Path"""

    def __call__(self, parser, ns, values, option_string=None):
        s = os.path.join(Env().install_path, "include", "cyclus")
        print(s)


class InstallPath(ZeroArgAction):
    """Prints the Cyclus Install Path"""

    def __call__(self, parser, ns, values, option_string=None):
        s = Env().install_path
        print(s)


class CMakeModulePath(ZeroArgAction):
    """Prints the Cyclus CMake Module Path"""

    def __call__(self, parser, ns, values, option_string=None):
        s = os.path.join(Env().install_path, "share", "cyclus", "cmake")
        print(s)


def make_parser():
    """Makes the Cyclus CLI parser."""
    p = ArgumentParser("cyclus", description="Cyclus command line "
                                             "(from Python)")
    p.add_argument('-V', action=CyclusVersion,
                   help='print cyclus core and dependency versions')
    p.add_argument('--restart', action=Restart,
                   help='restart from the specified simulation snapshot, '
                        'not supported.')
    p.add_argument('--schema', action=Schema,
                   help='dump the cyclus master schema including all '
                        'installed module schemas')
    p.add_argument('--agent-schema', action=AgentSchema,
                   dest='agent_schema',
                   help='dump the schema for the named agent')
    p.add_argument('--agent-version', action=AgentVersion,
                   dest='agent_version',
                   help='dump the version for the named agent')
    p.add_argument('--schema-path', dest='schema_path', default=None,
                   help='manually specify the path to the cyclus master schema')
    p.add_argument('--flat-schema', action='store_true', default=False,
                   dest='flat_schema',
                   help='use the flat master simulation schema')
    p.add_argument('--agent-annotations', action=AgentAnnotations,
                   dest='agent_annotations',
                   help='dump the annotations for the named agent')
    p.add_argument('-l', '--agent-listing', action=AgentListing,
                   dest='agent_listing',
                   help='dump the agents in a library')
    p.add_argument('-a', '--all-agent-listing', action=AllAgentListing,
                   help='dump all the agents cyclus knows about')
    p.add_argument('-m', '--metadata', action=Metadata,
                   help='dump metadata for all the agents cyclus knows about')
    p.add_argument('--no-agent', action=NoAgent,
                   help='only print log entries from cyclus core code')
    p.add_argument('--no-mem', action=NoMem,
                   help='exclude memory log statement from logger output')
    p.add_argument('-v', '--verb', action=Verbosity, dest='verbosity',
                   help='log verbosity. integer from 0 (quiet) to 11 (verbose)')
    p.add_argument('-o', '--output-path', dest='output_path',
                   default='cyclus.sqlite', help='output path')
    p.add_argument('--input-file', dest='input_file', default=None,
                   help="path to input file")
    p.add_argument('--warn-limit', dest='warn_limit', action=WarnLimit,
                   help='number of warnings to issue per kind, defaults to 42')
    p.add_argument('--warn-as-error', action=WarnAsError,
                   help='throw errors when warnings are issued')
    p.add_argument('-p', '--path', action=CyclusPath,
                   help='print the CYCLUS_PATH')
    p.add_argument('--include', action=IncludePath,
                   help='print the cyclus include directory')
    p.add_argument('--install-path', action=InstallPath,
                   help='print the cyclus install directory')
    p.add_argument('--cmake-module-path', action=CMakeModulePath,
                   help='print the cyclus CMake module path')
    return p


def main(args=None):
    """Main function for Cyclus CLI"""
    Env.set_nuc_data_path()
    p = make_parser()
    ns = p.parse_args(args=args)


if __name__ == '__main__':
    main()