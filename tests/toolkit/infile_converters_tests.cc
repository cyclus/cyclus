#include <gtest/gtest.h>

#include "cyclus.h"

std::string MakeInput() {
  return std::string(
          "<simulation>"
          " <control>"
          "  <duration>1200</duration>"
          "  <startmonth>1</startmonth>"
          "  <startyear>2000</startyear>"
          "  <simstart>0</simstart>"
          " </control>"
          "  <recipe>"
          "    <name>used_uo2_50gwd</name>"
          "    <basis>mass</basis>"
          "    <unit>assembly</unit>"
          "    <total>1000</total>"
          "    <nuclide>"
          "      <id>922340000</id>"
          "      <comp>0.01</comp>"
          "    </nuclide>"
          "    <nuclide>"
          "      <id>U-235</id>"
          "      <comp>0.02</comp>"
          "    </nuclide>"
          "    <nuclide>"
          "      <id>922380000</id>"
          "      <comp>0.97</comp>"
          "    </nuclide>"
          "    <nuclide>"
          "      <id>080160000</id>"
          "      <comp>0.13</comp>"
          "    </nuclide>"
          "  </recipe>"
          "  <!-- facility prototypes -->"
          "  <facility>"
          "    <name>fac</name>"
          "    <config>"
          "      <TestFacility>"
          "        <in_commods>"
          "          <val>waste</val>"
          "        </in_commods>"
          "        <capacity>200000</capacity>"
          "      </TestFacility>"
          "    </config>"
          "  </facility>"
          "  <!-- region definitions -->"
          "  <region>"
          "    <name>reg</name>"
          "    <config>"
          "      <TestRegion>"
          "        <commodity_name>power</commodity_name>"
          "      </TestRegion>"
          "    </config>"
          "    <!-- institution definitions -->"
          "    <institution>"
          "      <name>inst</name>"
          "      <config>"
          "        <TestInst>"
          "          <prototype>Mine</prototype>"
          "        </TestInst>"
          "      </config>"
          "    </institution>"
          "    <!-- end institution definitions -->"
          "  </region>"
          "</simulation>");
}

TEST(InfileConverters, JsonXmlRoundTrip) {
  using std::string;
  string inp = MakeInput();
  string j1 = cyclus::toolkit::XmlToJson(inp);
  string x1 = cyclus::toolkit::JsonToXml(j1);
  string j2 = cyclus::toolkit::XmlToJson(x1);
  string x2 = cyclus::toolkit::JsonToXml(j2);

  EXPECT_STREQ(j1.c_str(), j2.c_str());
  EXPECT_STREQ(x1.c_str(), x2.c_str());
}
