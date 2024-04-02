"""A Python interface for Agents and their subclasses."""
from __future__ import print_function, unicode_literals
from libcpp.map cimport map as std_map
from libcpp.set cimport set as std_set
from libcpp.vector cimport vector as std_vector
from libcpp.utility cimport pair as std_pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool
from libcpp.cast cimport reinterpret_cast, dynamic_cast
from cython.operator cimport dereference as deref
cimport cython
cimport openmp

from cpython cimport (PyObject, PyDict_New, PyDict_Contains,
    PyDict_GetItemString, PyDict_SetItemString, PyString_FromString,
    PyBytes_FromString, PyDict_GetItem, PyDict_SetItem, PyObject_GetAttrString)

import json
from inspect import getmro, getdoc
from copy import deepcopy
from collections.abc import Mapping

from cyclus cimport cpp_cyclus
from cyclus.cpp_cyclus cimport shared_ptr, reinterpret_pointer_cast
from cyclus cimport lib
from cyclus import lib
from cyclus cimport cpp_typesystem
from cyclus.typesystem cimport (py_to_any, any_to_py, str_py_to_cpp,
    std_string_to_py, bool_to_py, bool_to_cpp, std_set_std_string_to_py,
    std_set_std_string_to_cpp, std_vector_std_string_to_py)
from cyclus cimport typesystem as ts
from cyclus import typesystem as ts

from cyclus cimport cpp_jsoncpp
from cyclus cimport jsoncpp
from cyclus import jsoncpp

from cyclus import nucname

from cyclus.cycpp import VarDeclarationFilter, SchemaFilter


_VAR_DECL = VarDeclarationFilter()
_SCHEMA = SchemaFilter()


cdef int _GET_MAT_PREFS_TIME = -9999999999
cdef cpp_cyclus.PrefMap[cpp_cyclus.Material].type* _GET_MAT_PREFS_PTR = NULL
cdef dict _GET_MAT_PREFS = {}

cdef int _GET_PROD_PREFS_TIME = -9999999999
cdef cpp_cyclus.PrefMap[cpp_cyclus.Product].type* _GET_PROD_PREFS_PTR = NULL
cdef dict _GET_PROD_PREFS = {}

cdef openmp.omp_lock_t lock
openmp.omp_init_lock(&lock)

#
# Shims
#
cdef cppclass CyclusAgentShim "CyclusAgentShim" (cpp_cyclus.Agent):
    # A C++ class that acts as an Agent. It implements the Agent virtual
    # methods and dispatches the work to a Python/Cython object
    # that is has a reference to, as needed.

    CyclusAgentShim(cpp_cyclus.Context* ctx):  # C++BASES cyclus::Agent(ctx)
        pass

    std_string version() except *:
        rtn = (<object> this.self).version
        return str_py_to_cpp(rtn)

    cpp_cyclus.Agent* Clone() except *:
        cdef lib._Context ctx = lib.Context(init=False)
        (<lib._Context> ctx).ptx = this.context()
        cdef _Agent a = type(<object> this.self)(ctx)
        a.shim.InitFromAgent(this)
        (<lib._Agent> a)._free = False
        lib._AGENT_REFS[a.id] = a
        return dynamic_cast[agent_ptr]((<_Agent> a).shim)

    void InitFromAgent "InitFrom" (CyclusAgentShim* a) except *:
        cpp_cyclus.Agent.InitFromAgent(a)
        (<object> this.self).init_from_agent(<object> a.self)

    void InfileToDb(cpp_cyclus.InfileTree* tree, cpp_cyclus.DbInit di) except *:
        cpp_cyclus.Agent.InfileToDb(tree, di)
        # wrap interface
        cdef lib._InfileTree py_tree = lib.InfileTree(free=False)
        py_tree.ptx = tree
        cdef lib._DbInit py_di = lib.DbInit(free=False)
        py_di.ptx = &di
        # call generic python
        (<object> this.self).infile_to_db(py_tree, py_di)

    void InitFrom(cpp_cyclus.QueryableBackend* b) except *:
        cpp_cyclus.Agent.InitFrom(b)
        cdef cpp_cyclus.QueryResult qr = b.Query(std_string(<char*> "Info"), NULL)
        res, _ = lib.single_query_result_to_py(qr, 0)
        # call generic python
        self = (<object> this.self)
        self.init_from_dict(res)
        lib._AGENT_REFS[self.id] = self

    void Snapshot(cpp_cyclus.DbInit di) except *:
        cdef lib._DbInit py_di = lib.DbInit(free=False)
        py_di.ptx = &di
        (<object> this.self).snapshot(py_di)

    void InitInv(cpp_cyclus.Inventories& invs) except *:
        pyinvs = lib.inventories_to_py(invs)
        (<object> this.self).init_inv(pyinvs)

    cpp_cyclus.Inventories SnapshotInv() except *:
        pyinvs = (<object> this.self).snapshot_inv()
        return lib.inventories_to_cpp(pyinvs)

    std_string schema() except *:
        pyschema = (<object> this.self).schema
        return str_py_to_cpp(pyschema)

    cpp_jsoncpp.Value annotations() except *:
        pyanno = (<object> this.self).annotations_json
        return lib.str_to_json_value(pyanno)

    void Build(cpp_cyclus.Agent* parent) except *:
        cpp_cyclus.Agent.Build(parent)
        pyrent = lib.agent_to_py(parent, None)
        (<object> this.self).build(pyrent)

    void EnterNotify() except *:
        cpp_cyclus.Agent.EnterNotify()
        (<object> this.self).enter_notify()

    void BuildNotify() except *:
        (<object> this.self).build_notify()

    void DecomNotify() except *:
        (<object> this.self).decom_notify()

    void AdjustMatlPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Material].type& prefs) except *:
        # cache the commod_reqs wrappers globally
        global _GET_MAT_PREFS_TIME, _GET_MAT_PREFS_PTR, _GET_MAT_PREFS
        cdef int curr_time = this.context().time()
        cdef cpp_cyclus.PrefMap[cpp_cyclus.Material].type* curr_ptr = &prefs
        if curr_time == _GET_MAT_PREFS_TIME and curr_ptr == _GET_MAT_PREFS_PTR:
            pyprefs = _GET_MAT_PREFS
        else:
            pyprefs = ts.material_pref_map_to_py(prefs)
            _GET_MAT_PREFS_TIME = curr_time
            _GET_MAT_PREFS_PTR = curr_ptr
            _GET_MAT_PREFS = pyprefs
        # call the python function
        updates = (<object> this.self).adjust_material_prefs(pyprefs)
        if updates is None or len(updates) == 0:
            return
        # update the prefs objects
        pyprefs.update(updates)
        for (req, bid), pref in updates.items():
            prefs[(<ts._MaterialRequest> req).ptx][(<ts._MaterialBid> bid).ptx] = pref

    void AdjustProductPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Product].type& prefs) except *:
        # cache the commod_reqs wrappers globally
        global _GET_PROD_PREFS_TIME, _GET_PROD_PREFS_PTR, _GET_PROD_PREFS
        cdef int curr_time = this.context().time()
        cdef cpp_cyclus.PrefMap[cpp_cyclus.Product].type* curr_ptr = &prefs
        if curr_time == _GET_PROD_PREFS_TIME and curr_ptr == _GET_PROD_PREFS_PTR:
            pyprefs = _GET_PROD_PREFS
        else:
            pyprefs = ts.product_pref_map_to_py(prefs)
            _GET_PROD_PREFS_TIME = curr_time
            _GET_PROD_PREFS_PTR = curr_ptr
            _GET_PROD_PREFS = pyprefs
        # call the python function
        updates = (<object> this.self).adjust_product_prefs(pyprefs)
        if updates is None or len(updates) == 0:
            return
        # update the prefs objects
        pyprefs.update(updates)
        for (req, bid), pref in updates.items():
            prefs[(<ts._ProductRequest> req).ptx][(<ts._ProductBid> bid).ptx] = pref


cdef cppclass CyclusRegionShim "CyclusRegionShim" (cpp_cyclus.Region):
    # A C++ class that acts as a Region. It implements the Region virtual
    # methods and dispatches the work to a Python/Cython object
    # that is has a reference to, as needed.

    CyclusRegionShim(cpp_cyclus.Context* ctx):  # C++BASES cyclus::Region(ctx)
        pass

    std_string version() except *:
        rtn = (<object> this.self).version
        return str_py_to_cpp(rtn)

    cpp_cyclus.Agent* Clone() except *:
        cdef lib._Context ctx = lib.Context(init=False)
        (<lib._Context> ctx).ptx = this.context()
        cdef _Region a = type(<object> this.self)(ctx)
        (<CyclusRegionShim*> (<_Agent> a).shim).InitFromAgent(<CyclusRegionShim*> this)
        (<lib._Agent> a)._free = False
        lib._AGENT_REFS[a.id] = a
        return dynamic_cast[agent_ptr](
                reinterpret_cast[region_shim_ptr]((<_Agent> a).shim))

    void InitFromAgent "InitFrom" (CyclusRegionShim* a) except *:
        cpp_cyclus.Region.Region_InitFromAgent(a)
        (<object> this.self).init_from_agent(<object> a.self)

    void InfileToDb(cpp_cyclus.InfileTree* tree, cpp_cyclus.DbInit di) except *:
        cpp_cyclus.Region.Region_InfileToDb(tree, di)
        # wrap interface
        cdef lib._InfileTree py_tree = lib.InfileTree(free=False)
        py_tree.ptx = tree
        cdef lib._DbInit py_di = lib.DbInit(free=False)
        py_di.ptx = &di
        # call generic python
        (<object> this.self).infile_to_db(py_tree, py_di)

    void InitFrom(cpp_cyclus.QueryableBackend* b) except *:
        cpp_cyclus.Region.Region_InitFrom(b)
        cdef cpp_cyclus.QueryResult qr = b.Query(std_string(<char*> "Info"), NULL)
        res, _ = lib.single_query_result_to_py(qr, 0)
        # call generic python
        self = (<object> this.self)
        self.init_from_dict(res)
        lib._AGENT_REFS[self.id] = self

    void Snapshot(cpp_cyclus.DbInit di) except *:
        cdef lib._DbInit py_di = lib.DbInit(free=False)
        py_di.ptx = &di
        (<object> this.self).snapshot(py_di)

    void InitInv(cpp_cyclus.Inventories& invs) except *:
        pyinvs = lib.inventories_to_py(invs)
        (<object> this.self).init_inv(pyinvs)

    cpp_cyclus.Inventories SnapshotInv() except *:
        pyinvs = (<object> this.self).snapshot_inv()
        return lib.inventories_to_cpp(pyinvs)

    std_string schema() except *:
        pyschema = (<object> this.self).schema
        return str_py_to_cpp(pyschema)

    cpp_jsoncpp.Value annotations() except *:
        pyanno = (<object> this.self).annotations_json
        return lib.str_to_json_value(pyanno)

    void Build(cpp_cyclus.Agent* parent) except *:
        cpp_cyclus.Region.Region_Build(parent)
        pyrent = lib.agent_to_py(parent, None)
        (<object> this.self).build(pyrent)

    void EnterNotify() except *:
        cpp_cyclus.Region.Region_EnterNotify()
        (<object> this.self).enter_notify()

    void BuildNotify() except *:
        (<object> this.self).build_notify()

    void DecomNotify() except *:
        (<object> this.self).decom_notify()

    void AdjustMatlPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Material].type& prefs) except *:
        # cache the commod_reqs wrappers globally
        global _GET_MAT_PREFS_TIME, _GET_MAT_PREFS_PTR, _GET_MAT_PREFS
        cdef int curr_time = this.context().time()
        cdef cpp_cyclus.PrefMap[cpp_cyclus.Material].type* curr_ptr = &prefs
        if curr_time == _GET_MAT_PREFS_TIME and curr_ptr == _GET_MAT_PREFS_PTR:
            pyprefs = _GET_MAT_PREFS
        else:
            pyprefs = ts.material_pref_map_to_py(prefs)
            _GET_MAT_PREFS_TIME = curr_time
            _GET_MAT_PREFS_PTR = curr_ptr
            _GET_MAT_PREFS = pyprefs
        # call the python function
        updates = (<object> this.self).adjust_material_prefs(pyprefs)
        if updates is None or len(updates) == 0:
            return
        # update the prefs objects
        pyprefs.update(updates)
        for (req, bid), pref in updates.items():
            prefs[(<ts._MaterialRequest> req).ptx][(<ts._MaterialBid> bid).ptx] = pref

    void AdjustProductPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Product].type& prefs) except *:
        # cache the commod_reqs wrappers globally
        global _GET_PROD_PREFS_TIME, _GET_PROD_PREFS_PTR, _GET_PROD_PREFS
        cdef int curr_time = this.context().time()
        cdef cpp_cyclus.PrefMap[cpp_cyclus.Product].type* curr_ptr = &prefs
        if curr_time == _GET_PROD_PREFS_TIME and curr_ptr == _GET_PROD_PREFS_PTR:
            pyprefs = _GET_PROD_PREFS
        else:
            pyprefs = ts.product_pref_map_to_py(prefs)
            _GET_PROD_PREFS_TIME = curr_time
            _GET_PROD_PREFS_PTR = curr_ptr
            _GET_PROD_PREFS = pyprefs
        # call the python function
        updates = (<object> this.self).adjust_product_prefs(pyprefs)
        if updates is None or len(updates) == 0:
            return
        # update the prefs objects
        pyprefs.update(updates)
        for (req, bid), pref in updates.items():
            prefs[(<ts._ProductRequest> req).ptx][(<ts._ProductBid> bid).ptx] = pref

    void Tick() except *:
        openmp.omp_set_lock(&lock)
        (<object> this.self).tick()
        openmp.omp_unset_lock(&lock)

    void Tock() except *:
        openmp.omp_set_lock(&lock)
        (<object> this.self).tock()
        openmp.omp_unset_lock(&lock)

    void Decision() except *:
        (<object> this.self).decision()


cdef cppclass CyclusInstitutionShim "CyclusInstitutionShim" (cpp_cyclus.Institution):
    # A C++ class that acts as a Institution. It implements the Institution virtual
    # methods and dispatches the work to a Python/Cython object
    # that is has a reference to, as needed.

    CyclusInstitutionShim(cpp_cyclus.Context* ctx):  # C++BASES cyclus::Institution(ctx)
        pass

    std_string version() except *:
        rtn = (<object> this.self).version
        return str_py_to_cpp(rtn)

    cpp_cyclus.Agent* Clone() except *:
        cdef lib._Context ctx = lib.Context(init=False)
        (<lib._Context> ctx).ptx = this.context()
        cdef _Institution a = type(<object> this.self)(ctx)
        (<CyclusInstitutionShim*> (<_Agent> a).shim).InitFromAgent(<CyclusInstitutionShim*> this)
        (<lib._Agent> a)._free = False
        lib._AGENT_REFS[a.id] = a
        return dynamic_cast[agent_ptr](
                reinterpret_cast[institution_shim_ptr]((<_Agent> a).shim))

    void InitFromAgent "InitFrom" (CyclusInstitutionShim* a) except *:
        cpp_cyclus.Institution.Institution_InitFromAgent(a)
        (<object> this.self).init_from_agent(<object> a.self)

    void InfileToDb(cpp_cyclus.InfileTree* tree, cpp_cyclus.DbInit di) except *:
        cpp_cyclus.Institution.Institution_InfileToDb(tree, di)
        # wrap interface
        cdef lib._InfileTree py_tree = lib.InfileTree(free=False)
        py_tree.ptx = tree
        cdef lib._DbInit py_di = lib.DbInit(free=False)
        py_di.ptx = &di
        # call generic python
        (<object> this.self).infile_to_db(py_tree, py_di)

    void InitFrom(cpp_cyclus.QueryableBackend* b) except *:
        cpp_cyclus.Institution.Institution_InitFrom(b)
        cdef cpp_cyclus.QueryResult qr = b.Query(std_string(<char*> "Info"), NULL)
        res, _ = lib.single_query_result_to_py(qr, 0)
        # call generic python
        self = (<object> this.self)
        self.init_from_dict(res)
        lib._AGENT_REFS[self.id] = self

    void Snapshot(cpp_cyclus.DbInit di) except *:
        cdef lib._DbInit py_di = lib.DbInit(free=False)
        py_di.ptx = &di
        (<object> this.self).snapshot(py_di)

    void InitInv(cpp_cyclus.Inventories& invs) except *:
        pyinvs = lib.inventories_to_py(invs)
        (<object> this.self).init_inv(pyinvs)

    cpp_cyclus.Inventories SnapshotInv() except *:
        pyinvs = (<object> this.self).snapshot_inv()
        return lib.inventories_to_cpp(pyinvs)

    std_string schema() except *:
        pyschema = (<object> this.self).schema
        return str_py_to_cpp(pyschema)

    cpp_jsoncpp.Value annotations() except *:
        pyanno = (<object> this.self).annotations_json
        return lib.str_to_json_value(pyanno)

    void Build(cpp_cyclus.Agent* parent) except *:
        cpp_cyclus.Institution.Institution_Build(parent)
        pyrent = lib.agent_to_py(parent, None)
        (<object> this.self).build(pyrent)

    void EnterNotify() except *:
        cpp_cyclus.Institution.Institution_EnterNotify()
        (<object> this.self).enter_notify()

    void BuildNotify() except *:
        (<object> this.self).build_notify()

    void DecomNotify() except *:
        (<object> this.self).decom_notify()

    void AdjustMatlPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Material].type& prefs) except *:
        # cache the commod_reqs wrappers globally
        global _GET_MAT_PREFS_TIME, _GET_MAT_PREFS_PTR, _GET_MAT_PREFS
        cdef int curr_time = this.context().time()
        cdef cpp_cyclus.PrefMap[cpp_cyclus.Material].type* curr_ptr = &prefs
        if curr_time == _GET_MAT_PREFS_TIME and curr_ptr == _GET_MAT_PREFS_PTR:
            pyprefs = _GET_MAT_PREFS
        else:
            pyprefs = ts.material_pref_map_to_py(prefs)
            _GET_MAT_PREFS_TIME = curr_time
            _GET_MAT_PREFS_PTR = curr_ptr
            _GET_MAT_PREFS = pyprefs
        # call the python function
        updates = (<object> this.self).adjust_material_prefs(pyprefs)
        if updates is None or len(updates) == 0:
            return
        # update the prefs objects
        pyprefs.update(updates)
        for (req, bid), pref in updates.items():
            prefs[(<ts._MaterialRequest> req).ptx][(<ts._MaterialBid> bid).ptx] = pref

    void AdjustProductPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Product].type& prefs) except *:
        # cache the commod_reqs wrappers globally
        global _GET_PROD_PREFS_TIME, _GET_PROD_PREFS_PTR, _GET_PROD_PREFS
        cdef int curr_time = this.context().time()
        cdef cpp_cyclus.PrefMap[cpp_cyclus.Product].type* curr_ptr = &prefs
        if curr_time == _GET_PROD_PREFS_TIME and curr_ptr == _GET_PROD_PREFS_PTR:
            pyprefs = _GET_PROD_PREFS
        else:
            pyprefs = ts.product_pref_map_to_py(prefs)
            _GET_PROD_PREFS_TIME = curr_time
            _GET_PROD_PREFS_PTR = curr_ptr
            _GET_PROD_PREFS = pyprefs
        # call the python function
        updates = (<object> this.self).adjust_product_prefs(pyprefs)
        if updates is None or len(updates) == 0:
            return
        # update the prefs objects
        pyprefs.update(updates)
        for (req, bid), pref in updates.items():
            prefs[(<ts._ProductRequest> req).ptx][(<ts._ProductBid> bid).ptx] = pref

    void Tick() except *:
        openmp.omp_set_lock(&lock)
        (<object> this.self).tick()
        openmp.omp_unset_lock(&lock)

    void Tock() except *:
        openmp.omp_set_lock(&lock)
        cpp_cyclus.Institution.Institution_Tock()
        (<object> this.self).tock()
        openmp.omp_unset_lock(&lock)

    void Decision() except *:
        (<object> this.self).decision()


cdef int _GET_MAT_BIDS_TIME = -9999999999
cdef cpp_cyclus.CommodMap[cpp_cyclus.Material].type* _GET_MAT_BIDS_PTR = NULL
cdef dict _GET_MAT_BIDS = {}

cdef int _GET_PROD_BIDS_TIME = -9999999999
cdef cpp_cyclus.CommodMap[cpp_cyclus.Product].type* _GET_PROD_BIDS_PTR = NULL
cdef dict _GET_PROD_BIDS = {}

cdef cppclass CyclusFacilityShim "CyclusFacilityShim" (cpp_cyclus.Facility):
    # A C++ class that acts as a Facility. It implements the Facility virtual
    # methods and dispatches the work to a Python/Cython object
    # that is has a reference to, as needed.

    CyclusFacilityShim(cpp_cyclus.Context* ctx):  # C++BASES cyclus::Facility(ctx)
        pass

    std_string version() except *:
        rtn = (<object> this.self).version
        return str_py_to_cpp(rtn)

    cpp_cyclus.Agent* Clone() except *:
        cdef lib._Context ctx = lib.Context(init=False)
        (<lib._Context> ctx).ptx = this.context()
        cdef _Facility a = type(<object> this.self)(ctx)
        (<CyclusFacilityShim*> (<_Agent> a).shim).InitFromAgent(<CyclusFacilityShim*> this)
        (<lib._Agent> a)._free = False
        lib._AGENT_REFS[a.id] = a
        return dynamic_cast[agent_ptr](
                reinterpret_cast[facility_shim_ptr]((<_Agent> a).shim))

    void InitFromAgent "InitFrom" (CyclusFacilityShim* a) except *:
        cpp_cyclus.Facility.Facility_InitFromAgent(a)
        (<object> this.self).init_from_agent(<object> a.self)

    void InfileToDb(cpp_cyclus.InfileTree* tree, cpp_cyclus.DbInit di) except *:
        cpp_cyclus.Facility.Facility_InfileToDb(tree, di)
        # wrap interface
        cdef lib._InfileTree py_tree = lib.InfileTree(free=False)
        py_tree.ptx = tree
        cdef lib._DbInit py_di = lib.DbInit(free=False)
        py_di.ptx = &di
        # call generic python
        (<object> this.self).infile_to_db(py_tree, py_di)

    void InitFrom(cpp_cyclus.QueryableBackend* b) except *:
        cpp_cyclus.Facility.Facility_InitFrom(b)
        cdef cpp_cyclus.QueryResult qr = b.Query(std_string(<char*> "Info"), NULL)
        res, _ = lib.single_query_result_to_py(qr, 0)
        # call generic python
        self = (<object> this.self)
        self.init_from_dict(res)
        lib._AGENT_REFS[self.id] = self

    void Snapshot(cpp_cyclus.DbInit di) except *:
        cdef lib._DbInit py_di = lib.DbInit(free=False)
        py_di.ptx = &di
        (<object> this.self).snapshot(py_di)

    void InitInv(cpp_cyclus.Inventories& invs) except *:
        pyinvs = lib.inventories_to_py(invs)
        (<object> this.self).init_inv(pyinvs)

    cpp_cyclus.Inventories SnapshotInv() except *:
        pyinvs = (<object> this.self).snapshot_inv()
        return lib.inventories_to_cpp(pyinvs)

    std_string schema() except *:
        pyschema = (<object> this.self).schema
        return str_py_to_cpp(pyschema)

    cpp_jsoncpp.Value annotations() except *:
        pyanno = (<object> this.self).annotations_json
        return lib.str_to_json_value(pyanno)

    void Build(cpp_cyclus.Agent* parent) except *:
        cpp_cyclus.Facility.Facility_Build(parent)
        pyrent = lib.agent_to_py(parent, None)
        (<object> this.self).build(pyrent)

    void EnterNotify() except *:
        cpp_cyclus.Facility.Facility_EnterNotify()
        (<object> this.self).enter_notify()

    void BuildNotify() except *:
        (<object> this.self).build_notify()

    void DecomNotify() except *:
        (<object> this.self).decom_notify()

    void AdjustMatlPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Material].type& prefs) except *:
        # cache the commod_reqs wrappers globally
        global _GET_MAT_PREFS_TIME, _GET_MAT_PREFS_PTR, _GET_MAT_PREFS
        cdef int curr_time = this.context().time()
        cdef cpp_cyclus.PrefMap[cpp_cyclus.Material].type* curr_ptr = &prefs
        if curr_time == _GET_MAT_PREFS_TIME and curr_ptr == _GET_MAT_PREFS_PTR:
            pyprefs = _GET_MAT_PREFS
        else:
            pyprefs = ts.material_pref_map_to_py(prefs)
            _GET_MAT_PREFS_TIME = curr_time
            _GET_MAT_PREFS_PTR = curr_ptr
            _GET_MAT_PREFS = pyprefs
        # call the python function
        updates = (<object> this.self).adjust_material_prefs(pyprefs)
        if updates is None or len(updates) == 0:
            return
        # update the prefs objects
        pyprefs.update(updates)
        for (req, bid), pref in updates.items():
            prefs[(<ts._MaterialRequest> req).ptx][(<ts._MaterialBid> bid).ptx] = pref

    void AdjustProductPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Product].type& prefs) except *:
        # cache the commod_reqs wrappers globally
        global _GET_PROD_PREFS_TIME, _GET_PROD_PREFS_PTR, _GET_PROD_PREFS
        cdef int curr_time = this.context().time()
        cdef cpp_cyclus.PrefMap[cpp_cyclus.Product].type* curr_ptr = &prefs
        if curr_time == _GET_PROD_PREFS_TIME and curr_ptr == _GET_PROD_PREFS_PTR:
            pyprefs = _GET_PROD_PREFS
        else:
            pyprefs = ts.product_pref_map_to_py(prefs)
            _GET_PROD_PREFS_TIME = curr_time
            _GET_PROD_PREFS_PTR = curr_ptr
            _GET_PROD_PREFS = pyprefs
        # call the python function
        updates = (<object> this.self).adjust_product_prefs(pyprefs)
        if updates is None or len(updates) == 0:
            return
        # update the prefs objects
        pyprefs.update(updates)
        for (req, bid), pref in updates.items():
            prefs[(<ts._ProductRequest> req).ptx][(<ts._ProductBid> bid).ptx] = pref

    void Tick() except *:
        openmp.omp_set_lock(&lock)
        (<object> this.self).tick()
        openmp.omp_unset_lock(&lock)

    void Tock() except *:
        openmp.omp_set_lock(&lock)
        (<object> this.self).tock()
        openmp.omp_unset_lock(&lock)

    void Decision() except *:
        (<object> this.self).decision()

    cpp_bool CheckDecommissionCondition() except *:
        rtn = (<object> this.self).check_decommission_condition()
        return bool_to_cpp(rtn)

    std_set[shared_ptr[cpp_cyclus.RequestPortfolio[cpp_cyclus.Material]]] GetMatlRequests() except *:
        pyportfolios = (<object> this.self).get_material_requests()
        cdef std_set[shared_ptr[cpp_cyclus.RequestPortfolio[cpp_cyclus.Material]]] ports = \
            std_set[shared_ptr[cpp_cyclus.RequestPortfolio[cpp_cyclus.Material]]]()
        if isinstance(pyportfolios, Mapping):
            pyportfolios = [pyportfolios]
        for pyport in pyportfolios:
            normport = lib.normalize_request_portfolio(pyport)
            #ports.insert(ts.material_request_portfolio_to_cpp(normport, this))
            ports.insert(ts.material_request_portfolio_to_cpp(normport,
                dynamic_cast[trader_ptr](reinterpret_cast[facility_shim_ptr](<CyclusFacilityShim*> this))
                ))
        return ports

    std_set[shared_ptr[cpp_cyclus.RequestPortfolio[cpp_cyclus.Product]]] GetProductRequests() except *:
        pyportfolios = (<object> this.self).get_product_requests()
        cdef std_set[shared_ptr[cpp_cyclus.RequestPortfolio[cpp_cyclus.Product]]] ports = \
            std_set[shared_ptr[cpp_cyclus.RequestPortfolio[cpp_cyclus.Product]]]()
        if isinstance(pyportfolios, Mapping):
            pyportfolios = [pyportfolios]
        for pyport in pyportfolios:
            normport = lib.normalize_request_portfolio(pyport)
            ports.insert(ts.product_request_portfolio_to_cpp(normport, this))
        return ports

    std_set[shared_ptr[cpp_cyclus.BidPortfolio[cpp_cyclus.Material]]] GetMatlBids(cpp_cyclus.CommodMap[cpp_cyclus.Material].type& commod_requests) except *:
        # cache the commod_reqs wrappers globally
        global _GET_MAT_BIDS_TIME, _GET_MAT_BIDS_PTR, _GET_MAT_BIDS
        cdef int curr_time = this.context().time()
        cdef cpp_cyclus.CommodMap[cpp_cyclus.Material].type* curr_ptr = &commod_requests
        if curr_time == _GET_MAT_BIDS_TIME and curr_ptr == _GET_MAT_BIDS_PTR:
            pyreq = _GET_MAT_BIDS
        else:
            pyreq = ts.material_commod_map_to_py(commod_requests)
            _GET_MAT_BIDS_TIME = curr_time
            _GET_MAT_BIDS_PTR = curr_ptr
            _GET_MAT_BIDS = pyreq
        # call the Python funtion
        pyports = (<object> this.self).get_material_bids(pyreq)
        # convert to c++ and return
        cdef std_set[shared_ptr[cpp_cyclus.BidPortfolio[cpp_cyclus.Material]]] ports = \
            std_set[shared_ptr[cpp_cyclus.BidPortfolio[cpp_cyclus.Material]]]()
        if pyports is None:
            return ports
        elif isinstance(pyports, Mapping):
            pyports = [pyports]
        for pyport in pyports:
            normport = lib.normalize_bid_portfolio(pyport)
            #ports.insert(ts.material_bid_portfolio_to_cpp(normport, this))
            ports.insert(ts.material_bid_portfolio_to_cpp(normport,
                dynamic_cast[trader_ptr](reinterpret_cast[facility_shim_ptr](<CyclusFacilityShim*> this))
                #dynamic_cast[trader_ptr](<CyclusFacilityShim*> this)
                ))
        return ports

    std_set[shared_ptr[cpp_cyclus.BidPortfolio[cpp_cyclus.Product]]] GetProductBids(cpp_cyclus.CommodMap[cpp_cyclus.Product].type& commod_requests) except *:
        # cache the commod_reqs wrappers globally
        global _GET_PROD_BIDS_TIME, _GET_PROD_BIDS_PTR, _GET_PROD_BIDS
        cdef int curr_time = this.context().time()
        cdef cpp_cyclus.CommodMap[cpp_cyclus.Product].type* curr_ptr = &commod_requests
        if curr_time == _GET_PROD_BIDS_TIME and curr_ptr == _GET_PROD_BIDS_PTR:
            pyreq = _GET_PROD_BIDS
        else:
            pyreq = ts.product_commod_map_to_py(commod_requests)
            _GET_PROD_BIDS_TIME = curr_time
            _GET_PROD_BIDS_PTR = curr_ptr
            _GET_PROD_BIDS = pyreq
        # call the Python funtion
        pyports = (<object> this.self).get_product_bids(pyreq)
        # convert to c++ and return
        cdef std_set[shared_ptr[cpp_cyclus.BidPortfolio[cpp_cyclus.Product]]] ports = \
            std_set[shared_ptr[cpp_cyclus.BidPortfolio[cpp_cyclus.Product]]]()
        if pyports is None:
            return ports
        elif isinstance(pyports, Mapping):
            pyports = [pyports]
        for pyport in pyports:
            normport = lib.normalize_bid_portfolio(pyport)
            ports.insert(ts.product_bid_portfolio_to_cpp(normport, this))
        return ports

    void GetMatlTrades(const std_vector[cpp_cyclus.Trade[cpp_cyclus.Material]]& trades, std_vector[std_pair[cpp_cyclus.Trade[cpp_cyclus.Material], shared_ptr[cpp_cyclus.Material]]]& responses) except *:
        pytrades = ts.material_trade_vector_to_py(trades)
        pyresp = (<object> this.self).get_material_trades(pytrades)
        if pyresp is None or len(pyresp) == 0:
            return
        if not isinstance(pyresp, Mapping):
            pyresp = dict(pyresp)
        for trade, resp in pyresp.items():
            responses.push_back(std_pair[cpp_cyclus.Trade[cpp_cyclus.Material],
                                         shared_ptr[cpp_cyclus.Material]](
                deref((<ts._MaterialTrade> trade).ptx),
                reinterpret_pointer_cast[cpp_cyclus.Material, cpp_cyclus.Resource](
                    (<ts._Material> resp).ptx)
                ))

    void GetProductTrades(const std_vector[cpp_cyclus.Trade[cpp_cyclus.Product]]& trades, std_vector[std_pair[cpp_cyclus.Trade[cpp_cyclus.Product], shared_ptr[cpp_cyclus.Product]]]& responses) except *:
        pytrades = ts.product_trade_vector_to_py(trades)
        pyresp = (<object> this.self).get_product_trades(pytrades)
        if pyresp is None or len(pyresp) == 0:
            return
        if not isinstance(pyresp, Mapping):
            pyresp = dict(pyresp)
        for trade, resp in pyresp.items():
            responses.push_back(std_pair[cpp_cyclus.Trade[cpp_cyclus.Product],
                                         shared_ptr[cpp_cyclus.Product]](
                deref((<ts._ProductTrade> trade).ptx),
                reinterpret_pointer_cast[cpp_cyclus.Product, cpp_cyclus.Resource](
                    (<ts._Product> resp).ptx)
                ))

    void AcceptMatlTrades(const std_vector[std_pair[cpp_cyclus.Trade[cpp_cyclus.Material], shared_ptr[cpp_cyclus.Material]]]& responses) except *:
        pyresp = ts.material_responses_to_py(responses)
        (<object> this.self).accept_material_trades(pyresp)

    void AcceptProductTrades(const std_vector[std_pair[cpp_cyclus.Trade[cpp_cyclus.Product], shared_ptr[cpp_cyclus.Product]]]& responses) except *:
        pyresp = ts.product_responses_to_py(responses)
        (<object> this.self).accept_product_trades(pyresp)


#
# Wrapper classes
#
cdef class _Agent(lib._Agent):

    _statevars = None
    _inventories = None
    _schema = None
    _annotations_json = None
    entity = 'archetype'
    niche = None
    tooltip = None
    userlevel = 0

    def __cinit__(self, lib._Context ctx):
        # Let subclasses do cinit() and if they don't make an new instance,
        # make one here
        if self.ptx == NULL:
            self.ptx = self.shim = new CyclusAgentShim(ctx.ptx)
            self._free = True
            self.shim.self = <PyObject*> self

    def __init__(self, ctx):
        # gather the state variables and inventories
        cls = type(self)
        if cls._statevars is None:
            cls._init_statevars()
        if cls._inventories is None:
            cls._init_inventories()
        # copy state vars from class
        cdef list svs = []
        for name, var in cls._statevars:
            var = var.copy()
            if var.value is not None:
                pass
            elif var.default is None:
                var.value = ts.new_py_inst(var.uniquetypeid)
            else:
                var.value = var.default
            self.__dict__[name] = var
            svs.append((name, var))
        self._statevars = tuple(svs)
        # copy and init inventories from class
        cdef list invs = []
        for name, inv in cls._inventories:
            inv = inv.copy()
            inv._init()
            self.__dict__[name] = inv
            invs.append((name, inv))
        self._inventories = tuple(invs)

    @classmethod
    def _init_statevars(cls):
        cdef dict vars = {}
        for name in dir(cls):
            attr = getattr(cls, name)
            if not isinstance(attr, ts.StateVar):
                continue
            attr.name = name
            attr.alias = _VAR_DECL.canonize_alias(attr.type, name, attr.alias)
            attr.tooltip = _VAR_DECL.canonize_tooltip(attr.type, name, attr.tooltip)
            attr.uilabel = _VAR_DECL.canonize_uilabel(attr.type, name, attr.uilabel)
            attr.uitype = ts.prepare_type_representation(attr.type, attr.uitype)
            vars[name] = attr
        # make sure all state vars have a consistent index
        cls._statevars = index_and_sort_vars(vars)

    @classmethod
    def _init_inventories(cls):
        cdef dict invs = {}
        for name in dir(cls):
            attr = getattr(cls, name)
            if not isinstance(attr, ts.Inventory):
                continue
            attr.name = name
            invs[name] = attr
        cls._inventories = index_and_sort_vars(invs)

    def init_from_agent(self, other):
        """A dynamic version of InitFrom(Agent) that should work for all
        subclasses. Users should not need to call this ever. However, brave
        users may choose to override it in exceptional cases.
        """
        for name, var in self._statevars:
            setattr(self, name, deepcopy(getattr(other, name, None)))
        for (name, inv), (_, other_inv) in zip(self._inventories, other._inventories):
            inv.value.capacity = other_inv.value.capacity

    def infile_to_db(self, tree, di):
        """A dynamic version of InfileToDb(InfileTree*, DbInit) that should
        work for all subclasses. Users should not need to call this ever.
        However, brave users may choose to override it in exceptional cases.
        """
        sub = tree.subtree("config/*")
        for name, var in self._statevars:
            # set internal state variable values to default
            if var.internal:
                if var.default is None:
                    raise RuntimeError('state variables marked as internal '
                                       'must have a default')
                var.value = var.default
                continue
            query = var.alias if isinstance(var.alias, str) else var.alias[0]
            if var.default is not None and sub.nmatches(query) == 0:
                var.value = var.default
                continue
            else:
                var.value = self._read_from_infile(sub, var.alias, var.type, var.uitype)
        # Write out to db
        datum = di.new_datum("Info")
        for name, var in self._statevars:
            datum.add_val(name, var.value, shape=var.shape, type=var.uniquetypeid)
        datum.record()

    def _query_infile(self, tree, query, type, uitype, idx=0):
        if uitype == 'nuclide':
            rtn = tree.query(query, cpp_typesystem.STRING, idx)
            rtn = nucname.id(rtn)
        else:
            rtn = tree.query(query, type, idx)
        return rtn

    def _read_from_infile(self, tree, alias, type, uitype, idx=0, path=''):
        if isinstance(type, str):
            # read primitive
            rtn = self._query_infile(tree, path + alias, type, uitype, idx)
        else:
            type0 = type[0]
            sub = tree.subtree(alias[0], idx)
            if type0 == 'std::vector':
                rtn = self._vector_from_infile(sub, alias, type, uitype, idx, path)
            elif type0 == 'std::set':
                rtn = self._set_from_infile(sub, alias, type, uitype, idx, path)
            elif type0 == 'std::list':
                # note we can reuse vector implementation
                rtn = self._vector_from_infile(sub, alias, type, uitype, idx, path)
            elif type0 == 'std::pair':
                rtn = self._pair_from_infile(sub, alias, type, uitype, idx, path)
            elif type0 == 'std::map':
                rtn = self._map_from_infile(sub, alias, type, uitype, idx, path)
            else:
                raise TypeError('type not recognized while reading input file: '
                                + type0 + ' of ' + repr(type) + ' in ' + repr(alias))
        return rtn

    def _vector_from_infile(self, tree, alias, type, uitype, idx=0, path=''):
        cdef int i, n
        cdef list rtn = []
        # get val name
        if alias[1] is None:
            val = 'val'
        elif isinstance(alias[1], str):
            val = alias[1]
        else:
            val = alias[1][0]
        # read in values.
        n = tree.nmatches(val)
        for i in range(n):
            x = self._read_from_infile(tree, val, type[1], uitype[1], i, path)
            rtn.append(x)
        return rtn

    def _set_from_infile(self, tree, alias, type, uitype, idx=0, path=''):
        rtn = self._vector_from_infile(tree, alias, type, uitype, idx)
        rtn = set(rtn)
        return rtn

    def _pair_from_infile(self, tree, alias, type, uitype, idx=0, path=''):
        first = 'first' if alias[1] is None else alias[1]
        second = 'second' if alias[2] is None else alias[2]
        x = self._read_from_infile(tree, first, type[1], uitype[1], idx, path)
        y = self._read_from_infile(tree, second, type[2], uitype[2], idx, path)
        return (x, y)

    def _map_from_infile(self, tree, alias, type, uitype, idx=0, path=''):
        cdef int i, n
        cdef dict rtn = {}
        # get query names
        if isinstance(alias[0], str):
            item = 'item'
        elif alias[0][1] is None:
            item = 'item'
        else:
            item = alias[0][1]
        key = 'key' if alias[1] is None else alias[1]
        val = 'val' if alias[2] is None else alias[2]
        # read in values.
        itempath = item + '/'
        n = tree.nmatches(item)
        for i in range(n):
            k = self._read_from_infile(tree, key, type[1], uitype[1], i, itempath)
            v = self._read_from_infile(tree, val, type[2], uitype[2], i, itempath)
            rtn[k] = v
        return rtn

    def init_from_dict(self, d):
        """An initializer that reads state varaible values from a dictionary.
        This is used when InitFrom(QueryableBackend) is called.
        Users should not need to call this ever. However, brave
        users may choose to override it in exceptional cases.
        """
        for name, var in self._statevars:
            setattr(self, name, d[name])
        for name, inv in self._inventories:
            if isinstance(inv.capacity, str):
                inv.value.capacity = d[inv.capacity]
            elif inv.capacity is None:
                # infinite capacity when unspecified
                inv.value.capacity = 1e300
            else:
                inv.value.capacity = inv.capacity

    def snapshot(self, di):
        """A dynamic version of Snapshot(DbInit) that should
        work for all subclasses.
        Users should not need to call this ever. However, brave
        users may choose to override it in exceptional cases.
        """
        datum = di.new_datum("Info")
        for name, var in self._statevars:
            datum.add_val(name, var.value, shape=var.shape, type=var.uniquetypeid)
        datum.record()

    def init_inv(self, invs):
        """An initializer that sets up inventories.
        This is used when InitInv(Inventories&) is called.
        Users should not need to call this ever. However, brave
        users may choose to override it in exceptional cases.
        """
        for name, inv in self._inventories:
            if name not in invs:
                continue
            inv.value.push_many(invs[name])

    def snapshot_inv(self):
        """A dynamic version of SnapshotInv() that reports the current
        state of the inventories.
        Users should not need to call this ever. However, brave
        users may choose to override it in exceptional cases.
        """
        cdef dict invs = {}
        for name, inv in self._inventories:
            invs[name] = inv.value.pop_all_res()
            inv.value.push_many(invs[name])
        return invs

    @property
    def schema(self):
        """"The class schema based on the state variables."""
        if self._schema is None:
            cls = type(self)
            ctx = {name: var.to_dict() for name, var in cls._statevars}
            self._schema = _SCHEMA.xml_from_ctx(ctx)
        return self._schema

    @property
    def version(self):
        """Property that represents version of this archetype. Overwrite this to return
        a custom version string.
        """
        return "0.0.0"


    @property
    def annotations_json(self):
        if self._annotations_json is None:
            cls = type(self)
            vars = {}
            for name, var in cls._statevars:
                vars[name] = {k: v for k, v in var.to_dict() if v is not None}
                vars[name].pop('value', None)
            aj = {'vars': vars,
                  'name': cls.__name__,
                  'entity': cls.entity,
                  'parents': [b.__name__ for b in cls.__bases__],
                  'all_parents': [b.__name__ for b in getmro(cls)[1:]],
                  'doc': getdoc(cls),
                  'userlevel': cls.userlevel,
                  }
            niche = getattr(cls, 'niche', None)
            if niche is not None:
                aj['niche'] = niche
            tt = getattr(cls, 'tooltip', None)
            if tt is not None:
                aj['tooltip'] = tt
            self._annotations_json = json.dumps(aj, separators=(',', ':'))
        return self._annotations_json

    def build(self, parent):
        """Called when the agent enters the smiulation as an active participant and
        is only ever called once.  Agents should NOT register for services (such
        as ticks/tocks and resource exchange) in this function. If agents implement
        this function, they must call their superclass' Build function at the
        BEGINING of their Build function.
        """
        pass

    def enter_notify(self):
        """Called to give the agent an opportunity to register for services (e.g.
        ticks/tocks and resource exchange).  Note that this may be called more
        than once, and so agents should track their registrations carefully. If
        agents implement this function, they must call their superclass's
        EnterNotify function at the BEGINNING of their EnterNotify function.
        """
        pass

    def build_notify(self):
        """Called when a new child of this agent has just been built. It is possible
        for this function to be called before the simulation has started when
        initially existing agents are being setup.
        """
        pass

    def decom_notify(self):
        """Called when a new child of this agent is about to be decommissioned."""
        pass

    def adjust_material_prefs(self, prefs):
        """Product preferences adjustment."""
        return None

    def adjust_product_prefs(self, prefs):
        """Product preferences adjustment."""
        return None


class Agent(_Agent, lib.Agent):
    """Python Agent that is subclassable into any kind of agent.

    Parameters
    ----------
    ctx : cyclus.lib.Context
        The simulation execution context.  You don't normally
        need to call the initilaizer.
    """


cdef class _Region(_Agent):

    def __cinit__(self, lib._Context ctx):
        self.ptx = self.shim = <CyclusAgentShim*> new CyclusRegionShim(ctx.ptx)
        self._free = True
        (<CyclusRegionShim*> (<_Agent> self).shim).self = <PyObject*> self

    def __dealloc__(self):
        cdef CyclusRegionShim* cpp_ptx
        if self.ptx == NULL:
            return
        elif self._free:
            cpp_ptx = <CyclusRegionShim*> self.shim
            del cpp_ptx
            self.ptx = self.shim = NULL
        else:
            self.ptx = self.shim = NULL

    def children_str(self):
        """Returns recursively generated string of the parent-child tree."""
        return std_string_to_py((<CyclusRegionShim*> (<_Agent> self).shim).PrintChildren())

    def tree_strs(self, m):
        """Returns a list of children strings representing the parent-child tree
        at the node for Agent m.
        """
        cdef cpp_cyclus.Agent* cpp_m = <cpp_cyclus.Agent*> (<_Agent> m).ptx
        rtn = std_vector_std_string_to_py(
                (<CyclusRegionShim*> (<_Agent> self).shim).GetTreePrintOuts(cpp_m))
        return rtn

    def in_family_tree(self, other):
        """Returns true if this agent is in the parent-child family tree of an
        other agent.
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<CyclusRegionShim*> (<_Agent> self).shim).InFamilyTree(cpp_other))
        return rtn

    def ancestor_of(self, other):
        """Returns true if this agent is an ancestor of an other agent (i.e., resides
        above an other agent in the family tree).
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<CyclusRegionShim*> (<_Agent> self).shim).AncestorOf(cpp_other))
        return rtn

    def decendent_of(self, other):
        """Returns true if this agent is an decendent of an other agent (i.e., resides
        above an other agent in the family tree).
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<CyclusRegionShim*> (<_Agent> self).shim).DecendentOf(cpp_other))
        return rtn

    def decommission(self):
        """Decommissions the agent, removing it from the simulation. Results in
        destruction of the agent object. If agents write their own decommission()
        function, they must call their superclass' decommission function at the
        END of their decommission() function.
        """
        (<CyclusRegionShim*> (<_Agent> self).shim).Decommission()

    @property
    def annotations(self):
        """Agent annotations."""
        cdef jsoncpp.Value cpp_rtn = jsoncpp.Value()
        if self._annotations is None:
            cpp_rtn._inst[0] = (<CyclusRegionShim*> (<_Agent> self).shim).annotations()
        self._annotations = cpp_rtn
        return self._annotations

    @property
    def prototype(self):
        """The agent's prototype."""
        rtn = std_string_to_py((<CyclusRegionShim*> (<_Agent> self).shim).get_prototype())
        return rtn

    @prototype.setter
    def prototype(self, str p):
        cdef std_string cpp_p = str_py_to_cpp(p)
        (<CyclusRegionShim*> (<_Agent> self).shim).prototype(cpp_p)

    @property
    def spec(self):
        """The agent's spec."""
        rtn = std_string_to_py((<CyclusRegionShim*> (<_Agent> self).shim).get_spec())
        return rtn

    @spec.setter
    def spec(self, str new_impl):
        cdef std_string cpp_new_impl = str_py_to_cpp(new_impl)
        (<CyclusRegionShim*> (<_Agent> self).shim).spec(cpp_new_impl)

    @property
    def id(self):
        """The agent instance's unique ID within a simulation."""
        return (<CyclusRegionShim*> (<_Agent> self).shim).id()

    @property
    def id(self):
        """The agent instance's unique ID within a simulation."""
        return (<CyclusRegionShim*> (<_Agent> self).shim).id()

    @property
    def kind(self):
        """Returns a string that describes the agent subclass (e.g. Region,
        Facility, etc.)
        """
        rtn = std_string_to_py((<CyclusRegionShim*> (<_Agent> self).shim).kind())
        return rtn

    def __str__(self):
        rtn = std_string_to_py((<CyclusRegionShim*> (<_Agent> self).shim).str())
        return rtn

    def parent(self):
        """Returns parent of this agent.  Returns None if the agent has no parent.
        """
        rtn = lib.agent_to_py((<CyclusRegionShim*> (<_Agent> self).shim).parent(), None)
        return rtn

    @property
    def parent_id(self):
        """The id for this agent's parent or -1 if this agent has no parent."""
        return (<CyclusRegionShim*> (<_Agent> self).shim).parent_id()

    @property
    def enter_time(self):
        """The time step at which this agent's Build function was called
        (-1 if the agent has never been built).
        """
        return (<CyclusRegionShim*> (<_Agent> self).shim).enter_time()

    @property
    def lifetime(self):
        """The number of time steps this agent operates between building and
        decommissioning (-1 if the agent has an infinite lifetime)
        """
        return (<CyclusRegionShim*> (<_Agent> self).shim).get_lifetime()

    @lifetime.setter
    def lifetime(self, int n_timesteps):
        (<CyclusRegionShim*> (<_Agent> self).shim).lifetime(n_timesteps)

    def lifetime_force(self, int n_timesteps):
        (<CyclusRegionShim*> (<_Agent> self).shim).lifetime_force(n_timesteps)

    @property
    def exit_time(self):
        """The default time step at which this agent will exit the
        simulation (-1 if the agent has an infinite lifetime).

        Decommissioning happens at the end of a time step. With a lifetime of 1, we
        expect an agent to go through only 1 entire time step. In this case, the
        agent should be decommissioned on the same time step it was
        created. Therefore, for agents with non-infinite lifetimes, the exit_time
        will be the enter time plus its lifetime less 1.
        """
        return (<CyclusRegionShim*> (<_Agent> self).shim).exit_time()

    @property
    def children(self):
        """A frozen set of the children of this agent."""
        kids = []
        for kid_ptx in (<CyclusRegionShim*> (<_Agent> self).shim).children():
            kid = lib.agent_to_py(kid_ptx, None)
            kids.append(kid)
        return frozenset(kids)


class Region(_Region):
    """Python Region that is subclassable into a region archetype.

    Parameters
    ----------
    ctx : cyclus.lib.Context
        The simulation execution context.  You don't normally
        need to call the initilaizer.
    """
    entity = 'region'

    def tick(self):
        """This function is called each time step and is meant to be
        overlaoded in the subclass.
        """
        pass

    def tock(self):
        """This function is called each time step and is meant to be
        overlaoded in the subclass.
        """
        pass

    def decision(self):
        """This function is called each time step and is meant to be
        overlaoded in the subclass.
        """
        pass

cdef class _Institution(_Agent):

    def __cinit__(self, lib._Context ctx):
        self.ptx = self.shim = <CyclusAgentShim*> new CyclusInstitutionShim(ctx.ptx)
        self._free = True
        (<CyclusInstitutionShim*> (<_Agent> self).shim).self = <PyObject*> self

    def __dealloc__(self):
        cdef CyclusInstitutionShim* cpp_ptx
        if self.ptx == NULL:
            return
        elif self._free:
            cpp_ptx = <CyclusInstitutionShim*> self.shim
            del cpp_ptx
            self.ptx = self.shim = NULL
        else:
            self.ptx = self.shim = NULL

    def children_str(self):
        """Returns recursively generated string of the parent-child tree."""
        return std_string_to_py((<CyclusInstitutionShim*> (<_Agent> self).shim).PrintChildren())

    def tree_strs(self, m):
        """Returns a list of children strings representing the parent-child tree
        at the node for Agent m.
        """
        cdef cpp_cyclus.Agent* cpp_m = <cpp_cyclus.Agent*> (<_Agent> m).ptx
        rtn = std_vector_std_string_to_py(
                (<CyclusInstitutionShim*> (<_Agent> self).shim).GetTreePrintOuts(cpp_m))
        return rtn

    def in_family_tree(self, other):
        """Returns true if this agent is in the parent-child family tree of an
        other agent.
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<CyclusInstitutionShim*> (<_Agent> self).shim).InFamilyTree(cpp_other))
        return rtn

    def ancestor_of(self, other):
        """Returns true if this agent is an ancestor of an other agent (i.e., resides
        above an other agent in the family tree).
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<CyclusInstitutionShim*> (<_Agent> self).shim).AncestorOf(cpp_other))
        return rtn

    def decendent_of(self, other):
        """Returns true if this agent is an decendent of an other agent (i.e., resides
        above an other agent in the family tree).
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<CyclusInstitutionShim*> (<_Agent> self).shim).DecendentOf(cpp_other))
        return rtn

    def decommission(self):
        """Decommissions the agent, removing it from the simulation. Results in
        destruction of the agent object. If agents write their own decommission()
        function, they must call their superclass' decommission function at the
        END of their decommission() function.
        """
        (<CyclusInstitutionShim*> (<_Agent> self).shim).Decommission()

    @property
    def annotations(self):
        """Agent annotations."""
        cdef jsoncpp.Value cpp_rtn = jsoncpp.Value()
        if self._annotations is None:
            cpp_rtn._inst[0] = (<CyclusInstitutionShim*> (<_Agent> self).shim).annotations()
        self._annotations = cpp_rtn
        return self._annotations

    @property
    def prototype(self):
        """The agent's prototype."""
        rtn = std_string_to_py((<CyclusInstitutionShim*> (<_Agent> self).shim).get_prototype())
        return rtn

    @prototype.setter
    def prototype(self, str p):
        cdef std_string cpp_p = str_py_to_cpp(p)
        (<CyclusInstitutionShim*> (<_Agent> self).shim).prototype(cpp_p)

    @property
    def spec(self):
        """The agent's spec."""
        rtn = std_string_to_py((<CyclusInstitutionShim*> (<_Agent> self).shim).get_spec())
        return rtn

    @spec.setter
    def spec(self, str new_impl):
        cdef std_string cpp_new_impl = str_py_to_cpp(new_impl)
        (<CyclusInstitutionShim*> (<_Agent> self).shim).spec(cpp_new_impl)

    @property
    def id(self):
        """The agent instance's unique ID within a simulation."""
        return (<CyclusInstitutionShim*> (<_Agent> self).shim).id()

    @property
    def id(self):
        """The agent instance's unique ID within a simulation."""
        return (<CyclusInstitutionShim*> (<_Agent> self).shim).id()

    @property
    def kind(self):
        """Returns a string that describes the agent subclass (e.g. Region,
        Facility, etc.)
        """
        rtn = std_string_to_py((<CyclusInstitutionShim*> (<_Agent> self).shim).kind())
        return rtn

    def __str__(self):
        rtn = std_string_to_py((<CyclusInstitutionShim*> (<_Agent> self).shim).str())
        return rtn

    def parent(self):
        """Returns parent of this agent.  Returns None if the agent has no parent.
        """
        rtn = lib.agent_to_py((<CyclusInstitutionShim*> (<_Agent> self).shim).parent(), None)
        return rtn

    @property
    def parent_id(self):
        """The id for this agent's parent or -1 if this agent has no parent."""
        return (<CyclusInstitutionShim*> (<_Agent> self).shim).parent_id()

    @property
    def enter_time(self):
        """The time step at which this agent's Build function was called
        (-1 if the agent has never been built).
        """
        return (<CyclusInstitutionShim*> (<_Agent> self).shim).enter_time()

    @property
    def lifetime(self):
        """The number of time steps this agent operates between building and
        decommissioning (-1 if the agent has an infinite lifetime)
        """
        return (<CyclusInstitutionShim*> (<_Agent> self).shim).get_lifetime()

    @lifetime.setter
    def lifetime(self, int n_timesteps):
        (<CyclusInstitutionShim*> (<_Agent> self).shim).lifetime(n_timesteps)

    def lifetime_force(self, int n_timesteps):
        (<CyclusInstitutionShim*> (<_Agent> self).shim).lifetime_force(n_timesteps)

    @property
    def exit_time(self):
        """The default time step at which this agent will exit the
        simulation (-1 if the agent has an infinite lifetime).

        Decommissioning happens at the end of a time step. With a lifetime of 1, we
        expect an agent to go through only 1 entire time step. In this case, the
        agent should be decommissioned on the same time step it was
        created. Therefore, for agents with non-infinite lifetimes, the exit_time
        will be the enter time plus its lifetime less 1.
        """
        return (<CyclusInstitutionShim*> (<_Agent> self).shim).exit_time()

    @property
    def children(self):
        """A frozen set of the children of this agent."""
        kids = []
        for kid_ptx in (<CyclusInstitutionShim*> (<_Agent> self).shim).children():
            kid = lib.agent_to_py(kid_ptx, None)
            kids.append(kid)
        return frozenset(kids)


class Institution(_Institution):
    """Python Institution that is subclassable into a institution archetype.

    Parameters
    ----------
    ctx : cyclus.lib.Context
        The simulation execution context.  You don't normally
        need to call the initilaizer.
    """
    entity = 'institution'

    def tick(self):
        """This function is called each time step and is meant to be
        overlaoded in the subclass.
        """
        pass

    def tock(self):
        """This function is called each time step and is meant to be
        overlaoded in the subclass.
        """
        pass

    def decision(self):
        """This function is called each time step and is meant to be
        overlaoded in the subclass.
        """
        pass


cdef class _Facility(_Agent):

    def __cinit__(self, lib._Context ctx):
        self.ptx = self.shim = <CyclusAgentShim*> new CyclusFacilityShim(ctx.ptx)
        self._free = True
        (<CyclusFacilityShim*> (<_Agent> self).shim).self = <PyObject*> self

    def __dealloc__(self):
        cdef CyclusFacilityShim* cpp_ptx
        if self.ptx == NULL:
            return
        elif self._free:
            cpp_ptx = <CyclusFacilityShim*> self.shim
            del cpp_ptx
            self.ptx = self.shim = NULL
        else:
            self.ptx = self.shim = NULL

    def children_str(self):
        """Returns recursively generated string of the parent-child tree."""
        return std_string_to_py((<CyclusFacilityShim*> (<_Agent> self).shim).PrintChildren())

    def tree_strs(self, m):
        """Returns a list of children strings representing the parent-child tree
        at the node for Agent m.
        """
        cdef cpp_cyclus.Agent* cpp_m = <cpp_cyclus.Agent*> (<_Agent> m).ptx
        rtn = std_vector_std_string_to_py(
                (<CyclusFacilityShim*> (<_Agent> self).shim).GetTreePrintOuts(cpp_m))
        return rtn

    def in_family_tree(self, other):
        """Returns true if this agent is in the parent-child family tree of an
        other agent.
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<CyclusFacilityShim*> (<_Agent> self).shim).InFamilyTree(cpp_other))
        return rtn

    def ancestor_of(self, other):
        """Returns true if this agent is an ancestor of an other agent (i.e., resides
        above an other agent in the family tree).
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<CyclusFacilityShim*> (<_Agent> self).shim).AncestorOf(cpp_other))
        return rtn

    def decendent_of(self, other):
        """Returns true if this agent is an decendent of an other agent (i.e., resides
        above an other agent in the family tree).
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<CyclusFacilityShim*> (<_Agent> self).shim).DecendentOf(cpp_other))
        return rtn

    def decommission(self):
        """Decommissions the agent, removing it from the simulation. Results in
        destruction of the agent object. If agents write their own decommission()
        function, they must call their superclass' decommission function at the
        END of their decommission() function.
        """
        (<CyclusFacilityShim*> (<_Agent> self).shim).Decommission()

    @property
    def annotations(self):
        """Agent annotations."""
        cdef jsoncpp.Value cpp_rtn = jsoncpp.Value()
        if self._annotations is None:
            cpp_rtn._inst[0] = (<CyclusFacilityShim*> (<_Agent> self).shim).annotations()
        self._annotations = cpp_rtn
        return self._annotations

    @property
    def prototype(self):
        """The agent's prototype."""
        rtn = std_string_to_py((<CyclusFacilityShim*> (<_Agent> self).shim).get_prototype())
        return rtn

    @prototype.setter
    def prototype(self, str p):
        cdef std_string cpp_p = str_py_to_cpp(p)
        (<CyclusFacilityShim*> (<_Agent> self).shim).prototype(cpp_p)

    @property
    def spec(self):
        """The agent's spec."""
        rtn = std_string_to_py((<CyclusFacilityShim*> (<_Agent> self).shim).get_spec())
        return rtn

    @spec.setter
    def spec(self, str new_impl):
        cdef std_string cpp_new_impl = str_py_to_cpp(new_impl)
        (<CyclusFacilityShim*> (<_Agent> self).shim).spec(cpp_new_impl)

    @property
    def id(self):
        """The agent instance's unique ID within a simulation."""
        return (<CyclusFacilityShim*> (<_Agent> self).shim).id()

    @property
    def id(self):
        """The agent instance's unique ID within a simulation."""
        return (<CyclusFacilityShim*> (<_Agent> self).shim).id()

    @property
    def kind(self):
        """Returns a string that describes the agent subclass (e.g. Region,
        Facility, etc.)
        """
        rtn = std_string_to_py((<CyclusFacilityShim*> (<_Agent> self).shim).kind())
        return rtn

    def __str__(self):
        rtn = std_string_to_py((<CyclusFacilityShim*> (<_Agent> self).shim).str())
        return rtn

    def parent(self):
        """Returns parent of this agent.  Returns None if the agent has no parent.
        """
        rtn = lib.agent_to_py((<CyclusFacilityShim*> (<_Agent> self).shim).parent(), None)
        return rtn

    @property
    def parent_id(self):
        """The id for this agent's parent or -1 if this agent has no parent."""
        return (<CyclusFacilityShim*> (<_Agent> self).shim).parent_id()

    @property
    def enter_time(self):
        """The time step at which this agent's Build function was called
        (-1 if the agent has never been built).
        """
        return (<CyclusFacilityShim*> (<_Agent> self).shim).enter_time()

    @property
    def lifetime(self):
        """The number of time steps this agent operates between building and
        decommissioning (-1 if the agent has an infinite lifetime)
        """
        return (<CyclusFacilityShim*> (<_Agent> self).shim).get_lifetime()

    @lifetime.setter
    def lifetime(self, int n_timesteps):
        (<CyclusFacilityShim*> (<_Agent> self).shim).lifetime(n_timesteps)

    def lifetime_force(self, int n_timesteps):
        (<CyclusFacilityShim*> (<_Agent> self).shim).lifetime_force(n_timesteps)

    @property
    def exit_time(self):
        """The default time step at which this agent will exit the
        simulation (-1 if the agent has an infinite lifetime).

        Decommissioning happens at the end of a time step. With a lifetime of 1, we
        expect an agent to go through only 1 entire time step. In this case, the
        agent should be decommissioned on the same time step it was
        created. Therefore, for agents with non-infinite lifetimes, the exit_time
        will be the enter time plus its lifetime less 1.
        """
        return (<CyclusFacilityShim*> (<_Agent> self).shim).exit_time()

    @property
    def children(self):
        """A frozen set of the children of this agent."""
        kids = []
        for kid_ptx in (<CyclusFacilityShim*> (<_Agent> self).shim).children():
            kid = lib.agent_to_py(kid_ptx, None)
            kids.append(kid)
        return frozenset(kids)


class Facility(_Facility):
    """Python Facility that is subclassable into a facility archetype.

    Parameters
    ----------
    ctx : cyclus.lib.Context
        The simulation execution context.  You don't normally
        need to call the initilaizer.
    """
    entity = 'facility'

    def tick(self):
        """This function is called each time step and is meant to be
        overlaoded in the subclass.
        """
        pass

    def tock(self):
        """This function is called each time step and is meant to be
        overlaoded in the subclass.
        """
        pass

    def decision(self):
        """This function is called each time step and is meant to be
        overlaoded in the subclass.
        """
        pass

    def check_decommission_condition(self):
        """facilities over write this method if a condition must be met
        before their destructors can be called. Return True means that
        the facility is ready for decommission. False prevents decommission.
        """
        return True

    def get_material_requests(self):
        """Returns material requests for this agent on this time step.
        This is meant to be overridden is subclasses.
        """
        return []

    def get_product_requests(self):
        """Returns product requests for this agent on this time step.
        This is meant to be overridden is subclasses.
        """
        return []

    def get_material_bids(self, requests):
        """Returns material bids for this agent on this time step.
        This may be overridden is subclasses.
        """
        return []

    def get_product_bids(self, requests):
        """Returns product bids for this agent on this time step.
        This may be overridden is subclasses.
        """
        return []

    def get_material_trades(self, trades):
        """Implementation for responding to material trades.
        """
        return None

    def get_product_trades(self, trades):
        """Implementation for responding to product trades.
        """
        return None

    def accept_material_trades(self, responses):
        """Method for accepting materials from a trade deal."""
        pass

    def accept_product_trades(self, responses):
        """Method for accepting products from a trade deal."""
        pass

#
# Tools
#
cpdef int _by_index(object x):
    """key function for sorting by index."""
    return x[1].index


cdef tuple index_and_sort_vars(dict vars):
    cdef int nvars = len(vars)
    cdef list svs = list(vars.items())
    svs.sort(key=_by_index)
    cdef int i
    for i in range(nvars):
        svs[i][1].index = i
    rtn = tuple(svs)
    return rtn


cdef cpp_cyclus.Agent* dynamic_agent_ptr(object a) except *:
    """Dynamically casts an agent instance to the correct agent pointer"""
    if a is None:
        return NULL
    elif isinstance(a, Region):
        return dynamic_cast[agent_ptr](
            reinterpret_cast[region_shim_ptr]((<_Agent> a).shim))
    elif isinstance(a, Institution):
        return dynamic_cast[agent_ptr](
            reinterpret_cast[institution_shim_ptr]((<_Agent> a).shim))
    elif isinstance(a, Facility):
        return dynamic_cast[agent_ptr](
            reinterpret_cast[facility_shim_ptr]((<_Agent> a).shim))
    elif a.kind == "Region":
        return dynamic_cast[agent_ptr](
            reinterpret_cast[region_ptr]((<lib._Agent> a).ptx))
    elif a.kind == "Inst":
        return dynamic_cast[agent_ptr](
            reinterpret_cast[institution_ptr]((<lib._Agent> a).ptx))
    elif a.kind == "Facility":
        return dynamic_cast[agent_ptr](
            reinterpret_cast[facility_ptr]((<lib._Agent> a).ptx))
    elif isinstance(a, Agent):
        return dynamic_cast[agent_ptr]((<_Agent> a).shim)
    else:
        return dynamic_cast[agent_ptr](
            reinterpret_cast[agent_ptr]((<lib._Agent> a).ptx))
