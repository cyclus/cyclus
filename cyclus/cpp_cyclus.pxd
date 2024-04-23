"""C++ header wrapper for specific parts of cyclus."""
from libc.stdint cimport uint64_t
from libcpp.map cimport map
from libcpp.set cimport set
from libcpp.vector cimport vector
from libcpp.list cimport list
from libcpp.utility cimport pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool
from libcpp.typeinfo cimport type_info

# we use boost shared_ptrs
#from libcpp.memory cimport shared_ptr

from . cimport cpp_jsoncpp
from .cpp_typesystem cimport DbTypes
from .cpp_stringstream cimport stringstream


cdef extern from "<functional>" namespace "std":

    cdef cppclass function[T]:
        function()
        function(T)

cdef extern from "cyclus.h" namespace "boost::spirit":

    cdef cppclass hold_any:
        hold_any() except +
        hold_any(const char*) except +
        hold_any assign[T](T) except +
        T cast[T]() except +
        const type_info& type() except +

cdef extern from "cyclus.h" namespace "boost::uuids":

    cdef cppclass uuid:
        unsigned char data[16]


cdef extern from "cyclus.h" namespace "boost":

    cdef cppclass shared_ptr[T]:
        shared_ptr()
        shared_ptr(T*)
        T* get()
        T& operator*()
        cpp_bool unique()
        long use_count()
        swap(shared_ptr&)

    shared_ptr[T] reinterpret_pointer_cast[T,U](shared_ptr[U])

cdef extern from "version.h" namespace "cyclus::version":

    const char* describe() except +
    const char* core() except +
    const char* boost() except +
    const char* sqlite3() except +
    const char* hdf5() except +
    const char* xml2() except +
    const char* xmlpp() except +
    const char* coincbc() except +
    const char* coinclp() except +


cdef extern from "cyc_limits.h" namespace "cyclus":

    cdef double eps()
    cdef double eps_rsrc()


cdef extern from "cyclus.h" namespace "cyclus":

    cdef cppclass Datum:
        ctypedef pair[const char*, hold_any] Entry
        ctypedef vector[Entry] Vals
        ctypedef vector[int] Shape
        ctypedef vector[Shape] Shapes
        ctypedef vector[std_string] Fields

        Datum* AddVal(const char*, hold_any) except +
        Datum* AddVal(const char*, hold_any, vector[int]*) except +
        Datum* AddVal(std_string, hold_any) except +
        Datum* AddVal(std_string, hold_any, vector[int]*) except +
        void Record() except +
        std_string title() except +
        vector[Entry] vals() except +
        vector[vector[int]] shapes() except +
        vector[std_string] fields() except +


cdef extern from "rec_backend.h" namespace "cyclus":

    ctypedef vector[Datum*] DatumList

    cdef cppclass RecBackend:
        void Notify(DatumList) except +
        std_string Name() except +
        void Flush() except +
        void Close() except +


cdef extern from "cyclus.h" namespace "cyclus":

    ctypedef vector[hold_any] QueryRow

    cdef enum CmpOpCode:
        LT
        GT
        LE
        GE
        EQ
        NE

    cdef cppclass Blob:
        Blob() except +
        Blob(std_string) except +
        const std_string str() except +

    cdef cppclass Cond:
        Cond() except +
        Cond(std_string, std_string, hold_any) except +

        std_string field
        std_string op
        CmpOpCode opcode
        hold_any val

    cdef cppclass QueryResult:
        QueryResult() except +

        void Reset() except +
        T GetVal[T](std_string) except +
        T GetVal[T](std_string, int) except +

        vector[std_string] fields
        vector[DbTypes] types
        vector[QueryRow] rows

    cdef cppclass ColumnInfo:
        ColumnInfo()
        ColumnInfo(std_string, std_string, int, DbTypes, vector[int])
        std_string table
        std_string col
        int index
        DbTypes dbtype
        vector[int] shape

    cdef cppclass QueryableBackend:
        QueryResult Query(std_string, vector[Cond]*) except +
        map[std_string, DbTypes] ColumnTypes(std_string) except +
        list[ColumnInfo] Schema(std_string)
        set[std_string] Tables() except +

    cdef cppclass FullBackend(QueryableBackend, RecBackend):
        FullBackend() except +

    cdef cppclass Recorder:
        Recorder() except +
        Recorder(cpp_bool) except +

        unsigned int dump_count() except +
        void set_dump_count(unsigned int) except +
        cpp_bool inject_sim_id() except +
        void inject_sim_id(cpp_bool) except +
        uuid sim_id() except +
        Datum* NewDatum(std_string)
        void RegisterBackend(RecBackend*) except +
        void Flush() except +
        void Close() except +


cdef extern from "sqlite_back.h" namespace "cyclus":

    cdef cppclass SqliteBack(FullBackend):
        SqliteBack(std_string) except +


cdef extern from "hdf5_back.h" namespace "cyclus":

    cdef cppclass Hdf5Back(FullBackend):
        Hdf5Back(std_string) except +


cdef extern from "dynamic_module.h" namespace "cyclus":

    cdef cppclass AgentSpec:
        AgentSpec() except +
        #AgentSpec(InfileTree* t);
        AgentSpec(std_string, std_string, std_string, std_string) except +
        AgentSpec(std_string) except +
        std_string Sanitize() except +
        std_string LibPath() except +
        std_string str() except +
        std_string path() except +
        std_string lib() except +
        std_string agent() except +
        std_string alias() except +

cdef extern from "env.h" namespace "cyclus":

    cdef cppclass Env:
        @staticmethod
        std_string PathBase(std_string) except +
        @staticmethod
        const std_string GetInstallPath() except +
        @staticmethod
        const std_string GetBuildPath() except +
        @staticmethod
        std_string GetEnv(std_string) except +
        @staticmethod
        const std_string nuc_data() except +
        @staticmethod
        const std_string rng_schema() except +
        @staticmethod
        const std_string rng_schema(cpp_bool) except +
        @staticmethod
        const vector[std_string] cyclus_path() except +
        @staticmethod
        const cpp_bool allow_milps() except +
        @staticmethod
        const std_string EnvDelimiter() except +
        @staticmethod
        const std_string PathDelimiter() except +
        @staticmethod
        const void SetNucDataPath() except +
        @staticmethod
        const void SetNucDataPath(std_string) except +
        @staticmethod
        std_string FindModule(std_string) except +

cdef extern from "logger.h" namespace "cyclus":

    cdef enum LogLevel:
        LEV_ERROR
        LEV_WARN
        LEV_INFO1
        LEV_INFO2
        LEV_INFO3
        LEV_INFO4
        LEV_INFO5
        LEV_DEBUG1
        LEV_DEBUG2
        LEV_DEBUG3
        LEV_DEBUG4
        LEV_DEBUG5

    cdef cppclass Logger:
        Logger() except +
        @staticmethod
        LogLevel& ReportLevel() except +
        @staticmethod
        void SetReportLevel(LogLevel) except +
        @staticmethod
        cpp_bool& NoAgent() except +
        @staticmethod
        void SetNoAgent(cpp_bool) except +
        @staticmethod
        cpp_bool& NoMem() except +
        @staticmethod
        void SetNoMem(cpp_bool) except +
        @staticmethod
        LogLevel ToLogLevel(std_string) except +
        @staticmethod
        std_string ToString(LogLevel) except +


cdef extern from "error.h" namespace "cyclus":

    cdef unsigned int warn_limit
    cdef cpp_bool warn_as_error


cdef extern from "pyhooks.h" namespace "cyclus":

   cdef void PyAppendInitTab() except +
   cdef void PyImportInit() except +


cdef extern from "pyhooks.h" namespace "cyclus::toolkit":

    cdef std_string PyToJson(std_string) except +
    cdef std_string JsonToPy(std_string) except +


cdef extern from "xml_file_loader.h" namespace "cyclus":

    cdef void LoadStringstreamFromFile(stringstream&, std_string)
    cdef void LoadStringstreamFromFile(stringstream&, std_string, std_string)
    cdef std_string LoadStringFromFile(std_string)
    cdef std_string LoadStringFromFile(std_string, std_string)

    cdef cppclass XMLFileLoader:
        XMLFileLoader(Recorder*, QueryableBackend*, std_string)
        XMLFileLoader(Recorder*, QueryableBackend*, std_string,
                      const std_string)
        XMLFileLoader(Recorder*, QueryableBackend*, std_string,
                      const std_string, const std_string, cpp_bool ms_print)
        void LoadSim() except +


cdef extern from "xml_flat_loader.h" namespace "cyclus":

    cdef cppclass XMLFlatLoader(XMLFileLoader):
        XMLFlatLoader(Recorder*, QueryableBackend*, std_string)
        XMLFlatLoader(Recorder*, QueryableBackend*, std_string,
                      const std_string)
        XMLFlatLoader(Recorder*, QueryableBackend*, std_string,
                      const std_string, const std_string, cpp_bool ms_print)


cdef extern from "xml_parser.h" namespace "cyclus":

    cdef cppclass XMLParser:
        XMLParser() except +
        void Init(const stringstream) except +
        void Init(const std_string) except +


cdef extern from "infile_tree.h" namespace "cyclus":

    cdef cppclass InfileTree:
        InfileTree(XMLParser&)
        int NMatches(std_string)
        InfileTree* SubTree(std_string)
        InfileTree* SubTree(std_string, int)

    T Query[T](InfileTree*, std_string)
    T Query[T](InfileTree*, std_string, int)
    T OptionalQuery[T](InfileTree*, std_string, T)


cdef extern from "timer.h" namespace "cyclus":

    cdef cppclass Timer:
        Timer() except +
        void RunSim() except +


cdef extern from "sim_init.h" namespace "cyclus":

    cdef cppclass SimInit:
        SimInit() except +
        void Init(Recorder*, QueryableBackend*) except +
        Timer* timer() except +
        Context* context() except +


cdef extern from "toolkit/infile_converters.h" namespace "cyclus::toolkit":

    cdef std_string JsonToXml(std_string) except +
    cdef std_string XmlToJson(std_string) except +
    cdef std_string PyToXml(std_string) except +
    cdef std_string XmlToPy(std_string) except +


cdef extern from "db_init.h" namespace "cyclus":

    cdef cppclass Agent

    cdef cppclass DbInit:
        DbInit(Agent*) except +
        DbInit(Agent*, cpp_bool) except +
        Datum* NewDatum(std_string) except +


cdef extern from "resource.h" namespace "cyclus":

    ctypedef std_string ResourceType

    cdef cppclass Resource:
        ctypedef shared_ptr[Resource] Ptr
        Resource()
        const int obj_id()
        const int state_id()
        void BumpStateId()
        int qual_id()
        const ResourceType type()
        shared_ptr[Resource] Clone()
        void Record(Context*)
        std_string units()
        double quantity()
        shared_ptr[Resource] ExtractRes(double)


cdef extern from "composition.h" namespace "cyclus":

    ctypedef int Nuc
    ctypedef map[Nuc, double] CompMap

    cdef cppclass Composition:
        ctypedef shared_ptr[Composition] Ptr
        @staticmethod
        shared_ptr[Composition] CreateFromAtom(CompMap)
        @staticmethod
        shared_ptr[Composition] CreateFromMass(CompMap)
        int id()
        CompMap& atom()
        CompMap& mass()
        shared_ptr[Composition] Decay(int)
        shared_ptr[Composition] Decay(int, uint64_t)
        void Record(Context*)


cdef extern from "material.h" namespace "cyclus":

    cdef cppclass Agent

    cdef cppclass Material(Resource):
        ctypedef shared_ptr[Material] Ptr
        const ResourceType kType
        @staticmethod
        shared_ptr[Material] Create(Agent*, double, Composition.Ptr)
        @staticmethod
        shared_ptr[Material] CreateUntracked(double, Composition.Ptr)
        shared_ptr[Material] ExtractQty(double)
        shared_ptr[Material] ExtractComp(double, Composition.Ptr)
        shared_ptr[Material] ExtractComp(double, Composition.Ptr, double)
        void Absorb(shared_ptr[Material])
        void Transmute(Composition.Ptr)
        void Decay(int)
        int prev_decay_time()
        double DecayHeat()
        Composition.Ptr comp()


cdef extern from "product.h" namespace "cyclus":

    cdef cppclass Agent

    cdef cppclass Product(Resource):
        ctypedef shared_ptr[Product] Ptr
        const ResourceType kType
        @staticmethod
        shared_ptr[Product] Create(Agent*, double, std_string)
        @staticmethod
        shared_ptr[Product] CreateUntracked(double, std_string)
        const std_string& quality()
        shared_ptr[Product] Extract(double)
        void Absorb(shared_ptr[Product])


cdef extern from "request.h" namespace "cyclus":

    cdef cppclass Trader
    cdef cppclass RequestPortfolio[T]

    cdef cppclass Request[T]:
        ctypedef function[double(shared_ptr[T])] cost_function_t
        @staticmethod
        Request[T]* Create(shared_ptr[T], Trader*, shared_ptr[RequestPortfolio[T]],
                           std_string, double, cpp_bool)
        @staticmethod
        Request[T]* Create(shared_ptr[T], Trader*, shared_ptr[RequestPortfolio[T]],
                           std_string, double, cpp_bool, cost_function_t)
        @staticmethod
        Request[T]* Create(shared_ptr[T], Trader*, std_string, double, cpp_bool)
        @staticmethod
        Request[T]* Create(shared_ptr[T], Trader*, std_string, double, bool,
                           cost_function_t)
        shared_ptr[T] target()
        Trader* requester()
        std_string commodity()
        double preference()
        shared_ptr[RequestPortfolio[T]] portfolio()
        cpp_bool exclusive()
        cost_function_t cost_function()


cdef extern from "exchange_graph.h" namespace "cyclus":

    cdef cppclass Arc
    cdef cppclass ExchangeNodeGroup

    cdef cppclass ExchangeNode:
        ctypedef shared_ptr[ExchangeNode] Ptr
        ExchangeNode()
        ExchangeNode(double)
        ExchangeNode(double, cpp_bool)
        ExchangeNode(double, cpp_bool, std_string)
        ExchangeNode(double, cpp_bool, std_string, int)
        ExchangeNodeGroup* group
        map[Arc, vector[double]] unit_capacities
        map[Arc, double] prefs
        cpp_bool exclusives
        std_string commod
        int agent_id
        double qty

    cdef cppclass Arc:
        Arc()
        Arc(shared_ptr[ExchangeNode], shared_ptr[ExchangeNode])
        Arc(const Arc&)
        Arc& operator=(const Arc&)
        cpp_bool operator<(const Arc&)
        cpp_bool operator==(const Arc&)
        ExchangeNode.Ptr unode()
        ExchangeNode.Ptr vnode()
        cpp_bool exclusive()
        double excl_val()
        double pref()
        void pref(double)

    cpp_bool operator==(const ExchangeNode&, const ExchangeNode&)

    cdef cppclass ExchangeNodeGroup:
        ctypedef shared_ptr[ExchangeNodeGroup] Ptr
        vector[ExchangeNode.Ptr]& nodes()
        vector[vector[ExchangeNode.Ptr]]& excl_node_groups()
        vector[double]& capacities()
        void AddExchangeNode(ExchangeNode.Ptr)
        void AddExclGroup(vector[ExchangeNode.Ptr]&)
        cpp_bool HasArcs()
        void AddExclNode(ExchangeNode.Ptr)
        void AddCapacity(double)


cdef extern from "exchange_translation_context.h" namespace "cyclus":

    cdef cppclass ExchangeTranslationContext[T]:
        ctypedef Request[T]* request_ptr
        ctypedef Bid[T]* bid_ptr
        map[request_ptr, ExchangeNode.Ptr] request_to_node
        map[ExchangeNode.Ptr, request_ptr] node_to_request
        map[bid_ptr, ExchangeNode.Ptr] bid_to_node
        map[ExchangeNode.Ptr, bid_ptr] node_to_bid


cdef extern from "capacity_constraint.h" namespace "cyclus":

    cdef cppclass Converter[T]:
        ctypedef shared_ptr[Converter[T]] Ptr
        double convert(shared_ptr[T])
        double convert(shared_ptr[T], const Arc*)
        double convert(shared_ptr[T], const Arc*,
                       const ExchangeTranslationContext[T]*)

    cdef cppclass TrvialConverter[T](Converter[T]):
        pass

    cdef cppclass CapacityConstraint[T]:
        CapacityConstraint(double)
        CapacityConstraint(double, Converter[T].Ptr)
        double capacity()
        Converter[T].Ptr converter()
        double convert(shared_ptr[T])
        double convert(shared_ptr[T], const Arc*)
        double convert(shared_ptr[T], const Arc*,
                       const ExchangeTranslationContext[T]*)
        int id()



cdef extern from "request_portfolio.h" namespace "cyclus":

    cdef cppclass Trader

    cdef cppclass RequestPortfolio[T]:
        ctypedef shared_ptr[RequestPortfolio[T]] Ptr
        ctypedef function[double(shared_ptr[T])] cost_function_t
        ctypedef Request[T]* request_ptr
        RequestPortfolio()
        Request[T]* AddRequest(shared_ptr[T], Trader*, std_string, double, cpp_bool)
        Request[T]* AddRequest(shared_ptr[T], Trader*, std_string, double,
                               cpp_bool, cost_function_t)
        void AddMutualReqs(const vector[request_ptr]&)
        void AddConstraint(const CapacityConstraint[T]&)
        Trader* requester()
        double qty()
        vector[request_ptr]& requests()
        set[CapacityConstraint[T]]& constraints()
        Converter[T].Ptr qty_converter()


cdef extern from "bid.h" namespace "cyclus":

    cdef cppclass Trader
    cdef cppclass BidPortfolio[T]

    cdef cppclass Bid[T]:
        @staticmethod
        Bid[T]* Create(Request[T], shared_ptr[T], Trader*, cpp_bool)
        @staticmethod
        Bid[T]* Create(Request[T], shared_ptr[T], Trader*, cpp_bool, double)
        @staticmethod
        Bid[T]* Create(Request[T], shared_ptr[T], Trader*,
                       shared_ptr[BidPortfolio[T]], cpp_bool)
        @staticmethod
        Bid[T]* Create(Request[T], shared_ptr[T], Trader*,
                       shared_ptr[BidPortfolio[T]], cpp_bool, double)
        Request[T]* request()
        shared_ptr[T] offer()
        Trader* bidder()
        shared_ptr[BidPortfolio[T]] portfolio()
        cpp_bool exclusive()
        double preference()


cdef extern from "bid_portfolio.h" namespace "cyclus":

    cdef cppclass Trader

    cdef cppclass BidPortfolio[T]:
        ctypedef shared_ptr[BidPortfolio[T]] Ptr
        ctypedef Bid[T]* bid_ptr
        BidPortfolio()
        Bid[T]* AddBid(Request[T]*, shared_ptr[T], Trader*, cpp_bool)
        Bid[T]* AddBid(Request[T]*, shared_ptr[T], Trader*, cpp_bool, double)
        void AddConstraint(const CapacityConstraint[T]&)
        Trader* bidder()
        std_string commodity()
        set[bid_ptr]& bids()
        set[CapacityConstraint[T]]& constraints()


cdef extern from "exchange_context.h" namespace "cyclus":

    cdef cppclass PrefMap[T]:
        ctypedef Request[T]* request_ptr
        ctypedef Bid[T]* bid_ptr
        ctypedef map[request_ptr, map[bid_ptr, double]] type

    cdef cppclass CommodMap[T]:
        ctypedef Request[T]* request_ptr
        ctypedef map[std_string, vector[request_ptr]] type


cdef extern from "agent.h" namespace "cyclus":

    cdef cppclass Material
    cdef cppclass Product
    ctypedef map[std_string, vector[Resource.Ptr]] Inventories

    cdef cppclass Ider:
        const int id()

    cdef cppclass Agent(Ider):
        Agent(Context*)
        std_string version()
        Agent* Clone()
        void InfileToDb(InfileTree*, DbInit)
        void InitFromAgent "InitFrom" (Agent*)
        void InitFrom(QueryableBackend*)
        void Snapshot(DbInit)
        void InitInv(Inventories&)
        Inventories SnapshotInv()
        std_string PrintChildren()
        vector[std_string] GetTreePrintOuts(Agent*)
        cpp_bool InFamilyTree(Agent*)
        cpp_bool AncestorOf(Agent*)
        cpp_bool DecendentOf(Agent*)
        void Build(Agent*)
        void EnterNotify()
        void BuildNotify(Agent*)
        void DecomNotify(Agent*)
        void Decommission() except +
        void AdjustMatlPrefs(PrefMap[Material].type&)
        void AdjustProductPrefs(PrefMap[Product].type&)
        std_string schema()
        cpp_jsoncpp.Value annotations() except +
        const std_string get_prototype "prototype" ()
        void prototype(std_string)
        std_string get_spec "spec" ()
        void spec(std_string)
        const std_string kind()
        Context* context()
        std_string str()
        Agent* parent()
        const int parent_id()
        const int enter_time()
        const int get_lifetime "lifetime" ()
        void lifetime(int)
        void lifetime_force(int)
        const int exit_time()
        const set[Agent*]& children()


cdef extern from "time_listener.h" namespace "cyclus":

    cdef cppclass TimeListener(Ider):
        void Tick()
        void Tock()
        void Decision()
        cpp_bool IsShim()


cdef extern from "trade.h" namespace "cyclus":

    cdef cppclass Trade[T]:
        Trade()
        Trade(Request[T]*, Bid[T]*, double)
        Request[T]* request
        Bid[T]* bid
        double amt
        double price



cdef extern from "trader.h" namespace "cyclus":

    cdef cppclass Trader:
        Trader(Agent*)
        Agent* manager()
        set[RequestPortfolio[Material].Ptr] GetMatlRequests()
        set[RequestPortfolio[Product].Ptr] GetProductRequests()
        set[BidPortfolio[Material].Ptr] GetMatlBids(CommodMap[Material].type&)
        set[BidPortfolio[Product].Ptr] GetProductBids(CommodMap[Product].type&)
        void AdjustMatlPrefs(PrefMap[Material].type&)
        void AdjustProductPrefs(PrefMap[Product].type&)
        void GetMatlTrades(const vector[Trade[Material]]&,
                           vector[pair[Trade[Material], Material.Ptr]]&)
        void GetProductTrades(const vector[Trade[Product]]&,
                              vector[pair[Trade[Product], Product.Ptr]]&)
        void AcceptMatlTrades(const vector[pair[Trade[Material], Material.Ptr]]&)
        void AcceptProductTrades(const vector[pair[Trade[Product], Product.Ptr]]&)

cdef extern from "region.h" namespace "cyclus":

    cdef cppclass Region(Agent, TimeListener):
        Region(Context*)
        void Region_InitFromAgent "InitFrom" (Region*)
        void Region_InitFrom "InitFrom" (QueryableBackend*)
        void Region_InfileToDb "InfileToDb" (InfileTree*, DbInit)
        void Region_Build "Build" (Agent*)
        void Region_EnterNotify "EnterNotify" ()
        void Region_Tick "Tick" ()
        void Region_Tock "Tock" ()
        void Region_Decision "Decision" ()


cdef extern from "institution.h" namespace "cyclus":

    cdef cppclass Institution(Agent, TimeListener):
        Institution(Context*)
        void Institution_InitFromAgent "InitFrom" (Institution*)
        void Institution_InitFrom "InitFrom" (QueryableBackend*)
        void Institution_InfileToDb "InfileToDb" (InfileTree*, DbInit)
        void Institution_Build "Build" (Agent*)
        void Institution_EnterNotify "EnterNotify" ()
        void Institution_Tick "Tick" ()
        void Institution_Tock "Tock" ()
        void Institution_Decision "Decision" ()


cdef extern from "facility.h" namespace "cyclus":

    cdef cppclass Facility(TimeListener, Agent, Trader):
        Facility(Context*)
        void Facility_InitFromAgent "InitFrom" (Facility*)
        void Facility_InitFrom "InitFrom" (QueryableBackend*)
        void Facility_InfileToDb "InfileToDb" (InfileTree*, DbInit)
        void Facility_Build "Build" (Agent*)
        void Facility_EnterNotify "EnterNotify" ()
        void Facility_Tick "Tick" ()
        void Facility_Tock "Tock" ()
        void Facility_Decision "Decision" ()
        cpp_bool CheckDecommissionCondition() except +
        set[RequestPortfolio[Material].Ptr] GetMatlRequests()
        set[RequestPortfolio[Product].Ptr] GetProductRequests()
        set[BidPortfolio[Material].Ptr] GetMatlBids(CommodMap[Material].type&)
        set[BidPortfolio[Product].Ptr] GetProductBids(CommodMap[Product].type&)
        void AdjustMatlPrefs(PrefMap[Material].type&)
        void AdjustProductPrefs(PrefMap[Product].type&)
        void GetMatlTrades(const vector[Trade[Material]]&,
                           vector[pair[Trade[Material], Material.Ptr]]&)
        void GetProductTrades(const vector[Trade[Product]]&,
                              vector[pair[Trade[Product], Product.Ptr]]&)
        void AcceptMatlTrades(const vector[pair[Trade[Material], Material.Ptr]]&)
        void AcceptProductTrades(const vector[pair[Trade[Product], Product.Ptr]]&)


cdef extern from "dynamic_module.h" namespace "cyclus":

    cdef cppclass DynamicModule:
        DynamicModule() except +
        @staticmethod
        Agent* Make(Context*, AgentSpec) except +
        cpp_bool Exists(AgentSpec) except +
        void CloseAll() except +
        std_string path() except +


cdef extern from "context.h" namespace "cyclus":

    cdef cppclass Context:
        Context(Timer*, Recorder*) except +
        void DelAgent(Agent*) except +
        uuid sim_id() except +
        int time()
        uint64_t dt()
        const set[Trader*] traders()
        shared_ptr[Composition] GetRecipe(std_string)
        void SchedBuild(Agent*, std_string)
        void SchedBuild(Agent*, std_string, int)
        void SchedDecom(Agent*)
        void SchedDecom(Agent*, int)
        void AddRecipe(std_string, shared_ptr[Composition])
        Datum* NewDatum(std_string)
        #void RegisterAgent(Agent*)  # private
        void RegisterTrader(Trader*)
        void RegisterTimeListener(TimeListener*)


cdef extern from "discovery.h" namespace "cyclus":

    cdef set[std_string] DiscoverSpecs(std_string, std_string) except +
    cdef set[std_string] DiscoverSpecsInCyclusPath() except +
    cdef cpp_jsoncpp.Value DiscoverMetadataInCyclusPath() except +

#
# Positions
#

cdef extern from "toolkit/position.h" namespace "cyclus::toolkit":

    cdef cppclass Position:
        enum StringFormat:
            DEGREES = 1
            DEGREES_MINUTES
            DEGREES_MINUTES_SECONDS
        Position()
        Position(double, double)
        double latitude()
        double longitude()
        void latitude(double)
        void longitude(double)
        void set_position(double, double)
        double Distance(Position)
        std_string ToString()
        std_string ToString(StringFormat)


#
# Inventories and Resource Buffers
#

cdef extern from "toolkit/resource_buff.h" namespace "cyclus::toolkit":

    ctypedef vector[Resource.Ptr] Manifest

    cdef cppclass ResourceBuff:
        enum AccessDir:
            FRONT
            BACK
        ResourceBuff()
        double capacity()
        void set_capacity(double)
        int count()
        double quantity()
        double space()
        cpp_bool empty()
        Manifest PopQty(double)
        Manifest PopQty(double, double)
        Manifest PopN(int)
        Resource.Ptr Pop(AccessDir)
        shared_ptr[T] Pop[T]()
        void Push(shared_ptr[Resource])
        void PushAll[B](vector[B])

cdef extern from "toolkit/resource_buff.h" namespace "cyclus::toolkit::ResourceBuff":

        enum AccessDir:
            FRONT
            BACK

cdef extern from "toolkit/res_buf.h" namespace "cyclus::toolkit":

    ctypedef vector[Resource.Ptr] ResVec
    ctypedef vector[Material.Ptr] MatVec
    ctypedef vector[Product.Ptr] ProdVec

    cdef cppclass ResBuf[T]:
        double capacity()
        void capacity(double)
        int count()
        double quantity()
        double space()
        cpp_bool empty()
        shared_ptr[T] Pop(double)
        shared_ptr[T] Pop(double, double)
        vector[shared_ptr[T]] PopN(int)
        ResVec PopNRes(int)
        shared_ptr[T] Peek()
        shared_ptr[T] Pop()
        shared_ptr[T] PopBack()
        void Push(shared_ptr[Resource])
        # cannot overload template and non-template functions in Cython.
        # it is probably bad design on Cyclus's part anyway.
        #void Push[B](vector[B])

cdef extern from "toolkit/res_map.h" namespace "cyclus::toolkit":

    cdef cppclass ResMap[K,R]:
        ctypedef map[K, shared_ptr[R]] map_type
        ctypedef map[K, shared_ptr[R]].iterator iterator
        ctypedef map[K, shared_ptr[R]].const_iterator const_iterator

        ctypedef map[K, int] obj_type
        ctypedef map[K, int].iterator obj_iterator
        ctypedef map[K, int].const_iterator const_obj_iterator

        int size()
        double quantity()
        void obj_ids(obj_type)
        cpp_bool empty()
        shared_ptr[R]& operator[](K&)
        iterator begin()
        const_iterator begin()
        const_iterator cbegin()
        iterator end()
        const_iterator end()
        const_iterator cend()
        void erase(K&)
        void erase(iterator)
        void erase(iterator, iterator)
        void clear()
        vector[shared_ptr[R]] Values()
        vector[Resource.Ptr] ResValues()
        void Values(vector[shared_ptr[R]])
        void ResValues(vector[Resource.Ptr] vals)
        shared_ptr[R] Pop(K)

cdef extern from "toolkit/timeseries.h" namespace "cyclus::toolkit":

    cdef enum TimeSeriesType:
        POWER
        ENRICH_SWU
        ENRICH_FEED

    void RecordTimeSeries[T](std_string, Agent*, T)
    void RecordTimeSeriesPower "cyclus::toolkit::RecordTimeSeries<cyclus::toolkit::POWER>" (Agent*, double)
    void RecordTimeSeriesEnrichSWU "cyclus::toolkit::RecordTimeSeries<cyclus::toolkit::ENRICH_SWU>" (Agent*, double)
    void RecordTimeSeriesEnrichFeed "cyclus::toolkit::RecordTimeSeries<cyclus::toolkit::ENRICH_FEED>" (Agent*, double)
#
# Some cutsom pyne wrapping
#

#cdef extern from "nucname.h" namespace "pyne::nucname":
cdef extern from "pyne.h" namespace "pyne::nucname":
    # isnuclide
    bint isnuclide(int) except +
    bint isnuclide(char *) except +
    bint isnuclide(std_string) except +

    # iselement
    bint iselement(int) except +
    bint iselement(char *) except +
    bint iselement(std_string) except +

    # id functions
    int id(int) except +
    int id(char *) except +
    int id(std_string) except +

    # name Functions
    std_string name(int) except +
    std_string name(char *) except +
    std_string name(std_string) except +

    # znum functions
    int znum(int) except +
    int znum(char *) except +
    int znum(std_string) except +

    # anum functions
    int anum(int) except +
    int anum(char *) except +
    int anum(std_string) except +

    # znum functions
    int snum(int) except +
    int snum(char *) except +
    int snum(std_string) except +

    # zzaaam Functions
    int zzaaam(int) except +
    int zzaaam(char *) except +
    int zzaaam(std_string) except +
    int zzaaam_to_id(int) except +
    int zzaaam_to_id(char *) except +
    int zzaaam_to_id(std_string) except +

    # zzzaaa Functions
    int zzzaaa(int) except +
    int zzzaaa(char *) except +
    int zzzaaa(std_string) except +
    int zzzaaa_to_id(int) except +
    int zzzaaa_to_id(char *) except +
    int zzzaaa_to_id(std_string) except +

    # zzllaaam Functions
    std_string zzllaaam(int) except +
    std_string zzllaaam(char *) except +
    std_string zzllaaam(std_string) except +
    int zzllaaam_to_id(char *) except +
    int zzllaaam_to_id(std_string) except +

    # MCNP Functions
    int mcnp(int) except +
    int mcnp(char *) except +
    int mcnp(std_string) except +
    int mcnp_to_id(int) except +
    int mcnp_to_id(char *) except +
    int mcnp_to_id(std_string) except +

    # FLUKA Functions
    std_string fluka(int) except +
    int fluka_to_id(char *) except +
    int fluka_to_id(std_string) except +

    # Serpent Functions
    std_string serpent(int) except +
    std_string serpent(char *) except +
    std_string serpent(std_string) except +
    #int serpent_to_id(int) except +
    int serpent_to_id(char *) except +
    int serpent_to_id(std_string) except +

    # NIST Functions
    std_string nist(int) except +
    std_string nist(char *) except +
    std_string nist(std_string) except +
    #int nist_to_id(int) except +
    int nist_to_id(char *) except +
    int nist_to_id(std_string) except +

    # Cinder Functions
    int cinder(int) except +
    int cinder(char *) except +
    int cinder(std_string) except +
    int cinder_to_id(int) except +
    int cinder_to_id(char *) except +
    int cinder_to_id(std_string) except +

    # ALARA Functions
    std_string alara(int) except +
    std_string alara(char *) except +
    std_string alara(std_string) except +
    #int alara_to_id(int) except +
    int alara_to_id(char *) except +
    int alara_to_id(std_string) except +

    # SZA Functions
    int sza(int) except +
    int sza(char *) except +
    int sza(std_string) except +
    int sza_to_id(int) except +
    int sza_to_id(char *) except +
    int sza_to_id(std_string) except +

    # Groundstate Functions
    int groundstate(int) except +
    int groundstate(char *) except +
    int groundstate(std_string) except +

    # State id Functions
    int state_id_to_id(int state) except +
    int id_to_state_id(int nuc_id) except +

    # ENSDF id Functions
    int ensdf_to_id(char *) except +
