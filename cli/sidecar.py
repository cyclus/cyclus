import sys, importlib.util as iu
spec = iu.spec_from_file_location("cyclus.system", "../cyclus/system.py")
mod = iu.module_from_spec(spec)
sys.modules["cyclus.system"] = mod
spec.loader.exec_module(mod)

CY_LARGE_DOUBLE =  mod.CY_LARGE_DOUBLE
CY_LARGE_INT =  mod.CY_LARGE_INT
CY_NEAR_ZERO =  mod.CY_NEAR_ZERO