import os
import sys
import pprint

import nose
from nose.tools import assert_equal, assert_true, assert_false, assert_raises

cycdir = os.path.dirname(os.path.dirname(__file__))
sys.path.insert(0, os.path.join(cycdir, '../src'))

from hdf5_back_gen import Node, Var, Type, Decl, Expr, Assign, If, For, BinOp, LeftUnaryOp, \
    RightUnaryOp, FuncCall, Raw, DeclAssign, PrettyFormatter, CppGen, ExprStmt, Case

PRETTY = PrettyFormatter()
CPPGEN = CppGen()

#pretty print tests

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

#cppgen tests

def test_cppgen_var():
    exp = "x"
    n = Var(name="x")
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
def test_cppgen_type():
    exp = "std::string"
    n = Type(cpp="std::string")
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
def test_cppgen_decl():
    exp = "std::string s;"
    n = Decl(type=Type(cpp="std::string"), name=Var(name="s"))
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
def test_cppgen_assign():
    exp = "x = y;"
    n = Assign(target=Var(name="x"), value=Var(name="y"))
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)  
    
def test_cppgen_binop():
    exp = "x+y"
    n = BinOp(x=Var(name="x"), op="+", y=Var(name="y"))
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
def test_cppgen_leftunaryop():
    exp = "++x"
    n = LeftUnaryOp(op="++", name=Var(name="x"))
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
def test_cppgen_rightunaryop():
    exp = "x++"
    n = RightUnaryOp(name=Var(name="x"), op="++")
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)

def test_cppgen_if():
    exp = """
if(x==y){
  x = 1;
}else if(x>y){
  x = 2;
}else{
  x = 3;
}""".strip()
    n = If(cond=BinOp(x=Var(name="x"), op="==", y=Var(name="y")),\
           body=[Assign(target=Var(name="x"), value=Raw(code="1"))],\
           elifs=[(BinOp(x=Var(name="x"), op=">", y=Var(name="y")), [Assign(target=Var(name="x"), value=Raw(code="2"))])],\
           el=Assign(target=Var(name="x"), value=Raw(code="3")))
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
def test_cppgen_for():
    exp = """
for(int i=0;i<5;i++){
  a++;
  b++;
  c[i] = a+b;
}""".strip()
    n = For(adecl=DeclAssign(type=Type(cpp="int"), target=Var(name="i"), value=Raw(code="0")),\
            cond=BinOp(x=Var(name="i"), op="<", y=Raw(code="5")),\
            incr=RightUnaryOp(name=Var(name="i"), op="++"),\
            body=[ExprStmt(child=RightUnaryOp(name=Var(name="a"), op="++")),
                  ExprStmt(child=RightUnaryOp(name=Var(name="b"), op="++")),
                  Assign(target=RightUnaryOp(name=Var(name="c"), op="[i]"),
                         value=BinOp(x=Var(name="a"), op="+", y=Var(name="b")))])
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)

def test_cppgen_funccall():
    exp = """
mult_two<std::string,STRING>(a,b)""".strip()
    n = FuncCall(name=Var(name="mult_two"),\
                 args=[Var(name="a"), Var(name="b")],\
                 targs=[Type(cpp="std::string"), Var(name="STRING")])
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
def test_cppgen_case():
    exp = """
case 3:
  b++;
  break;""".strip()
    n = Case(cond=Raw(code="3"), 
             body=[ExprStmt(child=RightUnaryOp(name=Var(name="b"), op="++")),
                   Raw(code="break;")])
    
    
    
    
    
    
    
    
    
    
    
