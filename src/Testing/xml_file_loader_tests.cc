// xml_file_loader_tests.cc
#include "xml_file_loader_tests.h"

#include <string>
#include <sstream>
#include <set>
#include <iostream>
#include "model.h"
#include "error.h"
#include "dynamic_module.h"
#include "timer.h"

using namespace std;

void XMLFileLoaderTests::SetUp() {
  ctx_ = new cyclus::Context(&ti_, &em_);
  falseFile = "false.xml";
  CreateTestInputFile(falseFile, FalseSequence());

  controlFile = "control.xml";
  CreateTestInputFile(controlFile, ControlSequence());

  recipeFile = "recipes.xml";
  CreateTestInputFile(recipeFile, RecipeSequence());

  moduleFile = "modules.xml";
  CreateTestInputFile(moduleFile, ModuleSequence());
}

void XMLFileLoaderTests::TearDown() {
  unlink(falseFile.c_str());
  unlink(controlFile.c_str());
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
          "  <decay>-1</decay>"
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
          "    <isotope>"
          "      <id>92234</id>"
          "      <comp>0.01</comp>"
          "    </isotope>"
          "    <isotope>"
          "      <id>92235</id>"
          "      <comp>0.02</comp>"
          "    </isotope>"
          "    <isotope>"
          "      <id>92238</id>"
          "      <comp>0.97</comp>"
          "    </isotope>"
          "    <isotope>"
          "      <id>08016</id>"
          "      <comp>0.13</comp>"
          "    </isotope>"
          "  </recipe>"
          " </control>"
          "</simulation>";
}

std::string XMLFileLoaderTests::ModuleSequence() {
  return  "<simulation>"
          "  <!-- markets -->"
          "  <market>"
          "    <name>freshfuel</name>"
          "    <mktcommodity>fresh_fuel</mktcommodity>"
          "    <model>"
          "      <TestMarket/>"
          "    </model>"
          "  </market>"
          "  <!-- facility prototypes -->"
          "  <facility>"
          "    <name>fac</name>"
          "    <model>"
          "      <TestFacility/>"
          "    </model>"
          "  </facility>"
          "  <!-- region definitions -->"
          "  <region>"
          "    <name>reg</name>"
          "    <allowedfacility>fac</allowedfacility>"
          "    <model>"
          "      <TestRegion/>"
          "    </model>"
          "    <!-- institution definitions -->"
          "    <institution>"
          "      <name>inst</name>"
          "	 <availableprototype>fac</availableprototype>"
          "      <model>"
          "        <TestInst/>"
          "      </model>"
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
    "<element name=\"decay\">"
    "  <data type=\"integer\"/>"
    "</element>"
    "</element>"
    "</element>"
    "</start>"
    "</grammar>";
}

TEST_F(XMLFileLoaderTests, openfile) {
  using cyclus::XMLFileLoader;
  XMLFileLoader file(ctx_, controlFile);
  EXPECT_NO_THROW(file.Init(false));
}

TEST_F(XMLFileLoaderTests, throws) {
  using cyclus::XMLFileLoader;
  XMLFileLoader file(ctx_, "blah");
  EXPECT_THROW(file.Init(false), cyclus::IOError);
}

TEST_F(XMLFileLoaderTests, control) {
  using cyclus::XMLFileLoader;
  XMLFileLoader file(ctx_, controlFile);
  file.Init(false);
  EXPECT_NO_THROW(file.load_control_parameters(&ti_));
}

TEST_F(XMLFileLoaderTests, recipes) {
  using cyclus::XMLFileLoader;
  XMLFileLoader file(ctx_, recipeFile);
  file.Init(false);
  EXPECT_NO_THROW(file.load_recipes());
}

// This needs to be moved somewhere else! maybe to a new simulation
// constructor class..
TEST_F(XMLFileLoaderTests, modulesandsim) {
  using cyclus::XMLFileLoader;
  XMLFileLoader file(ctx_, moduleFile);
  file.Init(false);
  std::set<std::string> module_types = cyclus::Model::dynamic_module_types();
  file.load_dynamic_modules(module_types);
  EXPECT_NO_THROW(cyclus::Model::ConstructSimulation());
}

TEST_F(XMLFileLoaderTests, schema) {
  using cyclus::XMLFileLoader;
  XMLFileLoader file(ctx_, controlFile);
  file.Init(false);
  std::stringstream schema(ControlSchema());
  EXPECT_NO_THROW(file.ApplySchema(schema););
}
