import os
import sys
import pprint

import nose
from nose.tools import assert_equal, assert_true, assert_false, assert_raises

cycdir = os.path.dirname(os.path.dirname(__file__))
sys.path.insert(0, os.path.join(cycdir, 'src'))

from hdf5_back_gen import Node, Var, Type, Decl, Expr, Assign, If, For, BinOp, LeftUnaryOp, \
    RightUnaryOp, FuncCall, Raw, PrettyFormatter

PRETTY = PrettyFormatter()
    
def test_node_pretty():
    exp = "Node()"
    n = Node()
    obs = PRETTY.visit(n)
    assert_equal(exp, obs)
    
def test_var_pretty():
    exp = "Var(\n name='x'\n)"
    n = Var(name="x")
    obs = PRETTY.visit(n)
    assert_equal(exp, obs)
    
def test_decl_pretty():
    exp = "Decl(\n type='x',\n name='y'\n)"
    n = Decl(type="x", name="y")
    obs = PRETTY.visit(n)
    assert_equal(exp, obs)
    
