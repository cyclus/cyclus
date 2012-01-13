#include <map>
#include <gtest/gtest.h>
#include "IsoVector.h"
#include "InputXML.h"
#include "MassTable.h"
#include "CycException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class IsoVectorTest : public ::testing::Test {
  protected:
    IsoVector vect0, vect1, vect2;

    int v1_e1, v1_e2, v1_e3;
    int v2_e1, v2_e2, v2_e3;

    double v1_m1, v1_m2, v1_m3;
    double v2_m1, v2_m2, v2_m3;

    double v1_a1, v1_a2, v1_a3;
    double v2_a1, v2_a2, v2_a3;

    double total_mass1;
    double total_mass2;
    double total_moles1;
    double total_moles2;

    int isotope_lower_limit;
    int isotope_upper_limit;
    int isotope_not_present;

    CompMap comp_map;

    // decay stuff
    int u235, am241, th228, pb208;
    IsoVector vect_decay1, vect_decay2;
    long int u235_halflife;
    int th228_halflife;
    double vect_decay_size;

    virtual void SetUp() {
      double grams_per_kg = 1000;
      isotope_lower_limit = 1001;
      isotope_upper_limit = 1182949;
      isotope_not_present = 9001;

      v1_e1 = 8001;
      v1_e2 = 92235;
      v1_e3 = 92238;

      v2_e1 = 8001;
      v2_e2 = 92235;
      v2_e3 = 94240;

      v1_m1 = 1;
      v1_m2 = 10;
      v1_m3 = 100;

      v2_m1 = 2;
      v2_m2 = 20;
      v2_m3 = 2000;

      v1_a1 = v1_m1 * grams_per_kg / MT->getMassInGrams(v1_e1);
      v1_a2 = v1_m2 * grams_per_kg / MT->getMassInGrams(v1_e2);
      v1_a3 = v1_m3 * grams_per_kg / MT->getMassInGrams(v1_e3);

      v2_a1 = v2_m1 * grams_per_kg / MT->getMassInGrams(v2_e1);
      v2_a2 = v2_m2 * grams_per_kg / MT->getMassInGrams(v2_e2);
      v2_a3 = v2_m3 * grams_per_kg / MT->getMassInGrams(v2_e3);

      total_mass1 = v1_m1 + v1_m2 + v1_m3;
      total_mass2 = v2_m1 + v2_m2 + v2_m3;
      total_moles1 = v1_a1 + v1_a2 + v1_a3;
      total_moles2 = v2_a1 + v2_a2 + v2_a3;

      comp_map[v2_e1] = v2_a1;
      comp_map[v2_e2] = v2_a2;
      comp_map[v2_e3] = v2_a3;

      vect1.setMass(v1_e1, v1_m1);
      vect1.setMass(v1_e2, v1_m2);
      vect1.setMass(v1_e3, v1_m3);

      vect2 = CompMap(comp_map);

      // decay stuff
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
  EXPECT_TRUE(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, CompMapConstructor) {
  EXPECT_TRUE(true);
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
  EXPECT_TRUE(true);
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

  EXPECT_DOUBLE_EQ(vect1.mass(v1_e1), v1_m1);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e2), v1_m2);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e3), v1_m3);

  EXPECT_DOUBLE_EQ(vect2.mass(v2_e1), v2_m1);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e2), v2_m2);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e3), v2_m3);
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
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e1), v1_m1 * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e2), v1_m2 * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e3), v1_m3 * factor);

  EXPECT_DOUBLE_EQ(vect2.mass(v2_e1), v2_m1 * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e2), v2_m2 * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e3), v2_m3 * factor);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetIsotopeMassExceptions) {
  EXPECT_NO_THROW(vect1.setMass(v1_e1, 0));
  EXPECT_NO_THROW(vect1.setMass(v1_e1, 2));
  EXPECT_THROW(vect1.setMass(v1_e1, -1), CycRangeException);

  EXPECT_NO_THROW(vect1.setMass(isotope_lower_limit, 1));
  EXPECT_NO_THROW(vect1.setMass(isotope_upper_limit, 1));
  EXPECT_NO_THROW(vect1.setMass(v1_e1, 1));
  EXPECT_THROW(vect1.setMass(isotope_lower_limit - 1, 1), CycRangeException);
  EXPECT_THROW(vect1.setMass(isotope_upper_limit + 1, 1), CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetIsotopeMass) {
  double factor = 2;

  // check both vectors because they were initialized using different bases
  // (atom vs mass).
  vect1.setMass(v1_e1, v1_m1 * factor);
  vect2.setMass(v2_e1, v2_m1 * factor);

  EXPECT_DOUBLE_EQ(vect1.mass(v1_e1), v1_m1 * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e2), v1_m2);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e3), v1_m3);
  EXPECT_DOUBLE_EQ(vect1.mass(), v1_m1 * factor + v1_m2 + v1_m3);

  EXPECT_DOUBLE_EQ(vect2.mass(v2_e1), v2_m1 * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e2), v2_m2);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e3), v2_m3);
  EXPECT_DOUBLE_EQ(vect2.mass(), v2_m1 * factor + v2_m2 + v2_m3);
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

  EXPECT_DOUBLE_EQ(vect1.atomCount(v1_e1), v1_a1);
  EXPECT_DOUBLE_EQ(vect1.atomCount(v1_e2), v1_a2);
  EXPECT_DOUBLE_EQ(vect1.atomCount(v1_e3), v1_a3);

  EXPECT_DOUBLE_EQ(vect2.atomCount(v2_e1), v2_a1);
  EXPECT_DOUBLE_EQ(vect2.atomCount(v2_e2), v2_a2);
  EXPECT_DOUBLE_EQ(vect2.atomCount(v2_e3), v2_a3);
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
  EXPECT_DOUBLE_EQ(vect1.atomCount(v1_e1), v1_a1 * factor);
  EXPECT_DOUBLE_EQ(vect1.atomCount(v1_e2), v1_a2 * factor);
  EXPECT_DOUBLE_EQ(vect1.atomCount(v1_e3), v1_a3 * factor);

  EXPECT_DOUBLE_EQ(vect2.atomCount(v2_e1), v2_a1 * factor);
  EXPECT_DOUBLE_EQ(vect2.atomCount(v2_e2), v2_a2 * factor);
  EXPECT_DOUBLE_EQ(vect2.atomCount(v2_e3), v2_a3 * factor);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetIsotopeAtomCountExceptions) {
  EXPECT_NO_THROW(vect1.setAtomCount(v1_e1, 0));
  EXPECT_NO_THROW(vect1.setAtomCount(v1_e1, 2));
  EXPECT_THROW(vect1.setAtomCount(v1_e1, -1), CycRangeException);

  EXPECT_NO_THROW(vect1.setAtomCount(isotope_lower_limit, 1));
  EXPECT_NO_THROW(vect1.setAtomCount(isotope_upper_limit, 1));
  EXPECT_NO_THROW(vect1.setAtomCount(v1_e1, 1));
  EXPECT_THROW(vect1.setAtomCount(isotope_lower_limit - 1, 1), CycRangeException);
  EXPECT_THROW(vect1.setAtomCount(isotope_upper_limit + 1, 1), CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, SetIsotopeAtomCount) {
  double factor = 2;

  // check both vectors because they were initialized using different bases
  // (atom vs mass).
  vect1.setAtomCount(v1_e1, v1_a1 * factor);
  vect2.setAtomCount(v2_e1, v2_a1 * factor);

  EXPECT_DOUBLE_EQ(vect1.atomCount(v1_e1), v1_a1 * factor);
  EXPECT_DOUBLE_EQ(vect1.atomCount(v1_e2), v1_a2);
  EXPECT_DOUBLE_EQ(vect1.atomCount(v1_e3), v1_a3);
  EXPECT_DOUBLE_EQ(vect1.atomCount(), v1_a1 * factor + v1_a2 + v1_a3);

  EXPECT_DOUBLE_EQ(vect2.atomCount(v2_e1), v2_a1 * factor);
  EXPECT_DOUBLE_EQ(vect2.atomCount(v2_e2), v2_a2);
  EXPECT_DOUBLE_EQ(vect2.atomCount(v2_e3), v2_a3);
  EXPECT_DOUBLE_EQ(vect2.atomCount(), v2_a1 * factor + v2_a2 + v2_a3);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - Operator overloading- - - - - - - - - - - - - - - - - -    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorPlus) {
  IsoVector vect3 = vect1 + vect2;
  IsoVector vect4 = vect2 + vect1;
 
  // addition didn't affect vect1 and vect2
  EXPECT_DOUBLE_EQ(vect1.mass(), total_mass1);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e1), v1_m1);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e2), v1_m2);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e3), v1_m3);
 
  EXPECT_DOUBLE_EQ(vect2.mass(), total_mass2);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e1), v2_m1);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e2), v2_m2);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e3), v2_m3);
 
  // proper vect3 values
  EXPECT_DOUBLE_EQ(vect3.mass(), total_mass1 + total_mass2);
  EXPECT_DOUBLE_EQ(vect3.mass(v1_e1), v1_m1 + v2_m1);
  EXPECT_DOUBLE_EQ(vect3.mass(v1_e2), v1_m2 + v2_m2);
  EXPECT_DOUBLE_EQ(vect3.mass(v1_e3), v1_m3);
  EXPECT_DOUBLE_EQ(vect3.mass(v2_e3), v2_m3);
 
  // proper vect4 values
  EXPECT_DOUBLE_EQ(vect4.mass(), total_mass1 + total_mass2);
  EXPECT_DOUBLE_EQ(vect4.mass(v1_e1), v1_m1 + v2_m1);
  EXPECT_DOUBLE_EQ(vect4.mass(v1_e2), v1_m2 + v2_m2);
  EXPECT_DOUBLE_EQ(vect4.mass(v1_e3), v1_m3);
  EXPECT_DOUBLE_EQ(vect4.mass(v2_e3), v2_m3);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorMinusExceptions) {
  EXPECT_THROW(vect1 - vect2, CycRangeException);
  EXPECT_THROW(vect2 - vect1, CycRangeException);
  EXPECT_NO_THROW(vect1 - vect1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorMinus) {
  double tot_mass1 = total_mass1 - v1_m3;
  double tot_mass2 = total_mass2 - v2_m3;
 
  vect1.setMass(v1_e3, 0);
  vect2.setMass(v2_e3, 0);
 
  IsoVector vect3 = vect2 - vect1;
 
  // addition didn't affect vect1 and vect2
  EXPECT_DOUBLE_EQ(vect1.mass(), tot_mass1);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e1), v1_m1);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e2), v1_m2);
 
  EXPECT_DOUBLE_EQ(vect2.mass(), tot_mass2);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e1), v2_m1);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e2), v2_m2);
 
  // proper vect3 values
  EXPECT_DOUBLE_EQ(vect3.mass(), tot_mass2 - tot_mass1);
  EXPECT_DOUBLE_EQ(vect3.mass(v1_e1), v2_m1 - v1_m1);
  EXPECT_DOUBLE_EQ(vect3.mass(v1_e2), v2_m2 - v1_m2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorEQ_MisMatchIsotopes) {
  IsoVector vect3, vect4;
  vect3.setMass(v1_e2, v1_m2);
  vect4.setMass(v1_e3, v1_m3);

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
  vect1.setMass(v2_e1, v2_m1 + EPS_KG * 0.9);
  EXPECT_TRUE(vect1 == vect2);
  EXPECT_TRUE(vect2 == vect1);

  // the same with a devaition > EPS_KG for an isotope
  vect1.setMass(v2_e1, v2_m1 + EPS_KG * 1.1);
  EXPECT_FALSE(vect1 == vect2);
  EXPECT_FALSE(vect2 == vect1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(IsoVectorTest, OperatorEQ_TotalDeviation) {
  vect1 = vect2;

  // the same with a total devaition < EPS_KG
  vect1.setMass(v2_e1, v2_m1 + EPS_KG * 0.45);
  vect1.setMass(v2_e2, v2_m2 + EPS_KG * 0.45);
  EXPECT_TRUE(fabs(vect1.mass(v2_e1) - vect2.mass(v2_e1)) < EPS_KG);
  EXPECT_TRUE(fabs(vect1.mass(v2_e2) - vect2.mass(v2_e2)) < EPS_KG);

  EXPECT_TRUE(vect1 == vect2);
  EXPECT_TRUE(vect2 == vect1);

  // the same with a total devaition < EPS_KG
  vect1.setMass(v2_e1, v2_m1 + EPS_KG * 0.55);
  vect1.setMass(v2_e2, v2_m2 + EPS_KG * 0.55);
  EXPECT_TRUE(fabs(vect1.mass(v2_e1) - vect2.mass(v2_e1)) < EPS_KG);
  EXPECT_TRUE(fabs(vect1.mass(v2_e2) - vect2.mass(v2_e2)) < EPS_KG);

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
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e1), v1_m1 * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e2), v1_m2 * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(v1_e3), v1_m3 * factor);
  EXPECT_DOUBLE_EQ(vect1.mass(isotope_not_present), 0.0);

  EXPECT_DOUBLE_EQ(vect2.mass(), total_mass2 * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e1), v2_m1 * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e2), v2_m2 * factor);
  EXPECT_DOUBLE_EQ(vect2.mass(v2_e3), v2_m3 * factor);
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
  EXPECT_NO_THROW(vect1.isZero(v1_e2));

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
TEST_F(IsoVectorTest, Decay){
  IsoVector::loadDecayInfo();
  ASSERT_NEAR(vect_decay1.atomCount(u235), 1.0, 0.001);
  ASSERT_NEAR(vect_decay2.atomCount(u235),  0.5, 0.001);
  ASSERT_NEAR(vect_decay2.atomCount(th228), 0.5, 0.001);

  vect_decay1.executeDecay(2);
  EXPECT_NEAR(vect_decay1.atomCount(u235), 1, 0.001);

  vect_decay2.executeDecay(th228_halflife);
  EXPECT_NEAR(vect_decay2.atomCount(th228),  0.25, 0.01);
  EXPECT_NEAR(vect_decay2.atomCount(pb208),  0.25, 0.01);
}
