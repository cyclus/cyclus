#include <gtest/gtest.h>

#include "agent.h"

#include "../test_context.h"
#include "../test_agents/test_agent.h"

namespace cyclus {
namespace toolkit {

template <TimeSeriesType T>
void testseries(std::string tbl) {
  cyclus::PyStart();
  Recorder rec;
  Timer ti;
  Context ctx(&ti, &rec);
  SqliteBack* back = new SqliteBack(":memory:");
  rec.RegisterBackend(back);

  Agent* a = new TestAgent(&ctx);
  RecordTimeSeries<T>(a, 42.0);
  rec.Close();

  SqliteDb db = back->db();
  cyclus::SqlStatement::Ptr stmt = db.Prepare("SELECT agentid,value FROM TimeSeries" + tbl);
  ASSERT_NO_THROW(stmt->Step());
  EXPECT_EQ(a->id(), stmt->GetInt(0));
  EXPECT_EQ(42.0, stmt->GetDouble(1));

  delete back;
  cyclus::PyStop();
}


TEST(TimeSeriesTests, Power) {
  testseries<POWER>("Power");
}

TEST(TimeSeriesTests, EnrichSWU) {
  testseries<ENRICH_SWU>("EnrichmentSWU");
}

TEST(TimeSeriesTests, EnrichFeed) {
  testseries<ENRICH_FEED>("EnrichmentFeed");
}

TEST(TimeSeriesTests, RawPower) {
  TestContext tc;
  Agent* a = new TestAgent(tc.get());
  RecordTimeSeries<double>("Power", a, 42.0);
}


}  // namespace toolkit
}  // namespace cyclus
