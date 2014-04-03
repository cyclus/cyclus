#!/usr/bin/env python2
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
cycpp what to do with this pragma. Lastly, optionals arguments may be passed to
this decorator but all options *must* be on the same logical line as the
directive.  How the arguments are interpreted is a function of the decorators
themselves.  Most of them are simple Python statements or expressions.  See the
following handy table!

**Decorator Arguments:**

:var:  Add the following C++ statement as an Agent's state variable. There is one 
       argument which must be a Python expression that evaluates to a dictionary or
       other mapping.
:exec: Executes arbitrary python code that is passed in as the arguments and
       loads this into the context. This is useful for importing handy modules,
       declaring variables for later use, or any of the other things that Python
       is great for. Any variables defined here are kept in a seperate
       namespace from the classes.  Since this gives you direct access to the
       Python interpreter, try to be a little careful.

cycpp is implemented entirely in this file and with tools from the Python standard
library. It requires Python 2.7+ or Python 3.3+ to run.
"""
from __future__ import print_function
import os
import re
import sys
from collections import Sequence, MutableMapping
from itertools import takewhile
from subprocess import Popen, PIPE
from argparse import ArgumentParser, RawDescriptionHelpFormatter
from pprint import pprint, pformat

if sys.version_info[0] == 2:
    STRING_TYPES = (str, unicode, basestring)
elif sys.version_info[0] >= 3:
    STRING_TYPES = (str,)
# Non-capturing and must be used wit re.DOTALL, DO NOT COMPILE! 
RE_MULTILINE_COMMENT = "(?:\s*?/\*(?!\*/)*?\*/)"
RE_SINGLE_LINE_COMMENT = "(?:\s*?//[^\n]*?\n\s*?)"
RE_COMMENTS = "(?:" + RE_MULTILINE_COMMENT + "|" + RE_SINGLE_LINE_COMMENT + ")"

# This migh miss files which start with '#' - however, after canonization
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

PRIMITIVES = {'std::string', 'float', 'double', 'int'}

BUFFERS = {
    '{0}::ResourceBuff'.format(CYCNS),
}

WRANGLERS = {
    '{0}::Agent'.format(CYCNS), 
    '{0}::Facility'.format(CYCNS), 
    '{0}::Institution'.format(CYCNS), 
    '{0}::Region'.format(CYCNS), 
    'mi6::Spy',  # for testing!!
    }

#
# pass 1
#
def preprocess_file(filename, includes = [], cpp_path='cpp', cpp_args=('-xc++', '-pipe', '-E')):
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
        lms.append(fname + ":" + lineno)

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
    regex = re.compile("\s*namespace(\s+\w*)?\s*[^=]*", re.DOTALL)

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
    regex = re.compile("(?:\s*template\s*<[\s\w,]*>)?"
                       "\s*(?:class|struct)\s+(\w+)(\s*:[\n\s\w,:]+)?\s*", re.DOTALL)

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
    regex = re.compile('\s*(public|private|protected)\s*', re.DOTALL)

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
        # stack of (depth, class name) tuples, most nested is last
        self.classes = []  
        self.superclasses = {}  # map from classes to set of super classes.
        self.access = {}   # map of (classnames, current access control flags)
        self.namespaces = []  # stack of (depth, ns name) tuples
        self.using_namespaces = set()  # set of (depth, ns name) tuples
        self.aliases = set()  # set of (depth, name, alias) tuples
        self.var_annotations = None
        self.linemarkers = []
        self.filters = [ClassAndSuperclassFilter(self), AccessFilter(self), 
                        ExecFilter(self), UsingNamespaceFilter(self), 
                        NamespaceAliasFilter(self), NamespaceFilter(self), 
                        TypedefFilter(self), UsingFilter(self), LinemarkerFilter(self),
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

    def includeloc(self):
        """Current location of the file from includes as a string."""
        if len(self.linemarkers) == 0:
            return ""
        s = "\n Included from:\n  " + "\n  ".join(self.linemarkers)
        return s + "\n"

    #
    # type system
    #
    supported_types = PRIMITIVES
    supported_types |= BUFFERS
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
                msg = ("{i}The type of {c}::{n} ({t}) is not a recognized "
                       "template type: {p}.").format(i=self.includeloc(), t=t,
                       n=name, c=self.classname(),
                       p=", ".join(sorted(self.known_templates)))
                raise TypeError(msg)
        elif '<' in t:
            # string version of template type
            t = " ".join(t.strip().strip(scopz).split())
            t = self.canonize_type(parse_template(t))
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
                    p=", ".join(sorted(self.supported_types)))
                raise TypeError(msg)
        return t

    def _canonize_targs(self, newtname, targs):
        newt = [newtname]
        newt += [self.canonize_type(targ) for targ in targs]
        return tuple(newt)

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
    re_template = ("\s*#\s*pragma\s+cyclus\s*?"
                   "(\s+def\s+|\s+decl\s+|\s+impl\s+|\s*?)?"
                   "(?:\s*?{0}\s*?)(\s+?(?:[\w:\.]+)?)?")

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

    def transform(self, statement, sep):
        # basic setup
        cg = self.machine
        mode = (self.match.group(1) or '').strip()
        if len(mode) == 0:
            mode = "def"
        classname = self.match.group(2) if self.match.lastindex > 1 else None
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
        ctx = context[classname] = context.get(classname, {})
        in_class_decl = self.in_class_decl() 
        #ns = "" if in_class_decl else classname.split('::')[-1] + "::"
        ns = "" if in_class_decl else cg.scoped_classname(classname) + "::"
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
        return (len(self.machine.classes) > 0 and 
                self.given_classname == self.local_classname)

    def revert(self, statement, sep):
        super(CodeGeneratorFilter, self).revert(statement, sep)
        self.local_classname = None
        self.given_classname = None

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
        ctx = context[self.given_classname]
        impl = ""
        
        # add inheritance init froms
        rents = parent_intersection(self.given_classname, WRANGLERS, 
                                    self.machine.superclasses)
        for rent in rents:
            impl += ind + "{0}::InitFrom(m);\n".format(rent)
            
        cap_buffs = []

        for member, info in ctx.items():
            if info['type'] not in BUFFERS:
                impl += ind + "{0} = m->{0};\n".format(member)
            elif 'capacity' in info:
                cap_buffs.append((member,))

        for b in cap_buffs:
            impl += ind + "{0}.set_capacity(m->{0}.capacity());\n".format(b[0])

        return impl

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
        ctx = context[self.given_classname]
        impl = ''
        pods = []

        # add inheritance init froms
        rents = parent_intersection(self.given_classname, WRANGLERS, 
                                    self.machine.superclasses)
        for rent in rents:
            impl += ind + "{0}::InitFrom(b);\n".format(rent)

        cap_buffs = []
            
        for member, info in ctx.items():
            t = info['type']
            if t in BUFFERS:
                if 'capacity' in info:
                    cap_buffs.append((member, info['capacity']))
                continue
            if t[0] in ['std::map', 'std::set', 'std::list', 'std::vector']:
                if t[0] == 'std::map':
                    table = 'MapOf' + t[1].replace('std::', '').title() + 'To' + t[2].replace('std::', '').title() 
                else:
                    table = 'ListOf' + t[1].replace('std::', '').title()
                impl += ind + '{\n'
                impl += ind + '  std::vector<{0}::Cond> conds;\n'.format(CYCNS)
                impl += ind + '  conds.push_back({0}::Cond("Member", "==", "{1}"));\n'.format(CYCNS, member)
                impl += ind + '  {0}::QueryResult qr = b->Query("{1}", &conds);\n'.format(CYCNS, table)
                impl += ind + '  for (int i = 0; i < qr.rows.size(); ++i) {\n'
                ind += '    '
                if t[0] == 'std::map':
                    impl += ind + '{0} {1}_k = qr.GetVal<{0}>("Key", i);\n'.format(t[1], member)
                    impl += ind + '{0} {1}_v = qr.GetVal<{0}>("Value", i);\n'.format(t[2], member)
                    impl += ind + '{0}[{0}_k] = {0}_v;\n'.format(member)
                elif t[0] == 'std::set':
                    impl += ind + '{0}.insert(qr.GetVal<{1}>("Value", i);\n'.format(member, t[1])
                else:
                    impl += ind + '{0}.push_back(qr.GetVal<{1}>("Value", i));\n'.format(member, t[1])
                ind = ind[:-4]
                impl += ind + '  }\n'
                impl += ind + '}\n'
            elif t in PRIMITIVES:
                pods.append((member, t))
            elif t[0] == 'std::pair':
                pods.append((member, t))
            else:
                raise RuntimeError('{0}Unsupported type {1}'.format(self.machine.includeloc(), t))
        
        # add pod
        impl += ind + "{0}::QueryResult qr = b->Query(\"Info\", NULL);\n".format(CYCNS)
        for (member, t) in pods:
            if t[0] == 'std::pair':
                impl += ind + "{0}.first = qr.GetVal<{1}>(\"{0}A\");\n".format(member, t[1])
                impl += ind + "{0}.second = qr.GetVal<{1}>(\"{0}B\");\n".format(member, t[2])
            else:
                impl += ind + "{0} = qr.GetVal<{1}>(\"{0}\");\n".format(member, t)

        for b in cap_buffs:
            impl += ind + "{0}.set_capacity(qr.GetVal<double>(\"{1}\"));\n".format(b[0], b[1])
        return impl

class InfileToDbFilter(CodeGeneratorFilter):
    """Filter for handling InfileToDb() code generation:
        #pragma cyclus [def|decl|impl] infiletodb [classname]
    """
    methodname = "InfileToDb"
    pragmaname = "infiletodb"
    methodrtn = "void"

    def methodargs(self):
        return "{0}::InfileTree* tree, {0}::DbInit di".format(CYCNS)

    def impl(self, ind="  "):        
        cg = self.machine
        context = cg.context
        ctx = context[self.given_classname]
        pods = []
        impl = ""

        # add inheritance init froms
        rents = parent_intersection(self.given_classname, WRANGLERS, 
                                    self.machine.superclasses)
        for rent in rents:
            impl += ind + "{0}::InfileToDb(tree, di);\n".format(rent)

        impl += ind + "tree = tree->SubTree(\"agent/\" + agent_impl());\n"

        for member, info in ctx.items():
            t = info['type']
            if t in BUFFERS:
                continue
            d = info['default'] if 'default' in info else None
            code = info['derived_init'] if 'derived_init' in info else None
            if t[0] in ['std::set', 'std::vector', 'std::map', 'std::list']:
                if t[0] == 'std::map':
                    table = 'MapOf' + t[1].replace('std::', '').title() + 'To' + t[2].replace('std::', '').title() 
                else:
                    table = 'ListOf' + t[1].split('::')[-1].title()

                if d is not None:
                    impl += ind + 'if (tree->NMatches("{0}") > 0) {{\n'.format(member)
                else:
                    impl += ind + '{\n'
                ind += '  '

                impl += ind + '{0}::InfileTree* sub = tree->SubTree("{1}");\n'.format(CYCNS, member)
                impl += ind + 'int n = sub->NMatches("val");\n'
                impl += ind + 'for (int i = 0; i < n; ++i) {\n'
                impl += ind + '  di.NewDatum("{0}")\n'.format(table)
                impl += ind + '  ->AddVal("Member", "{0}")\n'.format(member)
                if t[0] == 'std::map':
                    impl += ind + '  ->AddVal("Key", {0}::Query<{1}>(sub, "key", i))\n'.format(CYCNS, t[1])
                    impl += ind + '  ->AddVal("Value", {0}::Query<{1}>(sub, "val", i))\n'.format(CYCNS, t[2])
                else:
                    impl += ind + '  ->AddVal("Value", {0}::Query<{1}>(sub, "val", i))\n'.format(CYCNS, t[1])
                impl += ind + '  ->Record();\n'
                impl += ind + '}\n'

                if d is not None:
                    ind = ind[:-2]
                    impl += ind + '} else {\n'
                    ind += '  '
                    if t[0] == 'std::map':
                        for k, v in d.items():
                            ktext = '"{0}"'.format(k) if t[1] == 'std::string' else '{0}'.format(k)
                            vtext = '"{0}"'.format(v) if t[2] == 'std::string' else '{0}'.format(v)
                            impl += ind + 'di.NewDatum("{0}")\n'.format(table)
                            impl += ind + '->AddVal("Member", "{0}")\n'.format(member)
                            impl += ind + '->AddVal("Key", {0})\n'.format(ktext)
                            impl += ind + '->AddVal("Value", {0})\n'.format(vtext)
                            impl += ind + '->Record();\n'
                    else:
                        for v in d:
                            vtext = '"{0}"'.format(v) if t[1] == 'std::string' else '{0}'.format(v)
                            impl += ind + 'di.NewDatum("{0}")\n'.format(table)
                            impl += ind + '->AddVal("Member", "{0}")\n'.format(member)
                            impl += ind + '->AddVal("Value", {0})\n'.format(vtext)
                            impl += ind + '->Record();\n'

                ind = ind[:-2]
                impl += ind + '}\n'            
            elif t in PRIMITIVES:
                pods.append((member, t, d, code))
            elif t[0] == 'std::pair':
                pods.append((member, t, d, code))
            else:
                raise RuntimeError('{0}Unsupported type {1}'.format(self.machine.includeloc(), t))

        head = ""  # from input to var
        body = ""  # derive vars
        tail = ""  # from far to db

        # handle pod in a single datum/table
        tail += ind + 'di.NewDatum("Info")\n'
        for (member, t, d, code) in pods:
            methname = "Query" if d is None else "OptionalQuery"
            opt = ''
            if t[0] == 'std::pair':
                opt2 = ''
                if d is not None:
                    opt = ', ' + d[0]
                    opt2 = ', ' + d[1]
                if code is None:
                    head += ind + ("{0}A = {1}::{2}<{3}>(tree, \"{0}/first\"{4});\n").format(member, CYCNS, methname, t[1], opt)
                    head += ind + ("{0}B = {1}::{2}<{3}>(tree, \"{0}/second\"{4});\n").format(member, CYCNS, methname, t[2], opt)
                    tail += ind + ("->AddVal(\"{0}A\", {0}A)\n\"").format(member)
                    tail += ind + ("->AddVal(\"{0}B\", {0}B)\n\"").format(member)
                else:
                    tail += ind + ("->AddVal(\"{0}A\", {0}.first)\n\"").format(member)
                    tail += ind + ("->AddVal(\"{0}B\", {0}.second)\n\"").format(member)
            else:
                if d is not None:
                    opt = ', "' + str(d) + '"' if t == "std::string" else ', ' + str(d) 
                if code is None:
                    head += ind + ("{0} = {1}::{2}<{3}>(tree, \"{0}\"{4});\n").format(member, CYCNS, methname, t, opt)
                tail += ind + ("->AddVal(\"{0}\", {0})\n").format(member)
            if code is not None:
                body += ind + code + '\n'

        tail += ind + '->Record();\n'
        impl += head + body + tail 
        return impl

class SchemaFilter(CodeGeneratorFilter):
    """Filter for handling Schema() code generation:
        #pragma cyclus [def|decl|impl] schema [classname]
    """
    methodname = "schema"
    pragmaname = "schema"
    methodrtn = "std::string"

    def impl(self, ind="  "):        
        cg = self.machine
        context = cg.context
        ctx = context[self.given_classname]
        i = Indenter(level=len(ind) / 2)
        xi = Indenter(n=4)

        if len(ctx.keys()) == 0:
            return i + 'return "<text/>";\n'

        impl = i.up() + 'return ""\n'
        impl += i +  '"<interleave>\\n"\n'
        for member, info in ctx.items():
            t = info['type']
            if t in BUFFERS: # buffer state, skip
                continue
            if 'derived_init' in info: # derived state, skip
                continue
            opt = True if 'default' in info else False
            if opt:
                impl += i + '"{0}<optional>\\n"\n'.format(xi.up())

            t = info['type']
            if t[0] in ['std::list', 'std::map', 'std::set', 'std::vector']:
                impl += i + '"{0}<element name=\\"{1}\\">\\n"\n'.format(xi.up(), member)
                impl += i + '"{0}<oneOrMore>\\n"\n'.format(xi.up())
                if t[0] in ['std::set', 'std::vector', 'std::list']:
                    el_type = t[1].replace('std::', '')
                    impl += i + '"{0}<element name=\\"val\\">\\n"\n'.format(xi.up())
                    impl += i + '"{0}<data type=\\"{1}\\" />\\n"\n'.format(xi, el_type)
                    impl += i + '"{0}</element>\\n"\n'.format(xi.down())
                else: # map
                    k_type = t[1].replace('std::', '')
                    v_type = t[2].replace('std::', '')
                    impl += i + '"{0}<element name=\\"key\\">\\n"\n'.format(xi.up())
                    impl += i + '"{0}<data type=\\"{1}\\" />\\n"\n'.format(xi, k_type)
                    impl += i + '"{0}</element>\\n"\n'.format(xi.down())
                    impl += i + '"{0}<element name=\\"val\\">\\n"\n'.format(xi.up())
                    impl += i + '"{0}<data type=\\"{1}\\" />\\n"\n'.format(xi, v_type)
                    impl += i + '"{0}</element>\\n"\n'.format(xi.down())

                impl += i + '"{0}</oneOrMore>\\n"\n'.format(xi.down())
                impl += i + '"{0}</element>\\n"\n'.format(xi.down(), member)
            elif t in PRIMITIVES:
                impl += i + '"{0}<element name=\\"{1}\\">\\n"\n'.format(xi.up(), member)
                impl += i + '"{0}<data type=\\"{1}\\" />\\n"\n'.format(xi, t.replace('std::', ''))
                impl += i + '"{0}</element>\\n"\n'.format(xi.down())
            elif t[0] == 'std::pair':
                impl += i + '"{0}<element name=\\"{1}\\">\\n"\n'.format(xi.up(), member)
                impl += i + '"{0}<element name=\\"first\\">\\n"\n'.format(xi.up())
                impl += i + '"{0}<data type=\\"{1}\\" />\\n"\n'.format(xi, t[1].replace('std::', ''))
                impl += i + '"{0}</element>\\n"\n'.format(xi.down())
                impl += i + '"{0}<element name=\\"second\\">\\n"\n'.format(xi.up())
                impl += i + '"{0}<data type=\\"{1}\\" />\\n"\n'.format(xi, t[2].replace('std::', ''))
                impl += i + '"{0}</element>\\n"\n'.format(xi.down())
                impl += i + '"{0}</element>\\n"\n'.format(xi.down())
            else:
                raise RuntimeError('{0}Unsupported type {1}'.format(self.machine.includeloc(), t))

            if opt:
                impl += i + '"{0}</optional>\\n"\n'.format(xi.down())
        
        impl += i +  '"</interleave>\\n"\n'
        impl += i + ";\n";
        return impl

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
        ctx = context[self.given_classname]
        impl = ""
        pod = {}
        for member, params in ctx.items():
            t = params["type"]
            if t in BUFFERS:
                continue
            if t[0] in ["std::vector", "std::list", "std::set"]:
                suffix = t[1].replace("std::", "").title()
                impl += ind + "{\n"
                impl += ind + "  {0}::iterator it;\n".format(type_to_str(t))
                impl += ind + "  for (it = {0}.begin(); it != {0}.end(); ++it) {{\n".format(member)
                impl += ind + "    di.NewDatum(\"ListOf{0}\")\n".format(suffix)
                impl += ind + "    ->AddVal(\"Member\", \"{0}\")\n".format(member)
                impl += ind + "    ->AddVal(\"Value\", *it)\n"
                impl += ind + "    ->Record();\n"
                impl += ind + "  }\n"
                impl += ind + "}\n"
            elif t[0] == "std::map":
                suffix = t[1].replace("std::", "").title() + "To" + t[2].replace("std::", "").title()
                impl += ind + "{\n"
                impl += ind + "  {0}::iterator it;\n".format(type_to_str(t))
                impl += ind + "  for (it = {0}.begin(); it != {0}.end(); ++it) {{\n".format(member)
                impl += ind + "    di.NewDatum(\"MapOf{0}\")\n".format(suffix)
                impl += ind + "    ->AddVal(\"Member\", \"{0}\")\n".format(member)
                impl += ind + "    ->AddVal(\"Key\", it->first)\n"
                impl += ind + "    ->AddVal(\"Value\", it->second)\n"
                impl += ind + "    ->Record();\n"
                impl += ind + "  }\n"
                impl += ind + "}\n"
            elif t[0] == "std::pair":
                pod[member] = t
            elif t in PRIMITIVES:
                pod[member] = t
            else:
                raise RuntimeError("{0}Unsupported type {1}".format(self.machine.includeloc(), t))

        impl += ind + "di.NewDatum(\"Info\")\n"
        for member, t in pod.items():
            if t[0] == "std::pair":
                impl += ind + "->AddVal(\"{0}A\", {0}.first)\n".format(member)
                impl += ind + "->AddVal(\"{0}B\", {0}.second)\n".format(member)
            else:
                impl += ind + "->AddVal(\"{0}\", {0})\n".format(member)
        impl += ind + "->Record();\n"

        return impl

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
        ctx = context[self.given_classname]
        impl = ""
        buffs = []
        for member, info in ctx.items():
            t = info['type']
            if t in BUFFERS:
                buffs.append(member)

        impl = ind + "{0}::Inventories invs;\n".format(CYCNS)

        for buff in buffs:
            impl += ind + ("invs[\"{0}\"] = "
                           "{0}.PopN({0}.count());\n").format(buff)
        impl += ind + "return invs;\n"
        return impl

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
        ctx = context[self.given_classname]
        impl = ""
        buffs = []
        for member, info in ctx.items():
            t = info['type']
            if t in BUFFERS:
                buffs.append(member)

        impl = ""
        for buff in buffs:
            impl += ind + "{0}.PushAll(inv[\"{0}\"]);\n".format(buff)
        return impl

class DefaultPragmaFilter(Filter):
    """Filter for handling default pragma code generation:
        #pragma cyclus [def|decl|impl]
    """
    regex = re.compile("\s*#\s*pragma\s+cyclus(\s+def|\s+decl|\s+impl)?\s*$", 
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

class Indenter(object):
    def __init__(self, n=2, level=0):
        str.__init__(self)
        self._n = int(n)
        self._level = int(level)

    def __add__(self, other):
        return '{0}{1}'.format(self, other)

    def __radd__(self, other):
        return '{0}{1}'.format(self, other)

    def __concat__(self, other):
        return '{0}{1}'.format(self, other)

    def __str__(self):
        return ' '*self._n*self._level

    def __repr__(self):
        return ' '*self._n*self._level

    def up(self):
        self._level += 1
        return Indenter(n=self._n, level=self._level-1)

    def down(self):
        self._level -= 1
        return Indenter(n=self._n, level=self._level)

class CodeGenerator(object):
    """The CodeGenerator class is the pass 3 state machine.

    This represents the file as code is being injected into it.  At the end of
    the traversal this final stage it will built up a brand new file for pass
    3. It manages both the code insertion pragmas and other bits of C++ syntax
    as needed to determine locality. It works by passing each statement through
    a sequence of filters, and injects code based on the directive and the
    state.
    """
    
    def __init__(self, context, superclasses):
        self.depth = 0
        self.context = context  # the results of pass 2
        self.superclasses = superclasses  # the results of pass 2
        self.statements = []    # the results of pass 3, waiting to be joined
        self.classes = []  # stack of (depth, class name) tuples, most nested is last
        self.access = {}   # map of (classnames, current access control flags)
        self.namespaces = []  # stack of (depth, ns name) tuples
        self.aliases = set()  # set of (depth, name, alias) tuples
        self.linemarkers = []
        # all basic code generating filters
        self.codegen_filters = [InitFromCopyFilter(self), 
                                InitFromDbFilter(self), InfileToDbFilter(self),
                                CloneFilter(self), SchemaFilter(self),
                                InitInvFilter(self), 
                                # SnapshotInv has to come before Snapshot for some 
                                # regex reason I don't understand
                                SnapshotInvFilter(self),
                                SnapshotFilter(self), 
                                ] 
        self.filters = self.codegen_filters + [ClassFilter(self), 
                                               AccessFilter(self), 
                                               NamespaceAliasFilter(self), 
                                               NamespaceFilter(self), 
                                               DefaultPragmaFilter(self),
                                               LinemarkerFilter(self)]
        
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

    def includeloc(self):
        """Current location of the file from includes as a string."""
        if len(self.linemarkers) == 0:
            return ""
        s = "\n Included from:\n  " + "\n  ".join(self.linemarkers)
        return s + "\n"

    def generate(self, statement, sep):
        """Modify the existing statements list by incoprorating, modifying, or
        ignoring this statement, which is partitioned from the next statement by
        sep.
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
    """returns all elements in query_set which are parents of classname and not
    parents of any other class in query_set
    """
    rents = queryset.intersection(superclasses[classname])
    grents = set()
    for parent in rents:
        grents |= parent_classes(parent, superclasses)
    return rents - grents

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
                        help=("On pass 3, use the preproccessed version of the "
                              "original file. This options is mutually exclusive"
                              "with --pass3-use-pp."), dest="pass3_use_pp")
    parser.add_argument('-o', '--output', help=("output file name"))
    parser.add_argument('--cpp-path', dest='cpp_path', help="preprocessor to use",
                        default='cpp')
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
    # pass 3
    newfile = generate_code(canon if ns.pass3_use_pp else orig, context, superclasses)
    if ns.output is None:
        print(newfile)
    else:
        with open(ns.output, "w") as f:
            f.write(newfile)

if __name__ == "__main__":
    main()
