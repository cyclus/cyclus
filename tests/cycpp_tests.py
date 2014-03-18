import os
import sys

import nose
from nose.tools import assert_equal, assert_true, assert_false

cycdir = os.path.dirname(os.path.dirname(__file__))
sys.path.insert(0, os.path.join(cycdir, 'cli'))

# pass 1 Filters
from cycpp import NamespaceFilter, TypedefFilter, UsingFilter,\
        UsingNamespaceFilter, NamespaceAliasFilter, ClassFilter, \
        AccessFilter

# pass 2 Filters
from cycpp import VarDecorationFilter, VarDeclarationFilter, ExecFilter


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

    def classname(self):
        """Implemented just for testing"""
        return ""

#
# pass 1 Filters
#

def test_tffilt():
    """Test TypedefFilter"""
    m = MockMachine()
    f = TypedefFilter(m)
    yield assert_false, f.isvalid("mis typedef kind")
    yield assert_false, f.isvalid("typedef kind")

    statement, sep = "typedef double db", ";"
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, len(m.aliases), 1
    yield assert_equal, (0, "double", "db"), m.aliases.pop()

    statement, sep = "typedef struct {int a; int b;} S, *pS", ";"
    yield assert_true, f.isvalid(statement)

def test_uffilt():
    """Test UsingFilter"""
    m = MockMachine()
    f = UsingFilter(m)
    yield assert_false, f.isvalid("not using namespace")

    statement, sep = "using std::cout", ""
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, len(m.aliases), 1
    yield assert_equal, (0, "std::cout", "cout"), m.aliases.pop()

def test_nsfilt():
    """Test NamespaceFilter"""
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

def test_unfilt():
    """Test UsingNamespaseFilter"""
    m = MockMachine()
    f = UsingNamespaceFilter(m)
    yield assert_false, f.isvalid("using cycamore")

    statement, sep = "using namespace std", ""
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, len(m.using_namespaces), 1
    yield assert_equal, (0, "std"), m.using_namespaces.pop()
    f.revert(statement, sep)
    yield assert_equal, len(m.using_namespaces), 0

def test_nafilter():
    """Test NamespaceAliasFilter"""
    m = MockMachine()
    f = NamespaceAliasFilter(m)
    yield assert_false, f.isvalid("namespace cycamore")

    statement, sep = "namespace cycamore = cm", ""
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, len(m.aliases), 1
    yield assert_equal, (0, "cm", "cycamore"), m.aliases.pop()

def test_cfilter():
    """Test ClassFilter"""
    m = MockMachine()
    f = ClassFilter(m)
    yield assert_false, f.isvalid("class ")

    statement, sep = "class Cyclus", ""
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, len(m.classes), 1
    yield assert_equal, m.classes[0], (0, "Cyclus")
    yield assert_equal, m.access[tuple(m.classes)], "private"
    f.revert(statement, sep)
    yield assert_equal, len(m.classes), 0

def test_afilter():
    """Test AccessFilter"""
    m = MockMachine()
    f = AccessFilter(m)
    yield assert_false, f.isvalid("new private")

    statement, sep = "private:", ""
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, m.access[tuple(m.classes)], "private"

#
# pass 2 Filters
#
def test_vdecorfilter():
    """Test VarDecorationFilter"""
    m = MockMachine()
    f = VarDecorationFilter(m)
    yield assert_false, f.isvalid("#pragma cyclus")

    statement, sep = "#pragma cyclus var {'name': 'James Bond'} ", "\n"
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, m.var_annotations, {'name': 'James Bond'}

def test_vdeclarfilter():
    """Test VarDeclarationFilter"""
    m = MockMachine()
    f = VarDeclarationFilter(m)
    yield assert_false, f.isvalid("one ")

    statement, sep = "one two", "\n"
    yield assert_true, f.isvalid(statement)
    m.classes = [(0, "trader")]
    m.access = {"trader": "public"}
   # m.var_annotations = {'name': 'James Bond'}
    f.transform(statement, sep)
    yield assert_equal, m.var_annotations, None

def test_execfilter():
    """Test ExecFilter"""
    m = MockMachine()
    f = ExecFilter(m)
    yield assert_false, f.isvalid("#pragma cyclus")

    statement, sep = "#pragma cyclus exec print('Hello World!')", "\n"
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    # What are the other possible tests



if __name__ == "__main__":
    nose.runmodule()
