from __future__ import print_function
import os
import sys
from argparse import Namespace

import nose
from nose.tools import assert_equal, assert_true, assert_false, assert_raises, \
    assert_in

cycdir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
reldir = os.path.join(cycdir, 'release')
blddir = os.path.join(cycdir, 'build')
sys.path.insert(0, reldir)

import smbchk

def test_load():
    ns = Namespace(filename=os.path.join(reldir, 'symbols.json'))
    db = smbchk.load(ns)
    assert_true(isinstance(db, list))

def test_nm():
    if os.name != 'posix' or not os.path.isdir(blddir):
        return
    ns = Namespace(prefix=blddir)
    syms = smbchk.nm(ns)
    assert_in("cyclus::Agent::Agent(cyclus::Context*)", syms)

def test_diff():
    db = [{'symbols': ["cyclus::Agent::Agent(cyclus::Context*)"], 
           'version': 'X', 'date': 'x.x.x'}, 
          {'symbols': ["cyclus::Agent::Agent(cyclus::Context*)", 
                       "cyclus::Agent::~Agent()"], 
           'version': 'Y', 'date': 'y.y.y'},]
    obs = smbchk.diff(db, 0, 1)
    assert_true(len(obs) > 0)

def test_check():
    # adds to API
    db = [{'symbols': ["cyclus::Agent::Agent(cyclus::Context*)"], 
           'version': 'X', 'date': 'x.x.x'}, 
          {'symbols': ["cyclus::Agent::Agent(cyclus::Context*)", 
                       "cyclus::Agent::~Agent()"], 
           'version': 'Y', 'date': 'y.y.y'},]
    obs = smbchk.check(db)
    assert_true(obs)

    # removes from API
    db.append({'symbols': ["cyclus::Agent::~Agent()"], 
               'version': 'Z', 'date': 'z.z.z'})
    obs = smbchk.check(db)
    assert_false(obs)

if __name__ == "__main__":
    nose.runmodule()
