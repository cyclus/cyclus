import json
import subprocess
import os
from tools import thread_count


inputfile = {'simulation': {'archetypes': {'spec': [
                                        {'lib': 'dummy_power_recorder', 'name': 'DummyPowerRecorder'},
                                        {'lib': 'agents', 'name': 'NullRegion'},
                                        {'lib': 'agents', 'name': 'NullInst'}
                                        ]},
                'control': {'duration': '2',
                            'startmonth': '1',
                            'startyear': '2000'},
                'facility': [{'config': {'DummyPowerRecorder': {}},
                              'name': 'Dummy'}],
                'region': {'config': {'NullRegion': None},
                           'institution': {'config': {'NullInst': None},
                                           'initialfacilitylist': {'entry': [{'number': '1',
                                                                              'prototype': 'Dummy'}]},
                                           'name': 'SingleInstitution'},
                           'name': 'SingleRegion'}}}

def test_record_time_series(thread_count):
    if os.path.exists('dummy.h5'):
        os.remove('dummy.h5')
    with open('dummy.json', 'w') as f:
        json.dump(inputfile, f)
    env = dict(os.environ)
    env['PYTHONPATH'] = "."
    s = subprocess.check_output(['cyclus', '-j', thread_count, '-o', 'dummy.h5', 'dummy.json'], universal_newlines=True, env=env)
    assert ("The power is 10" in  s)
    if os.path.exists('dummy.json'):
        os.remove('dummy.json')
    if os.path.exists('dummy.h5'):
        os.remove('dummy.h5')
