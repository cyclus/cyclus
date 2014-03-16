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
:exec: Executes arbitrary python code that is passed in as the arguments and loads 
       this into the context. This is useful for importing handy modules, declaring
       variables for later use, or any of the other things that Python is great for.
       Any variables defined here are kept in a seperate namespace from the classes.
       Since this gives you direct access to the Python interpreter, try to be a 
       little careful.

cycpp is implemented entirely in this file and with tools from the Python standard
library. It requires Python 2.7+ or Python 3.3+ to run.
"""
from __future__ import print_function
import os
import re
from collections import Sequence, MutableMapping
from subprocess import Popen, PIPE
from argparse import ArgumentParser, RawDescriptionHelpFormatter
from pprint import pprint

# This migh miss files which start with '#' - however, after canonization (through cpp)
# it shouldn't matter.
RE_STATEMENT = re.compile(
    #r'(\s*\n#)?'  # find the start of pragmas
    r'(\s*#)?'  # find the start of pragmas
    r'(\s+(public|private|protected)\s*'  # consider access control as statements
    #r'|[^{};]*)?'  # or, consider statement until we hit '{', '}', or ';'
    r'|(?(1)[^\n]|[^{};])*)?'  # or, consider statement until we hit '{', '}', or ';'
    # find end condition, '\n' for pragma, ':' for access, and '{', '}', ';' otherwise
    r'((?(1)\n|(?(3):|[{};])))', re.MULTILINE)

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
class Filter(object):
    """A basic, no-op filter."""

    regex = re.compile('a^')  # neat regex which fails even against empty strings

    def __init__(self, state=None, *args, **kwargs):
        self.state = state
        self.match = None

    def isvalid(self, statement):
        """Checks if a statement is valid for this fliter."""
        self.match = m = self.regex.match(statement)
        return m is not None

    def transform(self, statement, sep):
        """Performs a transformation given this."""
        raise NotImplementedError("no transformation function for "
                                  "{0}".format(self.__class__.__name__))

    def revert(self, statement, sep):
        """Reverts state transformation."""
        self.match = None

class TypedefFilter(MutableMapping):
    pass

class NamespaceFilter(Filter):
    """Filter for accumumating namespace encapsulations."""
    # handles anonymous namespaces as group(1) == None
    regex = re.compile("\s*namespace(\s+(\w*)?)?\s*$")

    def transform(self, statement, sep):
        state = self.state
        name = self.match.group(2)
        state.namespaces.append((state.depth, name))

    def revert(self, statement, sep):
        super(NamespaceFilter, self).revert(statement, sep)
        state = self.state
        if len(state.namespaces) == 0:
            return
        if state.depth == state.namespaces[-1][0]:
            del state.namespaces[-1]

class ClassFilter(Filter):
    """Filter for picking out class names."""
    regex = re.compile("\s*class\s+([\w:]+)\s*")

    def transform(self, statement, sep):
        state = self.state
        name = self.match.group(1)
        state.classes.append((state.depth, name))
        state.access[tuple(state.classes)] = "private"

    def revert(self, statement, sep):
        super(ClassFilter, self).revert(statement, sep)
        state = self.state
        if len(state.classes) == 0:
            return
        if state.depth == state.classes[-1][0]:
            del state.access[tuple(state.classes)]
            del state.classes[-1]

class AccessFilter(Filter):
    """Filter for setting the current access control flag."""
    regex = re.compile('\s*(public|private|protected)\s*')

    def transform(self, statement, sep):
        access = self.match.group(1)
        self.state.access[tuple(self.state.classes)] = access

class VarDecorationFilter(Filter):
    """Filter for handling state variable decoration of the form:

        #pragma cyclus var <dict>

    This evals the contents of dict and puts them in state.var_annotations, to be 
    consumed by the next match with VarDeclarationFilter.
    """
    regex = re.compile("#\s*pragma\s+cyclus\s+var\s+(.*)")

    def transform(self, statement, sep):
        state = self.state
        context = state.context
        classname = state.classname()
        raw = self.match.group(1)
        glb = dict(state.execns)
        glb.update(context)
        state.var_annotations = eval(raw, glb, context.get(classname, {}))

class VarDeclarationFilter(Filter):
    """State varible declaration.  Only oeprates if state.var_annotations is 
    not None. Access for member variable must be public.
    """
    regex = re.compile("(.*\w+.*?)\s+(\w+)")

    def transform(self, statement, sep):
        state = self.state
        annotations = state.var_annotations
        if annotations is None:
            return
        classname = state.classname()
        vtype, vname = self.match.groups()
        access = state.access[tuple(state.classes)]
        if access != "public":
            msg = ("access for state variable {0!r} on agent {1!r} must be public, "
                   "got {2!r}")
            raise ValueError(msg.format(vname, classname, access or 'private'))
        if classname not in state.context:
            state.context[classname] = {}
        annotations['type'] = vtype.strip().replace('\n', '')
        state.context[classname][vname] = annotations
        state.var_annotations = None

class ExecFilter(Filter):
    """Filter for executing arbitrary python code in the exec pragma and 
    adding the results to the context.  This pragma has the form:

        #pragma cyclus exec <code>

    Any Python statement(s) are valid as part of the code block. Be a little
    careful when using this pragma :).
    """
    regex = re.compile("#\s*pragma\s+cyclus\s+exec\s+(.*)")

    def transform(self, statement, sep):
        execns = self.state.execns
        context = self.state.context
        raw = self.match.group(1)
        exec(raw, context, execns)
        del context['__builtins__']

class StateAccumulator(object):
    """This represents the state of the file as it is being traversed.  
    At the end of the traversal the will have acquired all of the information
    needed for pass 2. It manages both the decorators and other needed bits 
    of C++ syntax.  It works by passing each statement through a sequence of 
    filters, and builds up or destroys context as it goes.

    Attributes
    ----------
    depth : int
        The current nesting level.
    """
    
    def __init__(self):
        self.depth = 0
        self.execns = {}   # execution namespace we have accumulated
        self.context = {}  # classes we have accumulated
        self.classes = []  # stack of (depth, class name) tuples, most nested is last
        self.access = {}   # map of (classnames, current access control flags)
        self.namespaces = []  # stack of stack of (depth, ns name) tuples
        self.var_annotations = None
        self.filters = [ClassFilter(self), AccessFilter(self), ExecFilter(self),
                        NamespaceFilter(self),
                        VarDecorationFilter(self), VarDeclarationFilter(self)]

    def classname(self):
        """Returns the current, most-nested class name."""
        return self.classes[-1][1]

    def accumulate(self, statement, sep):
        #print((repr(statement), repr(sep)))
        #print()
        # filters have to come before sep
        for filter in (() if len(statement) == 0 else self.filters):
            if filter.isvalid(statement):
                filter.transform(statement, sep)
                break
        # seps must come before revert
        if sep == '{':
            self.depth += 1
        elif sep == '}':
            self.depth -= 1
        # revert what is needed
        for filter in self.filters: 
            filter.revert(statement, sep)

def accumulate_state(canon):
    """Takes a canonical C++ source file and separates it out into statements
    which are fed into a state accumulator.  The state is returned.
    """
    state = StateAccumulator()
    canon = '\n' + canon if canon.startswith('#') else canon
    for m in RE_STATEMENT.finditer(canon):
        if m is None:
            continue
        prefix, statement, _, sep = m.groups()
        #print((prefix, statement, _, sep))
        #print()
        statement = statement if prefix is None else prefix + statement
        statement = statement.strip()
        state.accumulate(statement, sep)
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
    pprint(state.context)

if __name__ == "__main__":
    main()


