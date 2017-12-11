import os
import re
import json
import subprocess

from nose.tools import assert_in, assert_true, assert_equals


DEFAULTFILE = {'simulation': {'archetypes': {'spec': [
                                        {'lib': 'toaster', 'name': 'DefaultToaster'},
                                        {'lib': 'agents', 'name': 'NullRegion'},
                                        {'lib': 'agents', 'name': 'NullInst'}
                                        ]},
                'control': {'duration': '2',
                            'startmonth': '1',
                            'startyear': '2000'},
                'facility': [{'config': {'DefaultToaster': {'level': 10.0}},
                              'name': 'Dummy'}],
                'region': {'config': {'NullRegion': None},
                           'institution': {'config': {'NullInst': None},
                                           'initialfacilitylist': {'entry': [{'number': '1',
                                                                              'prototype': 'Dummy'}]},
                                           'name': 'SingleInstitution'},
                           'name': 'SingleRegion'}}}


def test_pyagent_defaults():
    if os.path.exists('default-toaster.h5'):
        os.remove('default-toaster.h5')
    with open('default-toaster.json', 'w') as f:
        json.dump(DEFAULTFILE, f)
    env = dict(os.environ)
    env['PYTHONPATH'] = "."
    s = subprocess.check_output(['cyclus', '-o', 'default-toaster.h5', 'default-toaster.json'],
                                universal_newlines=True, env=env)
    # tests default value set on facility
    assert_in("Bread is rye", s)
    # tests that value in input file overrides default value
    assert_in("Toast level is 10", s)
    if os.path.exists('default-toaster.json'):
        os.remove('default-toaster.json')
    if os.path.exists('default-toaster.h5'):
        os.remove('default-toaster.h5')


ATTRFILE = {'simulation': {'archetypes': {'spec': [
                                        {'lib': 'toaster', 'name': 'AttrToaster'},
                                        {'lib': 'agents', 'name': 'NullRegion'},
                                        {'lib': 'agents', 'name': 'NullInst'}
                                        ]},
                'control': {'duration': '2',
                            'startmonth': '1',
                            'startyear': '2000'},
                'facility': [{'config': {'AttrToaster': {}},
                              'name': 'HappyToaster'}],
                'region': {'config': {'NullRegion': None},
                           'institution': {'config': {'NullInst': None},
                                           'initialfacilitylist': {'entry': [{'number': '1',
                                                                              'prototype': 'HappyToaster'}]},
                                           'name': 'SingleInstitution'},
                           'name': 'SingleRegion'}}}


def test_pyagent_attr_toasters():
    oname = 'attr-toaster.h5'
    iname = 'attr-toaster.json'
    if os.path.exists(oname):
        os.remove(oname)
    with open(iname, 'w') as f:
        json.dump(ATTRFILE, f)
    env = dict(os.environ)
    env['PYTHONPATH'] = "."
    s = subprocess.check_output(['cyclus', '-o', oname, iname],
                                universal_newlines=True, env=env)
    info = s.split('=== Start AttrToaster ===\n')[-1].split('\n=== End AttrToaster ===')[0]
    info = json.loads(info)
    # test ids
    assert_true(isinstance(info['id'], int))
    assert_true(isinstance(info['parent'], int))
    assert_true(info['parent'] != info['id'])
    assert_true(0 <= info['parent'] < 10)
    assert_true(info['id'] == info['hash'])
    # test attrs
    assert_true(info['str'].startswith('Facility_HappyToaster'))
    assert_equals(info['kind'], 'Facility')
    assert_equals(info['spec'], ':toaster:AttrToaster')
    assert_equals(info['version'], '0.0.0')
    assert_equals(info['prototype'], 'HappyToaster')
    assert_equals(info['enter_time'], 0)
    assert_equals(info['lifetime'], -1)
    assert_equals(info['exit_time'], -1)
    assert_equals(len(info['childern']), 0)
    # clean up
    if os.path.exists(iname):
        os.remove(iname)
    if os.path.exists(oname):
        os.remove(oname)
