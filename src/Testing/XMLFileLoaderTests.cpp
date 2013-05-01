// XMLFileLoaderTests.cpp
#include "XMLFileLoaderTests.h"

#include <string>
#include <sstream>
#include <set>
#include <iostream>
#include "Model.h"
#include "DynamicModule.h"

using namespace std;

void XMLFileLoaderTests::SetUp() {
    falseFile = "false.xml";
    createTestInputFile(falseFile,falseSequence());

    controlFile = "control.xml";
    createTestInputFile(controlFile,controlSequence());

    recipeFile = "recipes.xml";
    createTestInputFile(recipeFile,recipeSequence());

    moduleFile = "modules.xml";
    createTestInputFile(moduleFile,moduleSequence());
}

void XMLFileLoaderTests::TearDown() {
    unlink(falseFile.c_str());
    unlink(controlFile.c_str());
    unlink(recipeFile.c_str());
    unlink(moduleFile.c_str());
}

std::string XMLFileLoaderTests::falseSequence() {
  return "XML is nice, but boooooooooooooooo";
}

std::string XMLFileLoaderTests::controlSequence() {
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

std::string XMLFileLoaderTests::recipeSequence() {
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

std::string XMLFileLoaderTests::moduleSequence() {
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

std::string XMLFileLoaderTests::controlSchema() {
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

TEST_F(XMLFileLoaderTests,openfile) {
  xmlFile = XMLFileLoader(controlFile); 
  EXPECT_NO_THROW(xmlFile.init(false));
}

TEST_F(XMLFileLoaderTests,throws) {
  XMLFileLoader file("blah");
  EXPECT_THROW(file.init(false), CycIOException);
}

TEST_F(XMLFileLoaderTests,control) {
  xmlFile = XMLFileLoader(controlFile);
  xmlFile.init(false);
  EXPECT_NO_THROW(xmlFile.load_control_parameters());
}

TEST_F(XMLFileLoaderTests,recipes) {
  xmlFile = XMLFileLoader(recipeFile);
  xmlFile.init(false);
  EXPECT_NO_THROW(xmlFile.load_recipes());
}

// This needs to be moved somewhere else! maybe to a new simulation
// constructor class..
TEST_F(XMLFileLoaderTests,modulesandsim) {
  xmlFile = XMLFileLoader(moduleFile);
  xmlFile.init(false);
  set<string> module_types = Model::dynamic_module_types();
  xmlFile.load_dynamic_modules(module_types);
  EXPECT_NO_THROW(Model::constructSimulation());
}

TEST_F(XMLFileLoaderTests,schema) {
  xmlFile = XMLFileLoader(controlFile);
  xmlFile.init(false);
  stringstream schema(controlSchema());
  EXPECT_NO_THROW(xmlFile.applySchema(schema););
}
