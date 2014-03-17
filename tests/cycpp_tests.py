import os
import sys

import nose
from nose.tools import assert_equal, assert_true, assert_false

cycdir = os.path.dirname(os.path.dirname(__file__))
sys.path.insert(0, os.path.join(cycdir, 'cli'))

from cycpp import NamespaceFilter

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

if __name__ == "__main__":
    nose.runmodule()