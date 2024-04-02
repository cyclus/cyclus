import os
import re
import json
import subprocess



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
    assert ("Bread is rye" in  s)
    # tests that value in input file overrides default value
    assert ("Toast level is 10" in  s)
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
    assert(isinstance(info['id'], int))
    assert(isinstance(info['parent'], int))
    assert(info['parent'] != info['id'])
    assert(0 <= info['parent'] < 100)
    assert(info['id'] == info['hash'])
    # test attrs
    assert(info['str'].startswith('Facility_HappyToaster'))
    assert (info['kind'] ==  'Facility')
    assert (info['spec'] ==  ':toaster:AttrToaster')
    assert (info['version'] ==  '0.0.0')
    assert (info['prototype'] ==  'HappyToaster')
    assert (info['enter_time'] ==  0)
    assert (info['lifetime'] ==  -1)
    assert (info['exit_time'] ==  -1)
    assert (len(info['childern']) ==  0)
    assert(len(info['annotations']) > 0)
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


def test_pyagent_attr_toaster_company(thread_count):
    oname = 'attr-toaster-company.h5'
    iname = 'attr-toaster-company.json'
    if os.path.exists(oname):
        os.remove(oname)
    with open(iname, 'w') as f:
        json.dump(COMPANYFILE, f)
    env = dict(os.environ)
    env['PYTHONPATH'] = "."
    s = subprocess.check_output(['cyclus', '-j', thread_count, '-o', oname, iname],
                                universal_newlines=True, env=env)
    info = s.split('=== Start AttrToasterCompany ===\n')[-1]
    info = info.split('\n=== End AttrToasterCompany ===')[0]
    info = json.loads(info)
    # test ids
    assert(isinstance(info['id'], int))
    assert(isinstance(info['parent'], int))
    assert(info['parent'] != info['id'])
    assert(0 <= info['parent'] <= 100)
    assert(info['id'] == info['hash'])
    # test attrs
    assert(info['str'].startswith('Inst_FamousToastersLLC'))
    assert (info['kind'] ==  'Inst')
    assert (info['spec'] ==  ':toaster:AttrToasterCompany')
    assert (info['version'] ==  '0.0.0')
    assert (info['prototype'] ==  'FamousToastersLLC')
    assert (info['enter_time'] ==  0)
    assert (info['lifetime'] ==  -1)
    assert (info['exit_time'] ==  -1)
    assert (len(info['childern']) ==  1)
    assert(len(info['annotations']) > 0)
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


def test_pyagent_attr_toaster_region(thread_count):
    oname = 'attr-toaster-region.h5'
    iname = 'attr-toaster-region.json'
    if os.path.exists(oname):
        os.remove(oname)
    with open(iname, 'w') as f:
        json.dump(REGIONFILE, f)
    env = dict(os.environ)
    env['PYTHONPATH'] = "."
    s = subprocess.check_output(['cyclus', '-j', thread_count, '-o', oname, iname],
                                universal_newlines=True, env=env)
    info = s.split('=== Start AttrToasterRegion ===\n')[-1]
    info = info.split('\n=== End AttrToasterRegion ===')[0]
    info = json.loads(info)
    # test ids
    assert(isinstance(info['id'], int))
    assert(isinstance(info['parent'], int))
    assert(info['parent'] != info['id'])
    assert (info['parent'] ==  -1)
    assert(info['id'] == info['hash'])
    # test attrs
    assert(info['str'].startswith('Region_RepublicOfToast'))
    assert (info['kind'] ==  'Region')
    assert (info['spec'] ==  ':toaster:AttrToasterRegion')
    assert (info['version'] ==  '0.0.0')
    assert (info['prototype'] ==  'RepublicOfToast')
    assert (info['enter_time'] ==  0)
    assert (info['lifetime'] ==  -1)
    assert (info['exit_time'] ==  -1)
    assert (len(info['childern']) ==  1)
    assert(len(info['annotations']) > 0)
    # clean up
    if os.path.exists(iname):
        os.remove(iname)
    if os.path.exists(oname):
        os.remove(oname)
