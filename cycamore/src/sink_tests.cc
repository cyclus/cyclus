#include <gtest/gtest.h>

#include "facility_tests.h"
#include "agent_tests.h"
#include "resource_helpers.h"
#include "infile_tree.h"
#include "xml_parser.h"

#include "sink_tests.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkTest::SetUp() {
  src_facility = new cycamore::Sink(tc_.get());
  trader = tc_.trader();
  InitParameters();
  SetUpSink();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkTest::TearDown() {
  delete src_facility;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkTest::InitParameters() {
  commod1_ = "acommod";
  commod2_ = "bcommod";
  commod3_ = "ccommod";
  capacity_ = 5;
  inv_ = capacity_ * 2;
  qty_ = capacity_ * 0.5;
  ncommods_ = 2;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkTest::SetUpSink() {
  src_facility->AddCommodity(commod1_);
  src_facility->AddCommodity(commod2_);
  src_facility->Capacity(capacity_);
  src_facility->SetMaxInventorySize(inv_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkTest, InitialState) {
  EXPECT_DOUBLE_EQ(0.0, src_facility->InventorySize());
  EXPECT_DOUBLE_EQ(capacity_, src_facility->Capacity());
  EXPECT_DOUBLE_EQ(inv_, src_facility->MaxInventorySize());
  EXPECT_DOUBLE_EQ(capacity_, src_facility->RequestAmt());
  EXPECT_DOUBLE_EQ(0.0, src_facility->InventorySize());
  std::string arr[] = {commod1_, commod2_};
  std::vector<std::string> vexp (arr, arr + sizeof(arr) / sizeof(arr[0]) );
  EXPECT_EQ(vexp, src_facility->input_commodities());

  src_facility->EnterNotify();
  double pref[] = {cyclus::kDefaultPref, cyclus::kDefaultPref};
  std::vector<double> vpref (pref, pref + sizeof(pref) / sizeof(pref[0]) );
  EXPECT_EQ(vpref, src_facility->input_commodity_preferences());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkTest, Clone) {
  using cycamore::Sink;
  Sink* cloned_fac = dynamic_cast<cycamore::Sink*>
                             (src_facility->Clone());

  EXPECT_DOUBLE_EQ(0.0, cloned_fac->InventorySize());
  EXPECT_DOUBLE_EQ(capacity_, cloned_fac->Capacity());
  EXPECT_DOUBLE_EQ(inv_, cloned_fac->MaxInventorySize());
  EXPECT_DOUBLE_EQ(capacity_, cloned_fac->RequestAmt());
  std::string arr[] = {commod1_, commod2_};
  std::vector<std::string> vexp (arr, arr + sizeof(arr) / sizeof(arr[0]) );
  EXPECT_EQ(vexp, cloned_fac->input_commodities());

  delete cloned_fac;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkTest, DISABLED_XMLInit) {
  std::stringstream ss;
  ss << "<start>"
     << "<name>fooname</name>"
     << "<config>"
     << "<UNSPECIFIED>"
     << "<input>"
     << "  <commodities>"
     << "  <incommodity>" << commod1_ << "</incommodity>"
     << "  <incommodity>" << commod2_ << "</incommodity>"
     << "  </commodities>"
     << "  <input_capacity>" << capacity_ << "</input_capacity>"
     << "  <inventorysize>" << inv_ << "</inventorysize>"
     << "</input>"
     << "</UNSPECIFIED>"
     << "</config>"
     << "</start>";

  cyclus::XMLParser p;
  p.Init(ss);
  cyclus::InfileTree engine(p);
  cycamore::Sink fac(tc_.get());

  // EXPECT_NO_THROW(fac.InitFrom(&engine););
  std::string arr[] = {commod1_, commod2_};
  std::vector<std::string> vexp (arr, arr + sizeof(arr) / sizeof(arr[0]) );
  EXPECT_EQ(vexp, fac.input_commodities());
  EXPECT_DOUBLE_EQ(capacity_, fac.Capacity());
  EXPECT_DOUBLE_EQ(inv_, fac.MaxInventorySize());
  EXPECT_DOUBLE_EQ(capacity_, fac.RequestAmt());
  EXPECT_DOUBLE_EQ(0.0, fac.InventorySize());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkTest, Requests) {
  using cyclus::Request;
  using cyclus::RequestPortfolio;
  using cyclus::CapacityConstraint;
  using cyclus::Material;

  std::string arr[] = {commod1_, commod2_};
  std::vector<std::string> commods (arr, arr + sizeof(arr) / sizeof(arr[0]) );

  src_facility->EnterNotify();
  std::set<RequestPortfolio<Material>::Ptr> ports =
      src_facility->GetMatlRequests();

  ASSERT_EQ(ports.size(), 1);
  ASSERT_EQ(ports.begin()->get()->qty(), capacity_);
  const std::vector<Request<Material>*>& requests =
      ports.begin()->get()->requests();
  ASSERT_EQ(requests.size(), 2);

  for (int i = 0; i < ncommods_; ++i) {
    Request<Material>* req = *(requests.begin() + i);
    EXPECT_EQ(req->requester(), src_facility);
    EXPECT_EQ(req->commodity(), commods[i]);
  }

  const std::set< CapacityConstraint<Material> >& constraints =
      ports.begin()->get()->constraints();
  EXPECT_EQ(constraints.size(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkTest, EmptyRequests) {
  using cyclus::Material;
  using cyclus::RequestPortfolio;

  src_facility->Capacity(0);
  std::set<RequestPortfolio<Material>::Ptr> ports =
      src_facility->GetMatlRequests();
  EXPECT_TRUE(ports.empty());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkTest, Accept) {
  using cyclus::Bid;
  using cyclus::Material;
  using cyclus::Request;
  using cyclus::Trade;
  using test_helpers::get_mat;

  double qty = qty_ * 2;
  std::vector< std::pair<cyclus::Trade<cyclus::Material>,
                         cyclus::Material::Ptr> > responses;

  Request<Material>* req1 =
      Request<Material>::Create(get_mat(922350000, qty_), src_facility,
                                commod1_);
  Bid<Material>* bid1 = Bid<Material>::Create(req1, get_mat(), trader);

  Request<Material>* req2 =
      Request<Material>::Create(get_mat(922350000, qty_), src_facility,
                                commod2_);
  Bid<Material>* bid2 =
      Bid<Material>::Create(req2, get_mat(922350000, qty_), trader);

  Trade<Material> trade1(req1, bid1, qty_);
  responses.push_back(std::make_pair(trade1, get_mat(922350000, qty_)));
  Trade<Material> trade2(req2, bid2, qty_);
  responses.push_back(std::make_pair(trade2, get_mat(922350000, qty_)));

  EXPECT_DOUBLE_EQ(0.0, src_facility->InventorySize());
  src_facility->AcceptMatlTrades(responses);
  EXPECT_DOUBLE_EQ(qty, src_facility->InventorySize());
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkTest, InRecipe){
// Create a context
  using cyclus::RequestPortfolio;
  using cyclus::Material;
  using cyclus::Request;
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);

  // define some test material in the context
  cyclus::CompMap m;
  m[922350000] = 1;
  m[922580000] = 2;

  cyclus::Composition::Ptr c = cyclus::Composition::CreateFromMass(m);
  ctx.AddRecipe("some_u",c) ;

  // create a sink facility to interact with the DRE
  cycamore::Sink* snk = new cycamore::Sink(&ctx);
  snk->AddCommodity("some_u");
  snk->EnterNotify();

  std::set<RequestPortfolio<Material>::Ptr> ports =
    snk->GetMatlRequests();
  ASSERT_EQ(ports.size(), 1);

  const std::vector<Request<Material>*>& requests =
    ports.begin()->get()->requests();
  ASSERT_EQ(requests.size(), 1);

  Request<Material>* req = *requests.begin();
  EXPECT_EQ(req->requester(), snk);
  EXPECT_EQ(req->commodity(),"some_u");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkTest, BidPrefs) {
  using cyclus::QueryResult;
  using cyclus::Cond;

  std::string config =
    "   <in_commods>"
    "     <val>commods_1</val>"
    "     <val>commods_2</val>"
    "   </in_commods>"
    "   <in_commod_prefs>"
    "     <val>10</val> "
    "     <val>1</val> "
    "   </in_commod_prefs>"
    "   <capacity>1</capacity>"
    "   <input_capacity>1.0</input_capacity> ";

  int simdur = 1;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Sink"), config, simdur);

  sim.AddSource("commods_1")
    .capacity(1)
    .Finalize();

  sim.AddSource("commods_2")
    .capacity(1)
    .Finalize();

  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("Commodity", "==", std::string("commods_1")));
  QueryResult qr = sim.db().Query("Transactions", &conds);

  // should trade only with #1 since it has highier priority
  EXPECT_EQ(1, qr.rows.size());

  std::vector<Cond> conds2;
  conds2.push_back(Cond("Commodity", "==", std::string("commods_2")));
  QueryResult qr2 = sim.db().Query("Transactions", &conds2);

  // should trade only with #1 since it has highier priority
  EXPECT_EQ(0, qr2.rows.size());

}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility->str());
}

TEST_F(SinkTest, PositionInitialize) {
  using cyclus::QueryResult;
  using cyclus::Cond;

  std::string config =
    "   <in_commods>"
    "     <val>commods_1</val>"
    "     <val>commods_2</val>"
    "   </in_commods>"
    "   <in_commod_prefs>"
    "     <val>10</val> "
    "     <val>1</val> "
    "   </in_commod_prefs>"
    "   <capacity>1</capacity>"
    "   <input_capacity>1.0</input_capacity> ";

  int simdur = 1;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Sink"), config, simdur);

  sim.AddSource("commods_1")
    .capacity(1)
    .Finalize();

  sim.AddSource("commods_2")
    .capacity(1)
    .Finalize();

  int id = sim.Run();

  QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 0.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 0.0);
}

TEST_F(SinkTest, PositionInitialize2) {
  using cyclus::QueryResult;
  using cyclus::Cond;

  std::string config =
    "   <in_commods>"
    "     <val>commods_1</val>"
    "     <val>commods_2</val>"
    "   </in_commods>"
    "   <in_commod_prefs>"
    "     <val>10</val> "
    "     <val>1</val> "
    "   </in_commod_prefs>"
    "   <capacity>1</capacity>"
    "   <input_capacity>1.0</input_capacity> "
    "   <latitude>50.0</latitude> "
    "   <longitude>35.0</longitude> ";

  int simdur = 1;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Sink"), config, simdur);

  sim.AddSource("commods_1")
    .capacity(1)
    .Finalize();

  sim.AddSource("commods_2")
    .capacity(1)
    .Finalize();

  int id = sim.Run();

  QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 35.0);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 50.0);

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* SinkConstructor(cyclus::Context* ctx) {
  return new cycamore::Sink(ctx);
}

// required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(SinkFac, FacilityTests, Values(&SinkConstructor));
INSTANTIATE_TEST_CASE_P(SinkFac, AgentTests, Values(&SinkConstructor));
