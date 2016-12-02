from __future__ import print_function, unicode_literals

from nose.tools import assert_equal, assert_true

from cyclus import lib

def test_agent_spec_empty():
    spec = lib.AgentSpec()
    yield assert_equal, spec.path, ""
    yield assert_equal, spec.lib, ""
    yield assert_equal, spec.agent, ""
    yield assert_equal, spec.alias, ""


def test_agent_spec_spec():
    spec = lib.AgentSpec(":wakka:Jawaka")
    yield assert_equal, spec.path, ""
    yield assert_equal, spec.lib, "wakka"
    yield assert_equal, spec.agent, "Jawaka"
    yield assert_equal, spec.alias, "Jawaka"


def test_agent_spec_full():
    spec = lib.AgentSpec("why", "not", "me", "?")
    yield assert_equal, spec.path, "why"
    yield assert_equal, spec.lib, "not"
    yield assert_equal, spec.agent, "me"
    yield assert_equal, spec.alias, "?"


def test_dm_exists():
    spec = lib.AgentSpec(":agents:NullRegion")
    print(spec)
    dm = lib.DynamicModule()
    obs = dm.exists(spec)
    assert_true(obs)
