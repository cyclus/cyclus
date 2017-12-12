#include <cmath>

#include <gtest/gtest.h>

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

TEST(IntegTests, RunAllInfiles) {
  std::vector<std::string> infiles;
  infiles.push_back("custom_dt.xml");
  infiles.push_back("inventory.xml");
  infiles.push_back("inventory_compact.xml");
  infiles.push_back("inventory_compact_false.xml");
  infiles.push_back("inventory_false.xml");
  infiles.push_back("lotka_volterra_determ.xml");
  infiles.push_back("minimal_cycle.xml");
  infiles.push_back("null_sink.xml");
  infiles.push_back("predator.xml");
  infiles.push_back("prey.xml");
  infiles.push_back("source_to_sink.xml");
  infiles.push_back("trivial_cycle.xml");

  for (int i = 0; i < infiles.size(); i++) {
    {
      SqliteBack back(":memory:");
      RunSim(infiles[i], &back);
    }
  }
}

TEST(IntegTests, CustomTimestepDur) {
  {
    SqliteBack back(":memory:");
    RunSim("custom_dt.xml", &back);
    QueryResult qr = back.Query("TimeStepDur", NULL);
    EXPECT_EQ(86400, qr.GetVal<int>("DurationSecs"));
  }
}

TEST(IntegTests, CustomTimestepDurFlat) {
  {
    SqliteBack back(":memory:");
    RunSim("custom_dt_flat.xml", &back);
    QueryResult qr = back.Query("TimeStepDur", NULL);
    EXPECT_EQ(86400, qr.GetVal<int>("DurationSecs"));
  }
}

}  // namespace cyclus
