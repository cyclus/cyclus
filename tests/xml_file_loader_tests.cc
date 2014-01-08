// xml_file_loader_tests.cc
#include "xml_file_loader_tests.h"

#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include "model.h"
#include "env.h"
#include "error.h"
#include "dynamic_module.h"

using namespace std;
using cyclus::XMLFileLoader;

void XMLFileLoaderTests::SetUp() {
  schema_path = cyclus::Env::GetInstallPath() + "/share/cyclus.rng.in";

  ctx_ = new cyclus::Context(&ti_, &rec_);
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
  EXPECT_NO_THROW(XMLFileLoader file(ctx_, schema_path, controlFile));
}

TEST_F(XMLFileLoaderTests, throws) {
  EXPECT_THROW(XMLFileLoader file(ctx_, schema_path, "blah"), cyclus::IOError);
}

TEST_F(XMLFileLoaderTests, control) {
  XMLFileLoader file(ctx_, schema_path, controlFile);
  EXPECT_NO_THROW(file.LoadControlParams());
}

TEST_F(XMLFileLoaderTests, recipes) {
  XMLFileLoader file(ctx_, schema_path, recipeFile);
  EXPECT_NO_THROW(file.LoadRecipes());
}

TEST_F(XMLFileLoaderTests, schema) {
  XMLFileLoader file(ctx_, schema_path, controlFile);
  std::stringstream schema(ControlSchema());
  EXPECT_NO_THROW(file.ApplySchema(schema););
}
