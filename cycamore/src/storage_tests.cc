#include <gtest/gtest.h>

#include "storage_tests.h"

namespace storage {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StorageTest::SetUp() {
  src_facility_ = new Storage(tc_.get());
  InitParameters();
  SetUpStorage();
}

void StorageTest::TearDown() {
  delete src_facility_;
}

void StorageTest::InitParameters(){
  in_r1 = "in_r1";
  in_c1.push_back("in_c1");
  out_c1.push_back("out_c1");
  residence_time = 10;
  max_inv_size = 200;
  throughput = 20;
  discrete_handling = 0;

  cyclus::CompMap v;
  v[922350000] = 1;
  v[922380000] = 2;
  cyclus::Composition::Ptr recipe = cyclus::Composition::CreateFromAtom(v);
  tc_.get()->AddRecipe(in_r1, recipe);
}

void StorageTest::SetUpStorage(){
  src_facility_->in_recipe = in_r1;
  src_facility_->in_commods = in_c1;
  src_facility_->out_commods = out_c1;
  src_facility_->residence_time = residence_time;
  src_facility_->max_inv_size = max_inv_size;
  src_facility_->throughput = throughput;
  src_facility_->discrete_handling = discrete_handling;
}

void StorageTest::TestInitState(Storage* fac){
  EXPECT_EQ(residence_time, fac->residence_time);
  EXPECT_EQ(max_inv_size, fac->max_inv_size);
  EXPECT_EQ(throughput, fac->throughput);
  EXPECT_EQ(in_r1, fac->in_recipe);
}

void StorageTest::TestAddMat(Storage* fac,
    cyclus::Material::Ptr mat){
  double amt = mat->quantity();
  double before = fac->inventory.quantity();
  fac->AddMat_(mat);
  double after = fac->inventory.quantity();
  EXPECT_EQ(amt, after - before);
}

void StorageTest::TestBuffers(Storage* fac, double inv,
    double proc, double ready, double stocks){
  double t = tc_.get()->time();

  EXPECT_EQ(inv, fac->inventory.quantity());
  EXPECT_EQ(proc, fac->processing.quantity());
  EXPECT_EQ(stocks, fac->stocks.quantity());
  EXPECT_EQ(ready, fac->ready.quantity());
}

void StorageTest::TestStocks(Storage* fac, cyclus::CompMap v){

  cyclus::toolkit::ResBuf<cyclus::Material>* buffer = &fac->stocks;
  Material::Ptr final_mat = cyclus::ResCast<Material>(buffer->PopBack());
  cyclus::CompMap final_comp = final_mat->comp()->atom();
  EXPECT_EQ(final_comp,v);

}

void StorageTest::TestCurrentCap(Storage* fac, double inv){

  EXPECT_EQ(inv, fac->current_capacity());
}

void StorageTest::TestReadyTime(Storage* fac, int t){

  EXPECT_EQ(t, fac->ready_time());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StorageTest, clone) {
  Storage* cloned_fac =
      dynamic_cast<Storage*> (src_facility_->Clone());
  TestInitState(cloned_fac);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StorageTest, InitialState) {
  // Test things about the initial state of the facility here
  TestInitState(src_facility_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StorageTest, CurrentCapacity){
  TestCurrentCap(src_facility_,max_inv_size);
  max_inv_size = 1e299;
  SetUpStorage();
  TestInitState(src_facility_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StorageTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility_->str());
  // Test Storage specific aspects of the print method here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StorageTest, AddMats) {
  double cap = max_inv_size;
  cyclus::Material::Ptr mat = cyclus::NewBlankMaterial(0.5*cap);
  TestAddMat(src_facility_, mat);

  cyclus::Composition::Ptr rec = tc_.get()->GetRecipe(in_r1);
  cyclus::Material::Ptr recmat = cyclus::Material::CreateUntracked(0.5*cap, rec);
  TestAddMat(src_facility_, recmat);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StorageTest, Tick) {
  ASSERT_NO_THROW(src_facility_->Tick());
  // Test Storage specific behaviors of the Tick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StorageTest, Tock) {

  // initially, nothing in the buffers
  TestBuffers(src_facility_,0,0,0,0);

  double cap = throughput;
  cyclus::Composition::Ptr rec = tc_.get()->GetRecipe(in_r1);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(cap, rec);
  TestAddMat(src_facility_, mat);

  // affter add, the inventory has the material
  TestBuffers(src_facility_,cap,0,0,0);

  EXPECT_NO_THROW(src_facility_->Tock());

  // after tock, the processing buffer has the material
  TestBuffers(src_facility_,0,cap,0,0);

  EXPECT_EQ(0, tc_.get()->time());
  for( int i = 1; i < residence_time-1; ++i){
    tc_.get()->time(i);
    EXPECT_NO_THROW(src_facility_->Tock());
    TestBuffers(src_facility_,0,cap,0,0);
  }

  tc_.get()->time(residence_time);
  EXPECT_EQ(residence_time, tc_.get()->time());
  TestReadyTime(src_facility_,0);
  src_facility_->Tock();
  TestBuffers(src_facility_,0,0,0,cap);

  tc_.get()->time(residence_time+1);
  TestReadyTime(src_facility_,1);
  src_facility_->Tock();
  TestBuffers(src_facility_,0,0,0,cap);

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StorageTest, NoProcessTime) {
  // tests what happens when the process time is zero
  residence_time = 0;
  SetUpStorage();
  EXPECT_EQ(0, residence_time);

  double cap = throughput;
  cyclus::Composition::Ptr rec = tc_.get()->GetRecipe(in_r1);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(cap, rec);
  TestAddMat(src_facility_, mat);

  // affter add, the inventory has the material
  TestBuffers(src_facility_,cap,0,0,0);

  EXPECT_NO_THROW(src_facility_->Tock());

  // affter tock, the stocks have the material
  TestBuffers(src_facility_,0,0,0,cap);
}

TEST_F(StorageTest, NoConvert) {
// Make sure no conversion occurs
  double cap = throughput;
  cyclus::Composition::Ptr rec = tc_.get()->GetRecipe(in_r1);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(cap, rec);
  TestAddMat(src_facility_, mat);

  EXPECT_NO_THROW(src_facility_->Tock());

  tc_.get()->time(residence_time);
  TestReadyTime(src_facility_,0);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,0,0,cap);
  cyclus::CompMap in_rec;
  in_rec[922350000] = 1;
  in_rec[922380000] = 2;
  TestStocks(src_facility_,in_rec);
}

TEST_F(StorageTest, MultipleSmallBatches) {
  // Add first small batch
  double cap = throughput;
  cyclus::Composition::Ptr rec = tc_.get()->GetRecipe(in_r1);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(0.2*cap, rec);
  TestAddMat(src_facility_, mat);

  // After add, material is in inventory
  TestBuffers(src_facility_,0.2*cap,0,0,0);

  // Move first batch into processing
  src_facility_->Tock();
  TestBuffers(src_facility_,0,0.2*cap,0,0);

  // Add second small batch
  tc_.get()->time(2);
  src_facility_->Tock();
  cyclus::Material::Ptr mat1 = cyclus::Material::CreateUntracked(0.3*cap, rec);
  TestAddMat(src_facility_,mat1);
  TestBuffers(src_facility_,0.3*cap,0.2*cap,0,0);

  // Move second batch into processing
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,0.5*cap,0,0);

  // Move first batch to stocks
  tc_.get()->time(residence_time);
  TestReadyTime(src_facility_,0);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,0.3*cap,0,0.2*cap);

  tc_.get()->time(residence_time+2);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,0,0,0.5*cap);
}


TEST_F(StorageTest, ChangeCapacity) {
  // src_facility_->discrete_handling_(0);
  max_inv_size = 10000;
  // Set throughput, add first batch
  throughput = 300;
  SetUpStorage();
  double cap1 = throughput;
  cyclus::Composition::Ptr rec = tc_.get()->GetRecipe(in_r1);
  cyclus::Material::Ptr mat1 = cyclus::Material::CreateUntracked(cap1, rec);
  TestAddMat(src_facility_, mat1);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,cap1,0,0);

  // Increase throughput, add second and third batches
  tc_.get()->time(2);
  throughput = 500;
  SetUpStorage();
  double cap2 = throughput;
  cyclus::Material::Ptr mat2 = cyclus::Material::CreateUntracked(cap2,rec);
  TestAddMat(src_facility_, mat2);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,cap2+cap1,0,0);
  tc_.get()->time(3);
  cyclus::Material::Ptr mat3 = cyclus::Material::CreateUntracked(cap2,rec);
  TestAddMat(src_facility_, mat3);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,cap2*2+cap1,0,0);

  // Move first batch to stocks
  tc_.get()->time(residence_time);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,2*cap2,0,cap1);

  // Decrease throughput and move portion of second batch to stocks
  throughput = 400;
  SetUpStorage();
  tc_.get()->time(residence_time+2);
  EXPECT_EQ(400, throughput);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,cap2,100,cap1+400);

  // Continue to move second batch // and portion of third
  tc_.get()->time(residence_time+3);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,0,200,cap1+cap2+300);

  // Move remainder of third batch
  tc_.get()->time(residence_time+4);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,0,0,cap1+cap2+cap2);

}

TEST_F(StorageTest, TwoBatchSameTime) {
  // Add first small batch
  double cap = throughput;
  cyclus::Composition::Ptr rec = tc_.get()->GetRecipe(in_r1);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(0.2*cap, rec);
  TestAddMat(src_facility_, mat);

  // After add, material is in inventory
  TestBuffers(src_facility_,0.2*cap,0,0,0);
  cyclus::Material::Ptr mat1 = cyclus::Material::CreateUntracked(0.2*cap, rec);
  TestAddMat(src_facility_, mat1);
  TestBuffers(src_facility_,0.4*cap,0,0,0);

  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,0.4*cap,0,0);

  // Move material to stocks
  tc_.get()->time(residence_time);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,0,0,0.4*cap);
}

TEST_F(StorageTest,ChangeProcessTime){
  // Initialize process time variable and add first batch
  int proc_time1 = residence_time;
  double cap = throughput;
  cyclus::Composition::Ptr rec = tc_.get()->GetRecipe(in_r1);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(cap, rec);
  TestAddMat(src_facility_, mat);
  TestBuffers(src_facility_,cap,0,0,0);

  // Move material to processing
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,cap,0,0);

  // Add second batch
  cyclus::Material::Ptr mat1 = cyclus::Material::CreateUntracked(cap,rec);
  tc_.get()->time(8);
  TestAddMat(src_facility_,mat1);
  TestBuffers(src_facility_,cap,cap,0,0);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,2*cap,0,0);

  // Increase process time
  residence_time = proc_time1+5;
  SetUpStorage();
  // src_facility_->residence_time = proc_time1+5;
  EXPECT_EQ(residence_time,proc_time1+5);
  EXPECT_EQ(residence_time,15);
  int proc_time2 = residence_time;

  // Make sure material doesn't move before new process time
  for( int i=proc_time1; i < proc_time2 - 1; ++i){
  tc_.get()->time(i);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,2*cap,0,0);
  }

  // Move first batch to stocks
  tc_.get()->time(proc_time2);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,cap,0,cap);

  // Decrease process time
  residence_time = proc_time2-3;
  SetUpStorage();
  int proc_time3 = residence_time;

  // Move second batch to stocks
  tc_.get()->time(proc_time3 +8);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,0,0,2*cap);

}

TEST_F(StorageTest,DifferentRecipe){
  // Initialize material with different recipe than in_recipe
  double cap = throughput;
  cyclus::CompMap v;
  v[922350000] = 3;
  v[922380000] = 1;
  cyclus::Composition::Ptr rec = cyclus::Composition::CreateFromAtom(v);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(cap, rec);

  // Move material through the facility
  TestAddMat(src_facility_, mat);
  TestBuffers(src_facility_,cap,0,0,0);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,cap,0,0);
  tc_.get()->time(residence_time);
  EXPECT_NO_THROW(src_facility_->Tock());
  TestBuffers(src_facility_,0,0,0,cap);
}

TEST_F(StorageTest, BehaviorTest){
  // Verify Storage behavior

  std::string config =
    "   <in_commods> <val>spent_fuel</val> </in_commods> "
    "   <out_commods> <val>dry_spent</val> </out_commods> "
    "   <residence_time>1</residence_time>"
    "   <max_inv_size>10</max_inv_size>";

  int simdur = 3;

  cyclus::MockSim sim(cyclus::AgentSpec (":cycamore:Storage"), config, simdur);

  sim.AddSource("spent_fuel").Finalize();
  sim.AddSink("dry_spent").Finalize();

  int id = sim.Run();

  // return all transactions where our sstorage facility is the sender
  std::vector<cyclus::Cond> conds;
  conds.push_back(cyclus::Cond("Commodity", "==", std::string("dry_spent")));
  cyclus::QueryResult qr = sim.db().Query("Transactions", &conds);
  int n_trans = qr.rows.size();
  EXPECT_EQ(1, n_trans) << "expected 1 transactions, got " << n_trans;

}

TEST_F(StorageTest, MultipleCommods){
  // Verify Storage accepting Multiple Commods

  std::string config =
    "   <in_commods> <val>spent_fuel</val>"
    "                <val>spent_fuel2</val> </in_commods>"
    "   <in_commod_prefs> <val>1</val>"
    "                     <val>1</val> </in_commod_prefs>"
    "   <out_commods> <val>dry_spent</val> </out_commods> "
    "   <max_inv_size>10</max_inv_size>";

  int simdur = 2;

  cyclus::MockSim sim(cyclus::AgentSpec (":cycamore:Storage"), config, simdur);

  sim.AddSource("spent_fuel").capacity(5).Finalize();
  sim.AddSource("spent_fuel2").capacity(5).Finalize();
  sim.AddSink("dry_spent").Finalize();

  int id = sim.Run();

  // return all transactions where our sstorage facility is the acceptor
  std::vector<cyclus::Cond> conds;
  conds.push_back(cyclus::Cond("Commodity", "==", std::string("spent_fuel")));
  cyclus::QueryResult qr = sim.db().Query("Transactions", &conds);
  int n_trans = qr.rows.size();
  EXPECT_EQ(1, n_trans) << "expected 1 transactions, got " << n_trans;

  std::vector<cyclus::Cond> conds2;
  conds2.push_back(cyclus::Cond("Commodity", "==", std::string("spent_fuel2")));
  cyclus::QueryResult qr2 = sim.db().Query("Transactions", &conds2);
  int n_trans2 = qr2.rows.size();
  EXPECT_EQ(1, n_trans2) << "expected 1 transactions, got " << n_trans;
}

TEST_F(StorageTest, PositionInitialize){
  // Verify Storage behavior

  std::string config =
    "   <in_commods> <val>spent_fuel</val> </in_commods> "
    "   <out_commods> <val>dry_spent</val> </out_commods> "
    "   <residence_time>1</residence_time>"
    "   <max_inv_size>10</max_inv_size>";

  int simdur = 3;

  cyclus::MockSim sim(cyclus::AgentSpec (":cycamore:Storage"), config, simdur);

  sim.AddSource("spent_fuel").Finalize();
  sim.AddSink("dry_spent").Finalize();

  int id = sim.Run();

  cyclus::QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 0.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 0.0);
}

TEST_F(StorageTest, Longitude){
  // Verify Storage behavior

  std::string config =
    "   <in_commods> <val>spent_fuel</val> </in_commods> "
    "   <out_commods> <val>dry_spent</val> </out_commods> "
    "   <residence_time>1</residence_time>"
    "   <max_inv_size>10</max_inv_size>"
    "   <latitude>50.0</latitude> "
    "   <longitude>35.0</longitude> ";

  int simdur = 3;

  cyclus::MockSim sim(cyclus::AgentSpec (":cycamore:Storage"), config, simdur);

  sim.AddSource("spent_fuel").Finalize();
  sim.AddSink("dry_spent").Finalize();

  int id = sim.Run();

  cyclus::QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 50.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 35.0);
}

} // namespace storage

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* StorageConstructor(cyclus::Context* ctx) {
  return new storage::Storage(ctx);
}

// required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif // CYCLUS_AGENT_TESTS_CONNECTED

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(StorageFac, FacilityTests,
                        ::testing::Values(&StorageConstructor));

INSTANTIATE_TEST_CASE_P(StorageFac, AgentTests,
                        ::testing::Values(&StorageConstructor));
