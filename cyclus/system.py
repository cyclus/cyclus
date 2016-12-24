"""Some system-specific info for cyclus."""
import sys
import importlib

from cyclus.lazyasd import lazyobject


@lazyobject
def PY_VERSION_TUPLE():
    return sys.version_info[:3]


@lazyobject
def curio():
    if PY_VERSION_TUPLE < (3, 5, 2):
        return None
    else:
        try:
            return importlib.import_module('curio')
        except ImportError:
            return None


@lazyobject
def QUEUE():
    """A global queue whose tasks should be spawned."""
    if hasattr(curio, 'Queue'):
        return curio.Queue()
    else:
        return None
