#!/usr/bin/env python
"""The cyclus preprocessor.

cycpp is a 3-pass preprocessor which adds reflection-like semantics to cyclus 
agents. This is needed to provide a high-level, user-facing API to cyclus.
Code that uses cycpp is entirely valid C++ code and will compile normally even
without first running it through the cycpp. This is because cycpp relies on 
custom #pragma decoration to annotate or inject into the code. These pragmas 
are skipped - by definition - by the C preprocessor and the C/C++ compiler.

The three passes of cycpp are:

1. run cpp normally to canonize all other preprocessor directives,
2. accumulate annotations for agents and state variables, and
3. generate code based on annotations.

All decorators have the following form::

    #pragma cyclus <decorator name> [args]

The ``#pragma cyclus`` portion is a flag so that *only* cycpp consumes this directive.
This is followed by the actual ``<decorator name>`` which tells cycpp what to do with 
this pragma. Lastly, optionals arguments may be passed to this decorator but all options
*must* be on the same logical line as the directive.  How the arguments are interpreted
is a function of the decorators themselves.  Most of them are simple Python statements
or expressions.  See the following handy table!

**Decorator Arguments:**

:var:  Add the following C++ statement as an Agent's state variable. There is one 
       argument which must be a Python expression that evaluates to a dictionary or
       other mapping.

cycpp is implemented entirely in this file and with tools from the Python standard
library. It requires Python 2.7+ or Python 3.3+ to run.
"""
from __future__ import print_function
import os
import re
from collections import Sequence, MutableMapping
from subprocess import Popen, PIPE
from argparse import ArgumentParser, RawDescriptionHelpFormatter

RE_STATEMENT = re.compile(r'(#?|[^{};]*?)(?(1)\n|[{};])', re.MULTILINE)

#
# pass 1
#
def preprocess_file(filename, cpp_path='cpp', cpp_args=('-xc++', '-pipe')):
    """Preprocess a file using cpp.

    Parameters
    ----------
    filename : str
        Name of the file you want to preprocess.
    cpp_path : str, optional
    cpp_args : str, optional
        Refer to the documentation of parse_file for the meaning of these arguments.

    Notes
    -----
    This was forked from pycparser: https://github.com/eliben/pycparser
    """
    path_list = [cpp_path]
    if isinstance(cpp_args, Sequence):
        path_list += cpp_args
    elif cpp_args != '':
        path_list += [cpp_args]
    path_list += [filename]
    try:
        # Note the use of universal_newlines to treat all newlines
        # as \n for Python's purpose
        pipe = Popen(path_list, stdout=PIPE, universal_newlines=True)
        text = pipe.communicate()[0]
    except OSError as e:
        raise RuntimeError(("Unable to invoke 'cpp'.  Make sure its path was passed "
                            "correctly\nOriginal error: {0}").format(e))
    return text

#
# pass 2
#
class TypedefFilter(MutableMapping):
    pass

class StateAccumulator(object):

    def accumulate(self, statement):
        print(repr(statement))

def accumulate_state(canon):
    """Takes a canonical C++ source file and separates it out into statements
    which are fed into a state accumulator.  The state is returned.
    """
    state = StateAccumulator()
    for m in RE_STATEMENT.finditer(canon):
        if m is None:
            continue
        statement, sep = m.groups()
        state.accumulate(statement)
        #state.accumulate(sep)
    return state

#
# pass 3
#

#
# meta
#
def main():
    parser = ArgumentParser(prog="cycpp", description=__doc__, 
                            formatter_class=RawDescriptionHelpFormatter)
    parser.add_argument('path', help="path to source file")
    ns = parser.parse_args()

    canon = preprocess_file(ns.path)  # pass 1
    state = accumulate_state(canon)   # pass 2

if __name__ == "__main__":
    main()


