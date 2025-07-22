"""A Python interface for Agents and their subclasses."""
from libcpp.map cimport map as std_map
from libcpp.set cimport set as std_set
from libcpp.vector cimport vector as std_vector
from libcpp.utility cimport pair as std_pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool

from cpython cimport PyObject

from cyclus cimport cpp_jsoncpp
from cyclus cimport cpp_cyclus
from cyclus cimport lib
from cyclus.cpp_cyclus cimport shared_ptr


cdef cppclass CyclusAgentShim "CyclusAgentShim" (cpp_cyclus.Agent):  # C++CONSTRUCTORS CyclusAgentShim(cyclus::Context*)
    # Agent interface
    CyclusAgentShim(cpp_cyclus.Context*)
    std_string version() except *
    cpp_cyclus.Agent* Clone() except *
    void InfileToDb(cpp_cyclus.InfileTree*, cpp_cyclus.DbInit) except *
    void InitFromAgent "InitFrom" (CyclusAgentShim*) except *
    void InitFrom(cpp_cyclus.QueryableBackend*) except *
    void Snapshot(cpp_cyclus.DbInit) except *
    void InitInv(cpp_cyclus.Inventories&) except *
    cpp_cyclus.Inventories SnapshotInv() except *
    std_string schema() except *
    cpp_jsoncpp.Value annotations() except *
    void Build(cpp_cyclus.Agent*) except *
    void EnterNotify() except *
    void BuildNotify() except *
    void DecomNotify() except *
    void AdjustMatlPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Material].type&) except *
    void AdjustProductPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Product].type&) except *
    # Extra interface
    PyObject* self  # the Python object we are shimming


# Can't inherit from CyclusAgentShim due to diamond problem and
# no way to avoid it in Cython. Also, due to cyclus, this **must be**
# a region.
cdef cppclass CyclusRegionShim "CyclusRegionShim" (cpp_cyclus.Region):  # C++CONSTRUCTORS CyclusRegionShim(cyclus::Context*)
    # Agent interface
    CyclusRegionShim(cpp_cyclus.Context*)
    std_string version() except *
    cpp_cyclus.Agent* Clone() except *
    void InfileToDb(cpp_cyclus.InfileTree*, cpp_cyclus.DbInit) except *
    void InitFromAgent "InitFrom" (CyclusRegionShim*) except *
    void InitFrom(cpp_cyclus.QueryableBackend*) except *
    void Snapshot(cpp_cyclus.DbInit) except *
    void InitInv(cpp_cyclus.Inventories&) except *
    cpp_cyclus.Inventories SnapshotInv() except *
    std_string schema() except *
    cpp_jsoncpp.Value annotations() except *
    void Build(cpp_cyclus.Agent*) except *
    void EnterNotify() except *
    void BuildNotify() except *
    void DecomNotify() except *
    void AdjustMatlPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Material].type&) except *
    void AdjustProductPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Product].type&) except *
    void Tick() except *
    void Tock() except *
    void Decision() except *
    cpp_bool IsShim() except *
    # Extra interface
    PyObject* self  # the Python object we are shimming


# Can't inherit from CyclusAgentShim due to diamond problem and
# no way to avoid it in Cython. Also, due to cyclus, this **must be**
# a institution.
cdef cppclass CyclusInstitutionShim "CyclusInstitutionShim" (cpp_cyclus.Institution):  # C++CONSTRUCTORS CyclusInstitutionShim(cyclus::Context*)
    # Agent interface
    CyclusInstitutionShim(cpp_cyclus.Context*)
    std_string version() except *
    cpp_cyclus.Agent* Clone() except *
    void InfileToDb(cpp_cyclus.InfileTree*, cpp_cyclus.DbInit) except *
    void InitFromAgent "InitFrom" (CyclusInstitutionShim*) except *
    void InitFrom(cpp_cyclus.QueryableBackend*) except *
    void Snapshot(cpp_cyclus.DbInit) except *
    void InitInv(cpp_cyclus.Inventories&) except *
    cpp_cyclus.Inventories SnapshotInv() except *
    std_string schema() except *
    cpp_jsoncpp.Value annotations() except *
    void Build(cpp_cyclus.Agent*) except *
    void EnterNotify() except *
    void BuildNotify() except *
    void DecomNotify() except *
    void AdjustMatlPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Material].type&) except *
    void AdjustProductPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Product].type&) except *
    void Tick() except *
    void Tock() except *
    void Decision() except *
    cpp_bool IsShim() except *
    # Extra interface
    PyObject* self  # the Python object we are shimming


# Can't inherit from CyclusAgentShim due to diamond problem and
# no way to avoid it in Cython. Also, due to cyclus, this **must be**
# a facility.
cdef cppclass CyclusFacilityShim "CyclusFacilityShim" (cpp_cyclus.Facility):  # C++CONSTRUCTORS CyclusFacilityShim(cyclus::Context*)
    # Agent interface
    CyclusFacilityShim(cpp_cyclus.Context*)
    std_string version() except *
    cpp_cyclus.Agent* Clone() except *
    void InfileToDb(cpp_cyclus.InfileTree*, cpp_cyclus.DbInit) except *
    void InitFromAgent "InitFrom" (CyclusFacilityShim*) except *
    void InitFrom(cpp_cyclus.QueryableBackend*) except *
    void Snapshot(cpp_cyclus.DbInit) except *
    void InitInv(cpp_cyclus.Inventories&) except *
    cpp_cyclus.Inventories SnapshotInv() except *
    std_string schema() except *
    cpp_jsoncpp.Value annotations() except *
    void Build(cpp_cyclus.Agent*) except *
    void EnterNotify() except *
    void BuildNotify() except *
    void DecomNotify() except *
    void AdjustMatlPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Material].type&) except *
    void AdjustProductPrefs(cpp_cyclus.PrefMap[cpp_cyclus.Product].type&) except *
    void Tick() except *
    void Tock() except *
    void Decision() except *
    cpp_bool IsShim() except *
    cpp_bool CheckDecommissionCondition() except *
    std_set[shared_ptr[cpp_cyclus.RequestPortfolio[cpp_cyclus.Material]]] GetMatlRequests() except *
    std_set[shared_ptr[cpp_cyclus.RequestPortfolio[cpp_cyclus.Product]]] GetProductRequests() except *
    std_set[shared_ptr[cpp_cyclus.BidPortfolio[cpp_cyclus.Material]]] GetMatlBids(cpp_cyclus.CommodMap[cpp_cyclus.Material].type&) except *
    std_set[shared_ptr[cpp_cyclus.BidPortfolio[cpp_cyclus.Product]]] GetProductBids(cpp_cyclus.CommodMap[cpp_cyclus.Product].type&) except *
    void GetMatlTrades(const std_vector[cpp_cyclus.Trade[cpp_cyclus.Material]]&, std_vector[std_pair[cpp_cyclus.Trade[cpp_cyclus.Material], shared_ptr[cpp_cyclus.Material]]]&) except *
    void GetProductTrades(const std_vector[cpp_cyclus.Trade[cpp_cyclus.Product]]&, std_vector[std_pair[cpp_cyclus.Trade[cpp_cyclus.Product], shared_ptr[cpp_cyclus.Product]]]&) except *
    void AcceptMatlTrades(const std_vector[std_pair[cpp_cyclus.Trade[cpp_cyclus.Material], shared_ptr[cpp_cyclus.Material]]]&) except *
    void AcceptProductTrades(const std_vector[std_pair[cpp_cyclus.Trade[cpp_cyclus.Product], shared_ptr[cpp_cyclus.Product]]]&) except *
    # Extra interface
    PyObject* self  # the Python object we are shimming

#
# Wrappers
#

ctypedef cpp_cyclus.Agent* agent_ptr
ctypedef CyclusAgentShim* agent_shim_ptr

ctypedef cpp_cyclus.Trader* trader_ptr
ctypedef cpp_cyclus.TimeListener* time_listener_ptr

cdef class _Agent(lib._Agent):
    # pointer declared on full backend, but that is untyped, shim is typed
    cdef agent_shim_ptr shim


ctypedef cpp_cyclus.Region* region_ptr
ctypedef CyclusRegionShim* region_shim_ptr

cdef class _Region(_Agent):
    pass

ctypedef cpp_cyclus.Institution* institution_ptr
ctypedef CyclusInstitutionShim* institution_shim_ptr

cdef class _Institution(_Agent):
    pass


ctypedef cpp_cyclus.Facility* facility_ptr
ctypedef CyclusFacilityShim* facility_shim_ptr

cdef class _Facility(_Agent):
    pass

#
# Tools
#

cdef tuple index_and_sort_vars(dict)
cdef cpp_cyclus.Agent* dynamic_agent_ptr(object) except *
