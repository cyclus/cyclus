"""Some system-specific info for cyclus."""
import sys

PY_VERSION_TUPLE = sys.version_info[:3]

if PY_VERSION_TUPLE < (3, 5, 2):
    curio = None
else:
    try:
        import curio
    except ImportError:
        curio = None

if PY_VERSION_TUPLE < (3, 5, 0):
    asyncio = None
    websockets = None
else:
    try:
        import asyncio
    except ImportError:
        asyncio = None

    try:
        import websockets
    except ImportError:
        websockets = None

if PY_VERSION_TUPLE < (3, 2, 0):
    concurrent_futures = None
else:
    try:
        import concurrent_futures
    except ImportError:
        concurrent_futures = None


