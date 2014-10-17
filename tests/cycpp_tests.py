import os
import sys
from collections import OrderedDict

import nose
from nose.tools import assert_equal, assert_true, assert_false, assert_raises

cycdir = os.path.dirname(os.path.dirname(__file__))
sys.path.insert(0, os.path.join(cycdir, 'cli'))

# pass 1 Filters
from cycpp import NamespaceFilter, TypedefFilter, UsingFilter,\
        UsingNamespaceFilter, NamespaceAliasFilter, ClassFilter, \
        AccessFilter, PragmaCyclusErrorFilter

# pass 2 Filters
from cycpp import VarDecorationFilter, VarDeclarationFilter, ExecFilter, \
    NoteDecorationFilter

# pass 3 Filters
from cycpp import CloneFilter, InitFromCopyFilter, \
        InitFromDbFilter, InfileToDbFilter, SchemaFilter, SnapshotFilter, \
        SnapshotInvFilter, InitInvFilter, DefaultPragmaFilter, AnnotationsFilter

class MockMachine(object):
    def __init__(self):
        self.depth = 0
        self.execns = {}
        self.context = {}
        self.statements = []
        self.classes = []
        self.superclasses = {}
        self.access = {}
        self.namespaces = []
        self.using_namespaces = set()
        self.aliases = set()
        self.var_annotations = None
        self.filters = []

    def includeloc(self, statement=None):
        return ""

    def classname(self):
        """Implemented just for testing"""
        return ""

    def ensure_class_context(self, classname):
        if classname not in self.context:
            self.context[classname] = OrderedDict()


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

def test_synerror():
    """Test PragmaCyclusErrorFilter"""
    m = MockMachine()
    f = PragmaCyclusErrorFilter(m)
    yield assert_false, f.isvalid("#pragma cyclus var {}")
    yield assert_false, f.isvalid("#pragma cyclus")

    yield assert_true, f.isvalid('#pragma cyclus nooooo')
    statement, sep = "#pragma cyclus var{}", "\n"
    yield assert_true, f.isvalid(statement)
    yield assert_raises, SyntaxError, f.transform, statement, sep

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

    statement, sep = "#pragma cyclus exec x = 42", "\n"
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    # What are the other possible tests
    yield assert_equal, m.execns["x"], 42

def test_notefilter():
    """Test NoteDecorationFilter"""
    m = MockMachine()
    f = NoteDecorationFilter(m)
    yield assert_false, f.isvalid("#pragma cyclus")

    statement, sep = "#pragma cyclus note {'doc': 'string'} ", "\n"
    yield assert_true, f.isvalid(statement)
    f.transform(statement, sep)
    yield assert_equal, m.context['']['doc'], 'string'

class MockAliasCodeGenMachine(object):
    """Mock machine for testing aliasing on pass 3 filters"""
    def __init__(self):
        self.depth = 0
        self.execns = {}
        self.context = {"MyFactory": OrderedDict([('vars', OrderedDict([
            ('bar_var', {
                'type': 'std::string',
                'alias': 'foo_alias',
            }),('foo_alias', 'bar_var'),
            ('bar_map_var', {
                'type': ('std::map', 'std::string', 'int'),
                'alias': 'foo_map_alias',
            }),('foo_map_alias', 'bar_map_var'),
            ]))
            ])}
        self.statements = []
        self.classes = []
        self.superclasses = {'MyFactory': ()}
        self.access = {}
        self.namespaces = []
        self.using_namespaces = set()
        self.aliases = set()
        self.var_annotations = None
        self.filters = []
        self.local_classname = "MyFactory"

    def classname(self):
        return self.local_classname

#
# pass 3 Filters
#
class MockCodeGenMachine(object):
    """Mock machine for testing pass 3 filters"""
    def __init__(self):
        self.depth = 0
        self.execns = {}
        self.context = {"MyFactory": OrderedDict([('vars', OrderedDict([
            ('x', {'type': 'int', 'schematype': 'positiveInteger'}),
            ('y', {'type': 'std::string',
                   'shape': [42],
                   'initfromcopy': 'y=m -> y;\n',
                   'initfromdb': 'WAKKA JAWAKA',
                   'infiletodb': {'read': 'THINGFISH\n',
                                  'write': 'ABSOLUTELY FREE\n'},
                   'schema': "FREAK OUT\n",
                   'snapshot': "JUST ANOTHER BAND FROM LA\n"
                   }),
            ]))
            ])}
        self.statements = []
        self.classes = []
        self.superclasses = {'MyFactory': ()}
        self.access = {}
        self.namespaces = []
        self.using_namespaces = set()
        self.aliases = set()
        self.var_annotations = None
        self.filters = []
        self.local_classname = "MyFactory"

    def classname(self):
        return self.local_classname

def test_clonefilter():
    """Test CloneFilter"""
    m = MockCodeGenMachine()
    f = CloneFilter(m)
    f.given_classname = 'MyFactory'
    impl = f.impl()
    exp_impl = "  MyFactory* m = new MyFactory(context());\n" + \
               "  m->InitFrom(this);\n  return m;\n"
    assert_equal(exp_impl, impl)

def test_ifcfilter():
    """Test InitFromCopyFilter"""
    m = MockCodeGenMachine()
    f = InitFromCopyFilter(m)
    f.given_classname = 'MyFactory'

    args = f.methodargs()
    exp_args = "MyFactory* m"
    yield assert_equal, exp_args, args

    impl = f.impl()
    exp_impl = ('  int rawcycpp_shape_y[1] = {42};\n'
                '  cycpp_shape_y = std::vector<int>(rawcycpp_shape_y, '
                                                   'rawcycpp_shape_y + 1);\n'
                "  x = m->x;\n"
                "y=m -> y;\n")
    yield assert_equal, exp_impl, impl

def test_ifdbfilter():
    """Test InitFromDbFilter"""
    m = MockCodeGenMachine()
    f = InitFromDbFilter(m)
    f.given_classname = 'MyFactory'

    args = f.methodargs()
    exp_args = "cyclus::QueryableBackend* b"
    yield assert_equal, exp_args, args

    impl = f.impl()
    exp_impl = ('  int rawcycpp_shape_y[1] = {42};\n'
                '  cycpp_shape_y = std::vector<int>(rawcycpp_shape_y, '
                                                   'rawcycpp_shape_y + 1);\n'
                '  cyclus::QueryResult qr = b->Query("Info", NULL);\n'
                '  x = qr.GetVal<int>("x");\n'
                "WAKKA JAWAKA")
    yield assert_equal, exp_impl, impl

def test_aliasing_schemafilter():
    impl = setup_alias(SchemaFilter)

    assert_true('foo_alias' in impl)
    assert_false('bar_var' in impl)
    assert_true('foo_map_alias' in impl)
    assert_false('bar_map_var' in impl)

def test_aliasing_snapshotfilter():
    impl = setup_alias(SnapshotFilter)

    assert_false('foo_alias' in impl)
    assert_true('bar_var' in impl)
    assert_false('foo_map_alias' in impl)
    assert_true('bar_map_var' in impl)

def test_aliasing_infiletodbfilter():
    impl = setup_alias(InfileToDbFilter)

    assert_true('foo_alias' in impl)
    assert_true('bar_var' in impl)
    assert_true('foo_map_alias' in impl)
    assert_true('bar_map_var' in impl)

def setup_alias(filt):
    m = MockAliasCodeGenMachine()
    f = filt(m)
    f.given_classname = 'MyFactory'
    return f.impl()

def test_itdbfilter():
    """Test InfileToDbFilter"""
    m = MockCodeGenMachine()
    f = InfileToDbFilter(m)
    f.given_classname = 'MyFactory'

    args = f.methodargs()
    exp_args = "cyclus::InfileTree* tree, cyclus::DbInit di"
    yield assert_equal, exp_args, args

    impl = f.impl()
    exp_impl = ('  int rawcycpp_shape_y[1] = {42};\n'
                '  cycpp_shape_y = std::vector<int>(rawcycpp_shape_y, '
                                                   'rawcycpp_shape_y + 1);\n'
                '  tree = tree->SubTree("config/*");\n'
                '  cyclus::InfileTree* sub;\n'
                '  int i;\n'
                '  int n;\n'
                '  x = cyclus::Query<int>(tree, "x");\n'
                'THINGFISH\n'
                '  di.NewDatum("Info")\n'
                '  ->AddVal("x", x)\n'
                'ABSOLUTELY FREE\n'
                '  ->Record();\n')
    yield assert_equal, exp_impl, impl

def test_schemafilter():
    """Test SchemaFilter"""
    m = MockCodeGenMachine()
    f = SchemaFilter(m)
    f.given_classname = 'MyFactory'

    args = f.methodargs()
    exp_args = ""
    yield assert_equal, exp_args, args

    impl = f.impl()
    exp_impl = ('  return ""\n'
                '    "<interleave>\\n"\n'
                '    "<element name=\\"x\\">\\n"\n'
                '    "    <data type=\\"positiveInteger\\" />\\n"\n'
                '    "</element>\\n"\n'
                "FREAK OUT\n"
                '    "</interleave>\\n"\n    ;\n')
    yield assert_equal, exp_impl, impl

    # schema type tests
    yield assert_equal, 'string', f._type('std::string')
    yield assert_equal, 'boolean', f._type('bool')
    yield assert_equal, 'token', f._type('std::string', 'token')

    m.context = {"MyFactory": OrderedDict([('vars', OrderedDict([
            ('x', {'type': ('std::map', 'int', 'double')}),
            ]))
            ])}
    impl = f.impl()
    exp_impl = ('  return ""\n'
                '    "<interleave>\\n"\n'
                '    "<element name=\\"x\\">\\n"\n'
                '    "    <oneOrMore>\\n"\n'
                '    "        <element name=\\"key\\">\\n"\n'
                '    "            <data type=\\"int\\" />\\n"\n'
                '    "        </element>\\n"\n'
                '    "        <element name=\\"val\\">\\n"\n'
                '    "            <data type=\\"double\\" />\\n"\n'
                '    "        </element>\\n"\n'
                '    "    </oneOrMore>\\n"\n'
                '    "</element>\\n"\n'
                '    "</interleave>\\n"\n    ;\n')
    yield assert_equal, exp_impl, impl

def test_annotationsfilter():
    """Test SchemaFilter"""
    m = MockCodeGenMachine()
    f = AnnotationsFilter(m)
    f.given_classname = 'MyFactory'

    args = f.methodargs()
    exp_args = ""
    yield assert_equal, exp_args, args

    impl = f.impl()
    yield assert_true, isinstance(impl, str)

def test_snapshotfilter():
    """Test SnapshotFilter"""
    m = MockCodeGenMachine()
    f = SnapshotFilter(m)
    f.given_classname = 'MyFactory'

    args = f.methodargs()
    exp_args = 'cyclus::DbInit di'
    yield assert_equal, exp_args, args

    impl = f.impl()
    exp_impl = ('  di.NewDatum("Info")\n'
                '  ->AddVal("x", x)\n'
                'JUST ANOTHER BAND FROM LA\n'
                '  ->Record();\n')
    yield assert_equal, exp_impl, impl

def test_sshinvfilter():
    """Test SnapshotInvFilter"""
    m = MockCodeGenMachine()
    f = SnapshotInvFilter(m)
    f.given_classname = 'MyFactory'

    args = f.methodargs()
    exp_args = ''
    yield assert_equal, exp_args, args

    impl = f.impl()
    exp_impl = ("  cyclus::Inventories invs;\n"
                "  return invs;\n")
    yield assert_equal, exp_impl, impl

def test_intinvfilter():
    """Test InitInvFilter"""
    m = MockCodeGenMachine()
    f = InitInvFilter(m)
    f.given_classname = 'MyFactory'

    args = f.methodargs()
    exp_args = "cyclus::Inventories& inv"
    yield assert_equal, exp_args, args

    impl = f.impl()
    exp_impl = ''
    yield assert_equal, exp_impl, impl

def test_defpragmafilter():
    """Test DefaultPragmaFilter"""
    m = MockCodeGenMachine()
    f = DefaultPragmaFilter(m)


if __name__ == "__main__":
    nose.runmodule()
