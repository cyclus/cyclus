// XMLFileLoaderTests.cpp
#include <string>
#include "XMLFileLoaderTests.h"


TEST_F(XMLFileLoaderTests, OpenFile) {

  std::string emptyString = "";
  ASSERT_ANY_THROW(xmlFile = XMLFileLoader(emptyString));

  std::string missingFile = "thisFileShouldNotExist.xml";
  ASSERT_ANY_THROW(xmlFile = XMLFileLoader(missingFile));

  ASSERT_NO_THROW(xmlFile = XMLFileLoader(testFile1));

}

TEST_F(XMLFileLoaderTests, ValidateFile) {

  ASSERT_NO_THROW(xmlFile = XMLFileLoader(testFile1));
  ASSERT_ANY_THROW(xmlFile.validate_file("thisSchemaDoseNotExist"));
  ASSERT_ANY_THROW(xmlFile.validate_file(XMLFileLoader::main_schema_));

  ASSERT_NO_THROW(xmlFile = XMLFileLoader(testFile3));
  ASSERT_ANY_THROW(xmlFile.validate_file(XMLFileLoader::main_schema_));

  ASSERT_NO_THROW(xmlFile = XMLFileLoader(testFile3));
  ASSERT_NO_THROW(xmlFile.validate_file(XMLFileLoader::main_schema_));

}

TEST_F(XMLFileLoaderTests, LoadCatalog) {

}

TEST_F(XMLFileLoaderTests, LoadFacilities ) {

}

TEST_F(XMLFileLoaderTests, LoadRecipes ) {

}

TEST_F(XMLFileLoaderTests, LoadAllModels ) {

}

TEST_F(XMLFileLoaderTests, LoadParams ) {

}


std::string XMLFileLoaderTests::testFile1Contents =
  "<start>"
  "  <elementA>some content A</elementA>"
  "  <elementB>some content B</elementB>"
  "</start>";

std::string XMLFileLoaderTests::testFile2Contents =
  "I am not an XML file.";

std::string XMLFileLoaderTests::testFile3Contents = 
"<?xml version=\"1.0\"?>"
"<!-- 1 GrowthRegion reactorRegion -->"
"<!-- power demand @ 5*exp(0.005*t)+1800 -->"
"<!-- 1 BuildInst reactorInst -->"
"<!-- BatchReactor ReactorA @ 1050 -->"
"<!-- BatchReactor ReactorB @ 750 -->"
"<!-- 1 BuildRegion supportRegion -->"
"<!-- 1 BuildInst supportInst -->"
"<!-- 1 SourceFacility FrontEnd @ t = 0, "
"     1 SinkFacility BackEnd    @ t = 0 -->"
"<!-- NullMarket freshfuel -->"
"<!-- NullMarket usedfuel -->"
""
"<simulation>"
"  <!-- master sim info -->"
"  <duration>1200</duration>"
"  <startmonth>1</startmonth>"
"  <startyear>2000</startyear>"
"  <simstart>0</simstart>"
"  <decay>-1</decay>"
"  <!-- end master sim info -->"
""
"  <!-- commodities -->"
"  <commodity>"
"    <name>fresh_fuel</name>"
"  </commodity>"
""
"  <commodity>"
"    <name>used_fuel</name>"
"  </commodity>"
"  <!-- end commodities -->"
""
"  <!-- markets -->"
"  <market>"
"    <name>freshfuel</name>"
"    <mktcommodity>fresh_fuel</mktcommodity>"
"    <model>"
"      <NullMarket/>"
"    </model>"
"  </market>"
""
"  <market>"
"    <name>usedfuel</name>"
"    <mktcommodity>used_fuel</mktcommodity>"
"    <model>"
"      <NullMarket/>"
"    </model>"
"  </market>"
"  <!-- end markets -->"
""
"  <!-- facility prototypes -->"
"  <facility>"
"    <name>FrontEnd</name>"
"    <model>"
"      <SourceFacility>"
"        <outcommodity>fresh_fuel</outcommodity>"
"        <capacity>1e4</capacity>"
"        <inventorysize>1e5</inventorysize>"
"        <commodprice>0</commodprice>"
"        <recipe>fresh_uo2</recipe>"
"      </SourceFacility>"
"    </model>"
"  </facility>"
""
"  <facility>"
"    <name>ReactorA</name>"
"    <model>"
"      <BatchReactor>"
"        <fuelpair>"
"          <incommodity>fresh_fuel</incommodity>"
"          <inrecipe>fresh_uo2</inrecipe>"
"          <outcommodity>used_fuel</outcommodity>"
"          <outrecipe>used_uo2_50gwd</outrecipe>"
"        </fuelpair>"
"	<cyclelength>3</cyclelength>"
"        <lifetime>12</lifetime>"
"	<coreloading>4</coreloading>"
"	<batchespercore>4</batchespercore>"
"      </BatchReactor>"
"    </model>"
"  </facility>"
""
"  <facility>"
"    <name>ReactorB</name>"
"    <model>"
"      <BatchReactor>"
"        <fuelpair>"
"          <incommodity>fresh_fuel</incommodity>"
"          <inrecipe>fresh_uo2</inrecipe>"
"          <outcommodity>used_fuel</outcommodity>"
"          <outrecipe>used_uo2_50gwd</outrecipe>"
"        </fuelpair>"
"	<cyclelength>3</cyclelength>"
"        <lifetime>12</lifetime>"
"	<coreloading>4</coreloading>"
"	<batchespercore>4</batchespercore>"
"      </BatchReactor>"
"    </model>"
"  </facility>"
""
"  <facility>"
"    <name>BackEnd</name>"
"    <model>"
"      <SinkFacility>"
"        <incommodity>used_fuel</incommodity>"
"        <capacity>1e4</capacity>"
"        <inventorysize>1e5</inventorysize>"
"        <commodprice>0</commodprice>"
"      </SinkFacility>"
"    </model>"
"  </facility>"
"  <!-- end facility prototypes -->"
""
"  <!-- region definitions -->"
"  <region>"
"    <name>reactorRegion</name>"
"    <allowedfacility>ReactorA</allowedfacility>"
"    <allowedfacility>ReactorB</allowedfacility>"
"    <model>"
"      <GrowthRegion>"
"	<gcommodity>"
"	  <name>power</name>"
"	  <demand>"
"	    <type>exponential</type>"
"	    <parameters>5 0.0005 1800</parameters>"
"	    <metby>"
"	      <facility>ReactorA</facility>"
"	      <capacity>1050</capacity>"
"	    </metby>"
"	    <metby>"
"	      <facility>ReactorB</facility>"
"	      <capacity>750</capacity>"
"	    </metby>"
"	  </demand>"
"	</gcommodity>"
"      </GrowthRegion>"
"    </model>"
""
"    <!-- institution definitions -->"
"    <institution>"
"      <name>reactorInst</name>"
"      <model>"
"        <BuildInst>"
"	  <availableprototype>ReactorA</availableprototype>"
"	  <availableprototype>ReactorB</availableprototype>"
"        </BuildInst>"
"      </model>"
"    </institution>"
"    <!-- end institution definitions -->"
"  </region>"
""
"  <region>"
"    <name>supportRegion</name>"
"    <allowedfacility>FrontEnd</allowedfacility>"
"    <allowedfacility>BackEnd</allowedfacility>"
"    <model>"
"      <BuildRegion>"
"	<prototyperequirement>"
"	  <prototypename>FrontEnd</prototypename>"
"	  <demandschedule>"
"	    <entry>"
"	      <time>0</time>"
"	      <number>1</number>"
"	    </entry>"
"	  </demandschedule>"
"	</prototyperequirement>"
"	<prototyperequirement>"
"	  <prototypename>BackEnd</prototypename>"
"	  <demandschedule>"
"	    <entry>"
"	      <time>0</time>"
"	      <number>1</number>"
"	    </entry>"
"	  </demandschedule>"
"	</prototyperequirement>"
"      </BuildRegion>"
"    </model>"
""
"    <!-- institution definitions -->"
"    <institution>"
"      <name>supportInst</name>"
"      <model>"
"        <BuildInst>"
"	  <availableprototype>FrontEnd</availableprototype>"
"	  <availableprototype>BackEnd</availableprototype>"
"        </BuildInst>"
"      </model>"
"    </institution>"
"    <!-- end institution definitions -->"
"  </region>"
"  <!-- end region definitions -->"
""
"  <!-- recipe definitions -->"
"  <recipe>"
"    <name>fresh_uo2</name>"
"    <basis>mass</basis>    "
"    <unit>assembly</unit>"
"    <total>1000</total>"
"    <isotope>"
"      <id>92235</id>"
"      <comp>0.05</comp>"
"    </isotope>"
"    <isotope>"
"      <id>92238</id>"
"      <comp>0.95</comp>"
"    </isotope>"
"    <isotope>"
"      <id>08016</id>"
"      <comp>0.13</comp>"
"    </isotope>"
"  </recipe>"
""
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
"  <!-- end recipe definitions -->"
""
"</simulation>"
;


