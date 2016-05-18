import os
import sys
import uuid
import pprint
import tempfile
from collections import OrderedDict
from subprocess import Popen, PIPE, STDOUT

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
    NoteDecorationFilter, StateAccumulator

# pass 3 Filters
from cycpp import CloneFilter, InitFromCopyFilter, \
        InitFromDbFilter, InfileToDbFilter, SchemaFilter, SnapshotFilter, \
        SnapshotInvFilter, InitInvFilter, DefaultPragmaFilter, AnnotationsFilter

import cycpp

assert_equal.__self__.maxDiff = None

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
    yield assert_equal, m.namespaces[0], (0, '<anonymous>')
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

def test_vdeclarfilter_canonize_alias():
    m = MockMachine()
    f = VarDeclarationFilter(m)
    cases = [
        # exp   type  name  alias
        ('x', 'float', 'x', None),
        (['x', 'val'], ['std::set', 'float'], 'x', None),
        (['x', 'first', 'second'], ['std::pair', 'int', 'int'], 'x', None),
        ([['x', 'item'], 'key', 'val'], ['std::map', 'int', 'int'], 'x', None),
        ([['x', 'item'], ['key', 'first', 'second'], ['val', 'val']],
            ['std::map', ['std::pair', 'int', 'int'], ['std::set', 'int']],
            'x', None),
        ('y', 'float', 'x', 'y'),
        (['x', 'y'], ['std::set', 'float'], 'x', [None, 'y']),
        (['y', 'first', 'second'], ['std::pair', 'int', 'int'],
            'x', ['y']),
        ([['x', 'item'], 'y', 'val'], ['std::map', 'int', 'int'], 'x',
            [None, 'y']),
        ([['x', 'ytem'], 'y', 'val'], ['std::map', 'int', 'int'], 'x',
            [['x', 'ytem'], 'y']),
        ([['x', 'ytem'], ['key', 'f', 's'], ['data', 'val']],
            ['std::map', ['std::pair', 'int', 'int'], ['std::set', 'int']],
            'x', [[None, 'ytem'], [None, 'f', 's'], 'data']),
        ]
    for exp, t, name, alias in cases:
        obs = f.canonize_alias(t, name, alias=alias)
        yield assert_equal, exp, obs

def test_vdeclarfilter_canonize_ui():
    m = MockMachine()
    f = VarDeclarationFilter(m)
    cases = [
        # exp   type  name  uilabel/tooltip
        ('foo', 'float', 'x', 'foo'),
        (['foo', ''], ['std::set', 'float'], 'x', 'foo'),
        (['foo', 'bar'], ['std::set', 'float'], 'x', ['foo', 'bar']),
        ([['foo', ''], ['', '', ''], ['', '']],
         ['std::map', ['std::pair', 'int', 'int'], ['std::set', 'int']],
         'x',
         'foo'),
        ([['foo', ''], ['bar', '', ''], ['', '']],
         ['std::map', ['std::pair', 'int', 'int'], ['std::set', 'int']],
         'x',
         ['foo', 'bar']),
        ([['foo', ''], ['bar', '', ''], ['baz', '']],
         ['std::map', ['std::pair', 'int', 'int'], ['std::set', 'int']],
         'x',
         ['foo', 'bar', 'baz']),
    ]
    for exp, t, name, x in cases:
        obs = f.canonize_uilabel(t, name, uilabel=x)
        yield assert_equal, exp, obs
        obs = f.canonize_tooltip(t, name, tooltip=x)
        yield assert_equal, exp, obs


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


def test_canon_type():
    sa = StateAccumulator()
    cases = [
        ('double', 'double'),
        ('std::string', 'std::string'),
        ('std::vector<int>', ('std::vector', 'int')),
        ('std::map<int, cyclus::Blob>', ('std::map', 'int', 'cyclus::Blob')),
        ('std::pair<int,std::string>', ('std::pair', 'int', 'std::string')),
        ('std::map<std::pair<int, std::string>, double>',
            ('std::map', ('std::pair', 'int', 'std::string'), 'double')),
        ]
    for t, exp in cases:
        obs = sa.canonize_type(t)
        yield assert_equal, exp, obs

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
    exp_impl = (
        '  int rawcycpp_shape_y[1] = {42};\n  cycpp_shape_y = std::vector<int>(rawcycpp_shape_y, rawcycpp_shape_y + 1);\n  cyclus::InfileTree* sub = tree->SubTree("config/*");\n  int i;\n  int n;\n  {\n    int x_val = cyclus::Query<int>(sub, "x");\n    x = x_val;\n  }\nTHINGFISH\n  di.NewDatum("Info")\n  ->AddVal("x", x)\nABSOLUTELY FREE\n  ->Record();\n'
        )

    yield assert_equal, exp_impl, impl

def test_itdbfilter_val():
    """Test InfileToDbFilter._val() Defaults"""
    m = MockCodeGenMachine()
    f = InfileToDbFilter(m)

    cases = [
        ('bool', True, 'foo', None, 'bool foo = true;\n'),
        ('bool', False, 'foo', None, 'bool foo = false;\n'),
        ('int', 42, 'foo', None, 'int foo = 42;\n'),
        ('int', 92235, 'foo', 'nuclide', 'int foo = pyne::nucname::id(92235);\n'),
        ('int', 'U-235', 'foo', 'nuclide', 'int foo = pyne::nucname::id("U-235");\n'),
        ('float', 42.0, 'foo', None, 'double foo = 42.0;\n'),
        ('double', 42.0, 'foo', None, 'double foo = 42.0;\n'),
        ('std::string', 'wakka', 'foo', None, 'std::string foo("wakka");\n'),
        ('cyclus::Blob', 'wakka', 'foo', None, 'cyclus::Blob foo("wakka");\n'),
        ('boost::uuids::uuid',
            '/#\xfb\xaf\x90\xc9N\xe9\x98:S\xea\xd6\xd6\x0fb', 'foo', None,
            'boost::uuids::uuid foo = "/#\xfb\xaf\x90\xc9N\xe9\x98:S\xea\xd6\xd6\x0fb";\n'),
        ('boost::uuids::uuid',
            uuid.UUID('2f23fbaf-90c9-4ee9-983a-53ead6d60f62'), 'foo', None,
            'boost::uuids::uuid foo = {0x2f, 0xf3, 0x2b, 0x3f, 0xf0, 0xb9, 0xae, 0xf9, 0x98, 0x0a, 0xc3, 0x9a, 0x46, 0xe6, 0xef, 0x92};\n'),
        (('std::vector', 'int'), [42], 'foo', None,
            ('std::vector< int > foo;\n'
             'foo.resize(1);\n'
             '{\n'
             '  {\n'
             '    int elem = 42;\n'
             '    foo[0] = elem;\n'
             '  }\n'
             '}\n'),
            ),
        (('std::vector', 'int'), [92235], 'foo', [None, 'nuclide'],
            ('std::vector< int > foo;\n'
             'foo.resize(1);\n'
             '{\n'
             '  {\n'
             '    int elem = pyne::nucname::id(92235);\n'
             '    foo[0] = elem;\n'
             '  }\n'
             '}\n'),
            ),
        (('std::set', 'int'), [42, 65], 'foo', None,
            ('std::set< int > foo;\n'
             '{\n'
             '  {\n'
             '    int elem = 42;\n'
             '    foo.insert(elem);\n'
             '  }\n'
             '  {\n'
             '    int elem = 65;\n'
             '    foo.insert(elem);\n'
             '  }\n'
             '}\n'),
            ),
        (('std::list', 'int'), [42, 65], 'foo', None,
            ('std::list< int > foo;\n'
             '{\n'
             '  {\n'
             '    int elem = 42;\n'
             '    foo.push_back(elem);\n'
             '  }\n'
             '  {\n'
             '    int elem = 65;\n'
             '    foo.push_back(elem);\n'
             '  }\n'
             '}\n'),
            ),
        (('std::pair', 'int', 'double'), [42, 65.0], 'foo', None,
            ('std::pair< int, double > foo;\n'
             '{\n'
             '  int first = 42;\n'
             '  double second = 65.0;\n'
             '  foo.first = first;\n'
             '  foo.second = second;\n'
             '}\n'),
            ),
        (('std::map', 'int', 'double'), {42: 65.0}, 'foo', None,
            ('std::map< int, double > foo;\n'
             '{\n'
             '  {\n'
             '    int key = 42;\n'
             '    double val = 65.0;\n'
             '    foo[key] = val;\n'
             '  }\n'
             '}\n'),
            ),
        (('std::map', 'std::string', ('std::pair', 'bool', ('std::vector', 'double'))),
            OrderedDict([('hello', [True, [1.1, 2.2, 3.3]]), ('goodbye', [False, [3.3, 2.2, 1.1]])]),
            'foo', None,
            ('std::map< std::string, std::pair< bool, std::vector< double > > > foo;\n'
             '{\n'
             '  {\n'
             '    std::string key("hello");\n'
             '    std::pair< bool, std::vector< double > > val;\n'
             '    {\n'
             '      bool first = true;\n'
             '      std::vector< double > second;\n'
             '      second.resize(3);\n'
             '      {\n'
             '        {\n'
             '          double elem = 1.1;\n'
             '          second[0] = elem;\n'
             '        }\n'
             '        {\n'
             '          double elem = 2.2;\n'
             '          second[1] = elem;\n'
             '        }\n'
             '        {\n'
             '          double elem = 3.3;\n'
             '          second[2] = elem;\n'
             '        }\n'
             '      }\n'
             '      val.first = first;\n'
             '      val.second = second;\n'
             '    }\n'
             '    foo[key] = val;\n'
             '  }\n'
             '  {\n'
             '    std::string key("goodbye");\n'
             '    std::pair< bool, std::vector< double > > val;\n'
             '    {\n'
             '      bool first = false;\n'
             '      std::vector< double > second;\n'
             '      second.resize(3);\n'
             '      {\n'
             '        {\n'
             '          double elem = 3.3;\n'
             '          second[0] = elem;\n'
             '        }\n'
             '        {\n'
             '          double elem = 2.2;\n'
             '          second[1] = elem;\n'
             '        }\n'
             '        {\n'
             '          double elem = 1.1;\n'
             '          second[2] = elem;\n'
             '        }\n'
             '      }\n'
             '      val.first = first;\n'
             '      val.second = second;\n'
             '    }\n'
             '    foo[key] = val;\n'
             '  }\n'
             '}\n'),
            ),
        ]

    for t, v, name, uitype, exp in cases:
        obs = f._val(t, val=v, name=name, uitype=uitype)
        yield assert_equal, exp, obs

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
                '    "    <element name=\\"x\\">\\n"\n'
                '    "        <data type=\\"positiveInteger\\"/>\\n"\n'
                '    "    </element>\\n"\n'
                '    "    FREAK OUT\\n"\n'
                '    "</interleave>\\n";\n')
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
    exp_impl = (
        '  return ""\n'
        '    "<interleave>\\n"\n'
        '    "    <element name=\\"x\\">\\n"\n'
        '    "        <oneOrMore>\\n"\n'
        '    "            <element name=\\"item\\">\\n"\n'
        '    "                <interleave>\\n"\n'
        '    "                    <element name=\\"key\\">\\n"\n'
        '    "                        <data type=\\"int\\"/>\\n"\n'
        '    "                    </element>\\n"\n'
        '    "                    <element name=\\"val\\">\\n"\n'
        '    "                        <data type=\\"double\\"/>\\n"\n'
        '    "                    </element>\\n"\n'
        '    "                </interleave>\\n"\n'
        '    "            </element>\\n"\n'
        '    "        </oneOrMore>\\n"\n'
        '    "    </element>\\n"\n'
        '    "</interleave>\\n";\n')
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

    f = SnapshotInvFilter(m)
    f.given_classname = 'MyFactory'
    f.mode = 'impl'
    impl = f.impl()
    exp_impl = ("  cyclus::Inventories invs;\n")
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

def test_schemafilter_buildschema():
    m = MockCodeGenMachine()
    m.context = {"MyFactory": OrderedDict([('vars', OrderedDict([
            ('x', {'type': 'int', 'uitype': 'nuclide'}),
            ]))
            ])}
    f = SchemaFilter(m)

    schematype = None
    uitype = None
    names = None

    cpptype = ['std::map', 'std::string', ['std::vector', 'double']]
    names = ['streams']
    exp = ('<element name="streams"><oneOrMore><element name="item">'
           '<interleave><element name="key"><data type="string" /></element>'
           '<element name="val"><oneOrMore><element name="val">'
           '<data type="double" /></element></oneOrMore></element>'
           '</interleave></element></oneOrMore></element>')
    obs = f._buildschema(cpptype, schematype, uitype, names)
    yield assert_equal, exp, obs

    cpptype = ['std::map', 'std::string', ['std::vector', 'double']]
    names = ['streams', 'name', ['efficiencies', 'val']]
    exp = ('<element name="streams"><oneOrMore><element name="item">'
           '<interleave><element name="name"><data type="string" /></element>'
           '<element name="efficiencies"><oneOrMore><element name="val">'
           '<data type="double" /></element></oneOrMore></element>'
           '</interleave></element></oneOrMore></element>')
    obs = f._buildschema(cpptype, schematype, uitype, names)
    yield assert_equal, exp, obs

    # test item aliasing
    cpptype = ['std::map', 'std::string', ['std::vector', 'double']]
    names = [['streams', 'entry'], 'name', ['efficiencies', 'val']]
    exp = ('<element name="streams"><oneOrMore><element name="entry">'
           '<interleave><element name="name"><data type="string" /></element>'
           '<element name="efficiencies"><oneOrMore><element name="val">'
           '<data type="double" /></element></oneOrMore></element>'
           '</interleave></element></oneOrMore></element>')
    obs = f._buildschema(cpptype, schematype, uitype, names)
    yield assert_equal, exp, obs

def test_escape_xml():
    """Test escape_xml"""
    xml = '<element name="mymap">' \
          '<element name="key"><text1/></element><element name="val"><text2/></element>' \
          '</element>'
    got = cycpp.escape_xml(xml)

    s = '    "<element name=\\"mymap\\">\\n"\n' \
        '    "    <element name=\\"key\\">\\n"\n' \
        '    "        <text1/>\\n"\n' \
        '    "    </element>\\n"\n' \
        '    "    <element name=\\"val\\">\\n"\n' \
        '    "        <text2/>\\n"\n' \
        '    "    </element>\\n"\n' \
        '    "</element>\\n"'

    yield assert_equal, s, got

def test_infiletodb_read_member1():
    m = MockCodeGenMachine()
    m.context = {"MyFactory": OrderedDict([('vars', OrderedDict([
            ('x', {'type': 'int', 'uitype': 'nuclide'}),
            ]))
            ])}
    f = InfileToDbFilter(m)

    cpptype = ('std::map', 'std::string', ('std::vector',
                ('std::vector', ('std::pair', 'double',
                  ('std::pair', 'int', ('std::list', ('std::set', 'bool')))))))
    alias = ['streams', 'name', ['efficiencies', 'val']]
    gen = f.read_member('mymap', alias, cpptype, uitype=None)
    exp_gen = (
        '  std::map< std::string, std::vector< std::vector< std::pair< double, std::pair< int, std::list< std::set< bool > > > > > > > mymap;\n'
        '  {\n'
        '    cyclus::InfileTree* bub = sub->SubTree("streams", 0);\n'
        '    cyclus::InfileTree* sub = bub;\n'
        '    int n1 = sub->NMatches("item");\n'
        '    std::map< std::string, std::vector< std::vector< std::pair< double, std::pair< int, std::list< std::set< bool > > > > > > > mymap_in;\n'
        '    for (int i1 = 0; i1 < n1; ++i1) {\n'
        '      std::string key;\n'
        '      {\n'
        '        std::string key_in = cyclus::Query<std::string>(sub, "item/name", i1);\n'
        '        key = key_in;\n'
        '      }\n'
        '      std::vector< std::vector< std::pair< double, std::pair< int, std::list< std::set< bool > > > > > > val;\n'
        '      {\n'
        '        cyclus::InfileTree* bub = sub->SubTree("item/efficiencies", i1);\n'
        '        cyclus::InfileTree* sub = bub;\n'
        '        int n2 = sub->NMatches("val");\n'
        '        std::vector< std::vector< std::pair< double, std::pair< int, std::list< std::set< bool > > > > > > val_in;\n'
        '        val_in.resize(n2);\n'
        '        for (int i2 = 0; i2 < n2; ++i2) {\n'
        '          std::vector< std::pair< double, std::pair< int, std::list< std::set< bool > > > > > elem;\n'
        '          {\n'
        '            cyclus::InfileTree* bub = sub->SubTree("val", i2);\n'
        '            cyclus::InfileTree* sub = bub;\n'
        '            int n3 = sub->NMatches("val");\n'
        '            std::vector< std::pair< double, std::pair< int, std::list< std::set< bool > > > > > elem_in;\n'
        '            elem_in.resize(n3);\n'
        '            for (int i3 = 0; i3 < n3; ++i3) {\n'
        '              std::pair< double, std::pair< int, std::list< std::set< bool > > > > elem;\n'
        '              {\n'
        '                cyclus::InfileTree* bub = sub->SubTree("val", i3);\n'
        '                cyclus::InfileTree* sub = bub;\n'
        '                  double firsti3;\n'
        '                  {\n'
        '                    double firsti3_in = cyclus::Query<double>(sub, "first", 0);\n'
        '                    firsti3 = firsti3_in;\n'
        '                  }\n'
        '                  std::pair< int, std::list< std::set< bool > > > secondi3;\n'
        '                  {\n'
        '                    cyclus::InfileTree* bub = sub->SubTree("second", 0);\n'
        '                    cyclus::InfileTree* sub = bub;\n'
        '                      int first0;\n'
        '                      {\n'
        '                        int first0_in = cyclus::Query<int>(sub, "first", 0);\n'
        '                        first0 = first0_in;\n'
        '                      }\n'
        '                      std::list< std::set< bool > > second0;\n'
        '                      {\n'
        '                        cyclus::InfileTree* bub = sub->SubTree("second", 0);\n'
        '                        cyclus::InfileTree* sub = bub;\n'
        '                        int n4 = sub->NMatches("val");\n'
        '                        std::list< std::set< bool > > second0_in;\n'
        '                        for (int i4 = 0; i4 < n4; ++i4) {\n'
        '                          std::set< bool > elem;\n'
        '                          {\n'
        '                            cyclus::InfileTree* bub = sub->SubTree("val", i4);\n'
        '                            cyclus::InfileTree* sub = bub;\n'
        '                            int n5 = sub->NMatches("val");\n'
        '                            std::set< bool > elem_in;\n'
        '                            for (int i5 = 0; i5 < n5; ++i5) {\n'
        '                              bool elem;\n'
        '                              {\n'
        '                                bool elem_in = cyclus::Query<bool>(sub, "val", i5);\n'
        '                                elem = elem_in;\n'
        '                              }\n'
        '                              elem_in.insert(elem);\n'
        '                            }\n'
        '                            elem = elem_in;\n'
        '                          }\n'
        '                          second0_in.push_back(elem);\n'
        '                        }\n'
        '                        second0 = second0_in;\n'
        '                      }\n'
        '                    std::pair< int, std::list< std::set< bool > > > secondi3_in(first0, second0);\n'
        '                    secondi3 = secondi3_in;\n'
        '                  }\n'
        '                std::pair< double, std::pair< int, std::list< std::set< bool > > > > elem_in(firsti3, secondi3);\n'
        '                elem = elem_in;\n'
        '              }\n'
        '              elem_in[i3] = elem;\n'
        '            }\n'
        '            elem = elem_in;\n'
        '          }\n'
        '          val_in[i2] = elem;\n'
        '        }\n'
        '        val = val_in;\n'
        '      }\n'
        '      mymap_in[key] = val;\n'
        '    }\n'
        '    mymap = mymap_in;\n'
        '  }\n')

    ## useful for debugging test failures
    #print(gen)
    #print(exp_gen)

    yield assert_equal, exp_gen, gen

def test_infiletodb_read_member2():
    m = MockCodeGenMachine()
    m.context = {"MyFactory": OrderedDict([('vars', OrderedDict([
            ('x', {'type': 'int', 'uitype': 'nuclide'}),
            ]))
            ])}
    f = InfileToDbFilter(m)

    alias = ['map', 'int', ['vector', ['pair', 'str1', 'str2']]]
    cpptype = ('std::map', 'int', ('std::vector', ('std::pair', 'std::string', 'std::string')))
    gen = f.read_member('mymap', alias, cpptype, uitype=None)
    exp_gen = (
        '  std::map< int, std::vector< std::pair< std::string, std::string > > > mymap;\n'
        '  {\n'
        '    cyclus::InfileTree* bub = sub->SubTree("map", 0);\n'
        '    cyclus::InfileTree* sub = bub;\n'
        '    int n1 = sub->NMatches("item");\n'
        '    std::map< int, std::vector< std::pair< std::string, std::string > > > mymap_in;\n'
        '    for (int i1 = 0; i1 < n1; ++i1) {\n'
        '      int key;\n'
        '      {\n'
        '        int key_in = cyclus::Query<int>(sub, "item/int", i1);\n'
        '        key = key_in;\n'
        '      }\n'
        '      std::vector< std::pair< std::string, std::string > > val;\n'
        '      {\n'
        '        cyclus::InfileTree* bub = sub->SubTree("item/vector", i1);\n'
        '        cyclus::InfileTree* sub = bub;\n'
        '        int n2 = sub->NMatches("pair");\n'
        '        std::vector< std::pair< std::string, std::string > > val_in;\n'
        '        val_in.resize(n2);\n'
        '        for (int i2 = 0; i2 < n2; ++i2) {\n'
        '          std::pair< std::string, std::string > elem;\n'
        '          {\n'
        '            cyclus::InfileTree* bub = sub->SubTree("pair", i2);\n'
        '            cyclus::InfileTree* sub = bub;\n'
        '              std::string firsti2;\n'
        '              {\n'
        '                std::string firsti2_in = cyclus::Query<std::string>(sub, "str1", 0);\n'
        '                firsti2 = firsti2_in;\n'
        '              }\n'
        '              std::string secondi2;\n'
        '              {\n'
        '                std::string secondi2_in = cyclus::Query<std::string>(sub, "str2", 0);\n'
        '                secondi2 = secondi2_in;\n'
        '              }\n'
        '            std::pair< std::string, std::string > elem_in(firsti2, secondi2);\n'
        '            elem = elem_in;\n'
        '          }\n'
        '          val_in[i2] = elem;\n'
        '        }\n'
        '        val = val_in;\n'
        '      }\n'
        '      mymap_in[key] = val;\n'
        '    }\n'
        '    mymap = mymap_in;\n'
        '  }\n'
    )
    # # useful for debugging test failures
    # print()
    # print(gen)
    # print(exp_gen)
    yield assert_equal, exp_gen, gen

def test_infiletodb_read_map():
    m = MockCodeGenMachine()
    m.context = {"MyFactory": OrderedDict([('vars', OrderedDict([
            ('x', {'type': 'int', 'uitype': 'nuclide'}),
            ]))
            ])}
    f = InfileToDbFilter(m)

    cpptype = ('std::map', 'int', 'double')
    alias = [['streams', 'entry'], 'id', 'mass']
    obs = f.read_member('mymap', alias, cpptype, uitype=None)

    exp = (
        '  std::map< int, double > mymap;\n'
        '  {\n'
        '    cyclus::InfileTree* bub = sub->SubTree("streams", 0);\n'
        '    cyclus::InfileTree* sub = bub;\n'
        '    int n1 = sub->NMatches("entry");\n'
        '    std::map< int, double > mymap_in;\n'
        '    for (int i1 = 0; i1 < n1; ++i1) {\n'
        '      int key;\n'
        '      {\n'
        '        int key_in = cyclus::Query<int>(sub, "entry/id", i1);\n'
        '        key = key_in;\n'
        '      }\n'
        '      double val;\n'
        '      {\n'
        '        double val_in = cyclus::Query<double>(sub, "entry/mass", i1);\n'
        '        val = val_in;\n'
        '      }\n'
        '      mymap_in[key] = val;\n'
        '    }\n'
        '    mymap = mymap_in;\n'
        '  }\n')

    yield assert_equal, exp, obs

def test_internal_schema():
    cases = [
        {'params': {'internal': True, 'default': 7}, 'want':
            '  return ""\n    "<interleave/>\\n";\n'
            },
        {'params': {'derived_init': 'x *= 42;'}, 'want':
            ('  return ""\n'
             '    "<interleave>\\n"\n'
             '    "    <element name=\\"x\\">\\n"\n'
             '    "        <data type=\\"int\\"/>\\n"\n'
             '    "    </element>\\n"\n'
             '    "</interleave>\\n";\n')
            },
        {'params': {'derived_init': 'x *= 42;', 'default': 7}, 'want':
            ('  return ""\n'
             '    "<interleave>\\n"\n'
             '    "    <optional>\\n"\n'
             '    "        <element name=\\"x\\">\\n"\n'
             '    "            <data type=\\"int\\"/>\\n"\n'
             '    "        </element>\\n"\n'
             '    "    </optional>\\n"\n'
             '    "</interleave>\\n";\n')
            },
        {'params': {'internal': True, 'derived_init': 'x *= 42;', 'default': 7}, 'want':
            '  return ""\n    "<interleave/>\\n";\n'
            },
    ]

    for i, case in enumerate(cases):
        keys = case['params'].copy()
        m = MockCodeGenMachine()
        params = {'type': 'int'}
        params.update(keys)
        m.context = {"MyFactory": OrderedDict([('vars', OrderedDict([
                ('x', params),
            ]))])}
        f = SchemaFilter(m)
        f.given_classname = 'MyFactory'

        impl = f.impl()

        want = case['want']
        msg = 'case {0} failed\n    ---- got ----\n    {1}\n    ---- want ----\n    {2}'.format(i + 1, impl.replace('\n', '\n    '), want.replace('\n', '\n    '))
        if want != impl:
            pprint.pprint(impl)
            assert_true(False, msg)

def test_internal_infiletodb():
    # the expected output (i.e. 'want':...) is set as 'throw' if the
    # configuration should cause an exception.
    cases = [
        {'params': {'internal': True}, 'want': 'throw'},
        {'params': {'internal': True, 'default': 7}, 'want':
            ('  cyclus::InfileTree* sub = tree->SubTree("config/*");\n'
             '  int i;\n'
             '  int n;\n'
             '  int x_tmp = 7;\n'
             '  x = x_tmp;\n'
             '  di.NewDatum("Info")\n'
             '  ->AddVal("x", x)\n'
             '  ->Record();\n')
            },
        {'params': {'derived_init': 'x *= 42;'}, 'want':
            ('  cyclus::InfileTree* sub = tree->SubTree("config/*");\n'
             '  int i;\n'
             '  int n;\n'
             '  {\n'
             '    int x_val = cyclus::Query<int>(sub, "x");\n'
             '    x = x_val;\n'
             '  }\n'
             '  x *= 42;\n'
             '  di.NewDatum("Info")\n'
             '  ->AddVal("x", x)\n'
             '  ->Record();\n')
            },
        {'params': {'derived_init': 'x *= 42;', 'default': 7}, 'want':
            ('  cyclus::InfileTree* sub = tree->SubTree("config/*");\n'
             '  int i;\n'
             '  int n;\n'
             '  if (sub->NMatches("x") > 0) {\n'
             '    {\n'
             '      int x_val = cyclus::Query<int>(sub, "x");\n'
             '      x = x_val;\n'
             '    }\n'
             '  } else {\n'
             '    int x_tmp = 7;\n'
             '    x = x_tmp;\n'
             '  }\n'
             '  x *= 42;\n'
             '  di.NewDatum("Info")\n'
             '  ->AddVal("x", x)\n'
             '  ->Record();\n')
            },
        {'params': {'internal': True, 'derived_init': 'x *= 42;', 'default': 7}, 'want':
            ('  cyclus::InfileTree* sub = tree->SubTree("config/*");\n'
             '  int i;\n'
             '  int n;\n'
             '  int x_tmp = 7;\n'
             '  x = x_tmp;\n'
             '  x *= 42;\n'
             '  di.NewDatum("Info")\n'
             '  ->AddVal("x", x)\n'
             '  ->Record();\n')
            },
    ]

    for i, case in enumerate(cases):
        keys = case['params'].copy()
        m = MockCodeGenMachine()
        params = {'type': 'int'}
        params.update(keys)
        m.context = {"MyFactory": OrderedDict([('vars', OrderedDict([
                ('x', params),
            ]))])}
        f = InfileToDbFilter(m)
        f.given_classname = 'MyFactory'

        want = case['want']

        impl = ''
        if want == 'throw':
            haderr = False
            try:
                impl = f.impl()
            except:
                haderr = True
            msg = 'case {0} failed: expected raised exception, got none.'
            assert_true(haderr, msg)
            continue
        else:
            impl = f.impl()

        msg = 'case {0} failed\n    ---- got ----\n    {1}\n    ---- want ----\n    {2}'.format(i + 1, impl.replace('\n', '\n    '), want.replace('\n', '\n    '))
        if want != impl:
            pprint.pprint(impl)
            assert_true(False, msg)

def test_nuclide_uitype():
    m = MockCodeGenMachine()
    m.context = {"MyFactory": OrderedDict([('vars', OrderedDict([
            ('x', {'type': 'int', 'uitype': 'nuclide'}),
            ]))
            ])}

    # test schema is set to string
    f = SchemaFilter(m)
    f.given_classname = 'MyFactory'
    impl = f.impl()
    exp_impl = ('  return ""\n'
                '    "<interleave>\\n"\n'
                '    "    <element name=\\"x\\">\\n"\n'
                '    "        <data type=\\"string\\"/>\\n"\n'
                '    "    </element>\\n"\n'
                '    "</interleave>\\n";\n')
    yield assert_equal, exp_impl, impl

    # test infiletodb updates
    f = InfileToDbFilter(m)
    f.given_classname = 'MyFactory'
    impl = f.impl()
    exp_impl = '  cyclus::InfileTree* sub = tree->SubTree("config/*");\n  int i;\n  int n;\n  {\n    int x_val = pyne::nucname::id(cyclus::Query<std::string>(sub, "x"));\n    x = x_val;\n  }\n  di.NewDatum("Info")\n  ->AddVal("x", x)\n  ->Record();\n'

    yield assert_equal, exp_impl, impl

    # test bad uitypes values fail
    m.context = {"MyFactory": OrderedDict([('vars', OrderedDict([
            ('x', {'type': 'int', 'uitype': 'WRONG'}),
            ]))
            ])}
    f = SchemaFilter(m)
    f.given_classname = 'MyFactory'
    yield assert_raises, TypeError, f.impl

def test_integration():
    inf = os.path.join(os.path.dirname(__file__), 'cycpp_tests.h')
    outf = tempfile.NamedTemporaryFile()
    cmd = 'cycpp.py {} -o {} --cpp-path `which g++`'.format(inf, outf.name)
    p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
    assert_equal('', p.stdout.read().decode())

if __name__ == "__main__":
    nose.runmodule()
