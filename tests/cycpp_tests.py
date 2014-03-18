import os
import sys

import nose
from nose.tools import assert_equal, assert_true, assert_false

cycdir = os.path.dirname(os.path.dirname(__file__))
sys.path.insert(0, os.path.join(cycdir, 'cli'))

from cycpp import NamespaceFilter, TypedefFilter, UsingFilter,\
        UsingNamespaceFilter, NamespaceAliasFilter, ClassFilter, \
        AccessFilter

class MockMachine(object):
    def __init__(self):
        self.depth = 0
        self.execns = {}
        self.context = {}
        self.statements = []
        self.classes = []
        self.access = {}
        self.namespaces = []
        self.using_namespaces = set()
        self.aliases = set()
        self.var_annotations = None
        self.filters = []

def test_nsfilt():
    m = MockMachine()
    f = NamespaceFilter(m)
    yield assert_false, f.isvalid("olzhas is not a namespace")

    # anonymous namespaces
    statement, sep = " namespace ", "{"
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, len(m.namespaces), 1
    yield assert_equal, m.namespaces[0], (0, None)
    f.revert(statement, sep)
    yield assert_equal, len(m.namespaces), 0

    # nymous namespace
    statement, sep = "namespace gorgus ", "{"
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, len(m.namespaces), 1
    yield assert_equal, m.namespaces[0], (0, "gorgus")
    f.revert(statement, sep)
    yield assert_equal, len(m.namespaces), 0

def test_tffilt():
    """Test TypedefFilter"""
    m = MockMachine()
    f = TypedefFilter(m)
    yield assert_false, f.isvalid("mis typedef kind")

    statement, sep = "typedef boost type ptr", ";"
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, len(m.aliases), 1
#    yield assert_true, (0, "typedef", "boost type ptr") in  m.aliases

    statement, sep = "typedef struct {int a; int b;} S, *pS", ";"
    # yield assert_false, f.isvalid(statement)

def test_uffilt():
    """Test UsingFilter"""
    m = MockMachine()
    f = UsingFilter(m)
    yield assert_false, f.isvalid("using namespace")

    statement, sep = "using std", ""
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, len(m.aliases), 1
    yield assert_equal, f.name, "using std"
    #yield assert_true, (...) in m.aliases

def test_unfilt():
    """Test UsingNamespaseFilter"""
    m = MockMachine()
    f = UsingNamespaceFilter(m)
    yield assert_false, f.isvalid("using cycamore")

    statement, sep = "using namespace", ""
    yield assert_true, f.isvalid(statement)

def test_nafilter():
    """Test NamespaceAliasFilter"""
    m = MockMachine()
    f = NamespaceAliasFilter(m)

def test_cfilter():
    """Test ClassFilter"""
    m = MockMachine()
    f = ClassFilter(m)

def test_afilter():
    """Test AccessFilter"""
    m = MockMachine()
    f = AccessFilter(m)

if __name__ == "__main__":
    nose.runmodule()
