"""Main Cyclus CLI entry point."""
from __future__ import unicode_literals, print_function
import atexit
from argparse import ArgumentParser, Action

from cyclus.lib import DynamicModule, Env, version


# ensure that Cyclus dynamic modules are closed when Python exits.
_DYNAMIC_MODULE = DynamicModule()
atexit.register(_DYNAMIC_MODULE.close_all)


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


def make_parser():
    """Makes the Cyclus CLI parser."""
    p = ArgumentParser("cyclus", description="Cyclus command line "
                                             "(from Python)")
    p.add_argument('-V', action=CyclusVersion)
    p.add_argument('--restart', action=Restart)
    return p


def main(args=None):
    """Main function for Cyclus CLI"""
    Env.set_nuc_data_path()
    p = make_parser()
    ns = p.parse_args(args=args)


if __name__ == '__main__':
    main()