#include <gtest/gtest.h>
#include "IsoVector.h"
#include "InputXML.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class IsoVectorTest : public ::testing::Test {
  protected:
    IsoVector vect1, vect2;

    virtual void SetUp() {
      vect1.setMass(1001, 1);
      vect1.setMass(92235, 10);
      vect1.setMass(92238, 200);

      vect2.setMass(1001, 2);
      vect2.setMass(92235, 20);
      vect2.setMass(94240, 1000);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, DefaultConstructor) {
  IsoVector vect;

  EXPECT_DOUBLE_EQ(vect.mass(), 0.0);
  EXPECT_DOUBLE_EQ(vect.mass(1001), 0.0);

  EXPECT_DOUBLE_EQ(vect.atomCount(), 0.0);
  EXPECT_DOUBLE_EQ(vect.atomCount(1001), 0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, CompMapConstructor) {
  CompMap empty_map;
  IsoVector vect(empty_map);
  
  EXPECT_DOUBLE_EQ(vect.mass(), 0.0);
  EXPECT_DOUBLE_EQ(vect.mass(1001), 0.0);

  EXPECT_DOUBLE_EQ(vect.atomCount(), 0.0);
  EXPECT_DOUBLE_EQ(vect.atomCount(1001), 0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, DISABLED_XMlConstructor){
  std::string mystr;
  mystr += "<recipe>\n";
  mystr += "  <name>fresh_50gwd</name>\n";
  mystr += "  <basis>mass</basis>\n";
  mystr += "  <unit>assembly</unit>\n";
  mystr += "  <total>1000</total>\n";
  mystr += "  <isotope>\n";
  mystr += "    <id>92235</id>\n";
  mystr += "    <comp>0.05</comp>\n";
  mystr += "  </isotope>\n";
  mystr += "  <isotope>\n";
  mystr += "    <id>92238</id>\n";
  mystr += "    <comp>0.95</comp>\n";
  mystr += "  </isotope>\n";
  mystr += "  <isotope>\n";
  mystr += "    <id>08016</id>\n";
  mystr += "    <comp>0.13</comp>\n";
  mystr += "  </isotope>\n";
  mystr += "</recipe>\n";

  const char* xmlcontent = mystr.c_str();
  xmlDocPtr xml_doc = xmlParseMemory(xmlcontent, strlen(xmlcontent));
  xmlNodePtr recipe_node = xml_doc->children;

  IsoVector vect(recipe_node);

  EXPECT_DOUBLE_EQ(vect.mass(), 1000.0);
}

