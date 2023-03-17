from __future__ import print_function, unicode_literals


from cyclus import lib

def test_agent_spec_empty():
    spec = lib.AgentSpec()
    assert spec.path == ""
    assert spec.lib == ""
    assert spec.agent == ""
    assert spec.alias == ""


def test_agent_spec_spec():
    spec = lib.AgentSpec(":wakka:Jawaka")
    assert spec.path == ""
    assert spec.lib == "wakka"
    assert spec.agent == "Jawaka"
    assert spec.alias == "Jawaka"


def test_agent_spec_full():
    spec = lib.AgentSpec("why", "not", "me", "?")
    assert spec.path == "why"
    assert spec.lib == "not"
    assert spec.agent == "me"
    assert spec.alias == "?"


def test_dm_exists():
    spec = lib.AgentSpec(":agents:NullRegion")
    print(spec)
    dm = lib.DynamicModule()
    obs = dm.exists(spec)
    assert(obs)
