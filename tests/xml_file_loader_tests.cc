#include "xml_file_loader_tests.h"

#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include "agent.h"
#include "dynamic_module.h"
#include "env.h"
#include "error.h"
#include "sqlite_back.h"

using namespace std;
using cyclus::XMLFileLoader;

void XMLFileLoaderTests::SetUp() {
  schema_path = cyclus::Env::GetInstallPath() + "/share/cyclus/cyclus.rng.in";
  b_ = new cyclus::SqliteBack("xmlfileloadtestdb.sqlite");
  rec_.RegisterBackend(b_);

  false_file = "false.xml";
  CreateTestInputFile(false_file, FalseSequence());

  control_file = "control.xml";
  CreateTestInputFile(control_file, ControlSequence());

  decay_control_file = "decaycontrol.xml";
  CreateTestInputFile(decay_control_file, ControlSequenceWithDecay());

  solver_control_file = "solvercontrol.xml";
  CreateTestInputFile(solver_control_file, ControlSequenceWithSolver());

  eps_file = "eps.xml";
  CreateTestInputFile(eps_file, ControlSequenceWithEps());

  recipe_file = "recipes.xml";
  CreateTestInputFile(recipe_file, RecipeSequence());

  module_file = "modules.xml";
  CreateTestInputFile(module_file, ModuleSequence());

  package_file = "packages.xml";
  CreateTestInputFile(package_file, PackageSequence());
}

void XMLFileLoaderTests::TearDown() {
  remove("xmlfileloadtestdb.sqlite");
  unlink(false_file.c_str());
  unlink(control_file.c_str());
  unlink(decay_control_file.c_str());
  unlink(eps_file.c_str());
  unlink(solver_control_file.c_str());
  unlink(recipe_file.c_str());
  unlink(module_file.c_str());
}

std::string XMLFileLoaderTests::FalseSequence() {
  return "XML is nice, but boooooooooooooooo";
}

std::string XMLFileLoaderTests::ControlSequence() {
  return  "<simulation>"
          " <control>"
          "  <duration>1200</duration>"
          "  <startmonth>1</startmonth>"
          "  <startyear>2000</startyear>"
          "  <simstart>0</simstart>"
          " </control>"
          "</simulation>";
}

std::string XMLFileLoaderTests::ControlSequenceWithDecay() {
  return  "<simulation>"
          " <control>"
          "  <duration>1200</duration>"
          "  <startmonth>1</startmonth>"
          "  <startyear>2000</startyear>"
          "  <simstart>0</simstart>"
          "  <decay>never</decay>"
          " </control>"
          "</simulation>";
}

std::string XMLFileLoaderTests::ControlSequenceWithSolver() {
  return  "<simulation>"
          " <control>"
          "  <duration>1200</duration>"
          "  <startmonth>1</startmonth>"
          "  <startyear>2000</startyear>"
          "  <simstart>0</simstart>"
          "  <solver>"
          "    <config>"
          "      <greedy>"
          "        <preconditioner>greedy</preconditioner>"
          "      </greedy>"
          "    </config>"
          "    <allow_exclusive_orders>true</allow_exclusive_orders>"
          "  </solver>"
          " </control>"
          "</simulation>";
}

std::string XMLFileLoaderTests::ControlSequenceWithEps() {
  return
          "<simulation>"
          "  <control>"
          "    <duration>1</duration>"
          "    <startmonth>1</startmonth>"
          "    <startyear>2000</startyear>"
          "    <tolerance_generic>0.5e-5</tolerance_generic>"
          "    <tolerance_resource>3e-4</tolerance_resource>"
          "  </control>"
          "  <archetypes>"
          "    <spec><lib>agents</lib><name>Source</name></spec>"
          "    <spec><lib>agents</lib><name>Sink</name></spec>"
          "    <spec><lib>agents</lib><name>NullRegion</name></spec>"
          "    <spec><lib>agents</lib><name>NullInst</name></spec>"
          "  </archetypes>"
          "  <facility>"
          "    <name>src</name>"
          "    <lifetime>1</lifetime>"
          "    <config>"
          "      <Source>"
          "        <commod>commod</commod>"
          "        <capacity>1</capacity>"
          "      </Source>"
          "    </config>"
          "  </facility>"
          "  <facility>"
          "    <name>snk</name>"
          "    <config>"
          "      <Sink>"
          "        <in_commods><val>commod</val></in_commods>"
          "        <recipe_name>commod_recipe</recipe_name>"
          "        <capacity>1</capacity>"
          "      </Sink>"
          "    </config>"
          "  </facility>"
          "  <region>"
          "    <name>SingleRegion</name>"
          "    <config> <NullRegion/> </config>"
          "    <institution>"
          "      <name>SingleInstitution</name>"
          "      <initialfacilitylist>"
          "        <entry> <prototype>src</prototype> <number>1</number> </entry>"
          "        <entry> <prototype>snk</prototype> <number>1</number> </entry>"
          "      </initialfacilitylist>"
          "      <config> <NullInst/> </config>"
          "    </institution>"
          "  </region>"
          "  <recipe>"
          "    <name>commod_recipe</name>"
          "    <basis>mass</basis>"
          "    <nuclide> <id>cs137</id> <comp>1</comp> </nuclide>"
          "  </recipe>"
          "</simulation>";
}

std::string XMLFileLoaderTests::RecipeSequence() {
  return  "<simulation>"
          " <control>"
          "  <recipe>"
          "    <name>used_uo2_50gwd</name>"
          "    <basis>mass</basis>"
          "    <unit>assembly</unit>"
          "    <total>1000</total>"
          "    <nuclide>"
          "      <id>922340000</id>"
          "      <comp>0.01</comp>"
          "    </nuclide>"
          "    <nuclide>"
          "      <id>U-235</id>"
          "      <comp>0.02</comp>"
          "    </nuclide>"
          "    <nuclide>"
          "      <id>922380000</id>"
          "      <comp>0.97</comp>"
          "    </nuclide>"
          "    <nuclide>"
          "      <id>080160000</id>"
          "      <comp>0.13</comp>"
          "    </nuclide>"
          "  </recipe>"
          " </control>"
          "</simulation>";
}

std::string XMLFileLoaderTests::ModuleSequence() {
  return  "<simulation>"
          "  <!-- facility prototypes -->"
          "  <facility>"
          "    <name>fac</name>"
          "    <config>"
          "      <TestFacility/>"
          "    </config>"
          "  </facility>"
          "  <!-- region definitions -->"
          "  <region>"
          "    <name>reg</name>"
          "    <config>"
          "      <TestRegion/>"
          "    </config>"
          "    <!-- institution definitions -->"
          "    <institution>"
          "      <name>inst</name>"
          "      <config>"
          "        <TestInst/>"
          "      </config>"
          "    </institution>"
          "    <!-- end institution definitions -->"
          "  </region>"
          "</simulation>";
}

std::string XMLFileLoaderTests::ControlSchema() {
  return
    "<grammar xmlns=\"http://relaxng.org/ns/structure/1.0\""
    "datatypeLibrary=\"http://www.w3.org/2001/XMLSchema-datatypes\">"
    "<start>"
    "<element name=\"simulation\">"
    "<element name=\"control\">"
    "<element name=\"duration\">"
    "  <data type=\"nonNegativeInteger\"/>"
    "</element>"
    "<element name=\"startmonth\">"
    "  <data type=\"nonNegativeInteger\"/>"
    "</element>"
    "<element name=\"startyear\">"
    "  <data type=\"nonNegativeInteger\"/>"
    "</element>"
    "<element name=\"simstart\">"
    "  <data type=\"nonNegativeInteger\"/>"
    "</element>"
    "</element>"
    "</element>"
    "</start>"
    "</grammar>";
}

std::string XMLFileLoaderTests::PackageSequence() {
  return  "<simulation>"
          " <control>"
          "  <package>"
          "    <name>TestPackage</name>"
          "    <fill_min>1</fill_min>"
          "    <fill_max>2</fill_max>"
          "    <strategy>First</strategy>"
          "  </package>"
          " </control>"
          "</simulation>";
}

TEST_F(XMLFileLoaderTests, openfile) {
  EXPECT_NO_THROW(XMLFileLoader file(&rec_, b_, schema_path, control_file));
}

TEST_F(XMLFileLoaderTests, decayfile) {
  EXPECT_NO_THROW(XMLFileLoader file(&rec_, b_, schema_path, decay_control_file));
}

TEST_F(XMLFileLoaderTests, solverfile) {
  EXPECT_NO_THROW(XMLFileLoader file(&rec_, b_, schema_path, solver_control_file));
}

// Checking default eps values
TEST_F(XMLFileLoaderTests, EpsDefault) {
  XMLFileLoader file(&rec_, b_, schema_path, control_file);

  EXPECT_DOUBLE_EQ(1e-6, cyclus::eps());
  EXPECT_DOUBLE_EQ(1e-6, cyclus::eps_rsrc());
}

// Changing eps values
TEST_F(XMLFileLoaderTests, EpsFile) {
  XMLFileLoader file(&rec_, b_, schema_path, eps_file);
  file.LoadSim();

  EXPECT_DOUBLE_EQ(0.5e-5, cyclus::eps());
  EXPECT_DOUBLE_EQ(3e-4, cyclus::eps_rsrc());
}

TEST_F(XMLFileLoaderTests, ExplicitFormat) {
  EXPECT_NO_THROW(XMLFileLoader file(&rec_, b_, schema_path, 
                                     XMLFileLoaderTests::ControlSequenceWithEps(), "xml"));
}

TEST_F(XMLFileLoaderTests, throws) {
  EXPECT_THROW(XMLFileLoader file(&rec_, b_, schema_path, "blah"), cyclus::IOError);
}
