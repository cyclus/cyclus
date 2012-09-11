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
  return  "<start>"
          " <control>"
          "  <duration>1200</duration>"
          "  <startmonth>1</startmonth>"
          "  <startyear>2000</startyear>"
          "  <simstart>0</simstart>"
          "  <decay>-1</decay>"
          " </control>"
          "</start>";
}

std::string XMLFileLoaderTests::recipeSequence() {
  return  "<start>"
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
          "</start>";
}

std::string XMLFileLoaderTests::moduleSequence() {
  return  "<start>"
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
          "</start>";
}

std::string XMLFileLoaderTests::moduleSchema() {
  return 
    "<start>"    
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
    "</start>";
}

TEST_F(XMLFileLoaderTests,openfile) {
  EXPECT_NO_THROW(xmlFile = new XMLFileLoader(controlFile,false); delete xmlFile;);
}

TEST_F(XMLFileLoaderTests,throws) {
  EXPECT_THROW(XMLFileLoader file("blah",false), CycIOException);
}

TEST_F(XMLFileLoaderTests,control) {
  xmlFile = new XMLFileLoader(controlFile,false);
  EXPECT_NO_THROW(xmlFile->load_control_parameters());
  delete xmlFile;
}

TEST_F(XMLFileLoaderTests,recipes) {
  xmlFile = new XMLFileLoader(recipeFile,false);
  EXPECT_NO_THROW(xmlFile->load_recipes());
  delete xmlFile;
}

TEST_F(XMLFileLoaderTests,modules) {
  xmlFile = new XMLFileLoader(moduleFile,false);  
  set<string> module_types = Model::dynamic_module_types();
  xmlFile->load_dynamic_modules(module_types);
  delete xmlFile;
}

TEST_F(XMLFileLoaderTests,schema) {
  xmlFile = new XMLFileLoader(moduleFile,false);  
  stringstream schema(moduleSchema());
  EXPECT_NO_THROW(xmlFile->applySchema(schema));
  delete xmlFile;
}
