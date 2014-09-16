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

  falseFile = "false.xml";
  CreateTestInputFile(falseFile, FalseSequence());

  controlFile = "control.xml";
  CreateTestInputFile(controlFile, ControlSequence());

  decayControlFile = "decaycontrol.xml";
  CreateTestInputFile(decayControlFile, ControlSequenceWithDecay());

  recipeFile = "recipes.xml";
  CreateTestInputFile(recipeFile, RecipeSequence());

  moduleFile = "modules.xml";
  CreateTestInputFile(moduleFile, ModuleSequence());
}

void XMLFileLoaderTests::TearDown() {
  remove("xmlfileloadtestdb.sqlite");
  unlink(falseFile.c_str());
  unlink(controlFile.c_str());
  unlink(decayControlFile.c_str());
  unlink(recipeFile.c_str());
  unlink(moduleFile.c_str());
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

TEST_F(XMLFileLoaderTests, openfile) {
  EXPECT_NO_THROW(XMLFileLoader file(&rec_, b_, schema_path, controlFile));
}

TEST_F(XMLFileLoaderTests, decayfile) {
  EXPECT_NO_THROW(XMLFileLoader file(&rec_, b_, schema_path, decayControlFile));
}

TEST_F(XMLFileLoaderTests, throws) {
  EXPECT_THROW(XMLFileLoader file(&rec_, b_, schema_path, "blah"), cyclus::IOError);
}
