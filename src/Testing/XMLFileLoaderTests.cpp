// XMLFileLoaderTests.cpp
#include "XMLFileLoaderTests.h"

#include <string>
#include <set>
#include <iostream>
#include "Model.h"
#include "DynamicModule.h"

using namespace std;

void XMLFileLoaderTests::SetUp() {
    falseFile = "false.xml";
    createTestInputFile(falseFile,falseSequence);

    controlFile = "control.xml";
    createTestInputFile(controlFile,controlSequence);

    recipeFile = "recipes.xml";
    createTestInputFile(recipeFile,recipeSequence);

    moduleFile = "modules.xml";
    createTestInputFile(moduleFile,moduleSequence);
    
    // here
    //testModuleOpening();
}

void XMLFileLoaderTests::testModuleOpening() {
    DynamicModule module("Market","TestMarket");
}

void XMLFileLoaderTests::TearDown() {
    unlink(falseFile.c_str());
    unlink(controlFile.c_str());
    unlink(recipeFile.c_str());
    unlink(moduleFile.c_str());
}

TEST_F(XMLFileLoaderTests, OpenFile) {
  ASSERT_ANY_THROW(xmlFile = new XMLFileLoader(falseFile,false); delete xmlFile;);
  ASSERT_NO_THROW(xmlFile = new XMLFileLoader(controlFile,false); delete xmlFile;);
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
  // here
  testModuleOpening();


  //  xmlFile = new XMLFileLoader(moduleFile,false);  
  //set<string> module_types = Model::dynamic_module_types();
  //xmlFile->load_modules_of_type("Market","/*/market");
  //xmlFile->load_dynamic_modules(module_types);
  //delete xmlFile;
}

std::string XMLFileLoaderTests::falseSequence =
  "I am not an XML file.";

std::string XMLFileLoaderTests::controlSequence =
          "<start>"
          " <control>"
          "  <duration>1200</duration>"
          "  <startmonth>1</startmonth>"
          "  <startyear>2000</startyear>"
          "  <simstart>0</simstart>"
          "  <decay>-1</decay>"
          " </control>"
          "</start>";

std::string XMLFileLoaderTests::recipeSequence =
          "<start>"
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

std::string XMLFileLoaderTests::moduleSequence = 
          "<start>"
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
