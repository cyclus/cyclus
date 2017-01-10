import os
import sys
import pprint

import nose
from nose.tools import assert_equal, assert_true, assert_false, assert_raises

cycdir = os.path.dirname(os.path.dirname(__file__))
sys.path.insert(0, os.path.join(cycdir, 'src'))

from hdf5_back_gen import Node, Var, Type, Decl, Expr, Assign, If, For, BinOp, LeftUnaryOp, \
    RightUnaryOp, FuncCall, Raw, DeclAssign, PrettyFormatter, CppGen, ExprStmt, Case, Block, \
    FuncDef, get_item_size

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
    exp = "std::string s"
    n = Decl(type=Type(cpp="std::string"), name=Var(name="s"))
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
def test_cppgen_assign():
    exp = "x=y"
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
  x=1;\n
}else if(x>y){
  x=2;\n
}else{
  x=3;\n
}""".strip()+"\n"
    n = If(cond=BinOp(x=Var(name="x"), op="==", y=Var(name="y")),\
           body=[ExprStmt(child=Assign(target=Var(name="x"), value=Raw(code="1")))],\
           elifs=[(BinOp(x=Var(name="x"), op=">", y=Var(name="y")), [ExprStmt(child=Assign(target=Var(name="x"), value=Raw(code="2")))])],\
           el=ExprStmt(child=Assign(target=Var(name="x"), value=Raw(code="3"))))
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
def test_cppgen_for():
    exp = """
for(int i=0;i<5;i++){
  a++;
  b++;
  c[i]=a+b;\n
}""".strip() + "\n"
    n = For(adecl=DeclAssign(type=Type(cpp="int"), target=Var(name="i"), value=Raw(code="0")),\
            cond=BinOp(x=Var(name="i"), op="<", y=Raw(code="5")),\
            incr=RightUnaryOp(name=Var(name="i"), op="++"),\
            body=[ExprStmt(child=RightUnaryOp(name=Var(name="a"), op="++")),
                  ExprStmt(child=RightUnaryOp(name=Var(name="b"), op="++")),
                  ExprStmt(child=Assign(target=RightUnaryOp(name=Var(name="c"), op="[i]"),
                         value=BinOp(x=Var(name="a"), op="+", y=Var(name="b"))))])
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)

def test_cppgen_funccall():
    exp = """
mult_two<std::string,STRING>(a, b)""".strip()
    n = FuncCall(name=Var(name="mult_two"),\
                 args=[Var(name="a"), Var(name="b")],\
                 targs=[Type(cpp="std::string"), Var(name="STRING")])
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
def test_cppgen_case():
    exp = """case 3: {
  b++;
  break;
}\n"""
    n = Case(cond=Raw(code="3"), 
             body=[ExprStmt(child=RightUnaryOp(name=Var(name="b"), op="++")),
                   Raw(code="break;")])
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
                   
def test_cppgen_block():
    exp = """int x=5;
int y=6;
int z=x+y;\n"""
    n = Block(nodes=[ExprStmt(child=DeclAssign(type=Type(cpp="int"), 
                                               target=Var(name="x"), 
                                               value=Raw(code="5"))),
                     Block(nodes=[ExprStmt(child=DeclAssign(
                                                       type=Type(cpp="int"), 
                                                       target=Var(name="y"), 
                                                       value=Raw(code="6"))),
                                  ExprStmt(child=DeclAssign(
                                                      type=Type(cpp="int"), 
                                                      target=Var(name="z"), 
                                                      value=BinOp(x=Var(name="x"),
                                                                  op="+", 
                                                                  y=Var(name="y"))))])])
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
def test_cppgen_funcdef():
    exp = """template<>
void hello(int a, std::string b) {
  int x=5;
  int y=6;
  int z=x+y;
}
"""
    n = FuncDef(type=Raw(code="void"), name=Var(name="hello"), 
                args=[Decl(type=Type(cpp="int"), name=Var(name="a")),
                      Decl(type=Type(cpp="std::string"), name=Var(name="b"))], 
                body=[ExprStmt(child=DeclAssign(type=Type(cpp="int"), 
                                               target=Var(name="x"), 
                                               value=Raw(code="5"))),
                      ExprStmt(child=DeclAssign(type=Type(cpp="int"), 
                                               target=Var(name="y"), 
                                               value=Raw(code="6"))),
                      ExprStmt(child=DeclAssign(type=Type(cpp="int"), 
                                                target=Var(name="z"), 
                                                value=BinOp(x=Var(name="x"),
                                                            op="+", 
                                                            y=Var(name="y"))))], 
                tspecial=True)
    obs = CPPGEN.visit(n)
    assert_equal(exp, obs)
    
#test various node structures
    
def test_get_item_size():
    exp1 = """((sizeof(int)+CYCLUS_SHA1_SIZE)*shape[0])"""
    obs1 = get_item_size(Type(cpp="std::map<int,std::string>", 
                              db="MAP_INT_VL_STRING", 
                              canon=("MAP","INT","VL_STRING")),
                         [0,1,2])
    exp2 = """((sizeof(int)+shape[2]))"""
    obs2 = get_item_size(Type(cpp="std::pair<int,std::string>", 
                              db="PAIR_INT_STRING", 
                              canon=("PAIR","INT","STRING")), 
                         [0,1,2])
    exp3 = """((sizeof(double))*shape[0])"""
    obs3 = get_item_size(Type(cpp="std::vector<double>", 
                              db="VECTOR_DOUBLE", 
                              canon=("VECTOR","DOUBLE")), 
                         [0,1])
    assert_equal(exp1, obs1)
    assert_equal(exp2, obs2)
    assert_equal(exp3, obs3)
    
    
    
