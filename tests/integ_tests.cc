#include "platform.h"
#if CYCLUS_IS_PARALLEL
#include <omp.h>
#endif // CYCLUS_IS_PARALLEL
#include <cmath>

#include <gtest/gtest.h>
#include <boost/shared_ptr.hpp>

#include "cyclus.h"
#include "pyhooks.h"
#include "sim_init.h"
#include "xml_file_loader.h"
#include "xml_flat_loader.h"

using pyne::nucname::id;
namespace fs = boost::filesystem;

namespace cyclus {

bool IsFlatSchema(std::string infile) {
  std::stringstream input;
  LoadStringstreamFromFile(input, infile);
  boost::shared_ptr<XMLParser> parser =
      boost::shared_ptr<XMLParser>(new XMLParser());
  parser->Init(input);
  InfileTree tree(*parser);
  return OptionalQuery<std::string>(&tree, "/simulation/schematype", "") == "flat";
}

std::string FullPath(std::string infile) {
  std::string p = Env::GetInstallPath() + "/share/cyclus/input/" + infile;
  if (fs::exists(p)) {
    return p;
  }

  p = Env::GetBuildPath() + "/share/cyclus/input/" + infile;
  if (fs::exists(p)) {
    return p;
  }

  throw IOError(infile + " not found in "
                + Env::GetInstallPath() + "/share/cyclus/input/ or "
                + Env::GetBuildPath() + "/share/cyclus/input/");
}

// RunSim runs the cyclus input file at path infile
// storing the results in back.
void RunSim(std::string infile, SqliteBack* back) {
  PyStart();
  Recorder r;
  r.RegisterBackend(back);

  infile = FullPath(infile);
  SimInit si;
  if (IsFlatSchema(infile)) {
    XMLFlatLoader l(&r, back, Env::rng_schema(true), infile);
    l.LoadSim();
  } else {
    XMLFileLoader l(&r, back, Env::rng_schema(false), infile);
    l.LoadSim();
  }
  si.Init(&r, back);
  si.timer()->RunSim();
  r.Flush();
  PyStop();
}

class IntegTestsFixture : public ::testing::TestWithParam<int> {
  protected:
    #if CYCLUS_IS_PARALLEL
    virtual void SetUp() {
      int nthreads = GetParam();
      omp_set_num_threads(nthreads);
    }

    virtual void TearDown() {
      omp_set_num_threads(1);
    }
    #endif // CYCLUS_IS_PARALLEL
};

TEST_P(IntegTestsFixture, RunAllInfiles) {
  std::vector<std::string> infiles;
  infiles.push_back("custom_dt.xml");
  infiles.push_back("custom_seed.xml");
  infiles.push_back("inventory.xml");
  infiles.push_back("inventory_compact.xml");
  infiles.push_back("inventory_compact_false.xml");
  infiles.push_back("inventory_false.xml");
  infiles.push_back("lotka_volterra_determ.xml");
  infiles.push_back("predator.xml");
  infiles.push_back("prey.xml");
#if CYCLUS_HAS_COIN
  infiles.push_back("null_sink.xml");
  infiles.push_back("source_to_sink.xml");
  infiles.push_back("minimal_cycle.xml");
  infiles.push_back("trivial_cycle.xml");
#endif

  for (int i = 0; i < infiles.size(); i++) {
    {
      SqliteBack back(":memory:");
      RunSim(infiles[i], &back);
    }
  }
}

TEST_P(IntegTestsFixture, CustomTimestepDur) {
  {
    SqliteBack back(":memory:");
    RunSim("custom_dt.xml", &back);
    QueryResult qr = back.Query("TimeStepDur", NULL);
    EXPECT_EQ(86400, qr.GetVal<int>("DurationSecs"));
  }
}

TEST_P(IntegTestsFixture, CustomTimestepDurFlat) {
  {
    SqliteBack back(":memory:");
    RunSim("custom_dt_flat.xml", &back);
    QueryResult qr = back.Query("TimeStepDur", NULL);
    EXPECT_EQ(86400, qr.GetVal<int>("DurationSecs"));
  }
}

TEST_P(IntegTestsFixture, CustomSeed) {
  SqliteBack back(":memory:");
    RunSim("custom_seed.xml", &back);
    QueryResult qr = back.Query("Info", NULL);
    EXPECT_EQ(20240101, qr.GetVal<int>("Seed"));
    EXPECT_EQ(1234, qr.GetVal<int>("Stride"));
}

#if CYCLUS_IS_PARALLEL
INSTANTIATE_TEST_CASE_P(IntegTestsParallel, IntegTestsFixture, ::testing::Values(1, 2, 3, 4));
#else
INSTANTIATE_TEST_CASE_P(IntegTests, IntegTestsFixture, ::testing::Values(1));
#endif // CYCLUS_IS_PARALLEL

}  // namespace cyclus
