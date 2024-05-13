#!/usr/bin/env python3
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

The ``#pragma cyclus`` portion is a flag so that *only* cycpp consumes this
#directive. This is followed by the actual ``<decorator name>`` which tells
cycpp what to do with this pragma. Lastly, optional arguments may be passed to
this decorator but all options *must* be on the same logical line as the
directive.  How the arguments are interpreted is a function of the decorators
themselves.  Most of them are simple Python statements or expressions. See the
following handy table!

**Decorator Arguments:**

:var:  Add the following C++ statement as an Agent's state variable. There is
       one argument which must be a Python expression that evaluates to
       a dictionary or other Mapping.
:exec: Executes arbitrary python code that is passed in as the arguments and
       loads this into the context. This is useful for importing handy modules,
       declaring variables for later use, or any of the other things that Python
       is great for. Any variables defined here are kept in a separate
       namespace from the classes.  Since this gives you direct access to the
       Python interpreter, try to be a little careful.
:note: Merges the argument (which like with var must evalutae to a dict) with the
       current class level annotations. Enrties here overwrite previous entries.

cycpp is implemented entirely in this file and with tools from the Python standard
library. It requires Python 2.7+ or Python 3.3+ to run.
"""
from __future__ import print_function, unicode_literals
import os
import re
import sys
import uuid
from collections.abc import Sequence, Mapping, MutableMapping
from collections import OrderedDict
from contextlib import contextmanager
from itertools import takewhile
from subprocess import Popen, PIPE
from argparse import ArgumentParser, RawDescriptionHelpFormatter
from pprint import pprint, pformat
import textwrap
import difflib
import xml.dom.minidom

try:
    import simplejson as json
except ImportError:
    import json

if sys.version_info[0] == 2:
    STRING_TYPES = (str, unicode, basestring)
elif sys.version_info[0] >= 3:
    STRING_TYPES = (str,)

# Get system $CPP variable
sys_cpp = os.environ.get('CPP', 'cpp')

# Non-capturing and must be used wit re.DOTALL, DO NOT COMPILE!
RE_MULTILINE_COMMENT = r"(?:\s*?/\*(?!\*/)*?\*/)"
RE_SINGLE_LINE_COMMENT = r"(?:\s*?//[^\n]*?\n\s*?)"
RE_COMMENTS = "(?:" + RE_MULTILINE_COMMENT + "|" + RE_SINGLE_LINE_COMMENT + ")"

# This might miss files which start with '#' - however, after canonization
# (through cpp) it shouldn't matter.
RE_STATEMENT = re.compile(
    # find the start of pragmas and comments
    r'(?:(\s*#|\s*//)|\s*(/\*))?'
    # consider access control as statements
    r'(\s+(public|private|protected)\s*|'
    # or, consider statement until we hit '{', '}', or ';'
    r'(?(1)[^\n]|(?(2).|[^{};]))*?)'
    # find end condition, '\n' for pragma and single line commentd,
    # ':' for access, '*/' for multiline comments, and '{', '}', ';' otherwise
    r'((?(1)\n|(?(2)\*/|(?(4):|[{};]))))',
    re.MULTILINE | re.DOTALL)

CYCNS = 'cyclus'

PRIMITIVES = {'bool', 'int', 'float', 'double', 'std::string', 'cyclus::Blob',
              'boost::uuids::uuid', }

BUFFERS = {'{0}::toolkit::ResBuf'.format(CYCNS),
           ('{0}::toolkit::ResBuf'.format(CYCNS), CYCNS + '::Resource'),
           ('{0}::toolkit::ResBuf'.format(CYCNS), CYCNS + '::Product'),
           ('{0}::toolkit::ResBuf'.format(CYCNS), CYCNS + '::Material'),
           '{0}::toolkit::ResMap'.format(CYCNS),
           '{0}::toolkit::TotalInvTracker'.format(CYCNS),
           ('{0}::toolkit::TotalInvTracker'.format(CYCNS), '{0}::toolkit::ResBuf'.format(CYCNS), CYCNS + '::Material'),
           }

TEMPLATES = {'std::vector', 'std::set', 'std::list', 'std::pair',
             'std::map', '{0}::toolkit::ResBuf'.format(CYCNS),
             CYCNS + '::toolkit::ResMap',
             CYCNS + '::toolkit::TotalInvTracker',}

WRANGLERS = {
    '{0}::Agent'.format(CYCNS),
    '{0}::Facility'.format(CYCNS),
    '{0}::Institution'.format(CYCNS),
    '{0}::Region'.format(CYCNS),
    'mi6::Spy',  # for testing!!
    }

ENTITIES = [('cyclus::Region', 'region'), ('cyclus::Institution', 'institution'),
            ('cyclus::Facility', 'facility'), ('cyclus::Agent', 'archetype')]

def escape_xml(s, ind='    '):
    """Escapes xml string s, prettifies it and puts in c++ string lit form."""

    s = xml.dom.minidom.parseString(s)
    s = s.toprettyxml(indent='    ')
    s = s.replace('"', '\\"')

    lines = s.splitlines()
    lines = lines[1:] # remove initial xml version tag

    clean = [line for line in lines if len(line.strip()) != 0]
    cleaner = ['{0}"{1}\\n"'.format(ind, line.rstrip()) for line in clean]
    return '\n'.join(cleaner)

def prepare_type(cpptype, othertype):
    """Updates othertype to conform to the length of cpptype using None's.
    """
    if not isinstance(cpptype, STRING_TYPES):
        if isinstance(othertype, STRING_TYPES):
            othertype = [othertype]

        if othertype is None:
            othertype = [None] * len(cpptype)
        elif len(othertype) < len(cpptype):
            othertype.extend([None] * (len(cpptype) - len(othertype)))
        return othertype
    else:
        return othertype

#
# pass 1
#
def preprocess_file(filename, includes=(), cpp_path=sys_cpp,
                    cpp_args=('-xc++', '-pipe', '-E', '-DCYCPP')):
    """Preprocess a file using cpp.

    Parameters
    ----------
    filename : str
        Name of the file you want to preprocess.
    includes : list
        A list of all include directories to tell the preprocessor about
    cpp_path : str, optional
    cpp_args : str, optional
        Refer to the documentation of parse_file for the meaning of these
        arguments.

    Notes
    -----
    This was forked from pycparser: https://github.com/eliben/pycparser
    """
    path_list = [cpp_path]
    for include in includes:
        path_list += ['-I', include]
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
        raise RuntimeError(("Unable to invoke 'cpp'.  Make sure its path was "
                            "passed correctly\nOriginal error: {0}").format(e))
    return text

#
# generic filters
#
class Filter(object):
    """A basic, no-op filter."""

    regex = re.compile('a^')  # neat regex which fails even against empty strings

    def __init__(self, machine=None, *args, **kwargs):
        self.machine = machine
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

    def ctxstr(self, classname=None, varname=None):
        mc = self.machine
        if classname is None and varname is None:
            s = "The {0} machine has the current context:\n{1}"
            s = s.format(mc.__class__.__name__, pformat(mc.context))
        elif varname is None:
            s = ("The {0!r} class in the {1} machine has the following "
                 "annotations:\n{2}")
            s = s.format(classname, mc.__class__.__name__,
                         pformat(mc.context[classname]))
        else:
            s = ("The {0!r} state variable on the {1!r} class in the {2} machine "
                 "has the following annotations:\n\n{3}\n")
            s = s.format(varname, classname, mc.__class__.__name__,
                         pformat(mc.context[classname]['vars'][varname]))
        return s

class LinemarkerFilter(Filter):
    """Filter for computing the current source code line
    from preprocessor line marker directives.

        # linenum filename [flags]

    This is useful for debugging. See the cpp for more info:
    http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
    """
    regex = re.compile(r'\s*#\s+(\d+)\s+"(.*?)"(\s+\d+)*?', re.DOTALL)
    allowed_flags = {'1', '2'}
    last_was_linemarker = False

    def transform(self, statement, sep):
        lineno, fname, flags = self.match.groups()
        if len(set((flags or '1').split()) ^ self.allowed_flags) == 0:
            return
        lms = self.machine.linemarkers
        if not self.last_was_linemarker:
            del lms[:]
        lms.append((fname, int(lineno)))
        self.machine.nlines_since_linemarker = -1

    def revert(self, statement, sep):
        self.last_was_linemarker = self.match is not None
        super(LinemarkerFilter, self).revert(statement, sep)

class AliasFilter(Filter):
    """Filter for managing alias (de-)scoping."""

    def revert(self, statement, sep):
        super(AliasFilter, self).revert(statement, sep)
        state = self.machine
        if len(state.aliases) == 0 or sep != '}':
            return
        # Only keep alias at or above current depth
        depth = state.depth
        state.aliases -= {d_n_a for d_n_a in state.aliases if d_n_a[0] > depth}

class TypedefFilter(AliasFilter):
    """Filter for handling typedef as aliases. Note that in-line compound
    typedefs of structs and unions are not supported.
    """
    regex = re.compile(r"\s*typedef\s+(.*?\s+.*)\s*$")

    def transform(self, statement, sep):
        state = self.machine
        g = self.match.group(1)
        g = outter_split(g)
        g0 = g[0].split()  # canonize the type name
        typ = " ".join(g0[:-1])
        depth = state.depth
        state.aliases |= {(depth, typ, a) for a in g0[-1:] + g[1:]}

class UsingFilter(AliasFilter):
    """Filter for accumumating using aliases."""
    regex = re.compile(r"\s*using\s+(?!namespace\s+)([\w:]+)\s*")

    def transform(self, statement, sep):
        state = self.machine
        name = self.match.group(1)
        if len(name.rsplit('::', 1)) > 1:
            state.aliases.add((state.depth, name, name.rsplit('::', 1)[1]))

class NamespaceFilter(Filter):
    """Filter for accumumating namespace encapsulations."""
    # handles anonymous namespaces as group(1) is None
    regex = re.compile(r"\s*namespace(\s+\w*)?\s*[^=]*", re.DOTALL)

    def transform(self, statement, sep):
        state = self.machine
        name = self.match.group(1)
        if name is not None:
            name = name.strip() or '<anonymous>'
        state.namespaces.append((state.depth, name))

    def revert(self, statement, sep):
        super(NamespaceFilter, self).revert(statement, sep)
        state = self.machine
        if len(state.namespaces) == 0:
            return
        if state.depth == state.namespaces[-1][0]:
            del state.namespaces[-1]

class UsingNamespaceFilter(Filter):
    """Filter for accumumating using namespace statement."""
    regex = re.compile(r"\s*using\s+namespace\s+([\w:]*)\s*")

    def transform(self, statement, sep):
        state = self.machine
        name = self.match.group(1)
        state.using_namespaces.add((state.depth, name))

    def revert(self, statement, sep):
        super(UsingNamespaceFilter, self).revert(statement, sep)
        state = self.machine
        if len(state.using_namespaces) == 0 or sep != '}':
            return
        # Only keep ns at or above current depth
        depth = state.depth
        state.using_namespaces -= {d_ns for d_ns in state.using_namespaces \
                                   if d_ns[0] > depth}

class NamespaceAliasFilter(AliasFilter):
    """Filter for accumumating namespace renames."""
    regex = re.compile(r"\s*namespace\s+(\w+)\s*=\s*([\w:]+)\s*")

    def transform(self, statement, sep):
        state = self.machine
        alias = self.match.group(1)
        name = self.match.group(2)
        state.aliases.add((state.depth, name, alias))

class ClassFilter(Filter):
    """Filter for picking out class names."""
    regex = re.compile(r"(?:\s*template\s*<[\s\w,]*>)?"
                       r"\s*(?:class|struct)\s+(\w+)(\s*:[\n\s\w,:]+)?\s*", re.DOTALL)

    def transform(self, statement, sep):
        state = self.machine
        name = self.match.group(1)
        state.classes.append((state.depth, name))
        state.access[tuple(state.classes)] = "private"

    def revert(self, statement, sep):
        super(ClassFilter, self).revert(statement, sep)
        machine = self.machine
        if len(machine.classes) == 0 or machine.depth != machine.classes[-1][0]:
            return
        del machine.access[tuple(machine.classes)]
        del machine.classes[-1]

class ClassAndSuperclassFilter(ClassFilter):
    """This accumulates superclass information as well as class information."""

    def transform(self, statement, sep):
        super(ClassAndSuperclassFilter, self).transform(statement, sep)
        state = self.machine
        classname = state.classname()
        superclasses = self.match.group(2)
        state.superclasses[classname] = sc = state.superclasses.get(classname,
                                                                    set())
        if superclasses is not None:
            superclasses = [s.strip().split()[-1] for s in superclasses.split(',')]
            for sup in superclasses:
                trysup = state.canonize_class(sup)
                if trysup is None:
                    # We cannot raise an error here becuase there are too many
                    # corner cases we do not and should not support in C++
                    continue
                sc.add(trysup)

class AccessFilter(Filter):
    """Filter for setting the current access control flag."""
    regex = re.compile(r'\s*(public|private|protected)\s*', re.DOTALL)

    def transform(self, statement, sep):
        access = self.match.group(1)
        self.machine.access[tuple(self.machine.classes)] = access

class PragmaCyclusErrorFilter(Filter):
    """Filter for handling invalid #pragma cyclus. This should be the last filter."""
    regex = re.compile(r'\s*#\s*pragma\s+cyclus(.*)')

    directives = frozenset(['var', 'note', 'exec', 'decl', 'def', 'impl'])

    def isvalid(self, statement):
        """Checks if a statement is valid for this fliter."""
        self.match = m = self.regex.match(statement)
        if m is None:
            return False
        g1 = m.group(1).strip()
        if len(g1) == 0:
            return False
        s0 = g1.split(None, 1)[0]
        return s0 not in self.directives

    def transform(self, statement, sep):
        m = self.machine
        msg = '{0}This appears to be a cyclus pragma but has an incorrect form!'
        inc = m.includeloc(statement=statement)
        if len(inc) == 0:
            inc = "For the statement:\n" + statement + "\n"
        raise SyntaxError(msg.format(inc))

#
# pass 2
#
class DecorationFilter(Filter):
    """Abstract class for annotation accumulation.
    """

    def _add_gbl_proxies(self, glb, path, val):
        """Proxies for global C++ scope."""
        prx = glb
        for p in path[:-1]:
            if p not in prx:
                prx[p] = Proxy({})
            prx = prx[p]
        prx[path[-1]] = Proxy(val)

    def _add_lcl_proxies(self, glb, clspaths, classpaths):
        """Proxy shortcuts for local C++ scope."""
        same_keys = []
        for x, y in zip(classpaths, clspaths):
            if x != y:
                break
            same_keys.append(x)
        if len(same_keys) == 0:
            return
        prx = glb
        for sk in same_keys:
            prx = prx[sk]
        for k in prx:
            glb[k] = prx[k]

    def _eval(self):
        state = self.machine
        context = state.context
        classname = state.classname()
        classpaths = classname.split('::')
        raw = self.match.group(1)
        glb = dict(state.execns)
        for cls, val in context.items():
            clspaths = cls.split('::')
            self._add_gbl_proxies(glb, clspaths, val['vars'])
            self._add_lcl_proxies(glb, clspaths, classpaths)
        lcl = context.get(classname, OrderedDict()).get('vars', OrderedDict())
        annotations = eval(raw, glb, lcl)
        return annotations

class VarDecorationFilter(DecorationFilter):
    """Filter for handling state variable decoration of the form:

        #pragma cyclus var <dict>

    This evals the contents of dict and puts them in state.var_annotations, to be
    consumed by the next match with VarDeclarationFilter.
    """
    regex = re.compile(r"\s*#\s*pragma\s+cyclus\s+var\s+(.*)")

    def transform(self, statement, sep):
        state = self.machine
        if isinstance(state, CodeGenerator):
            state.var_annotations = True
            return
        state.var_annotations = self._eval()

class VarDeclarationFilter(Filter):
    """State varible declaration.  Only operates if state.var_annotations is
    not None. Access for member variable must be public.
    """
    regex = re.compile(r"(.*\w+.*?)\s+(\w+)")

    def transform_pass2(self, statement, sep):
        state = self.machine
        annotations = state.var_annotations
        if annotations is None:
            return
        classname = state.classname()
        vtype, vname = self.match.groups()
        access = state.access[tuple(state.classes)]
        state.ensure_class_context(classname)
        annotations['type'] = state.canonize_type(vtype, vname,
                                                  statement=statement)
        annotations['index'] = len(state.context[classname]['vars'])
        annotations['shape'] = state.canonize_shape(annotations)
        state.context[classname]['vars'][vname] = annotations
        if 'alias' in annotations:
            alias = annotations['alias']
            while not isinstance(alias, STRING_TYPES):
                alias = alias[0]
            state.context[classname]['vars'][alias] = vname
        if annotations['type'][0] not in BUFFERS:
            annotations['alias'] = self.canonize_alias(
                annotations['type'], vname, alias=annotations.get('alias'))
            annotations['tooltip'] = self.canonize_tooltip(
                annotations['type'], vname, tooltip=annotations.get('tooltip'))
            annotations['uilabel'] = self.canonize_uilabel(
                annotations['type'], vname, uilabel=annotations.get('uilabel'))
        state.var_annotations = None

    def transform_pass3(self, statement, sep):
        cg = self.machine
        if cg.var_annotations is None:
            return
        classname = cg.classname()
        vtype, vname = self.match.groups()
        cg.var_annotations = None
        shape = cg.context.get(classname, {}).get('vars', {})\
                          .get(vname, {}).get('shape', None)
        if shape is None:
            return
        s = statement + sep + '\n'
        s += '  std::vector<int> cycpp_shape_{0};\n'.format(vname)
        return s

    def transform(self, statement, sep):
        if isinstance(self.machine, StateAccumulator):
            rtn = self.transform_pass2(statement, sep)
        elif isinstance(self.machine, CodeGenerator):
            rtn = self.transform_pass3(statement, sep)
        else:
            rtn = None
        return rtn

    def _canonize_ann(self, defaults, t, name, ann=None):
        """Computes the nested default annotation structure for a C++ type for with the
        given state variable name and defaults for non-nested containers.
        """
        if isinstance(t, STRING_TYPES):
            return ann or name
        template = defaults[t[0]]
        # expand ann if needed
        if ann is None:
            ann = [None] * len(t)
        elif isinstance(ann, STRING_TYPES):
            ann = [ann] + [None]*(len(t) - 1)
        elif len(ann) < len(t):
            ann = ann + [None]*(len(t) - len(ann))
        # find template name
        if template[0] is None:
            t0 = ann[0] or name
        else:
            # expand t0 ann if needed
            if ann[0] is None:
                ann[0] = [None] * len(template[0])
            elif isinstance(ann[0], STRING_TYPES):
                ann[0] = [ann[0]] + [None]*(len(template[0]) - 1)
            elif len(ann[0]) < len(template[0]):
                ann[0] = ann[0] + [None]*(len(template[0]) - len(ann[0]))
            t0 = [ann[0][0] or name] + \
                 [ai or ti for ai, ti in zip(ann[0][1:], template[0][1:])]
        a = [t0]
        for t_i, template_i, ann_i in zip(t[1:], template[1:], ann[1:]):
            a.append(self._canonize_ann(defaults, t_i, template_i, ann=ann_i))
        return a

    _default_aliases = {
        'std::vector': (None, 'val'),
        'std::set': (None, 'val'),
        'std::list': (None, 'val'),
        'std::pair': (None, 'first', 'second'),
        'std::map': ((None, 'item'), 'key', 'val'),
        }

    def canonize_alias(self, t, name, alias=None):
        """Computes the default alias structure for a C++ type for with the given state
        variable name.
        """
        return self._canonize_ann(self._default_aliases, t, name, alias)

    _default_ui = {
        'std::vector': (None, ''),
        'std::set': (None, ''),
        'std::list': (None, ''),
        'std::pair': (None, '', ''),
        'std::map': ((None, ''), '', ''),
        }

    def canonize_tooltip(self, t, name, tooltip=None):
        """Computes the default tooltip structure for a C++ type for with the given state
        variable name.
        """
        return self._canonize_ann(self._default_ui, t, name, tooltip)

    def canonize_uilabel(self, t, name, uilabel=None):
        """Computes the default uilabel structure for a C++ type for with the given state
        variable name.
        """
        return self._canonize_ann(self._default_ui, t, name, uilabel)

class ExecFilter(Filter):
    """Filter for executing arbitrary python code in the exec pragma and
    adding the results to the context.  This pragma has the form:

        #pragma cyclus exec <code>

    Any Python statement(s) are valid as part of the code block. Be a little
    careful when using this pragma :).
    """
    regex = re.compile(r"#\s*pragma\s+cyclus\s+exec\s+(.*)")

    def transform(self, statement, sep):
        execns = self.machine.execns
        context = self.machine.context
        raw = self.match.group(1)
        exec(raw, context, execns)
        del context['__builtins__']

class NoteDecorationFilter(DecorationFilter):
    """Filter for handling annotation decoration of the form:

        #pragma cyclus note <dict>

    This evals the contents of dict and merges them in as the class-level
    annotations dict.
    """
    regex = re.compile(r"\s*#\s*pragma\s+cyclus\s+note\s+(.*)")

    def transform(self, statement, sep):
        state = self.machine
        context = state.context
        classname = state.classname()
        annotations = self._eval()
        state.ensure_class_context(classname)
        self.update(context[classname], annotations)

    def update(self, old, new):
        """Updates the new annotations dictionary into the old one in-place recursively."""
        for key, val in new.items():
            if key not in old:
                old[key] = val
            elif isinstance(val, Mapping) and isinstance(old[key], Mapping):
                self.update(old[key], val)
            else:
                old[key] = val

class StateAccumulator(object):
    """The StateAccumulator class is the pass 2 state machine.

    This represents the state of the file as it is being traversed.
    At the end of the traversal this will have acquired all of the information
    needed for pass 2. It manages both the decorators and other needed bits
    of C++ syntax. It works by passing each statement through a sequence of
    filters, and builds up or destroys context as it goes.

    This class also functions as a typesystem for the types it sees.
    """

    def __init__(self):
        self.depth = 0
        self.execns = {}   # execution namespace we have accumulated
        self.context = {}  # classes we have accumulated
        # stack of (depth, class name) tuples, most nested is last
        self.classes = []
        self.superclasses = {}  # map from classes to set of super classes.
        self.access = {}   # map of (classnames, current access control flags)
        self.namespaces = []  # stack of (depth, ns name) tuples
        self.using_namespaces = set()  # set of (depth, ns name) tuples
        self.aliases = set()  # set of (depth, name, alias) tuples
        self.var_annotations = None
        self.linemarkers = []
        self.nlines_since_linemarker = -1
        self.filters = [ClassAndSuperclassFilter(self), AccessFilter(self),
                        ExecFilter(self), UsingNamespaceFilter(self),
                        NamespaceAliasFilter(self), NamespaceFilter(self),
                        TypedefFilter(self), UsingFilter(self), LinemarkerFilter(self),
                        NoteDecorationFilter(self),
                        VarDecorationFilter(self), VarDeclarationFilter(self),
                        PragmaCyclusErrorFilter(self),
                        ]

    def classname(self):
        """Returns the current, fully-expanded class name."""
        names = [n for d, n in self.namespaces]
        names += [n for d, n in self.classes]
        return "::".join(names)

    def ensure_class_context(self, classname):
        """Ensures that the context for the class at heand is well-formed."""
        if classname not in self.context:
            self.context[classname] = OrderedDict()
            parents = self.superclasses[classname]
            all_parents = parent_classes(classname, self.superclasses)
            for parent, entity in ENTITIES:
                if parent in all_parents:
                    break
            else:
                entity = 'unknown'
            self.context[classname]['name'] = classname
            self.context[classname]['entity'] = entity
            self.context[classname]['parents'] = sorted(parents)
            self.context[classname]['all_parents'] = sorted(all_parents)
        if 'vars' not in self.context[classname]:
            self.context[classname]['vars'] = OrderedDict()

    def accumulate(self, statement, sep):
        """Modify the existing state by incoprorating the statement, which is
        partitioned from the next statement by sep.
        """
        self.nlines_since_linemarker += statement.count('\n') + sep.count('\n')
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

    def includeloc(self, statement=None):
        """Current location of the file from includes as a string."""
        if len(self.linemarkers) == 0:
            return ""
        s = "\n Included from:\n  " + "\n  ".join([lm[0] + ":" + str(lm[1]) \
                                                  for lm in self.linemarkers])
        if statement is not None:
            s += "\n Snippet from " + self.linemarkers[-1][0] + ":\n  " + statement
        return s + "\n"

    #
    # type system
    #
    supported_types = PRIMITIVES
    supported_types |= BUFFERS
    supported_types |= {CYCNS+'::Resource',
                        CYCNS+'::Material',
                        CYCNS+'::Product',}
    known_templates = {
        'std::vector': ('T',),
        'std::set': ('T',),
        'std::list': ('T',),
        'std::pair': ('T1', 'T2'),
        'std::map': ('Key', 'T'),
        '{0}::toolkit::ResBuf'.format(CYCNS): ('T',),
        '{0}::toolkit::ResMap'.format(CYCNS): ('K', 'R'),
        }
    scopz = '::'  # intern the scoping operator

    def canonize_type(self, t, name="<member variable>", statement=None):
        """Returns the canonical form for a type given the current state.
        This should not be called for types other than state variables.
        The name argument here is provided for debugging & reporting purposes.
        """
        scopz = self.scopz
        if not isinstance(t, STRING_TYPES) and isinstance(t, Sequence):
            # template type
            tname, targs = t[0], t[1:]
            if tname in self.known_templates:
                return self._canonize_targs(tname, targs)
            taliases = [x for x in self.aliases if x[2] == tname]
            if len(taliases) > 0:
                taliases.sort()  # gets the alias at the maximum nesting
                talias = taliases[-1][1]
                return self._canonize_targs(talias, targs)
            for d, nsa in sorted(self.using_namespaces, reverse=True):
                if len(tname.split(scopz)) > len(nsa.split(scopz)):
                    # fixed point of reccursion when type would be more scoped than
                    # the alias - which is impossible.
                    continue
                try:
                    return self.canonize_type([nsa + scopz + tname] + targs, name,
                                              statement=statement)
                except TypeError:
                    pass  # This is the TypeError from below
            else:
                msg = ("{i}The type of {c}::{n} ({t}) is not a recognized "
                       "template type: {p}.").format(i=self.includeloc(statement=statement),
                       t=t, n=name, c=self.classname(),
                       p=", ".join(sorted(self.known_templates)))
                raise TypeError(msg)
        elif '<' in t:
            # string version of template type
            t = " ".join(t.strip().strip(scopz).split())
            t = self.canonize_type(parse_template(t), name=name, statement=statement)
        else:
            # primitive type
            t = " ".join(t.strip().strip(scopz).split())
            if t in self.supported_types:
                return t
            # grab aliases of t
            taliases = [x for x in self.aliases if x[2] == t]
            if len(taliases) > 0:
                taliases.sort()  # gets the alias at the maximum nesting
                talias = taliases[-1][1]
                return self.canonize_type(talias, name)
            for d, nsa in sorted(self.using_namespaces, reverse=True):
                if len(t.split(scopz)) > len(nsa.split(scopz)):
                    # fixed point of reccursion when type would be more scoped than
                    # the alias - which is impossible.
                    continue
                try:
                    return self.canonize_type(nsa + scopz + t, name)
                except TypeError:
                    pass  # This is the TypeError from below
            else:
                msg = ("{i}The type of {c}::{n} ({t}) is not a recognized "
                       "primitive type: {p}.").format(
                    i=self.includeloc(), t=t, n=name, c=self.classname(),
                    p=pformat(self.supported_types))
                raise TypeError(msg)
        return t

    def _canonize_targs(self, newtname, targs):
        newt = [newtname]
        newt += [self.canonize_type(targ) for targ in targs]
        return tuple(newt)

    def canonize_shape(self, ann_dict):
        """This canonizes a shape. We take a look at the current shape, and
        standardize its format if necessary. We append -1's to the shape if
        its length does not match our expected length. Returns the new shape
        or raises an error if the given shape was longer than expected.
        """
        type_canon = ann_dict['type']
        try:
            current_shape = ann_dict['shape']
        except KeyError:
            ann_dict['shape'] = None
            current_shape = ann_dict['shape']
        new_shape = []
        # Flatten list dimensions
        if isinstance(type_canon, STRING_TYPES):
            result = [type_canon]
        else:
            result = list(type_canon)
            i = 0
            while i < len(result):
                if isinstance(result[i], STRING_TYPES):
                    i += 1
                else:
                    temp = result[i][1:]
                    i += 1
                    for j in range(len(temp)):
                        result.insert(i+j, temp[j])
        expected_shape_length = len(result)
        # Shape wasn't given.
        if current_shape is None:
            new_shape = [-1] * expected_shape_length
        # Shape is correct as is.
        elif len(current_shape) == expected_shape_length:
            new_shape = current_shape
        # Shape is too short.
        elif len(current_shape) < expected_shape_length:
            diff = expected_shape_length - len(current_shape)
            new_shape = current_shape.extend([-1] * diff)
        # Shape is too long- we throw an error.
        elif len(current_shape) > expected_shape_length:
            err_string = ("Shape array for type {t} is not formatted correctly."
                          " Expected length {length} or less.")
            raise ValueError(err_string.format(t=str(type_canon),
                                               length=str(expected_shape_length)))
        return new_shape

    def canonize_class(self, cls, _usens=True):
        """This canonizes a classname.  The class name need not be the current
        class whose scope we are in, but may be any class whatsoever. Returns
        None if the class could not be canonized.
        """
        if cls in self.superclasses:
            return cls
        cls = cls.strip("::")
        scope = [ns for d, ns in self.namespaces] + [c for d, c in self.classes]
        # see if the class in in scope somehow
        for i in range(1, len(scope) + 1)[::-1]:
            trycls = "::".join(scope[:i]) + "::" + cls
            if trycls in self.superclasses:
                return trycls
        # see if there are usings that modify the scope
        if _usens:
            for d, ns in self.using_namespaces:
                trycls = self.canonize_class(ns + '::' + cls, _usens=False)
                if trycls is not None:
                    return trycls
        # see if there is an alias that applies
        for d, name, alias in sorted(self.aliases, key=len, reverse=True):
            if cls.startswith(alias):
                trycls = self.canonize_class(cls.replace(alias, name, 1))
                if trycls in self.superclasses:
                    return trycls

def accumulate_state(canon):
    """Takes a canonical C++ source file and separates it out into statements
    which are fed into a state accumulator. The state is returned.
    """
    state = StateAccumulator()
    for m in RE_STATEMENT.finditer(canon):
        if m is None:
            continue
        prefix1, prefix2, statement, _, sep = m.groups()
        statement = statement if prefix2 is None else prefix2 + statement
        statement = statement if prefix1 is None else prefix1 + statement
        statement = statement.strip()
        state.accumulate(statement, sep)
    return state.context, state.superclasses

#
# pass 3
#
class CodeGeneratorFilter(Filter):
    re_template = (r"\s*#\s*pragma\s+cyclus\s*?"
                   r"(\s+def\s+|\s+decl\s+|\s+impl\s+|\s*?)?"
                   r"(?:\s*?{0}\s*?)(\s+?(?:[\w:\.]+)?)?")

    def_template = "\n{ind}{virt}{rtn} {ns}{methodname}({args}){sep}\n"

    def __init__(self, *args, **kwargs):
        super(CodeGeneratorFilter, self).__init__(*args, **kwargs)
        pragmaname = self.pragmaname
        self.regex = re.compile(self.re_template.format(pragmaname), re.DOTALL)
        self.local_classname = None  # class we are currently in, if any
        # class we determine from pragma, if any, Note that we have no way of
        # reliably guessing scope on pass 3. Users will either *have* to give
        # class names that are in the current namespace (Spy) or use classnames
        # that are fully qualified (mi6::Spy).
        self.given_classname = None
        self.mode = 'def'

    def transform(self, statement, sep):
        # basic setup
        cg = self.machine
        groups = self.match.groups()
        self.mode = (groups[0] or 'def').strip()
        if len(self.mode) == 0:
            self.mode = "def"
        classname = groups[1] if len(groups) > 1 else None
        if classname is None:
            if len(cg.classes) == 0:
                TypeError("{0}Classname could not determined".format(
                        cg.includeloc()))
            classname = cg.classname()
        classname = classname.strip().replace('.', '::')
        context = cg.context
        self.given_classname = classname
        self.local_classname = cg.classname()

        # compute def line
        cg.ensure_class_context(classname)
        in_class_decl = self.in_class_decl()
        ns = "" if in_class_decl else cg.scoped_classname(classname) + "::"
        virt = "virtual " if in_class_decl else ""
        end = ";" if self.mode == "decl" else " {"
        ind = 2 * (cg.depth - len(cg.namespaces))
        definition = self.def_template.format(ind=" "*ind, virt=virt,
                        rtn=self.methodrtn, ns=ns, methodname=self.methodname,
                        args=self.methodargs(), sep=end)

        # compute implementation
        impl = ""
        ind += 2
        if self.mode != "decl":
            impl = self.impl(ind=ind * " ")
        ind -= 2
        if not impl.endswith("\n") and 0 != len(impl):
            impl += '\n'
        end = "" if self.mode == "decl" else " " * ind + "};\n"

        # compute return
        if self.mode == 'impl':
            return impl
        else:
            return definition + impl + end

    def methodargs(self):
        # overwriteable
        return ""

    def in_class_decl(self):
        return (len(self.machine.classes) > 0 and
                self.given_classname == self.local_classname)

    def revert(self, statement, sep):
        super(CodeGeneratorFilter, self).revert(statement, sep)
        self.local_classname = None
        self.given_classname = None

    def shapes_impl(self, ctx, ind="  "):
        s = ""
        for vname, annotations in ctx.items():
            if not isinstance(annotations, Mapping):
                # this member is a variable alias pointer
                continue

            shape = annotations.get('shape', None)
            if shape is None:
                continue
            shapename = "cycpp_shape_{0}".format(vname)
            s += ('{3}int raw{0}[{1}] = {{{2}}};\n'
                  '{3}{0} = std::vector<int>(raw{0}, raw{0} + {1});\n'
                  ).format(shapename, len(shape), ", ".join(map(str, shape)), ind)
        return s

class CloneFilter(CodeGeneratorFilter):
    """Filter for handling Clone() code generation:
        #pragma cyclus [def|decl|impl] clone [classname]
    """
    methodname = "Clone"
    pragmaname = "clone"
    methodrtn = "{0}::Agent*".format(CYCNS)

    def impl(self, ind="  "):
        classname = self.given_classname
        impl = ""
        impl += ind + "{0}* m = new {0}(context());\n".format(classname)
        impl += ind + "m->InitFrom(this);\n"
        impl += ind + "return m;\n"
        return impl

class InitFromCopyFilter(CodeGeneratorFilter):
    """Filter for handling copy-constructor-like InitFrom() code generation:
        #pragma cyclus [def|decl|impl] initfromcopy [classname]
    """
    methodname = "InitFrom"
    pragmaname = "initfromcopy"
    methodrtn = "void"

    def methodargs(self):
        return "{0}* m".format(self.given_classname)

    def impl(self, ind="  "):
        cg = self.machine
        context = cg.context
        ctx = context[self.given_classname]['vars']
        impl = ""

        # add inheritance init froms
        rents = parent_intersection(self.given_classname, WRANGLERS,
                                    self.machine.superclasses)
        for rent in rents:
            impl += ind + "{0}::InitFrom(m);\n".format(rent)

        impl += self.shapes_impl(ctx, ind)
        cap_buffs = {}
        for member, info in ctx.items():
            if not isinstance(info, Mapping):
                # this member is a variable alias pointer
                continue

            if self.pragmaname in info:
                impl += info[self.pragmaname]
            elif info['type'] not in BUFFERS:
                impl += ind + "{0} = m->{0};\n".format(member)
            else:
                cap_buffs[member] = info

        for b, info in cap_buffs.items():
            t_info = info['type']
            key = t_info if isinstance(t_info, STRING_TYPES) else t_info[0]
            t_impl = self.res_impl.get(key, None)
            if t_impl is None:
                msg = 'type {0!r} could not be found for InitFromCopy() code gen.'
                raise TypeError(msg.format(t_info))
            t_impl = t_impl.format(var=b)
            impl += ind + t_impl.replace('\n', '\n' + ind).strip(' ')

        return impl

    res_impl = {
        CYCNS + '::toolkit::ResBuf': "{var}.capacity(m->{var}.capacity());\n",
        CYCNS + '::toolkit::ResMap': '{var}.obj_ids(m->obj_ids());\n',
        CYCNS + '::toolkit::TotalInvTracker': "{var}.capacity();\n",
        }

class InitFromDbFilter(CodeGeneratorFilter):
    """Filter for handling db-constructor-like InitFrom() code
    generation:
    #pragma cyclus [def|decl|impl] initfromdb [classname]
    """
    methodname = "InitFrom"
    pragmaname = "initfromdb"
    methodrtn = "void"

    def methodargs(self):
        return "{0}::QueryableBackend* b".format(CYCNS)

    def impl(self, ind="  "):
        cg = self.machine
        context = cg.context
        ctx = context[self.given_classname]['vars']
        impl = ''
        # add inheritance init froms
        rents = parent_intersection(self.given_classname, WRANGLERS,
                                    self.machine.superclasses)
        for rent in rents:
            impl += ind + "{0}::InitFrom(b);\n".format(rent)
        # create body
        cap_buffs = {}
        impl += self.shapes_impl(ctx, ind)
        impl += ind + '{0}::QueryResult qr = b->Query("Info", NULL);\n'.format(CYCNS)
        for member, info in ctx.items():
            if not isinstance(info, Mapping):
                # this member is a variable alias pointer
                continue

            if self.pragmaname in info:
                impl += info[self.pragmaname]
                continue
            t = info['type']
            key = t if isinstance(t, STRING_TYPES) else t[0]
            if key in BUFFERS:
                if 'capacity' not in info:
                    info = dict(info)
                    info['capacity'] = 1e300
                cap_buffs[member] = info
                continue
            tstr = type_to_str(t)
            if tstr.endswith('>'):
                tstr += ' '
            impl += ind + '{0} = qr.GetVal<{1}>("{0}");\n'.format(member, tstr)

        for b, info in cap_buffs.items():
            t_info = info['type']
            key = t_info if isinstance(t_info, STRING_TYPES) else t_info[0]
            t_impl = self.res_impl.get(key, None)
            if t_impl is None:
                msg = 'type {0!r} could not be found for InitFromDb() code gen.'
                raise TypeError(msg.format(t_info))
            t_impl = t_impl.format(var=b, tstr=type_to_str(t_info), **info)
            impl += ind + t_impl.replace('\n', '\n' + ind).strip(' ')

        return impl

    res_impl = {
        CYCNS + '::toolkit::ResBuf': '{var}.capacity({capacity});\n',
        CYCNS + '::toolkit::ResMap': (
            '{var}.obj_ids(qr.GetVal<{tstr}>("{var}"))\n;'),
        CYCNS + '::toolkit::TotalInvTracker': '{var}.capacity();\n',
        }


class InfileToDbFilter(CodeGeneratorFilter):
    """Filter for handling InfileToDb() code generation:
        #pragma cyclus [def|decl|impl] infiletodb [classname]
    """
    methodname = "InfileToDb"
    pragmaname = "infiletodb"
    methodrtn = "void"

    def __init__(self, *args, **kwargs):
        super(InfileToDbFilter, self).__init__(*args, **kwargs)
        self.readers = {
            'bool': self.read_primitive,
            'int': self.read_primitive,
            'float': self.read_primitive,
            'double': self.read_primitive,
            'std::string': self.read_primitive,
            'cyclus::Blob': self.read_primitive,
            'boost::uuids::uuid': self.read_primitive,
            'std::vector': self.read_vector,
            'std::set': self.read_set,
            'std::list': self.read_list,
            'std::pair': self.read_pair,
            'std::map': self.read_map,
            }
        self._vals = {
            'bool': self._val_bool,
            'int': self._val_int,
            'float': self._val_floating,
            'double': self._val_floating,
            'std::string': self._val_string,
            'cyclus::Blob': self._val_blob,
            'boost::uuids::uuid': self._val_uuid,
            'std::vector': self._val_vector,
            'std::set': self._val_set,
            'std::list': self._val_list,
            'std::pair': self._val_pair,
            'std::map': self._val_map,
            }
        self._idx_lev = 0

    @contextmanager
    def _nest_idx(self):
        self._idx_lev += 1
        yield
        self._idx_lev -= 1

    def methodargs(self):
        return "{0}::InfileTree* tree, {0}::DbInit di".format(CYCNS)

    def _fmt(self, t):
        """returns a format string for a type t"""
        return '"{0}"' if t == 'std::string' else '{0}'

    def _val(self, t, val=None, name=None, uitype=None, ind=''):
        """Returns a string that represents a Python value (val) of a given
        type (t) in C++. For types that do not have an expression
        representation, the variable (name) may also be used. If a value
        is not provided, the default for the type will be provided.
        """
        key = t if isinstance(t, STRING_TYPES) else t[0]
        if val is None:
            return self._vals[key](t, name=name, uitype=uitype, ind=ind)
        else:
            return self._vals[key](t, val=val, name=name, uitype=uitype, ind=ind)

    def _val_bool(self, t, val=False, name='boolvar', uitype=None, ind=''):
        return ind + 'bool {0} = {1};\n'.format(name, 'true' if val else 'false')

    def _val_int(self, t, val=0, name='intvar', uitype=None, ind=''):
        if uitype == 'nuclide':
            fmt = '"{0}"' if isinstance(val, STRING_TYPES) else '{0}'
            v = fmt.format(val)
            v = ind + 'int {0} = pyne::nucname::id({1});\n'.format(name, v)
        else:
            v = ind + 'int {0} = {1};\n'.format(name, val)
        return v

    def _val_floating(self, t, val=0.0, name='floatvar', uitype=None, ind=''):
        return ind + 'double {0} = {1};\n'.format(name, val)

    def _val_string(self, t, val='', name='stringvar', uitype=None, ind=''):
        return ind + 'std::string {0}("{1}");\n'.format(name, val)

    def _val_blob(self, t, val='', name='blobvar', uitype=None, ind=''):
        return ind + 'cyclus::Blob {0}("{1}");\n'.format(name, val)

    def _val_uuid(self, t, val='', name='uuidvar', uitype=None, ind=''):
        v = ind + 'boost::uuids::uuid {0} = '.format(name)
        if isinstance(val, STRING_TYPES):
            v += '"{0}"'.format(val)
        elif isinstance(val, uuid.UUID):
            l = [x+y for x, y in zip(val.hex[::1], val.hex[1::2])]
            v += '{0x' + ', 0x'.join(l) + '}'
        else:
            msg = "could not interpret UUID type of {0}"
            raise TypeError(msg.format(val))
        return v + ';\n'

    def _val_vector(self, t, val=(), name='vecvar', uitype=None, ind=''):
        uitype = prepare_type(t, uitype)
        elemname = 'elem'

        v = ind + '{0} {1};\n'.format(type_to_str(t), name)
        v += ind + '{0}.resize({1});\n'.format(name, len(val))
        v += ind + '{\n'
        ind += '  '
        for i, x in enumerate(val):
            v += ind + '{\n'
            ind += '  '
            v += self._val(t[1], val=x, uitype=uitype[1],
                           name=elemname, ind=ind)
            v += ind + '{0}[{1}] = {2};\n'.format(name, i, elemname)
            ind = ind[:-2]
            v += ind + '}\n'
        ind = ind[:-2]
        v += ind + '}\n'

        return v

    def _val_set(self, t, val=frozenset(), name='setvar', uitype=None, ind=''):
        uitype = prepare_type(t, uitype)
        elemname = 'elem'

        v = ind + '{0} {1};\n'.format(type_to_str(t), name)
        v += ind + '{\n'
        ind += '  '
        for i, x in enumerate(val):
            v += ind + '{\n'
            ind += '  '
            v += self._val(t[1], val=x, uitype=uitype[1],
                           name=elemname, ind=ind)
            v += ind + '{0}.insert({1});\n'.format(name, elemname)
            ind = ind[:-2]
            v += ind + '}\n'
        ind = ind[:-2]
        v += ind + '}\n'

        return v

    def _val_list(self, t, val=(), name='listvar', uitype=None, ind=''):
        uitype = prepare_type(t, uitype)
        elemname = 'elem'

        v = ind + '{0} {1};\n'.format(type_to_str(t), name)
        v += ind + '{\n'
        ind += '  '
        for i, x in enumerate(val):
            v += ind + '{\n'
            ind += '  '
            v += self._val(t[1], val=x, uitype=uitype[1],
                           name=elemname, ind=ind)
            v += ind + '{0}.push_back({1});\n'.format(name, elemname)
            ind = ind[:-2]
            v += ind + '}\n'
        ind = ind[:-2]
        v += ind + '}\n'

        return v

    def _val_pair(self, t, val=(None, None), name=None,
                  uitype=(None, None, None), ind=''):
        ftype, stype = t[1], t[2]
        uitype = prepare_type(t, uitype)

        fname, sname = 'first', 'second'
        v = ind + '{0} {1};\n'.format(type_to_str(t), name)
        v += ind + '{\n'
        ind += '  '
        v += self._val(ftype, val=val[0], uitype=uitype[1], name=fname, ind=ind)
        v += self._val(stype, val=val[1], uitype=uitype[2], name=sname, ind=ind)
        v += ind + '{0}.first = {1};\n'.format(name, fname)
        v += ind + '{0}.second = {1};\n'.format(name, sname)
        ind = ind[:-2]
        v += ind + '}\n'
        return v

    def _val_map(self, t, val=None, name=None, uitype=None, ind=''):
        uitype = prepare_type(t, uitype)
        keyname = 'key'
        valname = 'val'

        v = ind + '{0} {1};\n'.format(type_to_str(t), name)
        v += ind + '{\n'
        ind += '  '
        for k, x in val.items():
            v += ind + '{\n'
            ind += '  '
            v += self._val(t[1], val=k, uitype=uitype[1], name=keyname, ind=ind)
            v += self._val(t[2], val=x, uitype=uitype[2], name=valname, ind=ind)
            v += ind + '{0}[{1}] = {2};\n'.format(name, keyname, valname)
            ind = ind[:-2]
            v += ind + '}\n'
        ind = ind[:-2]
        v += ind + '}\n'

        return v

    def _query(self, tree, alias, t, uitype=None, idx=None, path=''):
        tstr = type_to_str(t)
        if tstr.endswith('>'):
            tstr += " "
        # Get keys
        kw = {'cycns': CYCNS, 'type': tstr, 'alias': alias, 'tree': tree,
              'path': path}
        kw['index'] = '' if idx is None else ', {0}'.format(idx)
        # get template
        if uitype == 'nuclide':
            template = ('pyne::nucname::id({cycns}::Query<std::string>({tree}, '
                        '"{path}{alias}"{index}))')
        else:
            template = '{cycns}::Query<{type}>({tree}, "{path}{alias}"{index})'
        # fill in template and return
        return template.format(**kw)

    def read_member(self, member, alias, t, uitype=None, ind='  ', idx=None,
                    path=''):
        uitype = prepare_type(t, uitype)
        alias = prepare_type(t, alias)

        s = ind + '{0} {1};\n'.format(type_to_str(t), member)
        mname = member + '_in'
        tt = t if isinstance(t, STRING_TYPES) else t[0]
        reader = self.readers.get(tt, None)
        s += ind + '{\n'
        ind += '  '
        s += reader(mname, alias, t, uitype, ind=ind, idx=idx, path=path)
        s += ind + '{0} = {1};\n'.format(member, mname)
        ind = ind[:-2]
        s += ind + '}\n'
        return s

    def read_primitive(self, member, alias, t, uitype=None, ind='  ', idx=None,
                       path=''):
        query = self._query('sub', alias, t, uitype, idx=idx, path=path)
        s = ind + '{t} {member} = {query};\n'.format(t=t, member=member, query=query)
        return s

    def read_vector(self, member, alias, t, uitype=None, ind='  ', idx=None,
                    path=''):
        uitype = prepare_type(t, uitype)
        alias = prepare_type(t, alias)
        if alias[1] is None:
            val = 'val'
            alias[1] = val
        elif isinstance(alias[1], STRING_TYPES):
            val = alias[1]
        else:
            val = alias[1][0]

        # the extra assignment (bub, sub) is because we want the intial sub
        # rhs to be from outer scope - otherwise the newly defined sub will be
        # in scope causing segfaults
        tree_idx = idx or '0'
        s = '{ind}{0}::InfileTree* bub = sub->SubTree("{path}{1}", {2});\n'
        s = s.format(CYCNS, alias[0], tree_idx, path=path, ind=ind)
        s += ind + '{0}::InfileTree* sub = bub;\n'.format(CYCNS)
        with self._nest_idx():
            lev = self._idx_lev
            s += ind + 'int n{lev} = sub->NMatches("{0}");\n'.format(
                val, lev=lev)
            s += ind + '{0} {1};\n'.format(type_to_str(t), member)
            s += ind + '{0}.resize(n{lev});\n'.format(member, lev=lev)
            s += ind + 'for (int i{lev} = 0; i{lev} < n{lev}; ++i{lev})'.format(
                lev=lev) + ' {\n'
            s += self.read_member(
                'elem', alias[1], t[1], uitype[1],
                ind+'  ', idx='i{lev}'.format(lev=lev))
            s += ind + '  {0}[{idx}] = elem;\n'.format(
                member, idx='i{lev}'.format(lev=lev))
            s += ind + '}\n'
        return s

    def read_set(self, member, alias, t, uitype=None, ind="  ", idx=None,
                 path=''):
        uitype = prepare_type(t, uitype)
        alias = prepare_type(t, alias)
        if alias[1] is None:
            val = 'val'
            alias[1] = val
        elif isinstance(alias[1], STRING_TYPES):
            val = alias[1]
        else:
            val = alias[1][0]
        # the extra assignment (bub, sub) is because we want the intial sub
        # rhs to be from outer scope - otherwise the newly defined sub will be
        # in scope causing segfaults
        tree_idx = idx or '0'
        s = '{ind}{0}::InfileTree* bub = sub->SubTree("{path}{1}", {2});\n'
        s = s.format(CYCNS, alias[0], tree_idx, path=path, ind=ind)
        s += ind + '{0}::InfileTree* sub = bub;\n'.format(CYCNS)
        with self._nest_idx():
            lev = self._idx_lev
            s += ind + 'int n{lev} = sub->NMatches("{0}");\n'.format(
                val, lev=self._idx_lev)
            s += ind + '{0} {1};\n'.format(type_to_str(t), member)
            s += ind + 'for (int i{lev} = 0; i{lev} < n{lev}; ++i{lev})'.format(
                lev=self._idx_lev) + ' {\n'
            s += self.read_member(
                'elem', alias[1], t[1], uitype[1],
                ind+'  ', idx='i{lev}'.format(lev=self._idx_lev))
            s += ind + '  {0}.insert(elem);\n'.format(member)
            s += ind + '}\n'
        return s

    def read_list(self, member, alias, t, uitype=None, ind="  ", idx=None,
                  path=''):
        uitype = prepare_type(t, uitype)
        alias = prepare_type(t, alias)
        if alias[1] is None:
            val = 'val'
            alias[1] = val
        elif isinstance(alias[1], STRING_TYPES):
            val = alias[1]
        else:
            val = alias[1][0]
        # the extra assignment (bub, sub) is because we want the intial sub
        # rhs to be from outer scope - otherwise the newly defined sub will be
        # in scope causing segfaults
        tree_idx = idx or '0'
        s = '{ind}{0}::InfileTree* bub = sub->SubTree("{path}{1}", {2});\n'
        s = s.format(CYCNS, alias[0], tree_idx, path=path, ind=ind)
        s += ind + '{0}::InfileTree* sub = bub;\n'.format(CYCNS)
        with self._nest_idx():
            lev = self._idx_lev
            s += ind + 'int n{lev} = sub->NMatches("{0}");\n'.format(
                val, lev=lev)
            s += ind + '{0} {1};\n'.format(type_to_str(t), member)
            s += ind + 'for (int i{lev} = 0; i{lev} < n{lev}; ++i{lev})'.format(
                lev=lev) + ' {\n'
            s += self.read_member(
                'elem', alias[1], t[1], uitype[1],
                ind+'  ', idx='i{lev}'.format(lev=lev))
            s += ind + '  {0}.push_back(elem);\n'.format(member)
            s += ind + '}\n'
        return s

    def read_pair(self, member, alias, t, uitype=None, ind="  ", idx=None,
                  path=''):
        uitype = prepare_type(t, uitype)
        alias = prepare_type(t, alias)
        if alias[1] is None:
            alias[1] = 'first'
        if alias[2] is None:
            alias[2] = 'second'
        # the extra assignment (bub, sub) is because we want the intial sub
        # rhs to be from outer scope - otherwise the newly defined sub will be
        # in scope causing segfaults
        tree_idx = idx or '0'
        first = 'first{}'.format(tree_idx)
        second = 'second{}'.format(tree_idx)
        s = '{ind}{0}::InfileTree* bub = sub->SubTree("{path}{1}", {2});\n'
        s = s.format(CYCNS, alias[0], tree_idx, path=path, ind=ind)
        s += ind + '{0}::InfileTree* sub = bub;\n'.format(CYCNS)
        s += self.read_member(first, alias[1], t[1], uitype[1], ind+'  ', idx='0')
        s += self.read_member(second, alias[2], t[2], uitype[2], ind+'  ', idx='0')
        s += ind + '{0} {1}({2}, {3});\n'.format(type_to_str(t), member, first, second)
        return s

    def read_map(self, member, alias, t, uitype=None, ind="  ", idx=None,
                 path=''):
        uitype = prepare_type(t, uitype)
        alias = prepare_type(t, alias)
        #import pdb; pdb.set_trace()
        if isinstance(alias[0], STRING_TYPES):
            alias[0] = [alias[0], None]
        if alias[0][1] is None:
            alias[0][1] = 'item'
        if alias[1] is None:
            alias[1] = 'key'
        if alias[2] is None:
            alias[2] = 'val'
        # the extra assignment (bub, sub) is because we want the intial sub
        # rhs to be from outer scope - otherwise the newly defined sub will be
        # in scope causing segfaults
        # subtree must be specified if in recursive level
        itempath = alias[0][1] + '/'
        tree_idx = idx or '0'
        s = '{ind}{0}::InfileTree* bub = sub->SubTree("{path}{1}", {2});\n'
        s = s.format(CYCNS, alias[0][0], tree_idx, path=path, ind=ind)
        s += ind + '{0}::InfileTree* sub = bub;\n'.format(CYCNS)
        with self._nest_idx():
            lev = self._idx_lev
            s += ind + 'int n{lev} = sub->NMatches("{item}");\n'.format(
                        lev=lev, item=alias[0][1])
            s += ind + '{0} {1};\n'.format(type_to_str(t), member)
            s += ind + 'for (int i{lev} = 0; i{lev} < n{lev}; ++i{lev})'.format(
                lev=lev) + ' {\n'
            s += self.read_member('key', alias[1], t[1], uitype[1],
                                  ind+'  ', idx='i{lev}'.format(lev=lev),
                                  path=itempath)
            s += self.read_member('val', alias[2], t[2], uitype[2],
                                  ind+'  ', idx='i{lev}'.format(lev=lev),
                                  path=itempath)
            s += ind + '  {0}[key] = val;\n'.format(member)
            s += ind + '}\n'
        return s

    def impl(self, ind="  "):
        cg = self.machine
        context = cg.context
        ctx = context[self.given_classname]['vars']
        pods = []
        impl = ""

        # add inheritance init froms
        rents = parent_intersection(self.given_classname, WRANGLERS,
                                    self.machine.superclasses)
        for rent in rents:
            impl += ind + "{0}::InfileToDb(tree, di);\n".format(rent)
        impl += self.shapes_impl(ctx, ind)

        # read data from infile onto class
        impl += ind + '{0}::InfileTree* sub = tree->SubTree("config/*");\n'.format(CYCNS)
        impl += ind + 'int i;\n'
        impl += ind + 'int n;\n'
        for member, info in ctx.items():
            if not isinstance(info, Mapping):
                # this member is a variable alias pointer
                continue

            if self.pragmaname in info and 'read' in info[self.pragmaname]:
                impl += info[self.pragmaname]['read']
                continue
            t = info['type']
            key = t if isinstance(t, STRING_TYPES) else t[0]
            uitype = info.get('uitype', None)
            if key in BUFFERS:
                continue
            d = info['default'] if 'default' in info else None

            if 'internal' in info and info['internal'] == True:
                # do NOT combine above and below ifs into a single if
                if d is not None:
                    mname = member + '_tmp'
                    impl += self._val(t, val=d, name=mname, uitype=uitype, ind=ind)
                    impl += ind + '{0} = {1};\n'.format(member, mname)
                else:
                    raise RuntimeError('state variables marked as internal must have a default')
            else:
                labels = info.get('alias', None)
                if labels is None:
                    labels = member if isinstance(t, STRING_TYPES) else [member]
                reader = self.readers.get(t, self.readers.get(t[0], None))

                # generate condition to choose default vs given val if default exists
                if d is not None:
                    name = labels
                    while not isinstance(name, STRING_TYPES):
                        name = name[0]
                    impl += ind + 'if (sub->NMatches("{0}") > 0) {{\n'.format(name)
                    ind += '  '

                mname = member + '_val'
                impl += ind + '{\n'
                impl += reader(mname, labels, t, uitype, ind+'  ')
                impl += ind + '  {0} = {1};\n'.format(member, mname)
                impl += ind + '}\n'

                # generate code to assign default val if no other is specified
                if d is not None:
                    ind = ind[:-2]
                    impl += ind + '} else {\n'
                    ind += '  '
                    mname = member + '_tmp'
                    impl += self._val(t, val=d, name=mname, uitype=uitype, ind=ind)
                    impl += ind + '{0} = {1};\n'.format(member, mname)
                    ind = ind[:-2]
                    impl += ind + '}\n'

            # this must run after all other codegen for the current statevar:
            if 'derived_init' in info:
                impl += ind + info['derived_init'] + '\n'

        # write obj to database
        impl += ind + 'di.NewDatum("Info")\n'
        for member, info in ctx.items():
            if not isinstance(info, Mapping):
                # this member is a variable alias pointer
                continue

            if self.pragmaname in info and 'write' in info[self.pragmaname]:
                impl += info[self.pragmaname]['write']
                continue
            if info['type'] in BUFFERS:
                continue
            shape = ', &cycpp_shape_{0}'.format(member) if 'shape' in info else ''
            impl += ind + '->AddVal("{0}", {0}{1})\n'.format(member, shape)
        impl += ind + '->Record();\n'
        return impl

class SchemaFilter(CodeGeneratorFilter):
    """Filter for handling schema() code generation:
        #pragma cyclus [def|decl|impl] schema [classname]
    """
    methodname = "schema"
    pragmaname = "schema"
    methodrtn = "std::string"

    alltypes = frozenset(['anyType', 'anySimpleType', 'string', 'boolean', 'decimal',
                          'float', 'double', 'duration', 'dateTime', 'time', 'date',
                          'gYearMonth', 'gYear', 'gMonthDay', 'gDay', 'gMonth',
                          'hexBinary', 'base64Binary', 'anyURI', 'QName', 'NOTATION',
                          'normalizedString', 'token', 'language', 'NMTOKEN',
                          'NMTOKENS', 'Name', 'NCName', 'ID', 'IDREF', 'IDREFS',
                          'ENTITY', 'ENTITIES', 'integer', 'nonPositiveInteger',
                          'negativeInteger', 'long', 'int', 'short', 'byte',
                          'nonNegativeInteger', 'unsignedLong', 'unsignedInt',
                          'unsignedShort', 'unsignedByte', 'positiveInteger'])

    # C++ type -> XML Schema type
    default_types = {
        # Primitive types
        'bool': 'boolean',
        'std::string': 'string',
        'int': 'int',
        'float': 'float',
        'double': 'double',
        'cyclus::Blob': 'string',
        'boost::uuids::uuid': 'token',
        # UI types
        'nuclide': 'string',
        'commodity': None,
        'incommodity': None,
        'outcommodity': None,
        'range': None,
        'combobox': None,
        'facility': None,
        'prototype': None,
        'recipe': None,
        'inrecipe': None,
        'outrecipe': None,
        'package': None,
        'inpackage': None,
        'outpackage': None,
        'none': None,
        None: None,
        '': None,
        }

    def _type(self, cpp, given=None):
        """Finds a schema type for a C++ type with a possible type given."""
        if given is not None:
            if given in self.alltypes:
                return given
            elif given in self.default_types:
                return self.default_types[given] or self.default_types[cpp]
            msg = ("{0}\nNote that {1!r} is not a valid XML schema data type, see "
                   "http://www.w3.org/TR/xmlschema-2/ for more information.")
            cs = self.ctxstr(self.given_classname, self._member)
            raise TypeError(msg.format(cs, given))
        return self.default_types[cpp]

    def _buildschema(self, cpptype, schematype=None, uitype=None, names=None):
        schematype = prepare_type(cpptype, schematype)
        uitype = prepare_type(cpptype, uitype)
        names = prepare_type(cpptype, names)

        impl = ''
        t = cpptype if isinstance(cpptype, STRING_TYPES) else cpptype[0]
        if t in PRIMITIVES:
            name = 'val'
            if names is not None:
                name = names
            d_type = self._type(t, schematype or uitype)
            impl += '<element name="{0}">'.format(name)
            impl += '<data type="{0}" />'.format(d_type)
            impl += '</element>'
        elif t in ['std::list', 'std::set', 'std::vector']:
            name = 'list' if isinstance(cpptype, STRING_TYPES) else ['list']
            if names[0] is not None:
                name = names[0]
            impl += '<element name="{0}">'.format(name)
            impl += '<oneOrMore>'
            impl += self._buildschema(cpptype[1], schematype[1], uitype[1], names[1])
            impl += '</oneOrMore>'
            impl += '</element>'
        elif t == 'std::map':
            name = 'map'
            if names[0] is None or isinstance(names[0], STRING_TYPES):
                names[0] = [names[0], None]
            if names[0][0] is not None:
                name = names[0][0]
            itemname ='item' if names[0][1] is None else names[0][1]
            keynames = 'key' if isinstance(cpptype[1], STRING_TYPES) else ['key']
            if names[1] is not None:
                keynames = names[1]
            valnames = 'val' if isinstance(cpptype[2], STRING_TYPES) else ['val']
            if names[1] is not None:
                valnames = names[2]
            impl += '<element name="{0}">'.format(name)
            impl += '<oneOrMore>'
            impl += '<element name="{0}">'.format(itemname)
            impl += '<interleave>'
            impl += self._buildschema(cpptype[1], schematype[1], uitype[1], keynames)
            impl += self._buildschema(cpptype[2], schematype[2], uitype[2], valnames)
            impl += '</interleave>'
            impl += '</element>'
            impl += '</oneOrMore>'
            impl += '</element>'
        elif t == 'std::pair':
            name = 'pair'
            if names[0] is not None:
                name = names[0]
            firstname = 'first' if isinstance(cpptype[1], STRING_TYPES) else ['first']
            if names[1] is not None:
                firstname = names[1]
            secondname = 'second' if isinstance(cpptype[2], STRING_TYPES) else ['second']
            if names[2] is not None:
                secondname = names[2]
            impl += '<element name="{0}">'.format(name)
            impl += '<interleave>'
            impl += self._buildschema(cpptype[1], schematype[1], uitype[1], firstname)
            impl += self._buildschema(cpptype[2], schematype[2], uitype[2], secondname)
            impl += '</interleave>'
            impl += '</element>'
        else:
            msg = '{0}Unsupported type {1}'.format(self.machine.includeloc(), t)
            raise RuntimeError(msg)

        return impl

    def xml_from_ctx(self, ctx, ind="  "):
        """Creates an XML string for an agent."""
        if len(ctx) == 0:
            return '<text/>'

        xml = '<interleave>'
        for member, info in ctx.items():
            self._member = member
            if not isinstance(info, Mapping):
                # this member is a variable alias pointer
                continue

            alias = member
            if 'alias' in info:
                alias = info['alias']
            if info.get(self.pragmaname, False):
                xml += info[self.pragmaname]
                continue
            t = info['type']
            key = t if isinstance(t, STRING_TYPES) else t[0]
            uitype = info.get('uitype', None)
            schematype = info.get('schematype', None)
            labels = info.get('alias', None)
            if labels is None:
                labels = alias if isinstance(t, STRING_TYPES) else [alias]

            if key in BUFFERS:  # buffer state, skip
                continue
            if info.get('internal', False):
                continue

            opt = info.get('default', None) is not None
            if opt:
                xml += '<optional>'

            xml += self._buildschema(t, schematype, uitype, labels)

            if opt:
                xml += '</optional>'
        xml += '</interleave>'
        del self._member
        return xml

    def impl(self, ind="  "):
        cg = self.machine
        context = cg.context
        ctx = context[self.given_classname]['vars']
        xml = self.xml_from_ctx(ctx, ind=ind)
        return ind + 'return ""\n' + escape_xml(xml, ind=ind+'  ') + ';\n'


class AnnotationsFilter(CodeGeneratorFilter):
    """Filter for handling annotations() code generation:
        #pragma cyclus [def|decl|impl] annotations [classname]
    """
    methodname = "annotations"
    pragmaname = "annotations"
    methodrtn = "Json::Value"

    def impl(self, ind="  "):
        cg = self.machine
        context = cg.context
        ctx = context[self.given_classname]
        s = ind + 'Json::Value root;\n'
        s += ind + 'Json::Reader reader;\n'
        s += ind + 'bool parsed_ok = reader.parse({1}, root);\n'
        s += ind + 'if (!parsed_ok) {{\n'
        s += ind + ('  throw cyclus::ValueError("failed to parse annotations '
                    'for {0}.");\n')
        s += ind + '}}\n'
        s += ind + 'return root;\n'
        jstr = json.dumps(ctx, separators=(',', ':'))
        if len(jstr) > 50:
            tw = textwrap.wrap(jstr, 50, drop_whitespace=False)
            jstr = [j.replace('\\', '\\\\').replace('"', '\\"') for j in tw]
            jstr = ('"\n  ' + ind + '"').join(jstr)
            jstr = '\n  ' + ind + '"' + jstr + '"'
        else:
            jstr = '"' + jstr.replace('"', '\\"') + '"'
        s = s.format(self.given_classname, jstr)
        return s

class SnapshotFilter(CodeGeneratorFilter):
    """Filter for handling copy-constructor-like InitFrom() code generation:
        #pragma cyclus [def|decl|impl] snapshot [classname]
    """
    methodname = 'Snapshot'
    pragmaname = 'snapshot'
    methodrtn = 'void'

    def methodargs(self):
        return CYCNS + '::DbInit di'

    def impl(self, ind="  "):
        cg = self.machine
        context = cg.context
        ctx = context[self.given_classname]['vars']
        impl = ind + 'di.NewDatum("Info")\n'
        for member, info in ctx.items():
            if not isinstance(info, Mapping):
                # this member is a variable alias pointer
                continue
            if self.pragmaname in info:
                impl += info[self.pragmaname]
                continue

            # get expression
            t = info["type"]
            key = t if isinstance(t, STRING_TYPES) else t[0]
            if t in BUFFERS:
                expr = self.res_exprs.get(key, None)
                if expr is None:
                    continue
                expr = expr.format(var=member)
            else:
                expr = member

            shape = ', &cycpp_shape_{0}'.format(member) if 'shape' in info else ''
            impl += ind + '->AddVal("{0}", {1}{2})\n'.format(member, expr, shape)
        impl += ind + "->Record();\n"

        return impl

    res_exprs = {
        CYCNS + '::toolkit::ResBuf': None,
        CYCNS + '::toolkit::ResMap': '{var}.obj_ids()',
        CYCNS + '::toolkit::TotalInvTracker': None,
        }


class SnapshotInvFilter(CodeGeneratorFilter):
    """Filter for handling SnapshotInv() code generation:
        #pragma cyclus [def|decl|impl] snapshotinv [classname]
    """
    methodname = "SnapshotInv"
    pragmaname = "snapshotinv"
    methodrtn = "{0}::Inventories".format(CYCNS)

    def impl(self, ind="  "):
        cg = self.machine
        context = cg.context
        ctx = context[self.given_classname]['vars']
        impl = ""
        buffs = {}
        for member, info in ctx.items():
            if not isinstance(info, Mapping):
                # this member is a variable alias pointer
                continue
            t = info['type']
            if t in BUFFERS or t[0] in BUFFERS:
                buffs[member] = info

        impl = ind + "{0}::Inventories invs;\n".format(CYCNS)

        for buff, info in buffs.items():
            if self.pragmaname in info:
                impl += info[self.pragmaname]
                continue
            t_info = info['type']
            key = t_info if isinstance(t_info, STRING_TYPES) else t_info[0]
            t_impl = self.res_impl.get(key, None)
            if t_impl is None:
                msg = 'type {0!r} could not be found for SnapshotInv() code gen.'
                raise TypeError(msg.format(t_info))
            s = t_impl.format(var=buff, t_str=type_to_str(t_info), **info)
            impl += ind + s.replace('\n', '\n' + ind).strip(' ')

        # if in impl mode, archetype dev is responsible for adding the return
        # statement
        if self.mode != 'impl':
            impl += ind + "return invs;\n"
        return impl

    res_impl = {
        CYCNS + '::toolkit::ResBuf': (
            'invs[\"{var}\"] = {var}.PopNRes({var}.count());\n'
            '{var}.Push(invs["{var}"]);\n'),
        CYCNS + '::toolkit::ResMap': "invs[\"{var}\"] = {var}.ResValues();\n",
        CYCNS + '::toolkit::TotalInvTracker': ';\n',
        }


class InitInvFilter(CodeGeneratorFilter):
    """Filter for handling InitInv() code generation:
        #pragma cyclus [def|decl|impl] initinv [classname]
    """
    methodname = "InitInv"
    pragmaname = "initinv"
    methodrtn = "void"

    def methodargs(self):
        return "{0}::Inventories& inv".format(CYCNS)

    def impl(self, ind="  "):
        cg = self.machine
        context = cg.context
        ctx = context[self.given_classname]['vars']
        impl = ""
        buffs = {}
        for member, info in ctx.items():
            if not isinstance(info, Mapping):
                # this member is a variable alias pointer
                continue

            t = info['type']
            if t in BUFFERS:
                buffs[member] = info

        for buff, info in buffs.items():
            if self.pragmaname in info:
                impl += info[self.pragmaname]
                continue
            t_info = info['type']
            key = t_info if isinstance(t_info, STRING_TYPES) else t_info[0]
            t_impl = self.res_impl.get(key, None)
            if t_impl is None:
                msg = 'type {0!r} could not be found for InitInv() code gen.'
                raise TypeError(msg.format(t_info))
            s = t_impl.format(var=buff)
            impl += ind + s.replace('\n', '\n' + ind)

        return impl

    res_impl = {
        CYCNS + '::toolkit::ResBuf': "{var}.Push(inv[\"{var}\"]);\n",
        CYCNS + '::toolkit::ResMap': "{var}.ResValues(inv[\"{var}\"]);\n",
        CYCNS + '::toolkit::TotalInvTracker': ";\n",
        }


class DefaultPragmaFilter(Filter):
    """Filter for handling default pragma code generation:
        #pragma cyclus [def|decl|impl]
    """
    regex = re.compile(r"\s*#\s*pragma\s+cyclus(\s+def|\s+decl|\s+impl)?\s*$",
                       re.DOTALL)

    def transform(self, statement, sep):
        rtn = ""
        for f in self.machine.codegen_filters:
            f.match = self.match
            rtn += f.transform(statement, sep)
        return rtn

    def revert(self, statement, sep):
        for f in self.machine.codegen_filters:
            f.revert(statement, sep)

class CodeGenerator(object):
    """The CodeGenerator class is the pass 3 state machine.

    This represents the file as code is being injected into it.  At the end of
    the traversal this final stage it will built up a brand new file for pass
    3. It manages both the code insertion pragmas and other bits of C++ syntax
    as needed to determine locality. It works by passing each statement through
    a sequence of filters, and injects code based on the directive and the
    state.
    """

    def __init__(self, context, superclasses, filename=None):
        self.depth = 0
        self.context = context  # the results of pass 2
        self.superclasses = superclasses  # the results of pass 2
        self.statements = []    # the results of pass 3, waiting to be joined
        self.classes = []  # stack of (depth, class name) tuples, most nested is last
        self.access = {}   # map of (classnames, current access control flags)
        self.namespaces = []  # stack of (depth, ns name) tuples
        self.aliases = set()  # set of (depth, name, alias) tuples
        self.linemarkers = []
        self.nlines_since_linemarker = -1
        self.var_annotations = None
        # all basic code generating filters for core methods
        self.codegen_filters = [InitFromCopyFilter(self),
                                InitFromDbFilter(self), InfileToDbFilter(self),
                                CloneFilter(self), SchemaFilter(self),
                                AnnotationsFilter(self), InitInvFilter(self),
                                # SnapshotInv has to come before Snapshot for some
                                # regex reason I don't understand
                                SnapshotInvFilter(self),
                                SnapshotFilter(self),
                                ]
        self.filters = self.codegen_filters + [ClassFilter(self),
                                               AccessFilter(self),
                                               NamespaceAliasFilter(self),
                                               NamespaceFilter(self),
                                               VarDecorationFilter(self),
                                               VarDeclarationFilter(self),
                                               LinemarkerFilter(self),
                                               DefaultPragmaFilter(self),
                                               PragmaCyclusErrorFilter(self),
                                               ]

    def classname(self):
        """Returns the current, fully-expanded class name."""
        names = [n for d, n in self.namespaces]
        names += [n for d, n in self.classes]
        return "::".join(names)

    def scoped_classname(self, classname=None):
        if classname is None:
            classname = self.classname()
        scope = [n for d, n in self.namespaces] + [n for d, n in self.classes[:-1]]
        clspath = classname.split('::')
        clspath, clsname = clspath[:-1], clspath[-1]
        same_prefix = []
        for s, c in zip(scope, clspath):
            if s != c:
                break
            same_prefix.append(s)
        return "::".join(clspath[len(same_prefix):] + [clsname])

    def ensure_class_context(self, classname):
        """Ensures that the context for the class at heand is well-formed."""
        if classname not in self.context:
            self.context[classname] = OrderedDict()
            parents = self.superclasses[classname]
            all_parents = parent_classes(classname, self.superclasses)
            for parent, entity in ENTITIES:
                if parent in all_parents:
                    break
            else:
                entity = 'unknown'
            self.context[classname]['name'] = classname
            self.context[classname]['entity'] = entity
            self.context[classname]['parents'] = sorted(parents)
            self.context[classname]['all_parents'] = sorted(all_parents)
        if 'vars' not in self.context[classname]:
            self.context[classname]['vars'] = OrderedDict()

    def includeloc(self, statement=None):
        """Current location of the file from includes as a string."""
        if len(self.linemarkers) == 0:
            return ""
        s = "\n Included from:\n  " + "\n  ".join([lm[0] + ":" + str(lm[1]) \
                                                  for lm in self.linemarkers])
        if statement is not None:
            s += "\n Snippet from " + self.linemarkers[-1][0] + ":\n  " + statement
        return s + "\n"

    def generate(self, statement, sep):
        """Modify the existing statements list by incoprorating, modifying, or
        ignoring this statement, which is partitioned from the next statement by
        sep.
        """
        nnewlines = statement.count('\n') + sep.count('\n')
        self.nlines_since_linemarker += nnewlines
        # filters have to come before sep
        for filter in (() if len(statement) == 0 else self.filters):
            if filter.isvalid(statement):
                transformed = filter.transform(statement, sep)
                break
        else:
            transformed = None
        # assign transformation to the new statements.
        if transformed is None:
            transformed = statement + sep
        self.statements.append(transformed)
        # seps must come before revert
        if sep == '{':
            self.depth += 1
        elif sep == '}':
            self.depth -= 1
        elif sep == "\n" and "pragma" in statement:
            # gross fix for not using cpp
            self.depth += statement.count('{') - statement.count('}')
        # revert what is needed
        for filter in self.filters:
            reverted = filter.revert(statement, sep)
            if reverted is not None:
                self.statements.append(reverted)

def generate_code(orig, context, superclasses):
    """Takes a canonical C++ source file and separates it out into statements
    which are fed into a code generator. The new file is returned.
    """
    cg = CodeGenerator(context, superclasses)
    for m in RE_STATEMENT.finditer(orig):
        if m is None:
            continue
        prefix1, prefix2, statement, _, sep = m.groups()
        statement = statement if prefix2 is None else prefix2 + statement
        statement = statement if prefix1 is None else prefix1 + statement
        cg.generate(statement, sep)
    newfile = "".join(cg.statements)
    return newfile

#
# meta
#
class Proxy(MutableMapping):
    """A proxy object for scoping purposes."""

    def __init__(self, d):
        """d is a dict-like object"""
        self.__dict__['_d'] = d if isinstance(d, MutableMapping) else dict(d)

    #
    # object interface
    #
    def __getattr__(self, key):
        d = self.__dict__['_d']
        return d[key] if key in d else self.__dict__[key]

    def __setattr__(self, key, value):
        self.__dict__['_d'][key] = value

    def __delattr__(self, key):
        d = self.__dict__['_d']
        if key in d:
            del d[key]
        else:
            del self.__dict__[key]

    #
    # Mapping interface
    #
    def __getitem__(self, key):
        return self.__dict__['_d'][key]

    def __setitem__(self, key, value):
        self.__dict__['_d'][key] = value

    def __delitem__(self, key, value):
        del self.__dict__['_d'][key]

    def __len__(self):
        return len(self.__dict__['_d'])

    def __iter__(self):
        return iter(self.__dict__['_d'])

    def __contains__(self, key):
        return key in self.__dict__['_d']

def outter_split(s, open_brace='(', close_brace=')', separator=','):
    """Takes a string and only split the outter most level."""
    outter = []
    ns = s.split(separator)
    count = 0
    val = ''
    for n in ns:
        count += n.count(open_brace)
        count -= n.count(close_brace)
        val += n
        if count == 0:
            outter.append(val.strip())
            val = ''
        else:
            val += separator
    return outter

def parse_template(s, open_brace='<', close_brace='>', separator=','):
    """Takes a string -- which may represent a template specialization -- and
    returns the corresponding type.

    It calls parse_arg to return the type(s) for any template arguments the
    type may have. For example:

    >>> parse_template('std::map<int, double>')
    ['std::map', 'int', 'double']
    >>> parse_template('std::map<int, std::map<int, std::map<int, int> > >')
    ['std::map', 'int', ['std::map', 'int', ['std::map', 'int', 'int']]]
    """

    s = s.replace(' ', '')
    if open_brace not in s and separator not in s:
        return s

    i = s.find(open_brace)
    j = s.rfind(close_brace)
    t = [s[:i]]
    inner = s[i+1:j]
    t.extend(parse_arg(inner, open_brace, close_brace, separator))
    return t

def parse_arg(s, open_brace='<', close_brace='>', separator=','):
    """Takes a string containing one or more c++ template args and returns a
    list of the argument types as strings.

    This is called by parse_template to handle the inner portion of the
    template braces.  For example:

    >>> parse_arg('int, double')
    ['int', 'double']
    >>> parse_arg('std::map<int, double>')
    [['std::map', 'int', 'double']]
    >>> parse_arg('int, std::map<int, double>')
    ['int', ['std::map', 'int', 'double']]
    """

    nest = 0
    ts = []
    start = 0
    for i in range(len(s)):
        ch = s[i]
        if ch == open_brace:
            nest += 1
        elif ch == close_brace:
            nest -= 1

        if ch == separator and nest == 0:
            t = parse_template(s[start:i], open_brace, close_brace, separator)
            ts.append(t)
            start = i+1

    if start < len(s):
        t = parse_template(s[start:], open_brace, close_brace, separator)
        ts.append(t)
    return ts

def type_to_str(t):
    if t in PRIMITIVES:
        return t
    else:
        s = t[0] + '< '
        s += type_to_str(t[1])
        for thing in t[2:]:
            s += ', ' + type_to_str(thing)
        s += ' >'
        return s

def parent_classes(classname, pdict):
    rents = set()
    vals = pdict[classname] = pdict.get(classname, set())
    for val in pdict[classname]:
        rents.add(val)
        rents |= parent_classes(val, pdict)
    return rents

def parent_intersection(classname, queryset, superclasses):
    """Returns all elements in query_set which are parents of classname and not
    parents of any other class in query_set
    """
    rents = queryset.intersection(superclasses[classname])
    grents = set()
    for parent in rents:
        grents |= parent_classes(parent, superclasses)
    return rents - grents

ensure_startswith_newlinehash = lambda x: '\n' + x if x.startswith('#') else x

def insert_line_directives(newfile, filename):
    """Inserts line directives based on diff of original file."""
    with open(filename) as f:
        orig = f.read()
    origlines = orig.splitlines()
    newlines = newfile.splitlines()
    sm = difflib.SequenceMatcher(a=origlines, b=newlines, autojunk=False)
    blocks = list(sm.get_matching_blocks())
    for i, j, n in blocks[-2::-1]:
        newlines.insert(j, '#line {0} "{1}"'.format(i+1, filename))
    return "\n".join(newlines)

def main():
    doc = __doc__ + "\nfilename: " + os.path.abspath(__file__)
    parser = ArgumentParser(prog="cycpp", description=doc,
                            formatter_class=RawDescriptionHelpFormatter)
    parser.add_argument('path', help="path to source file")
    parser.add_argument('--pass3-use-pp', action="store_true", default=True,
                        help=("On pass 3, use the preproccessed version of the "
                              "original file. This options is mutually exclusive"
                              "with --pass3-use-orig."), dest="pass3_use_pp")
    parser.add_argument('--pass3-use-orig', action="store_false",
                        help=("On pass 3, use the preproccessed version of the "
                              "original file. This options is mutually exclusive"
                              "with --pass3-use-pp."), dest="pass3_use_pp")
    parser.add_argument('-o', '--output', help=("output file name"))
    parser.add_argument('--cpp-path', dest='cpp_path', help="preprocessor to use",
                        default=sys_cpp)
    parser.add_argument('-I', '--includes', action="append",
                        help=("include directories for preprocessing. Can be "
                              "a variable number of arguments (i.e., list of "
                              "include directories), a single argument. If it "
                              "is a single argument, it can either be a single "
                              "directory or a semi-colon separated list of "
                              "directories (a la CMake)."))
    ns = parser.parse_args()

    includes = [] if ns.includes is None else ns.includes
    if len(includes) == 1:
        if ";" in includes[0]:
            includes = includes[0].split(";")
        elif ":" in includes[0]:
            includes = includes[0].split(":")

    canon = preprocess_file(ns.path, includes, cpp_path=ns.cpp_path)  # pass 1
    canon = ensure_startswith_newlinehash(canon)
    context, superclasses = accumulate_state(canon)   # pass 2
    if not ns.pass3_use_pp:
        with open(ns.path) as f:
            orig = f.read()
        orig = ensure_startswith_newlinehash(orig)
        orig = orig.replace('\\\n', '') # line continuation
    # pass 3
    newfile = generate_code(canon if ns.pass3_use_pp else orig, context, superclasses)
    newfile = insert_line_directives(newfile, ns.path)
    if ns.output is None:
        print(newfile)
    else:
        with open(ns.output, "w") as f:
            f.write(newfile)

if __name__ == "__main__":
    main()
