#include <string>

#include <gtest/gtest.h>
#include "error.h"

#include "package.h"
#include "test_context.h"
#include "test_agents/test_facility.h"

using cyclus::Package;

TEST(PackageTests, Create) {

    std::string p_exp_name = "foo";
    double exp_min = 0.1;
    double exp_max = 0.9;
    std::string exp_strat = "first";

    Package::Ptr p = Package::Create(p_exp_name, exp_min, exp_max, exp_strat);

    EXPECT_EQ(p_exp_name, p->name());
    EXPECT_DOUBLE_EQ(exp_min, p->fill_min());
    EXPECT_DOUBLE_EQ(exp_max, p->fill_max());
    EXPECT_EQ(exp_strat, p->strategy());

    EXPECT_NE(Package::unpackaged_name(), p->name());

    std::string q_exp_name = "bar";
    Package::Ptr q = Package::Create(q_exp_name, exp_min, exp_max, exp_strat);
    EXPECT_NE(Package::unpackaged_name(), q->name());
    EXPECT_NE(q->name(), p->name());

}

TEST(PackageTests, UnpackagedID) {
    EXPECT_EQ("unpackaged", Package::unpackaged_name());
}

TEST(PackageTests, InvalidPackage) {
    // can't create package with name "unpackaged"
    EXPECT_THROW(Package::Create("unpackaged", 0, 1, "first"), cyclus::ValueError);
    // can't have negative min/max
    EXPECT_THROW(Package::Create("foo", -1, 1, "first"), cyclus::ValueError);
    EXPECT_THROW(Package::Create("foo", 0, -1, "first"), cyclus::ValueError);
    // can't have min bigger than max
    EXPECT_THROW(Package::Create("foo", 100, 1, "first"), cyclus::ValueError);
}

TEST(PackageTests, GetFillMass) {
    double min = 0.3;
    double max = 0.9;
    double tight_min = 0.85;

    Package::Ptr p = Package::Create("foo", min, max, "first");
    Package::Ptr q = Package::Create("bar", min, max, "equal");
    Package::Ptr r = Package::Create("bar", tight_min, max, "equal");

    double exp;

    double no_fit = 0.05;
    EXPECT_EQ(0, p->GetFillMass(no_fit));
    EXPECT_EQ(0, q->GetFillMass(no_fit));

    double perfect_fit = 0.9;
    EXPECT_EQ(perfect_fit, p->GetFillMass(perfect_fit));
    EXPECT_EQ(perfect_fit, q->GetFillMass(perfect_fit));

    double partial_fit = 1;
    EXPECT_EQ(max, p->GetFillMass(partial_fit));
    exp = partial_fit / 2;
    EXPECT_EQ(exp, q->GetFillMass(partial_fit));
    EXPECT_EQ(max, r->GetFillMass(partial_fit));

    double two_packages = 1.4;
    EXPECT_EQ(max, p->GetFillMass(two_packages));
    exp = two_packages / 2;
    EXPECT_EQ(exp, q->GetFillMass(two_packages));
}