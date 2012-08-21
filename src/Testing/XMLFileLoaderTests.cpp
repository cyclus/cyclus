// XMLFileLoaderTests.cpp
#include <string>
#include "XMLFileLoaderTests.h"

std::string XMLFileLoaderTests::testFile1Contents = 
"<start> \n\
   <element1>content 1</element1> \n\
   <element2>content 2</element2> \n\
</start>";

TEST_F(XMLFileLoaderTests, OpenFile) {

  std::string emptyString = "";
  ASSERT_ANY_THROW(xmlFile = XMLFileLoader(emptyString));

  std::string missingFile = "thisFileShouldNotExist.xml";
  ASSERT_ANY_THROW(xmlFile = XMLFileLoader(missingFile));

  ASSERT_NO_THROW(xmlFile = XMLFileLoader(testFile1));

}

TEST_F(XMLFileLoaderTests, ValidateFile) {

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
