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
def asyncio():
    if PY_VERSION_TUPLE < (3, 5, 0):
        return None
    else:
        try:
            return importlib.import_module('asyncio')
        except ImportError:
            return None


@lazyobject
def websockets():
    if PY_VERSION_TUPLE < (3, 5, 0):
        return None
    else:
        try:
            return importlib.import_module('websockets')
        except ImportError:
            return None


@lazyobject
def concurrent_futures():
    if PY_VERSION_TUPLE < (3, 2, 0):
        return None
    else:
        try:
            import concurrent.futures as m
            return m
        except ImportError:
            return None

