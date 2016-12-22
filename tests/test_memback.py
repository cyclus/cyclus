"""Tests Python memory backend."""
from __future__ import print_function, unicode_literals

import nose
from nose.tools import assert_equal, assert_true

from cyclus import memback
from cyclus import lib
from cyclus import typesystem as ts

import numpy as np
import pandas as pd
from pandas.util.testing import assert_frame_equal


def test_simple():
    rec = lib.Recorder(inject_sim_id=False)
    back = memback.MemBack()
    rec.register_backend(back)
    d = rec.new_datum("test")
    d.add_val("col0", 1, dbtype=ts.INT)
    d.add_val("col1", 42.0, dbtype=ts.DOUBLE)
    d.add_val("col2", "wakka", dbtype=ts.VL_STRING)
    d.record()
    rec.flush()

    exp = pd.DataFrame({"col0": [1], "col1": [42.0], "col2": ["wakka"]},
                       columns=['col0', 'col1', 'col2'])
    obs = back.query("test")
    assert_frame_equal(exp, obs)
    rec.close()


if __name__ == "__main__":
    nose.runmodule()