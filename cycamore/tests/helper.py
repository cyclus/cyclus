"""A set of tools for use in integration tests."""
import os
import tempfile
import subprocess
import sys
from hashlib import sha1
import numpy as np
import tables
from nose.tools import assert_equal


CYCLUS_HAS_COIN = None


if sys.version_info[0] >= 3:
    str_types = (bytes, str)
else:
    str_types = (str, unicode)

def hasher(x):
    return int(sha1(x.encode()).hexdigest(), 16)

def idx(h):
    ind = [None] * 5
    for i in range(4, -1, -1):
        h, ind[i] = divmod(h, 2**32)
    return tuple(ind)

sha1array = lambda x: np.array(idx(hasher(x)), np.uint32)

def table_exist(db, tables):
    """Checks if hdf5 database contains the specified tables.
    """
    return all([t in db.root for t in tables])

def find_ids(data, data_table, id_table):
    """Finds ids of the specified data located in the specified data_table,
    and extracts the corresponding id from the specified id_table.
    """
    ids = []
    for i, d in enumerate(data_table):
        if isinstance(d, np.ndarray) and isinstance(data, np.ndarray):
            if (d == data).all():
                ids.append(id_table[i])
        elif isinstance(d, np.ndarray) and not isinstance(data, np.ndarray):
            if (d == sha1array(data)).all():
                ids.append(id_table[i])
        elif d == data:
            ids.append(id_table[i])
    return ids

def exit_times(agent_id, exit_table):
    """Finds exit times of the specified agent from the exit table.
    """
    i = 0
    exit_times = []
    for index in exit_table["AgentId"]:
        if index == agent_id:
            exit_times.append(exit_table["ExitTime"][i])
        i += 1

    return exit_times


def run_cyclus(cyclus, cwd, in_path, out_path):
    """Runs cyclus with various inputs and creates output databases
    """
    holdsrtn = [1]  # needed because nose does not send() to test generator
    # make sure the output target directory exists
    cmd = [cyclus, "-o", out_path, "--input-file", in_path]
    check_cmd(cmd, cwd, holdsrtn)


def check_cmd(args, cwd, holdsrtn):
    """Runs a command in a subprocess and verifies that it executed properly.
    """
    if not isinstance(args, str_types):
        args = " ".join(args)
    print("TESTING: running command in {0}:\n\n{1}\n".format(cwd, args))
    env = dict(os.environ)
    env['_'] = subprocess.check_output(['which', 'cyclus'], cwd=cwd).strip()
    with tempfile.NamedTemporaryFile() as f:
        rtn = subprocess.call(args, shell=True, cwd=cwd, stdout=f, stderr=f, env=env)
        if rtn != 0:
            f.seek(0)
            print("STDOUT + STDERR:\n\n" + f.read().decode())
    holdsrtn[0] = rtn
    assert_equal(rtn, 0)


def cyclus_has_coin():
    global CYCLUS_HAS_COIN
    if CYCLUS_HAS_COIN is not None:
        return CYCLUS_HAS_COIN
    s = subprocess.check_output(['cyclus', '--version'], universal_newlines=True)
    s = s.strip().replace('Dependencies:', '')
    m = {k.strip(): v.strip() for k,v in [line.split()[:2] for line in s.splitlines()
                                          if line != '']}
    CYCLUS_HAS_COIN = m['Coin-Cbc'] != '-1'
    return CYCLUS_HAS_COIN

