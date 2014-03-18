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
import sys
from collections import Sequence, MutableMapping
from subprocess import Popen, PIPE
from argparse import ArgumentParser, RawDescriptionHelpFormatter
from pprint import pprint

if sys.version_info[0] == 2:
    STRING_TYPES = (str, unicode, basestring)
elif sys.version_info[0] >= 3:
    STRING_TYPES = (str,)

# This migh miss files which start with '#' - however, after canonization (through cpp)
# it shouldn't matter.
RE_STATEMENT = re.compile(
    r'(\s*#)?'  # find the start of pragmas
    r'(\s+(public|private|protected)\s*'  # consider access control as statements
    r'|(?(1)[^\n]|[^{};])*)?'  # or, consider statement until we hit '{', '}', or ';'
    # find end condition, '\n' for pragma, ':' for access, and '{', '}', ';' otherwise
    r'((?(1)\n|(?(3):|[{};])))', re.MULTILINE)


CYCNS = "cyclus"

PRIMITIVES = {'std::string', 'float', 'double', 'int'}

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
    """Filter for handling typedef as aliases. Note that in-line compound typedefs of
    structs and unions are not supported.
    """
    regex = re.compile("\s*typedef\s+(.*?\s+.*)\s*$")

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
    regex = re.compile("\s*using\s+(?!namespace\s+)([\w:]+)\s*")

    def transform(self, statement, sep):
        state = self.machine
        name = self.match.group(1)
        state.aliases.add((state.depth, name, name.rsplit('::', 1)[1]))

class NamespaceFilter(Filter):
    """Filter for accumumating namespace encapsulations."""
    # handles anonymous namespaces as group(1) == None
    regex = re.compile("\s*namespace(\s+\w*)?\s*$")

    def transform(self, statement, sep):
        state = self.machine
        name = self.match.group(1)
        if name is not None:
            name = name.strip() or None
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
    regex = re.compile("\s*using\s+namespace\s+([\w:]*)\s*")

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
    regex = re.compile("\s*namespace\s+(\w+)\s*=\s*([\w:]+)\s*")

    def transform(self, statement, sep):
        state = self.machine
        alias = self.match.group(1)
        name = self.match.group(2)
        state.aliases.add((state.depth, name, alias))

class ClassFilter(Filter):
    """Filter for picking out class names."""
    regex = re.compile("\s*class\s+([\w:]+)\s*")

    def transform(self, statement, sep):
        state = self.machine
        name = self.match.group(1)
        state.classes.append((state.depth, name))
        state.access[tuple(state.classes)] = "private"

    def revert(self, statement, sep):
        super(ClassFilter, self).revert(statement, sep)
        state = self.machine
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
        self.machine.access[tuple(self.machine.classes)] = access

#
# pass 2
#
class VarDecorationFilter(Filter):
    """Filter for handling state variable decoration of the form:

        #pragma cyclus var <dict>

    This evals the contents of dict and puts them in state.var_annotations, to be
    consumed by the next match with VarDeclarationFilter.
    """
    regex = re.compile("#\s*pragma\s+cyclus\s+var\s+(.*)")

    def transform(self, statement, sep):
        state = self.machine
        context = state.context
        classname = state.classname()
        classpaths = classname.split('::')
        raw = self.match.group(1)
        glb = dict(state.execns)
        for cls, val in context.items():
            clspaths = cls.split('::')
            self._add_gbl_proxies(glb, clspaths, val)
            self._add_lcl_proxies(glb, clspaths, classpaths)
        state.var_annotations = eval(raw, glb, context.get(classname, {}))

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

class VarDeclarationFilter(Filter):
    """State varible declaration.  Only oeprates if state.var_annotations is
    not None. Access for member variable must be public.
    """
    regex = re.compile("(.*\w+.*?)\s+(\w+)")

    def transform(self, statement, sep):
        state = self.machine
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
        annotations['type'] = state.canonize_type(vtype, vname)
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
        execns = self.machine.execns
        context = self.machine.context
        raw = self.match.group(1)
        exec(raw, context, execns)
        del context['__builtins__']

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
        self.classes = []  # stack of (depth, class name) tuples, most nested is last
        self.access = {}   # map of (classnames, current access control flags)
        self.namespaces = []  # stack of (depth, ns name) tuples
        self.using_namespaces = set()  # set of (depth, ns name) tuples
        self.aliases = set()  # set of (depth, name, alias) tuples
        self.var_annotations = None
        self.filters = [ClassFilter(self), AccessFilter(self), ExecFilter(self),
                        NamespaceFilter(self), UsingNamespaceFilter(self),
                        NamespaceAliasFilter(self), TypedefFilter(self),
                        UsingFilter(self),
                        VarDecorationFilter(self), VarDeclarationFilter(self)]

    def classname(self):
        """Returns the current, fully-expanded class name."""
        names = [n for d, n in self.namespaces]
        names += [n for d, n in self.classes]
        return "::".join(names)

    def accumulate(self, statement, sep):
        """Modify the existing state by incoprorating the statement, which is
        partitioned from the next statement by sep.
        """
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

    #
    # type system
    #
    known_primitives = {'std::string', 'float', 'double', 'int'}
    known_templates = {
        'std::set': ('T',),
        'std::map': ('Key', 'T'),
        'std::pair': ('T1', 'T2'),
        'std::list': ('T',),
        'std::vector': ('T',),
        }
    scopz = '::'  # intern the scoping operator

    def canonize_type(self, t, name="<member variable>"):
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
                    return self.canonize_type([nsa + scopz + tname] + targs, name)
                except TypeError:
                    pass  # This is the TypeError from below
            else:
                msg = ("the type of {c}::{n} ({t}) is not a recognized template "
                       "type: {p}.").format(t=t, n=name, c=self.classname(),
                                            p=", ".join(sorted(self.known_templates)))
                raise TypeError(msg)
        elif '<' in t:
            # string version of template type
            t = " ".join(t.strip().strip(scopz).split())
            t = self.canonize_type(parse_template(t))
        else:
            # primitive type
            t = " ".join(t.strip().strip(scopz).split())
            if t in self.known_primitives:
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
                msg = ("the type of {c}::{n} ({t}) is not a recognized primitive "
                       "type: {p}.").format(t=t, n=name, c=self.classname(),
                                            p=", ".join(sorted(self.known_primitives)))
                raise TypeError(msg)
        return t

    def _canonize_targs(self, newtname, targs):
        newt = [newtname]
        newt += [self.canonize_type(targ) for targ in targs]
        return newt

def accumulate_state(canon):
    """Takes a canonical C++ source file and separates it out into statements
    which are fed into a state accumulator. The state is returned.
    """
    state = StateAccumulator()
    for m in RE_STATEMENT.finditer(canon):
        if m is None:
            continue
        prefix, statement, _, sep = m.groups()
        statement = statement if prefix is None else prefix + statement
        statement = statement.strip()
        state.accumulate(statement, sep)
    return state.context

#
# pass 3
#
class CodeGeneratorFilter(Filter):
    re_template = ("\s*#\s*pragma\s+cyclus+"
                   "(\sdef\s|\sdecl\s|\simpl\s|\s)+"
                   "{0}(\s+.*)?")

    def_template = "{ind}{virt}{rtn} {ns}{methodname}({args}){sep}\n"

    def __init__(self, *args, **kwargs):
        super(CodeGeneratorFilter, self).__init__(*args, **kwargs)
        pragmaname = self.pragmaname
        self.regex = re.compile(self.re_template.format(pragmaname))
        self.local_classname = None

    def transform(self, statement, sep):
        # basic setup
        cg = self.machine
        mode = self.match.group(1)
        if mode is None or mode is ' ':
            mode = "def"
        mode = mode.strip()
        classname = self.match.group(2)
        if classname is None:
            classname = cg.classname()
        classname = classname.strip().replace('.', '::')
        context = cg.context
        if classname not in context:
            msg = "{0} not found! Options include: {1}."
            raise KeyError(msg.format(classname, ", ".join(sorted(context.keys()))))
        self.local_classname = classname

        # compute def line
        ctx = context[classname]
        in_class_decl = self.in_class_decl()
        ns = "" if in_class_decl else classname + "::"
        virt = "virtual " if in_class_decl else ""
        end = ";" if mode == "decl" else " {"
        ind = 2 * (cg.depth - len(cg.namespaces))
        definition = self.def_template.format(ind=" "*ind, virt=virt,
                        rtn=self.methodrtn, ns=ns, methodname=self.methodname,
                        args=self.methodargs(), sep=end)

        # compute implementation
        impl = ""
        ind += 2
        if mode != "decl":
            impl = self.impl(ind=ind * " ")
        ind -= 2
        if not impl.endswith("\n") and 0 != len(impl):
            impl += '\n'
        end = "" if mode == "decl" else " " * ind + "};\n"

        # compute return
        if mode == 'impl':
            return impl
        else:
            return definition + impl + end

    def methodargs(self):
        # overwriteable
        return ""

    def in_class_decl(self):
        classname = self.local_classname
        return (len(self.machine.classes) > 0 and
                self.machine.classes[-1][1] == classname)

    def revert(self, statement, sep):
        super(CodeGeneratorFilter, self).revert(statement, sep)
        self.local_classname = None

class CloneFilter(CodeGeneratorFilter):
    """Filter for handling Clone() code generation:
        #pragma cyclus [def|decl|impl] clone [classname]
    """
    methodname = "Clone"
    pragmaname = "clone"
    methodrtn = "{0}::Model*".format(CYCNS)

    def impl(self, ind="  "):
        classname = self.local_classname
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
        return "{0}* m".format(self.local_classname)

    def impl(self, ind="  "):
        cg = self.machine
        context = cg.context
        ctx = context[self.local_classname]
        impl = ""
        for member in ctx.keys():
            impl += ind + "{0} = m->{0}".format(member)
        return impl

class InitFromDbFilter(CodeGeneratorFilter):
    """Filter for handling db-constructor-like InitFrom() code generation:
        #pragma cyclus [def|decl|impl] initfromdb [classname]
    """
    methodname = "InitFrom"
    pragmaname = "initfromdb"
    methodrtn = "void"

    def methodargs(self):
        return "{0}::QueryBackend* b".format(CYCNS)

    def impl(self, ind="  "):
        cg = self.machine
        context = cg.context
        ctx = context[self.local_classname]
        impl = ""
        pods = []
        lists = []
        for member, info in ctx.items():
            t = info['type']
            if t in PRIMITIVES:
                pods.append((member, t))
            else:
                pass # add lists appropriately
        impl = ""
        # add most-derived agent class call, e.g. cyc::FacilityModel::InitFrom(b);
        impl += ind + "cyc::QueryResult qr = b->Query(\"Info\", NULL);\n"
        for pod in pods:
            impl += ind + "{0} = qr.GetVal<{1}>(\"{0}\");\n".format(pod[0], pod[1])
        # add lists appropriately
        return impl

class CodeGenerator(object):
    """The CodeGenerator class is the pass 3 state machine.

    This represents the file as code is being injected into it.
    At the end of the traversal this final stage it will built up a brand new
    file for pass 3. It manages both the code insertion pragmas and other bits
    of C++ syntax as needed to determine locality. It works by passing each statement
    through a sequence of filters, and injects code based on the directive and the
    state.
    """

    def __init__(self, context):
        self.depth = 0
        self.context = context  # the results of pass 2
        self.statements = []    # the results of pass 3, waiting to be joined
        self.classes = []  # stack of (depth, class name) tuples, most nested is last
        self.access = {}   # map of (classnames, current access control flags)
        self.namespaces = []  # stack of (depth, ns name) tuples
        self.filters = [ClassFilter(self), AccessFilter(self), NamespaceFilter(self),
                        InitFromCopyFilter(self), InitFromDbFilter(self), CloneFilter(self)]

    def classname(self):
        """Returns the current, fully-expanded class name."""
        names = [n for d, n in self.namespaces]
        names += [n for d, n in self.classes]
        return "::".join(names)

    def generate(self, statement, sep):
        """Modify the existing statements list by incoprorating, modifying, or
        ignoring this statement, which is partitioned from the next statement by sep.
        """
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
            filter.revert(statement, sep)

def generate_code(orig, context):
    """Takes a canonical C++ source file and separates it out into statements
    which are fed into a code generator. The new file is returned.
    """
    cg = CodeGenerator(context)
    for m in RE_STATEMENT.finditer(orig):
        if m is None:
            continue
        prefix, statement, _, sep = m.groups()
        statement = statement if prefix is None else prefix + statement
        #print(cg.depth, repr(statement + sep))
        cg.generate(statement, sep)
    newfile = "".join(cg.statements)
    return newfile

#
# meta
#

class Proxy(MutableMapping):
    """A porxy object for scoping purposes."""

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
    # mapping interface
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

def split_template_args(s, open_brace='<', close_brace='>', separator=','):
    """Takes a string with template specialization and returns a list
    of the argument values as strings. Mostly cribbed from xdress.
    """
    targs = []
    ns = s.split(open_brace, 1)[-1].rsplit(close_brace, 1)[0].split(separator)
    count = 0
    targ_name = ''
    for n in ns:
        count += int(open_brace in n)
        count -= int(close_brace in n)
        targ_name += n
        if count == 0:
            targs.append(targ_name.strip())
            targ_name = ''
    return targs

def parse_template(s, open_brace='<', close_brace='>', separator=','):
    """Takes a string -- which may represent a template specialization --
    and returns the corresponding type. Mostly cribbed from xdress.
    """
    if open_brace not in s and close_brace not in s:
        return s
    t = [s.split(open_brace, 1)[0]]
    targs = split_template_args(s, open_brace=open_brace,
                                close_brace=close_brace, separator=separator)
    for targ in targs:
        t.append(parse_template(targ, open_brace=open_brace,
                                close_brace=close_brace, separator=separator))
    return t

ensure_startswith_newlinehash = lambda x: '\n' + x if x.startswith('#') else x

def main():
    parser = ArgumentParser(prog="cycpp", description=__doc__,
                            formatter_class=RawDescriptionHelpFormatter)
    parser.add_argument('path', help="path to source file")
    parser.add_argument('--pass3-use-pp', action="store_true", default=True,
                        help=("On pass 3, use the preproccessed version of the "
                              "original file. This options is mutually exclusive"
                              "with --pass3-use-orig."), dest="pass3_use_pp")
    parser.add_argument('--pass3-use-orig', action="store_false",
                        help=("On pass 3, use the original version of the "
                              "file. This options is mutually exclusive"
                              "with --pass3-use-pp."), dest="pass3_use_pp")
    ns = parser.parse_args()

    canon = preprocess_file(ns.path)  # pass 1
    canon = ensure_startswith_newlinehash(canon)
    context = accumulate_state(canon)   # pass 2
    #pprint(context)
    if not ns.pass3_use_pp:
        with open(ns.path) as f:
            orig = f.read()
        orig = ensure_startswith_newlinehash(orig)
    newfile = generate_code(canon if ns.pass3_use_pp else orig, context)  # pass 3
    print(newfile)

if __name__ == "__main__":
    main()
