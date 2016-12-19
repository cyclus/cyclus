"""Main Cyclus CLI entry point."""
from __future__ import unicode_literals, print_function
import atexit
from argparse import ArgumentParser, Action

from cyclus.jsoncpp import CustomWriter
from cyclus.lib import (DynamicModule, Env, version, load_string_from_file,
    Recorder, Timer, Context, set_warn_limit)


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


class AgentSchema(Action):
    """Displays an agent schema"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.agent_schema = values
        set_warn_limit(0)
        rec = Recorder()
        ti = Timer()
        ctx = Context(ti, rec)
        agent = DynamicModule.make(ctx, ns.agent_schema)
        print(agent.schema.rstrip())
        ctx.del_agent(agent)


class AgentVersion(Action):
    """Displays an agent version"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.agent_version = values
        set_warn_limit(0)
        rec = Recorder()
        ti = Timer()
        ctx = Context(ti, rec)
        agent = DynamicModule.make(ctx, ns.agent_version)
        print(agent.version)
        ctx.del_agent(agent)


class AgentAnnotations(Action):
    """Displays an agent annotations"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.agent_annotations = values
        set_warn_limit(0)
        rec = Recorder()
        ti = Timer()
        ctx = Context(ti, rec)
        agent = DynamicModule.make(ctx, ns.agent_annotations)
        anno = agent.annotations
        writer = CustomWriter("{", "}", "[", "]", ": ", ", ", " ", 80)
        s = writer.write(anno)
        print(s.rstrip())
        ctx.del_agent(agent)


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
    return p


def main(args=None):
    """Main function for Cyclus CLI"""
    Env.set_nuc_data_path()
    p = make_parser()
    ns = p.parse_args(args=args)


if __name__ == '__main__':
    main()