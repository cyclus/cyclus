#include <map>
#include <gtest/gtest.h>
#include "IsoVector.h"
#include "InputXML.h"
#include "MassTable.h"
#include "CycException.h"
#include "Logger.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class IsoVectorTest : public ::testing::Test {
  protected:
    IsoVector vect0, vect1, vect2;

    int oxygen, u235, u238, pu240;

    double v1_m_oxygen, v1_m_u235, v1_m_u238;
    double v2_m_oxygen, v2_m_u235, v2_m_pu240;

    double v1_a_oxygen, v1_a_u235, v1_a_u238;
    double v2_a_oxygen, v2_a_u235, v2_a_pu240;

    double total_mass1;
    double total_mass2;
    double total_moles1;
    double total_moles2;

    int isotope_lower_limit;
    int isotope_upper_limit;
    int isotope_not_present;

    CompMap comp_map;

    virtual void SetUp() {
      try {
        double grams_per_kg = 1000;

        isotope_lower_limit = 1001;
        isotope_upper_limit = 1182949;
        isotope_not_present = 9001;

        oxygen = 8001;
        u235 = 92235;
        u238 = 92238;
        pu240 = 94240;

        v1_m_oxygen = 1;
        v1_m_u235 = 10;
        v1_m_u238 = 100;

        v2_m_oxygen = 2;
        v2_m_u235 = 20;
        v2_m_pu240 = 2000;

        v1_a_oxygen = v1_m_oxygen * grams_per_kg / MT->getMassInGrams(oxygen);
        v1_a_u235 = v1_m_u235 * grams_per_kg / MT->getMassInGrams(u235);
        v1_a_u238 = v1_m_u238 * grams_per_kg / MT->getMassInGrams(u238);

        v2_a_oxygen = v2_m_oxygen * grams_per_kg / MT->getMassInGrams(oxygen);
        v2_a_u235 = v2_m_u235 * grams_per_kg / MT->getMassInGrams(u235);
        v2_a_pu240 = v2_m_pu240 * grams_per_kg / MT->getMassInGrams(pu240);

        total_mass1 = v1_m_oxygen + v1_m_u235 + v1_m_u238;
        total_mass2 = v2_m_oxygen + v2_m_u235 + v2_m_pu240;
        total_moles1 = v1_a_oxygen + v1_a_u235 + v1_a_u238;
        total_moles2 = v2_a_oxygen + v2_a_u235 + v2_a_pu240;

        comp_map[oxygen] = v2_a_oxygen;
        comp_map[u235] = v2_a_u235;
        comp_map[pu240] = v2_a_pu240;

        vect1.setMass(oxygen, v1_m_oxygen);
        vect1.setMass(u235, v1_m_u235);
        vect1.setMass(u238, v1_m_u238);

        vect2 = CompMap(comp_map);
      } catch (std::exception err) {
        LOG(LEV_ERROR, "none!") << err.what();
        FAIL() << "An exception was thrown in the fixture SetUp.";
      }
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - Constructors- - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, ZeroMassOnInit) {

  EXPECT_DOUBLE_EQ(vect0.mass(), 0.0);
  EXPECT_DOUBLE_EQ(vect0.mass(1001), 0.0);

  EXPECT_DOUBLE_EQ(vect0.atomCount(), 0.0);
  EXPECT_DOUBLE_EQ(vect0.atomCount(1001), 0.0);

  CompMap empty_map;
  vect0 = IsoVector(empty_map);
  
  EXPECT_DOUBLE_EQ(vect0.mass(), 0.0);
  EXPECT_DOUBLE_EQ(vect0.mass(1001), 0.0);

  EXPECT_DOUBLE_EQ(vect0.atomCount(), 0.0);
  EXPECT_DOUBLE_EQ(vect0.atomCount(1001), 0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, DefaultConstructor) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, CompMapConstructor) {
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - Recipe handling - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, DISABLED_RecipeLoadAndRetrieve){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - Mass getters/setters  - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, GetTotalMass) {
  EXPECT_NO_THROW(vect1.mass());

  EXPECT_DOUBLE_EQ(vect1.mass(), total_mass1);
  EXPECT_DOUBLE_EQ(vect2.mass(), total_mass2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, GetIsotopeMassExceptions) {
  EXPECT_NO_THROW(vect1.mass(isotope_lower_limit));
  EXPECT_NO_THROW(vect1.mass(isotope_upper_limit));
  EXPECT_THROW(vect1.mass(isotope_lower_limit - 1), CycRangeException);
  EXPECT_THROW(vect1.mass(isotope_upper_limit + 1), CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, GetIsotopeMass) {
  EXPECT_DOUBLE_EQ(vect1.mass(isotope_not_present), 0);
  EXPECT_DOUBLE_EQ(vect2.mass(isotope_not_present), 0);

  EXPECT_DOUBLE_EQ(vect1.mass(oxygen), v1_m_oxygen);
  EXPECT_DOUBLE_EQ(vect1.mass(u235), v1_m_u235);
  EXPECT_DOUBLE_EQ(vect1.mass(u238), v1_m_u238);

  EXPECT_DOUBLE_EQ(vect2.mass(oxygen), v2_m_oxygen);
  EXPECT_DOUBLE_EQ(vect2.mass(u235), v2_m_u235);
  EXPECT_DOUBLE_EQ(vect2.mass(pu240), v2_m_pu240);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetTotalMassExceptions) {
  // should not be able to call setMass on a zero-mass IsoVector
  EXPECT_THROW(vect0.setMass(1), CycRangeException);
  EXPECT_THROW(vect0.setMass(0), CycRangeException);

  EXPECT_THROW(vect1.setMass(-1), CycRangeException);
  EXPECT_NO_THROW(vect1.setMass(2));
  EXPECT_NO_THROW(vect1.setMass(0));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetTotalMass) {
  // change the mass
  double factor = 2;

  // check both vectors because they were initialized using different bases
  // (atom vs mass).
  vect1.setMass(total_mass1 * factor);
  vect2.setMass(total_mass2 * factor);

  // check new total mass and atom count
  EXPECT_DOUBLE_EQ(vect1.mass(), total_mass1 * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(), total_mass2 * factor);

  // check constituent masses
  EXPECT_DOUBLE_EQ(vect1.mass(oxygen), v1_m_oxygen * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(u235), v1_m_u235 * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(u238), v1_m_u238 * factor);

  EXPECT_DOUBLE_EQ(vect2.mass(oxygen), v2_m_oxygen * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(u235), v2_m_u235 * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(pu240), v2_m_pu240 * factor);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetIsotopeMassExceptions) {
  EXPECT_NO_THROW(vect1.setMass(oxygen, 0));
  EXPECT_NO_THROW(vect1.setMass(oxygen, 2));
  EXPECT_THROW(vect1.setMass(oxygen, -1), CycRangeException);

  EXPECT_NO_THROW(vect1.setMass(isotope_lower_limit, 1));
  EXPECT_NO_THROW(vect1.setMass(isotope_upper_limit, 1));
  EXPECT_NO_THROW(vect1.setMass(oxygen, 1));
  EXPECT_THROW(vect1.setMass(isotope_lower_limit - 1, 1), CycRangeException);
  EXPECT_THROW(vect1.setMass(isotope_upper_limit + 1, 1), CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetIsotopeMass) {
  double factor = 2;

  // check both vectors because they were initialized using different bases
  // (atom vs mass).
  vect1.setMass(oxygen, v1_m_oxygen * factor);
  vect2.setMass(oxygen, v2_m_oxygen * factor);

  EXPECT_DOUBLE_EQ(vect1.mass(oxygen), v1_m_oxygen * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(u235), v1_m_u235);
  EXPECT_DOUBLE_EQ(vect1.mass(u238), v1_m_u238);
  EXPECT_DOUBLE_EQ(vect1.mass(), v1_m_oxygen * factor + v1_m_u235 + v1_m_u238);

  EXPECT_DOUBLE_EQ(vect2.mass(oxygen), v2_m_oxygen * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(u235), v2_m_u235);
  EXPECT_DOUBLE_EQ(vect2.mass(pu240), v2_m_pu240);
  EXPECT_DOUBLE_EQ(vect2.mass(), v2_m_oxygen * factor + v2_m_u235 + v2_m_pu240);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - -Atom Count getters/setters - - - - - - - - - - - - - - -    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, GetTotalAtomCount) {
  EXPECT_NO_THROW(vect1.atomCount());

  EXPECT_DOUBLE_EQ(vect1.atomCount(), total_moles1);
  EXPECT_DOUBLE_EQ(vect2.atomCount(), total_moles2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, GetIsotopeAtomCountExceptions) {
  EXPECT_NO_THROW(vect1.atomCount(isotope_lower_limit));
  EXPECT_NO_THROW(vect1.atomCount(isotope_upper_limit));
  EXPECT_THROW(vect1.atomCount(isotope_lower_limit - 1), CycRangeException);
  EXPECT_THROW(vect1.atomCount(isotope_upper_limit + 1), CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, GetIsotopeAtomCount) {
  EXPECT_DOUBLE_EQ(vect1.atomCount(isotope_not_present), 0);
  EXPECT_DOUBLE_EQ(vect2.atomCount(isotope_not_present), 0);

  EXPECT_DOUBLE_EQ(vect1.atomCount(oxygen), v1_a_oxygen);
  EXPECT_DOUBLE_EQ(vect1.atomCount(u235), v1_a_u235);
  EXPECT_DOUBLE_EQ(vect1.atomCount(u238), v1_a_u238);

  EXPECT_DOUBLE_EQ(vect2.atomCount(oxygen), v2_a_oxygen);
  EXPECT_DOUBLE_EQ(vect2.atomCount(u235), v2_a_u235);
  EXPECT_DOUBLE_EQ(vect2.atomCount(pu240), v2_a_pu240);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetTotalAtomCountExceptions) {
  // should not be able to call setMass on a zero-mass IsoVector
  EXPECT_THROW(vect0.setAtomCount(1), CycRangeException);
  EXPECT_THROW(vect0.setAtomCount(0), CycRangeException);

  EXPECT_THROW(vect1.setAtomCount(-1), CycRangeException);
  EXPECT_NO_THROW(vect1.setAtomCount(2));
  EXPECT_NO_THROW(vect1.setAtomCount(0));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetTotalAtomCount) {
  // change the mass
  double factor = 2;

  // check both vectors because they were initialized using different bases
  // (atom vs mass).
  vect1.setAtomCount(total_moles1 * factor);
  vect2.setAtomCount(total_moles2 * factor);

  // check new total atom count
  EXPECT_DOUBLE_EQ(vect1.atomCount(), total_moles1 * factor);
  EXPECT_DOUBLE_EQ(vect2.atomCount(), total_moles2 * factor);

  // check constituent atom counts
  EXPECT_DOUBLE_EQ(vect1.atomCount(oxygen), v1_a_oxygen * factor);
  EXPECT_DOUBLE_EQ(vect1.atomCount(u235), v1_a_u235 * factor);
  EXPECT_DOUBLE_EQ(vect1.atomCount(u238), v1_a_u238 * factor);

  EXPECT_DOUBLE_EQ(vect2.atomCount(oxygen), v2_a_oxygen * factor);
  EXPECT_DOUBLE_EQ(vect2.atomCount(u235), v2_a_u235 * factor);
  EXPECT_DOUBLE_EQ(vect2.atomCount(pu240), v2_a_pu240 * factor);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetIsotopeAtomCountExceptions) {
  EXPECT_NO_THROW(vect1.setAtomCount(oxygen, 0));
  EXPECT_NO_THROW(vect1.setAtomCount(oxygen, 2));
  EXPECT_THROW(vect1.setAtomCount(oxygen, -1), CycRangeException);

  EXPECT_NO_THROW(vect1.setAtomCount(isotope_lower_limit, 1));
  EXPECT_NO_THROW(vect1.setAtomCount(isotope_upper_limit, 1));
  EXPECT_NO_THROW(vect1.setAtomCount(oxygen, 1));
  EXPECT_THROW(vect1.setAtomCount(isotope_lower_limit - 1, 1), CycRangeException);
  EXPECT_THROW(vect1.setAtomCount(isotope_upper_limit + 1, 1), CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetIsotopeAtomCount) {
  double factor = 2;

  // check both vectors because they were initialized using different bases
  // (atom vs mass).
  vect1.setAtomCount(oxygen, v1_a_oxygen * factor);
  vect2.setAtomCount(oxygen, v2_a_oxygen * factor);

  EXPECT_DOUBLE_EQ(vect1.atomCount(oxygen), v1_a_oxygen * factor);
  EXPECT_DOUBLE_EQ(vect1.atomCount(u235), v1_a_u235);
  EXPECT_DOUBLE_EQ(vect1.atomCount(u238), v1_a_u238);
  EXPECT_DOUBLE_EQ(vect1.atomCount(), v1_a_oxygen * factor + v1_a_u235 + v1_a_u238);

  EXPECT_DOUBLE_EQ(vect2.atomCount(oxygen), v2_a_oxygen * factor);
  EXPECT_DOUBLE_EQ(vect2.atomCount(u235), v2_a_u235);
  EXPECT_DOUBLE_EQ(vect2.atomCount(pu240), v2_a_pu240);
  EXPECT_DOUBLE_EQ(vect2.atomCount(), v2_a_oxygen * factor + v2_a_u235 + v2_a_pu240);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - Operator overloading- - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorPlus) {
  IsoVector vect3 = vect1 + vect2;
  IsoVector vect4 = vect2 + vect1;
 
  // addition didn't affect vect1 and vect2
  EXPECT_DOUBLE_EQ(vect1.mass(), total_mass1);
  EXPECT_DOUBLE_EQ(vect1.mass(oxygen), v1_m_oxygen);
  EXPECT_DOUBLE_EQ(vect1.mass(u235), v1_m_u235);
  EXPECT_DOUBLE_EQ(vect1.mass(u238), v1_m_u238);
 
  EXPECT_DOUBLE_EQ(vect2.mass(), total_mass2);
  EXPECT_DOUBLE_EQ(vect2.mass(oxygen), v2_m_oxygen);
  EXPECT_DOUBLE_EQ(vect2.mass(u235), v2_m_u235);
  EXPECT_DOUBLE_EQ(vect2.mass(pu240), v2_m_pu240);
 
  // proper vect3 values
  EXPECT_DOUBLE_EQ(vect3.mass(), total_mass1 + total_mass2);
  EXPECT_DOUBLE_EQ(vect3.mass(oxygen), v1_m_oxygen + v2_m_oxygen);
  EXPECT_DOUBLE_EQ(vect3.mass(u235), v1_m_u235 + v2_m_u235);
  EXPECT_DOUBLE_EQ(vect3.mass(u238), v1_m_u238);
  EXPECT_DOUBLE_EQ(vect3.mass(pu240), v2_m_pu240);
 
  // proper vect4 values
  EXPECT_DOUBLE_EQ(vect4.mass(), total_mass1 + total_mass2);
  EXPECT_DOUBLE_EQ(vect4.mass(oxygen), v1_m_oxygen + v2_m_oxygen);
  EXPECT_DOUBLE_EQ(vect4.mass(u235), v1_m_u235 + v2_m_u235);
  EXPECT_DOUBLE_EQ(vect4.mass(u238), v1_m_u238);
  EXPECT_DOUBLE_EQ(vect4.mass(pu240), v2_m_pu240);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorMinusExceptions) {
  EXPECT_THROW(vect1 - vect2, CycRangeException);
  EXPECT_THROW(vect2 - vect1, CycRangeException);
  EXPECT_NO_THROW(vect1 - vect1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorMinus) {
  double tot_mass1 = total_mass1 - v1_m_u238;
  double tot_mass2 = total_mass2 - v2_m_pu240;
 
  vect1.setMass(u238, 0);
  vect2.setMass(pu240, 0);
 
  IsoVector vect3 = vect2 - vect1;
 
  // addition didn't affect vect1 and vect2
  EXPECT_DOUBLE_EQ(vect1.mass(), tot_mass1);
  EXPECT_DOUBLE_EQ(vect1.mass(oxygen), v1_m_oxygen);
  EXPECT_DOUBLE_EQ(vect1.mass(u235), v1_m_u235);
 
  EXPECT_DOUBLE_EQ(vect2.mass(), tot_mass2);
  EXPECT_DOUBLE_EQ(vect2.mass(oxygen), v2_m_oxygen);
  EXPECT_DOUBLE_EQ(vect2.mass(u235), v2_m_u235);
 
  // proper vect3 values
  EXPECT_DOUBLE_EQ(vect3.mass(), tot_mass2 - tot_mass1);
  EXPECT_DOUBLE_EQ(vect3.mass(oxygen), v2_m_oxygen - v1_m_oxygen);
  EXPECT_DOUBLE_EQ(vect3.mass(u235), v2_m_u235 - v1_m_u235);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorEQ_MisMatchIsotopes) {
  IsoVector vect3, vect4;
  vect3.setMass(u235, v1_m_u235);
  vect4.setMass(u238, v1_m_u238);

  // overlap with both having their own unique
  EXPECT_FALSE(vect1 == vect2);
  EXPECT_FALSE(vect2 == vect1);

  // overlap with one being enclosed by the other
  EXPECT_FALSE(vect3 == vect1);
  EXPECT_FALSE(vect1 == vect3);

  // no overlap
  EXPECT_FALSE(vect3 == vect4);
  EXPECT_FALSE(vect4 == vect3);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorEQ_ExactSame) {
  vect1 = vect2;

  EXPECT_TRUE(vect1 == vect1);
  EXPECT_TRUE(vect1 == vect2);
  EXPECT_TRUE(vect2 == vect1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorEQ_IsotopicDeviation) {
  vect1 = vect2;

  // the same with a devaition < EPS_KG for an isotope
  vect1.setMass(oxygen, v2_m_oxygen + EPS_KG * 0.9);
  EXPECT_TRUE(vect1 == vect2);
  EXPECT_TRUE(vect2 == vect1);

  // the same with a devaition > EPS_KG for an isotope
  vect1.setMass(oxygen, v2_m_oxygen + EPS_KG * 1.1);
  EXPECT_FALSE(vect1 == vect2);
  EXPECT_FALSE(vect2 == vect1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorEQ_TotalDeviation) {
  vect1 = vect2;

  // the same with a total devaition < EPS_KG
  vect1.setMass(oxygen, v2_m_oxygen + EPS_KG * 0.45);
  vect1.setMass(u235, v2_m_u235 + EPS_KG * 0.45);
  EXPECT_TRUE(fabs(vect1.mass(oxygen) - vect2.mass(oxygen)) < EPS_KG);
  EXPECT_TRUE(fabs(vect1.mass(u235) - vect2.mass(u235)) < EPS_KG);

  EXPECT_TRUE(vect1 == vect2);
  EXPECT_TRUE(vect2 == vect1);

  // the same with a total devaition < EPS_KG
  vect1.setMass(oxygen, v2_m_oxygen + EPS_KG * 0.55);
  vect1.setMass(u235, v2_m_u235 + EPS_KG * 0.55);
  EXPECT_TRUE(fabs(vect1.mass(oxygen) - vect2.mass(oxygen)) < EPS_KG);
  EXPECT_TRUE(fabs(vect1.mass(u235) - vect2.mass(u235)) < EPS_KG);

  EXPECT_FALSE(vect1 == vect2);
  EXPECT_FALSE(vect2 == vect1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - -Misc methods - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, MultBy) {
  double factor = 2.3;

  // check both vectors because they were initialized using different bases
  // (atom vs mass).
  vect1.multBy(factor);
  vect2.multBy(factor);

  EXPECT_DOUBLE_EQ(vect1.mass(), total_mass1 * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(oxygen), v1_m_oxygen * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(u235), v1_m_u235 * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(u238), v1_m_u238 * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(isotope_not_present), 0.0);

  EXPECT_DOUBLE_EQ(vect2.mass(), total_mass2 * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(oxygen), v2_m_oxygen * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(u235), v2_m_u235 * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(pu240), v2_m_pu240 * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(isotope_not_present), 0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, GetComp) {
  int isotope;
  CompMap comp2 = vect2.comp();

  ASSERT_EQ(comp_map.size(), comp2.size());

  std::map<int, double>::const_iterator iter = comp2.begin();
  while (iter != comp2.end()) {
    isotope = iter->first;
    EXPECT_DOUBLE_EQ(comp2[isotope], comp_map[isotope]);
    iter++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, IsZeroExceptions) {
  EXPECT_NO_THROW(vect1.isZero(isotope_lower_limit));
  EXPECT_NO_THROW(vect1.isZero(isotope_upper_limit));
  EXPECT_NO_THROW(vect1.isZero(u235));

  EXPECT_THROW(vect1.isZero(isotope_lower_limit - 1), CycRangeException);
  EXPECT_THROW(vect1.isZero(isotope_upper_limit + 1), CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, IsZero) {
  EXPECT_TRUE(vect1.isZero(isotope_not_present));

  vect1.setMass(isotope_not_present, EPS_KG * 0.9);

  EXPECT_TRUE(vect1.isZero(isotope_not_present));

  vect1.setMass(isotope_not_present, EPS_KG * 1.1);

  EXPECT_FALSE(vect1.isZero(isotope_not_present));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - - Decay Testing - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    

class IsoVectorDecayTest : public ::testing::Test {
  protected:
    int u235, am241, th228, pb208;
    IsoVector vect_decay1, vect_decay2;
    long int u235_halflife;
    int th228_halflife;
    double vect_decay_size;

    virtual void SetUp() {
      u235 = 92235;
      am241 = 95241;
      th228 = 90228;
      pb208 = 82208;
      u235_halflife = 8445600000; // approximate, in months
      th228_halflife = 2 * 11; // approximate, in months
      vect_decay_size = 1.0;

      vect_decay1.setAtomCount(u235, vect_decay_size);

      vect_decay2.setAtomCount(u235, 1.0);
      vect_decay2.setAtomCount(th228, 1.0);
      vect_decay2.setAtomCount(vect_decay_size);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorDecayTest, ExecuteDecay) {
  ASSERT_NEAR(vect_decay1.atomCount(u235), 1.0, 0.001);
  ASSERT_NEAR(vect_decay2.atomCount(u235),  0.5, 0.001);
  ASSERT_NEAR(vect_decay2.atomCount(th228), 0.5, 0.001);

  vect_decay1.executeDecay(2);
  EXPECT_NEAR(vect_decay1.atomCount(u235), 1, 0.001);

  vect_decay2.executeDecay(th228_halflife);
  EXPECT_NEAR(vect_decay2.atomCount(th228),  0.25, 0.01);
  EXPECT_NEAR(vect_decay2.atomCount(pb208),  0.25, 0.01);
}

