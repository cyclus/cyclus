#!/usr/bin/env python
from __future__ import print_function, unicode_literals
import re

RE_LAST_WORD = re.compile('.*?(\w+)[^\w]*$')
RE_SQUARE_BRACKETS = re.compile('(\[[^\[\]]+?\])')
RE_ANGLE_BRACKETS = re.compile('(<[^<>]+?>)')


def remove_cython_templates(s):
    """Removes Cython templates from a string."""
    prev = ''
    while s != prev:
        s, prev = RE_SQUARE_BRACKETS.sub('', s), s
    return s


def remove_cpp_templates(s):
    """Removes C++ templates from a string."""
    prev = ''
    while s != prev:
        s, prev = RE_ANGLE_BRACKETS.sub('', s), s
    return s


def cppbases_replace_one(src, start):
    """Replaces a single instace of C++BASES at the start location."""
    # first get the cython arg names
    prev_nl = src.rfind('\n', 0, start)
    open_sig = src.find('(', prev_nl)
    close_sig = src.find(')', open_sig)
    cyarg_str = src[open_sig+1:close_sig]
    cyarg_str = remove_cython_templates(cyarg_str)
    cyargs = cyarg_str.split(',')
    cyargs = [RE_LAST_WORD.match(x).group(1) for x in cyargs]
    # now get bases str
    next_nl = src.find('\n', start)
    bases_str = src[start+8:next_nl]
    bases_str, _, _ = bases_str.rpartition('#')
    bases_str = bases_str.strip()
    # Now, get C++ args
    comment_end = src.find('*/', next_nl)
    open_sig = src.find('(', comment_end)
    close_sig = src.find(')', open_sig)
    cpparg_str = src[open_sig+1:close_sig]
    cpparg_str = remove_cpp_templates(cpparg_str)
    cppargs = cpparg_str.split(',')
    cppargs = [RE_LAST_WORD.match(x).group(1) for x in cppargs]
    # apply C++ names to base str
    bases_parts = bases_str.split('(')
    nclose = bases_parts[0].count(')')  # this should be zero
    for i in range(1, len(bases_parts)):
        part = bases_parts[i]
        if i == nclose:
            continue
        for cyarg, cpparg in zip(cyargs, cppargs):
            part = part.replace(cyarg, cpparg)
        bases_parts[i] = part
        nclose += part.count(')')
    bases = '('.join(bases_parts)
    # insert bases
    pre, post = src[:close_sig+1], src[close_sig+1:]
    src = pre + ' : ' + bases + post
    return src, close_sig + 3 + len(bases)


def cppbases(src):
    """Finds and replaces instances of C++BASES and replaces the relevant
    Cython-generated code with actual calls to the base class constructors.
    For example, in Cython you would write::

        cdef cppclass Child "Child" (cpp_example.Base):

            Child(char* arg):  # C++BASES example::Base(arg)
                pass

    Note that the base classes must use a fully-qualified C++ type, but the
    arguments are the Cython argument name.  This post-processor will translate
    the argument names automatically, but can't do the same for the base class
    names.

    Also note that even without this post-processor, this is still valid Cython.
    It just won't perform as expected at runtime.
    """
    start = src.find('C++BASES', 0)
    while start != -1:
        src, stop = cppbases_replace_one(src, start)
        start = src.find('C++BASES', stop)
    return src


def cppconstructors_replace_one(src, start):
    """Replaces a single instace of C++CONSTRUCTORS at the start location."""
    # now get bases str
    next_nl = src.find('\n', start)
    con_str = src[start+15:next_nl]
    con_str, _, _ = con_str.rpartition('#')
    con_str = con_str.strip()
    cons = [c.strip() for c in con_str.split(';')]
    cons = ';\n  '.join(cons)
    # Now, insert into code
    comment_end = src.find('*/', next_nl)
    next_nl = src.find('\n', comment_end) + 1
    prev, post = src[:next_nl], src[next_nl:]
    src = prev + '  ' + cons + ';\n' + post
    return src, next_nl + 4 + len(cons)


def cppconstructors(src):
    """Finds instances of C++CONSTRUCTORS and adds the relevant
    constructors that Cython may have not declared.
    For example, in Cython you would write::

        cdef cppclass Child "Child" (Base): # C++CONSTRUCTORS Child(char*); Child(int)
            Child(char*)
            Child(int)

    Note that the must fully-qualified C++. Constructors are separated by semicolons
    so as to not overlap with other C++ declaration syntax.

    Also note that even without this post-processor, this is still valid Cython.
    It likely won't compile due to missing declarations.
    """
    start = src.find('C++CONSTRUCTORS', 0)
    while start != -1:
        src, stop = cppconstructors_replace_one(src, start)
        start = src.find('C++CONSTRUCTORS', stop)
    return src



def main(args=None):
    fname = args[1]
    with open(fname, 'r') as f:
        orig = src = f.read()
    src = cppbases(src)
    src = cppconstructors(src)
    if src != orig:
        with open(fname, 'w') as f:
            f.write(src)

if __name__ == '__main__':
    import sys
    main(args=sys.argv)

