#include <gtest/gtest.h>

#include <sstream>

#include "facility_tests.h"
#include "toolkit/mat_query.h"
#include "agent_tests.h"
#include "resource_helpers.h"
#include "infile_tree.h"
#include "env.h"

#include "enrichment_tests.h"

using cyclus::QueryResult;
using cyclus::Cond;
using cyclus::CompMap;
using cyclus::toolkit::MatQuery;
using pyne::nucname::id;
using cyclus::Material;

namespace cycamore {

Composition::Ptr c_nou235() {
  cyclus::CompMap m;
  m[922380000] = 1.0;
  return Composition::CreateFromMass(m);
};
Composition::Ptr c_natu1() {
  cyclus::CompMap m;
  m[922350000] = 0.007;
  m[922380000] = 0.993;
  return Composition::CreateFromMass(m);
};
Composition::Ptr c_natu2() {
  cyclus::CompMap m;
  m[922350000] = 0.01;
  m[922380000] = 0.99;
  return Composition::CreateFromMass(m);
};
Composition::Ptr c_leu() {
  cyclus::CompMap m;
  m[922350000] = 0.04;
  m[922380000] = 0.96;
  return Composition::CreateFromMass(m);
};
Composition::Ptr c_heu() {
  cyclus::CompMap m;
  m[922350000] = 0.20;
  m[922380000] = 0.80;
  return Composition::CreateFromMass(m);
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, RequestQty) {
  // this tests verifies that requests for input material are fulfilled
  // without providing any extra

  std::string config =
    "   <feed_commod>natu</feed_commod> "
    "   <feed_recipe>natu1</feed_recipe> "
    "   <product_commod>enr_u</product_commod> "
    "   <tails_commod>tails</tails_commod> "
    "   <max_feed_inventory>1.0</max_feed_inventory> "
    "   <tails_assay>0.003</tails_assay> ";

  int simdur = 1;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Enrichment"), config, simdur);
  sim.AddRecipe("natu1", c_natu1());

  sim.AddSource("natu")
    .recipe("natu1")
    .Finalize();

  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("Commodity", "==", std::string("natu")));
  QueryResult qr = sim.db().Query("Transactions", &conds);
  Material::Ptr m = sim.GetMaterial(qr.GetVal<int>("ResourceId"));

  // Should be only one transaction into the EF,
  // and it should be exactly 1kg of natu
  EXPECT_EQ(1.0, qr.rows.size());
  EXPECT_NEAR(1.0, m->quantity(), 1e-10) <<
    "matched trade provides the wrong quantity of material";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, CheckSWUConstraint) {
  // Tests that request for enrichment that exceeds the SWU constraint
  // fulfilled only up to the available SWU.
  // Also confirms that initial_feed flag works.
  // 388 SWU = 10kg 80% enriched HEU from 486kg feed matl

  std::string config =
    "   <feed_commod>natu</feed_commod> "
    "   <feed_recipe>natu1</feed_recipe> "
    "   <product_commod>enr_u</product_commod> "
    "   <tails_commod>tails</tails_commod> "
    "   <tails_assay>0.003</tails_assay> "
    "   <initial_feed>1000</initial_feed> "
    "   <swu_capacity>195</swu_capacity> ";

  int simdur = 1;

  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Enrichment"), config, simdur);

  sim.AddRecipe("natu1", c_natu1());
  sim.AddRecipe("heu", c_heu());

  sim.AddSink("enr_u")
    .recipe("heu")
    .capacity(10)
    .Finalize();

  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("Commodity", "==", std::string("enr_u")));
  QueryResult qr = sim.db().Query("Transactions", &conds);
  Material::Ptr m = sim.GetMaterial(qr.GetVal<int>("ResourceId"));

  EXPECT_EQ(1.0, qr.rows.size());
  EXPECT_NEAR(5.0, m->quantity(), 0.1) <<
    "traded quantity exceeds SWU constraint";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, CheckCapConstraint) {
  // Tests that a request for more material than is available in
  // inventory is partially filled with only the inventory quantity.

  std::string config =
    "   <feed_commod>natu</feed_commod> "
    "   <feed_recipe>natu1</feed_recipe> "
    "   <product_commod>enr_u</product_commod> "
    "   <tails_commod>tails</tails_commod> "
    "   <tails_assay>0.003</tails_assay> "
    "   <initial_feed>243</initial_feed> ";

  int simdur = 1;

  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Enrichment"), config, simdur);


  sim.AddRecipe("natu1", c_natu1());
  sim.AddRecipe("heu", c_heu());

  sim.AddSink("enr_u")
    .recipe("heu")
    .capacity(10)
    .Finalize();

  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("Commodity", "==", std::string("enr_u")));
  QueryResult qr = sim.db().Query("Transactions", &conds);
  Material::Ptr m = sim.GetMaterial(qr.GetVal<int>("ResourceId"));

  EXPECT_EQ(1.0, qr.rows.size());
  EXPECT_LE(m->quantity(), 5.0) <<
    "traded quantity exceeds capacity constraint";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, RequestEnrich) {
  // this tests verifies that requests for output material exceeding
  // the maximum allowed enrichment are not fulfilled.

  std::string config =
    "   <feed_commod>natu</feed_commod> "
    "   <feed_recipe>natu1</feed_recipe> "
    "   <product_commod>enr_u</product_commod> "
    "   <tails_commod>tails</tails_commod> "
    "   <tails_assay>0.003</tails_assay> "
    "   <max_enrich>0.19</max_enrich> ";

  int simdur = 2;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Enrichment"), config, simdur);
  sim.AddRecipe("natu1", c_natu1());
  sim.AddRecipe("leu", c_leu());
  sim.AddRecipe("heu", c_heu());

  sim.AddSource("natu")
    .recipe("natu1")
    .Finalize();
  sim.AddSink("enr_u")
    .recipe("leu")
    .capacity(1.0)
    .Finalize();
  sim.AddSink("enr_u")
    .recipe("heu")
    .Finalize();

  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("Commodity", "==", std::string("enr_u")));
  QueryResult qr = sim.db().Query("Transactions", &conds);
  Material::Ptr m = sim.GetMaterial(qr.GetVal<int>("ResourceId"));

  // Should be only one transaction out of the EF,
  // and it should be 1kg of LEU
  EXPECT_EQ(1.0, qr.rows.size());
  EXPECT_NEAR(1.0, m->quantity(), 0.01) <<
    "Not providing the requested quantity" ;

  CompMap got = m->comp()->mass();
  CompMap want = c_leu()->mass();
  cyclus::compmath::Normalize(&got);
  cyclus::compmath::Normalize(&want);

  CompMap::iterator it;
  for (it = want.begin(); it != want.end(); ++it) {
    EXPECT_DOUBLE_EQ(it->second, got[it->first]) <<
      "nuclide qty off: " << pyne::nucname::name(it->first);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, TradeTails) {
  // this tests whether tails are being traded.

  std::string config =
    "   <feed_commod>natu</feed_commod> "
    "   <feed_recipe>natu1</feed_recipe> "
    "   <product_commod>enr_u</product_commod> "
    "   <tails_commod>tails</tails_commod> "
    "   <tails_assay>0.003</tails_assay> ";

  // time 1-source to EF, 2-Enrich, add to tails, 3-tails avail. for trade
  int simdur = 3;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Enrichment"), config, simdur);
  sim.AddRecipe("natu1", c_natu1());
  sim.AddRecipe("leu", c_leu());

  sim.AddSource("natu")
    .recipe("natu1")
    .Finalize();
  sim.AddSink("enr_u")
    .recipe("leu")
    .Finalize();
   sim.AddSink("tails")
    .Finalize();

  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("Commodity", "==", std::string("tails")));
  QueryResult qr = sim.db().Query("Transactions", &conds);

  // Should be exactly one tails transaction
  EXPECT_EQ(1, qr.rows.size());

}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  TEST_F(EnrichmentTest, TailsQty) {
  // this tests whether tails are being traded at correct quantity when
  // requested amount is larger than qty in a single tails-buffer element

  std::string config =
    "   <feed_commod>natu</feed_commod> "
    "   <feed_recipe>natu1</feed_recipe> "
    "   <product_commod>enr_u</product_commod> "
    "   <tails_commod>tails</tails_commod> "
    "   <tails_assay>0.003</tails_assay> ";

  // time 1-source to EF, 2-Enrich, add to tails, 3-tails avail. for trade
  int simdur = 3;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Enrichment"), config, simdur);
  sim.AddRecipe("natu1", c_natu1());
  sim.AddRecipe("leu", c_leu());

  sim.AddSource("natu")
    .recipe("natu1")
    .Finalize();
  sim.AddSink("enr_u")
    .recipe("leu")
    .capacity(0.5)
    .Finalize();
  sim.AddSink("enr_u")
    .recipe("leu")
    .capacity(0.5)
    .Finalize();
  sim.AddSink("tails")
    .Finalize();

  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("Commodity", "==", std::string("tails")));
  QueryResult qr = sim.db().Query("Transactions", &conds);
  Material::Ptr m = sim.GetMaterial(qr.GetVal<int>("ResourceId"));

  // Should be 2 tails transactions, one from each LEU sink, each 4.125kg.
  // Q * (e_p - e_f)/(e_f - e_t) = 0.5 * (0.04 - 0.007)/(0.007 - 0.003) = 4.125
  EXPECT_EQ(2, qr.rows.size());

  cyclus::SqlStatement::Ptr stmt = sim.db().db().Prepare(
      "SELECT SUM(r.Quantity) FROM Transactions AS t"
      " INNER JOIN Resources AS r ON r.ResourceId = t.ResourceId"
      " WHERE t.Commodity = ?;"
      );

  stmt->BindText(1, "tails");
  stmt->Step();
  EXPECT_NEAR(8.25,stmt->GetDouble(0), 0.01) <<
    "Not providing the requested quantity" ;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, BidPrefs) {
  // This tests that natu sources are preference-ordered by
  // U235 content

  std::string config =
    "   <feed_commod>natu</feed_commod> "
    "   <feed_recipe>natu1</feed_recipe> "
    "   <product_commod>enr_u</product_commod> "
    "   <tails_commod>tails</tails_commod> "
    "   <tails_assay>0.003</tails_assay> "
    "   <max_feed_inventory>1.0</max_feed_inventory> ";

  int simdur = 1;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Enrichment"), config, simdur);
  sim.AddRecipe("natu1", c_natu1());
  sim.AddRecipe("natu2", c_natu2());

  sim.AddSource("natu")
    .recipe("natu1")
    .capacity(1)
    .Finalize();

  sim.AddSource("natu")
    .recipe("natu2")
    .capacity(1)
    .Finalize();

  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("Commodity", "==", std::string("natu")));
  QueryResult qr = sim.db().Query("Transactions", &conds);

  // should trade only with #2 since it has higher U235
  EXPECT_EQ(1, qr.rows.size());

  Material::Ptr m = sim.GetMaterial(qr.GetVal<int>("ResourceId"));
  CompMap got = m->comp()->mass();
  CompMap want = c_natu2()->mass();
  cyclus::compmath::Normalize(&got);
  cyclus::compmath::Normalize(&want);

  CompMap::iterator it;
  for (it = want.begin(); it != want.end(); ++it) {
    EXPECT_DOUBLE_EQ(it->second, got[it->first]) <<
      "nuclide qty off: " << pyne::nucname::name(it->first);
  }

}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  TEST_F(EnrichmentTest, NoBidPrefs) {
  // This tests that preference-ordering for sources
  // turns off correctly if flag is used

  std::string config =
    "   <feed_commod>natu</feed_commod> "
    "   <feed_recipe>natu1</feed_recipe> "
    "   <product_commod>enr_u</product_commod> "
    "   <tails_commod>tails</tails_commod> "
    "   <tails_assay>0.003</tails_assay> "
    "   <max_feed_inventory>2.0</max_feed_inventory> "
    "   <order_prefs>0</order_prefs> ";

  int simdur = 1;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Enrichment"), config, simdur);
  sim.AddRecipe("natu1", c_natu1());
  sim.AddRecipe("natu2", c_natu2());

  sim.AddSource("natu")
    .recipe("natu1")
    .capacity(1)
    .Finalize();

  sim.AddSource("natu")
    .recipe("natu2")
    .capacity(1)
    .Finalize();

  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("Commodity", "==", std::string("natu")));
  QueryResult qr = sim.db().Query("Transactions", &conds);

  // should trade with both to meet its capacity limit
  EXPECT_EQ(2, qr.rows.size());
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, ZeroU235) {
  // Test that offers of natu with no u235 content are rejected

  std::string config =
    "   <feed_commod>natu</feed_commod> "
    "   <feed_recipe>natu1</feed_recipe> "
    "   <product_commod>enr_u</product_commod> "
    "   <tails_commod>tails</tails_commod> "
    "   <tails_assay>0.003</tails_assay> "
    "   <max_feed_inventory>1.0</max_feed_inventory> ";

  int simdur = 1;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Enrichment"), config, simdur);
  sim.AddRecipe("no_u235", c_nou235());
  sim.AddRecipe("natu1", c_natu1());

  sim.AddSource("natu")
    .recipe("no_u235")
    .capacity(1)
    .Finalize();

  int id = sim.Run();

  std::vector<Cond> conds;
  conds.push_back(Cond("Commodity", "==", std::string("natu")));
  // DB table should be empty since there are no transactions
  EXPECT_THROW(sim.db().Query("Transactions", &conds),
         std::exception);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentTest::SetUp() {
  cyclus::Env::SetNucDataPath();
  cyclus::Context* ctx = tc_.get();
  src_facility = new Enrichment(ctx);
  trader = tc_.trader();
  InitParameters();
  SetUpSource();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentTest::TearDown() {
  delete src_facility;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentTest::InitParameters() {
  cyclus::Context* ctx = tc_.get();

  feed_commod = "incommod";
  product_commod = "outcommod";
  tails_commod = "tailscommod";

  feed_recipe = "recipe";
  feed_assay = 0.0072;

  cyclus::CompMap v;
  v[922350000] = feed_assay;
  v[922380000] = 1 - feed_assay;
  recipe = cyclus::Composition::CreateFromMass(v);
  ctx->AddRecipe(feed_recipe, recipe);

  tails_assay = 0.002;
  swu_capacity = 100; //**
  inv_size = 5;

  reserves = 105.5;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentTest::SetUpSource() {
  src_facility->feed_recipe = feed_recipe;
  src_facility->feed_commod = feed_commod;
  src_facility->product_commod = product_commod;
  src_facility->tails_commod = tails_commod;
  src_facility->tails_assay = tails_assay;
  src_facility->max_enrich = max_enrich;
  src_facility->SetMaxInventorySize(inv_size);
  src_facility->SwuCapacity(swu_capacity);
  src_facility->initial_feed = reserves;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Material::Ptr EnrichmentTest::GetMat(double qty) {
  return cyclus::Material::CreateUntracked(qty,
                                           tc_.get()->GetRecipe(feed_recipe));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentTest::DoAddMat(cyclus::Material::Ptr mat) {
  src_facility->AddMat_(mat);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Material::Ptr EnrichmentTest::DoRequest() {
  return src_facility->Request_();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Material::Ptr
EnrichmentTest::DoOffer(cyclus::Material::Ptr mat) {
  return src_facility->Offer_(mat);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Material::Ptr
EnrichmentTest::DoEnrich(cyclus::Material::Ptr mat, double qty) {
  return src_facility->Enrich_(mat, qty);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, Request) {
  // Tests that quantity in material request is accurate
  double req = inv_size;
  double add = 0;
  cyclus::Material::Ptr mat = DoRequest();
  EXPECT_DOUBLE_EQ(mat->quantity(), req);
  EXPECT_EQ(mat->comp(), tc_.get()->GetRecipe(feed_recipe));

  add = 2 * inv_size / 3;
  req -= add;
  DoAddMat(GetMat(add));
  mat = DoRequest();
  EXPECT_DOUBLE_EQ(mat->quantity(), req);
  EXPECT_EQ(mat->comp(), tc_.get()->GetRecipe(feed_recipe));

  add = inv_size / 3;
  req = 0;
  DoAddMat(GetMat(add));
  mat = DoRequest();
  EXPECT_DOUBLE_EQ(mat->quantity(), req);
  EXPECT_EQ(mat->comp(), tc_.get()->GetRecipe(feed_recipe));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, ValidReq) {
  // Tests that material requests have U235/(U235+U238) > tails assay
  using cyclus::CompMap;
  using cyclus::Composition;
  using cyclus::Material;

  double qty = 4.5;  // some magic number

  cyclus::CompMap v1;
  v1[922350000] = 1;
  Material::Ptr mat = Material::CreateUntracked
    (qty,Composition::CreateFromAtom(v1));
  EXPECT_FALSE(src_facility->ValidReq(mat));  // u238 = 0

  cyclus::CompMap v2;
  v2[922350000] = tails_assay;
  v2[922380000] = 1 - tails_assay;
  mat = Material::CreateUntracked(qty, Composition::CreateFromAtom(v2));
  // u235 / (u235 + u238) <= tails_assay
  EXPECT_FALSE(src_facility->ValidReq(mat));

  cyclus::CompMap v3;
  v3[922350000] = 1;
  v3[922380000] = 1;
  mat = Material::CreateUntracked(qty, Composition::CreateFromAtom(v3));
  EXPECT_TRUE(src_facility->ValidReq(mat));  // valid
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  TEST_F(EnrichmentTest, ConstraintConverters) {
    // Tests the SWU and NatU converters to make sure that amount of
    // feed and SWU required are correct to fulfill the enrichment request.
  using cyclus::CompMap;
  using cyclus::Material;
  using cyclus::toolkit::MatQuery;
  using cyclus::Composition;
  using cyclus::toolkit::Assays;
  using cyclus::toolkit::UraniumAssayMass;
  using cyclus::toolkit::SwuRequired;
  using cyclus::toolkit::FeedQty;
  using cyclus::toolkit::MatQuery;
  cyclus::Env::SetNucDataPath();

  double qty = 5;  // 5 kg
  double product_assay = 0.05;  // of 5 w/o enriched U
  CompMap v;
  v[922350000] = product_assay;
  v[922380000] = 1 - product_assay;
  v[94239] = 0.5;  // 94239 shouldn't be taken into account
  Material::Ptr target = Material::CreateUntracked(
      qty, Composition::CreateFromMass(v));

  std::set<cyclus::Nuc> nucs;
  nucs.insert(922350000);
  nucs.insert(922380000);

  MatQuery mq(target);
  double mass_frac = mq.mass_frac(nucs);

  SWUConverter swuc(feed_assay, tails_assay);
  NatUConverter natuc(feed_assay, tails_assay);

  Material::Ptr offer = DoOffer(target);

  EXPECT_NEAR(swuc.convert(target), swuc.convert(offer), 0.001);
  EXPECT_NEAR(natuc.convert(target) * mass_frac, natuc.convert(offer), 0.001);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, Enrich) {
  // this test asks the facility to enrich a material that results in an amount
  // of natural uranium required that is exactly its inventory level. that
  // inventory will be comprised of two materials to test the manifest/absorb
  // strategy employed in Enrich_.
  using cyclus::CompMap;
  using cyclus::Material;
  using cyclus::toolkit::MatQuery;
  using cyclus::Composition;
  using cyclus::toolkit::Assays;
  using cyclus::toolkit::UraniumAssayMass;
  using cyclus::toolkit::SwuRequired;
  using cyclus::toolkit::FeedQty;

  double qty = 5;  // kg
  double product_assay = 0.05;  // of 5 w/o enriched U
  cyclus::CompMap v;
  v[922350000] = product_assay;
  v[922380000] = 1 - product_assay;
  // target qty need not be = to request qty
  Material::Ptr target = cyclus::Material::CreateUntracked(
      qty + 10, cyclus::Composition::CreateFromMass(v));

  Assays assays(feed_assay, UraniumAssayMass(target), tails_assay);
  double swu_req = SwuRequired(qty, assays);
  double natu_req = FeedQty(qty, assays);
  double tails_qty = TailsQty(qty, assays);

  double swu_cap = swu_req * 5;
  src_facility->SwuCapacity(swu_cap);
  src_facility->SetMaxInventorySize(natu_req);
  DoAddMat(GetMat(natu_req / 2));
  DoAddMat(GetMat(natu_req / 2));

  Material::Ptr response;
  EXPECT_NO_THROW(response = DoEnrich(target, qty));
  EXPECT_DOUBLE_EQ(src_facility->Tails().quantity(), tails_qty);

  MatQuery q(response);
  EXPECT_EQ(response->quantity(), qty);
  EXPECT_EQ(q.mass_frac(922350000), product_assay);
  EXPECT_EQ(q.mass_frac(922380000), 1 - product_assay);

  // test too much natu request
  DoAddMat(GetMat(natu_req - 1));
  EXPECT_THROW(response = DoEnrich(target, qty), cyclus::Error);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, Response) {
  // this test asks the facility to respond to multiple requests for enriched
  // uranium. two requests are provided, whose total equals the swu capacity of
  // the facility while not exceeding its inventory capacity (that's taken care
  // of in the Enrich tests).
  //
  // note that response quantity and quality need not be tested, because they
  // are covered by the Enrich and RequestEnrich tests
  using cyclus::Bid;
  using cyclus::CompMap;
  using cyclus::Material;
  using cyclus::Request;
  using cyclus::Trade;
  using cyclus::toolkit::Assays;
  using cyclus::toolkit::FeedQty;
  using cyclus::toolkit::SwuRequired;
  using cyclus::toolkit::UraniumAssayMass;

  // problem set up
  std::vector< cyclus::Trade<cyclus::Material> > trades;
  std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                        cyclus::Material::Ptr> > responses;

  double qty = 5;  // kg
  double trade_qty = qty / 3;
  double product_assay = 0.05;  // of 5 w/o enriched U

  cyclus::CompMap v;
  v[922350000] = product_assay;
  v[922380000] = 1 - product_assay;
  // target qty need not be = to request qty
  Material::Ptr target = cyclus::Material::CreateUntracked(
      qty + 10, cyclus::Composition::CreateFromMass(v));

  Assays assays(feed_assay, UraniumAssayMass(target), tails_assay);
  double swu_req = SwuRequired(qty, assays);
  double natu_req = FeedQty(qty, assays);

  src_facility->SetMaxInventorySize(natu_req * 4);  // not capacitated by nat
  src_facility->SwuCapacity(swu_req);  // swu capacitated

  src_facility->GetMatlTrades(trades, responses);

  // set up state
  DoAddMat(GetMat(natu_req * 2));

  src_facility->GetMatlTrades(trades, responses);

  Request<Material>* req =
      Request<Material>::Create(target, trader, product_commod);
  Bid<Material>* bid = Bid<Material>::Create(req, target, src_facility);
  Trade<Material> trade(req, bid, trade_qty);
  trades.push_back(trade);

  // 2 trades, SWU = SWU cap
    ASSERT_GT(src_facility->SwuCapacity() - 2 * swu_req / 3,
              -1 * cyclus::eps());
  trades.push_back(trade);
  responses.clear();
  EXPECT_NO_THROW(src_facility->GetMatlTrades(trades, responses));
  EXPECT_EQ(responses.size(), 2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTest, PositionInitialize) {
  // this tests verifies the initialization of the latitude variable

  std::string config =
    "   <feed_commod>natu</feed_commod> "
    "   <feed_recipe>natu1</feed_recipe> "
    "   <product_commod>enr_u</product_commod> "
    "   <tails_commod>tails</tails_commod> "
    "   <max_feed_inventory>1.0</max_feed_inventory> "
    "   <tails_assay>0.003</tails_assay> ";

  int simdur = 1;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Enrichment"), config, simdur);
  sim.AddRecipe("natu1", c_natu1());

  sim.AddSource("natu")
    .recipe("natu1")
    .Finalize();

  int id = sim.Run();

  QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 0.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 0.0);
}

TEST_F(EnrichmentTest, PositionInitialize2) {
  // this tests verifies the initialization of the longitude
  // variable

  std::string config =
    "   <feed_commod>natu</feed_commod> "
    "   <feed_recipe>natu1</feed_recipe> "
    "   <product_commod>enr_u</product_commod> "
    "   <tails_commod>tails</tails_commod> "
    "   <max_feed_inventory>1.0</max_feed_inventory> "
    "   <tails_assay>0.003</tails_assay> "
    "   <latitude>50.0</latitude> "
    "   <longitude>35.0</longitude> ";

  int simdur = 1;
  cyclus::MockSim sim(cyclus::AgentSpec
          (":cycamore:Enrichment"), config, simdur);
  sim.AddRecipe("natu1", c_natu1());

  sim.AddSource("natu")
    .recipe("natu1")
    .Finalize();

  int id = sim.Run();

  QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 50.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 35.0);
}


}  // namespace cycamore

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* EnrichmentConstructor(cyclus::Context* ctx) {
  return new cycamore::Enrichment(ctx);
}

// required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(EnrichmentFac, FacilityTests,
                        Values(&EnrichmentConstructor));
INSTANTIATE_TEST_CASE_P(EnrichmentFac, AgentTests,
                        Values(&EnrichmentConstructor));
