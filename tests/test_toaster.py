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
    assert_true(0 <= info['parent'] < 100)
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
    assert_true(len(info['annotations']) > 0)
    # clean up
    if os.path.exists(iname):
        os.remove(iname)
    if os.path.exists(oname):
        os.remove(oname)


COMPANYFILE = {'simulation': {'archetypes': {'spec': [
                                        {'lib': 'toaster', 'name': 'DefaultToaster'},
                                        {'lib': 'agents', 'name': 'NullRegion'},
                                        {'lib': 'toaster', 'name': 'AttrToasterCompany'}
                                        ]},
                'control': {'duration': '2',
                            'startmonth': '1',
                            'startyear': '2000'},
                'facility': [{'config': {'DefaultToaster': {}},
                              'name': 'HappyToaster'}],
                'region': {'config': {'NullRegion': None},
                           'institution': {'config': {'AttrToasterCompany': {}},
                                           'initialfacilitylist': {'entry': [{'number': '1',
                                                                              'prototype': 'HappyToaster'}]},
                                           'name': 'FamousToastersLLC'},
                           'name': 'SingleRegion'}}}


def test_pyagent_attr_toaster_company():
    oname = 'attr-toaster-company.h5'
    iname = 'attr-toaster-company.json'
    if os.path.exists(oname):
        os.remove(oname)
    with open(iname, 'w') as f:
        json.dump(COMPANYFILE, f)
    env = dict(os.environ)
    env['PYTHONPATH'] = "."
    s = subprocess.check_output(['cyclus', '-o', oname, iname],
                                universal_newlines=True, env=env)
    info = s.split('=== Start AttrToasterCompany ===\n')[-1]
    info = info.split('\n=== End AttrToasterCompany ===')[0]
    info = json.loads(info)
    # test ids
    assert_true(isinstance(info['id'], int))
    assert_true(isinstance(info['parent'], int))
    assert_true(info['parent'] != info['id'])
    assert_true(0 <= info['parent'] <= 100)
    assert_true(info['id'] == info['hash'])
    # test attrs
    assert_true(info['str'].startswith('Inst_FamousToastersLLC'))
    assert_equals(info['kind'], 'Inst')
    assert_equals(info['spec'], ':toaster:AttrToasterCompany')
    assert_equals(info['version'], '0.0.0')
    assert_equals(info['prototype'], 'FamousToastersLLC')
    assert_equals(info['enter_time'], 0)
    assert_equals(info['lifetime'], -1)
    assert_equals(info['exit_time'], -1)
    assert_equals(len(info['childern']), 1)
    assert_true(len(info['annotations']) > 0)
    # clean up
    if os.path.exists(iname):
        os.remove(iname)
    if os.path.exists(oname):
        os.remove(oname)


REGIONFILE = {'simulation': {'archetypes': {'spec': [
                                        {'lib': 'toaster', 'name': 'DefaultToaster'},
                                        {'lib': 'toaster', 'name': 'AttrToasterRegion'},
                                        {'lib': 'agents', 'name': 'NullInst'}
                                        ]},
                'control': {'duration': '2',
                            'startmonth': '1',
                            'startyear': '2000'},
                'facility': [{'config': {'DefaultToaster': {}},
                              'name': 'HappyToaster'}],
                'region': {'config': {'AttrToasterRegion': {}},
                           'institution': {'config': {'NullInst': None},
                                           'initialfacilitylist': {'entry': [{'number': '1',
                                                                              'prototype': 'HappyToaster'}]},
                                           'name': 'SingleInstitution'},
                           'name': 'RepublicOfToast'}}}


def test_pyagent_attr_toaster_region():
    oname = 'attr-toaster-region.h5'
    iname = 'attr-toaster-region.json'
    if os.path.exists(oname):
        os.remove(oname)
    with open(iname, 'w') as f:
        json.dump(REGIONFILE, f)
    env = dict(os.environ)
    env['PYTHONPATH'] = "."
    s = subprocess.check_output(['cyclus', '-o', oname, iname],
                                universal_newlines=True, env=env)
    info = s.split('=== Start AttrToasterRegion ===\n')[-1]
    info = info.split('\n=== End AttrToasterRegion ===')[0]
    info = json.loads(info)
    # test ids
    assert_true(isinstance(info['id'], int))
    assert_true(isinstance(info['parent'], int))
    assert_true(info['parent'] != info['id'])
    assert_equals(info['parent'], -1)
    assert_true(info['id'] == info['hash'])
    # test attrs
    assert_true(info['str'].startswith('Region_RepublicOfToast'))
    assert_equals(info['kind'], 'Region')
    assert_equals(info['spec'], ':toaster:AttrToasterRegion')
    assert_equals(info['version'], '0.0.0')
    assert_equals(info['prototype'], 'RepublicOfToast')
    assert_equals(info['enter_time'], 0)
    assert_equals(info['lifetime'], -1)
    assert_equals(info['exit_time'], -1)
    assert_equals(len(info['childern']), 1)
    assert_true(len(info['annotations']) > 0)
    # clean up
    if os.path.exists(iname):
        os.remove(iname)
    if os.path.exists(oname):
        os.remove(oname)
