#! /usr/bin/env python
"""Collects & diffs public symobls in libcyclus.so. Used to ensure stability 
between versions. Now with 100% fewer vowels!
"""
from __future__ import print_function
import os
import io
import sys
import argparse
import difflib
import subprocess
import pprint
try:
    import simplejson as json
except ImportError:
    import json

def load(ns):
    """Loads a database of symbols or returns an empty list."""
    if not os.path.isfile(ns.filename):
        return []
    with io.open(ns.filename, 'rt') as f:
        db = json.loads(f)
    return db

def save(db, ns):
    """Saves a database of symbols."""
    with io.open(ns.filename, 'wb') as f:
        json.dump(db, f, indent=1, separators=(',', ': '))

def nm(ns):
    """Obtains the latest symbols as a sorted list by running and parsing the 
    posix nm utility. Note that this is *not* compatible with Darwin's nm 
    utility because Apple is unfathomably 'special.'
    """
    # in the nm command, the following option are for:
    #   -g: external only, ie this prints only public symbols
    #   -p: no-sort, doesn't sort the output first, since we will be sorting
    #       later this makes things a tad faster
    #   -C: demangles the C++ symbols.
    #   -fbsd: formats output in default 'bsd' style
    # note that we are only going to pick up symbols in the cyclus namespace
    # because everything else should be external to the cares of cyclus stability
    lib = os.path.abspath(os.path.join(ns.prefix, 'lib', 'libcyclus.so'))
    stdout = subprocess.check_output(['nm', '-g', '-p', '-C', '-fbsd', lib])
    types = set()
    names = set()
    ok_types = {'B', 'b', 'D', 'd', 'R', 'r', 'S', 's', 'T', 't', 'W', 'w', 'u'}
    for line in stdout.splitlines():
        line = line.strip()
        if len(line) == 0 or not line[0].isdigit():
            continue
        val, typ, name = line.split(None, 2)
        types.add(typ)
        if not name.startswith('cyclus::'):
            continue
        if ' ' in name:
            # handle funny private pointer cases
            pre, post = name.split(' ', 1)
            if pre.endswith('*') or post.startswith('std::__'):
                continue
        if typ in ok_types:
            names.add(name)
    return sorted(names)

def git_log():
    """Returns git SHA, date, and timestamp from log."""
    stdout = subprocess.check_output(['git', 'log', '--pretty=format:%H/%ci/%ct', '-n1'])
    return stdout.strip().split('/')

def core_version():
    stdout = subprocess.check_output(['cyclus', '--version'])
    return stdout.splitlines()[0].strip()

def update(db, ns):
    """Updates a symbol database with the latest values."""
    if ns.tag is None:
        sys.exit("a tag for the version must be given when updating, eg '--tag 1.1'")
    symbols = nm(ns)
    sha, d, t = git_log()
    entry = {'symbols': symbols, 'sha': sha, 'date': d, 'timestamp': t, 
             'tag': ns.tag, 'version': core_version(),}
    db.append(entry)

def diff(db, i, j):
    """Diffs two database indices, returns string unified diff."""
    x = db[i]
    y = db[j]
    lines = list(difflib.unified_diff(x['symbols'], y['symbols'], 
                                      fromfile=x['version'], tofile=y['version'], 
                                      fromfiledate=x['date'], tofiledate=y['date']))
    return '\n'.join(lines)

def check(db):
    """Checks if an API is stable, returns bool, prints debug info."""
    if len(db) < 2:
        sys.exit('too few entries in database to check for stability')
    stable = True
    for i, (x, y) in enumerate(zip(db[:-1], db[1:])):
        if not (frozenset(x['symbols']) <= frozenset(y['symbols'])):
            stable = False
            d = diff(db, i, i+1)
            print(d)
    if stable:
        print('API stability has been achieved!')
    return stable

def main():
    if os.name != 'posix':
        sys.exit("must be run on a posix system, "
                 "'nm' utility not compatible elsewhere.")
    p = argparse.ArgumentParser('smbchk', description=__doc__)
    p.add_argument('--prefix', dest='prefix', default='../build', 
                   help="location of lib dir with libcyclus, default '../build'")
    p.add_argument('-f', '--filename', dest='filename', default='symbols.json', 
                   help="historical symbols database, default 'symbols.json'")
    p.add_argument('--dump', action='store_true', default=False, dest='dump', 
                   help='dumps existing symbols')
    p.add_argument('--update', dest='update', action='store_true', default=False, 
                   help='updates the symbols with the current version')
    p.add_argument('--save', action='store_true', default=True, dest='save', 
                   help='saves the database')
    p.add_argument('--no-save', action='store_false', default=True, dest='save', 
                   help='does not save the database')
    p.add_argument('-t', '--tag', dest='tag', default=None, 
                   help='version tag used when updating, eg 1.0.0-rc5')
    p.add_argument('-c', '--check', action='store_true', dest='check', default=False, 
                   help='checks that the API is stable')
    p.add_argument('-d', '--diff', nargs=2, dest='diff', type=int, default=(), 
                   help='takes the difference between two database indices')
    ns = p.parse_args()

    db = load(ns)
    if ns.update:
        update(db, ns)
        if ns.save:
            save(db, ns)
    if ns.dump:
        pprint.pprint(db)
    if ns.check:
        check(db)
    if len(ns.diff) == 2:
        d = diff(db, ns.diff[0], ns.diff[1])
        print(d)

if __name__ == '__main__':
    main()