#include <string>

#include <gtest/gtest.h>
#include "error.h"

#include "package.h"
#include "test_context.h"
#include "test_agents/test_facility.h"

using cyclus::Package;
using cyclus::TransportUnit;

TEST(PackageTests, CreatePackage) {

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

TEST(PackageTests, Unpackaged) {
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

TEST(PackageTests, GetPackageFillMass) {
    double min = 0.3;
    double max = 0.9;
    double tight_min = 0.85;

    Package::Ptr p = Package::Create("foo", min, max, "first");
    Package::Ptr q = Package::Create("bar", min, max, "equal");
    Package::Ptr r = Package::Create("baz", tight_min, max, "equal");

    std::pair<double, int> p_fill, q_fill, r_fill;

    double exp;

    // no fit
    double no_fit = 0.05;
    p_fill = p->GetFillMass(no_fit);;
    EXPECT_EQ(0, p_fill.first);
    EXPECT_EQ(0, p_fill.second);

    q_fill = q->GetFillMass(no_fit);
    EXPECT_EQ(0, q_fill.first);
    EXPECT_EQ(0, q_fill.second);

    // perfect fit
    double perfect_fit = 0.9;
    p_fill = p->GetFillMass(perfect_fit);
    EXPECT_EQ(perfect_fit, p_fill.first);
    EXPECT_EQ(1, p_fill.second);

    q_fill = q->GetFillMass(perfect_fit);
    EXPECT_EQ(perfect_fit, q_fill.first);
    EXPECT_EQ(1, q_fill.second);

    // partial fit 
    double partial_fit = 1;

    p_fill = p->GetFillMass(partial_fit);
    EXPECT_EQ(max, p_fill.first);
    EXPECT_EQ(1, p_fill.second);

    q_fill = q->GetFillMass(partial_fit);
    exp = partial_fit / 2;
    EXPECT_EQ(exp, q_fill.first);
    EXPECT_EQ(2, q_fill.second);

    r_fill = r->GetFillMass(partial_fit);
    EXPECT_EQ(max, r_fill.first);
    EXPECT_EQ(1, r_fill.second);

    // two full packages for equal, only one for 
    double two_packages = 1.4;

    p_fill = p->GetFillMass(two_packages);
    EXPECT_EQ(max, p_fill.first);
    EXPECT_EQ(1, p_fill.second);

    q_fill = q->GetFillMass(two_packages);
    exp = two_packages / 2;
    EXPECT_EQ(exp, q_fill.first);
}

TEST(PackageTests, CreateTransportUnit) {

    std::string p_exp_name = "foo";
    int exp_min = 0.1;
    int exp_max = 0.9;
    std::string exp_strat = "first";

    TransportUnit::Ptr p = TransportUnit::Create(p_exp_name, exp_min, exp_max,
                                                 exp_strat);

    EXPECT_EQ(p_exp_name, p->name());
    EXPECT_EQ(exp_min, p->fill_min());
    EXPECT_EQ(exp_max, p->fill_max());
    EXPECT_EQ(exp_strat, p->strategy());

    EXPECT_NE(TransportUnit::unrestricted_name(), p->name());

    std::string q_exp_name = "bar";
    TransportUnit::Ptr q = TransportUnit::Create(q_exp_name, exp_min, exp_max,
                                                 exp_strat);
    EXPECT_NE(TransportUnit::unrestricted_name(), q->name());
    EXPECT_NE(q->name(), p->name());
}

TEST(PackageTests, Unrestricted) {
    EXPECT_EQ("unrestricted", TransportUnit::unrestricted_name());
}

TEST(PackageTests, InvalidTransportUnit) {
    // can't create package with name "unrestricted"
    EXPECT_THROW(TransportUnit::Create("unrestricted", 0, 1, "first"),
                 cyclus::ValueError);
    // can't have negative min/max
    EXPECT_THROW(TransportUnit::Create("foo", -1, 1, "first"),
                 cyclus::ValueError);
    EXPECT_THROW(TransportUnit::Create("foo", 0, -1, "first"),
                 cyclus::ValueError);
    // can't have min bigger than max
    EXPECT_THROW(TransportUnit::Create("foo", 100, 1, "first"),
                 cyclus::ValueError);
}

TEST(PackageTests, GetTransportUnitFillMass) {
    int min = 3;
    int max = 9;
    int tight_min = 8;

    TransportUnit::Ptr p = TransportUnit::Create("foo", min, max, "first");
    TransportUnit::Ptr q = TransportUnit::Create("bar", min, max, "equal");
    TransportUnit::Ptr r = TransportUnit::Create("baz", tight_min, max, "equal");

    double exp;

    int no_fit = 1;
    EXPECT_EQ(0, p->GetTransportUnitFill(no_fit));
    EXPECT_EQ(0, q->GetTransportUnitFill(no_fit));
    EXPECT_EQ(0, r->GetTransportUnitFill(no_fit));

    int perfect_fit = 9;
    EXPECT_EQ(perfect_fit, p->GetTransportUnitFill(perfect_fit));
    EXPECT_EQ(perfect_fit, q->GetTransportUnitFill(perfect_fit));
    EXPECT_EQ(perfect_fit, r->GetTransportUnitFill(perfect_fit));

    int two_full_packages = 18;
    EXPECT_EQ(max, p->GetTransportUnitFill(perfect_fit));
    EXPECT_EQ(max, q->GetTransportUnitFill(perfect_fit));
    EXPECT_EQ(max, r->GetTransportUnitFill(perfect_fit));

    int partial_fit = 11;
    EXPECT_EQ(max, p->GetTransportUnitFill(partial_fit));
    exp = std::floor(partial_fit / 2);
    EXPECT_EQ(exp, q->GetTransportUnitFill(partial_fit));
    EXPECT_EQ(max, r->GetTransportUnitFill(partial_fit));

    int two_partial_packages = 17;
    EXPECT_EQ(max, p->GetTransportUnitFill(two_partial_packages));
    exp = std::floor(two_partial_packages / 2);
    EXPECT_EQ(exp, q->GetTransportUnitFill(two_partial_packages));
}

TEST(PackageTests, MaxShippablePackages) {
    int p_min = 3;
    int p_max = 4;
    int q_min = 6;
    int q_max = 8;
    TransportUnit::Ptr p_first = TransportUnit::Create("foo", p_min, p_max, "first");
    TransportUnit::Ptr p_equal = TransportUnit::Create("bar", p_min, p_max, "equal");
    TransportUnit::Ptr p_hybrid = TransportUnit::Create("baz", p_min, p_max, "hybrid");

    TransportUnit::Ptr q_first = TransportUnit::Create("foobar", q_min, q_max, "first");
    TransportUnit::Ptr q_equal = TransportUnit::Create("foobaz", q_min, q_max, "equal");
    TransportUnit::Ptr q_hybrid = TransportUnit::Create("foobaz", q_min, q_max, "hybrid");
    
    int exp;

    int none_fit = 2;
    EXPECT_EQ(0, p_first->MaxShippablePackages(none_fit));
    EXPECT_EQ(0, p_equal->MaxShippablePackages(none_fit));
    EXPECT_EQ(0, p_hybrid->MaxShippablePackages(none_fit));
    EXPECT_EQ(0, q_first->MaxShippablePackages(none_fit));
    EXPECT_EQ(0, q_equal->MaxShippablePackages(none_fit));
    EXPECT_EQ(0, q_hybrid->MaxShippablePackages(none_fit));

    int all_fit = 8;
    EXPECT_EQ(all_fit, p_first->MaxShippablePackages(all_fit));
    EXPECT_EQ(all_fit, p_equal->MaxShippablePackages(all_fit));
    EXPECT_EQ(all_fit, q_first->MaxShippablePackages(all_fit));
    EXPECT_EQ(all_fit, q_equal->MaxShippablePackages(all_fit));
    EXPECT_EQ(all_fit, q_hybrid->MaxShippablePackages(all_fit));

    // all can ship for p_hybrid, but will go 4-3-3. Only 8 ship for other
    int partial = 10; 
    EXPECT_EQ(p_max * 2, p_first->MaxShippablePackages(partial));
    EXPECT_EQ(p_min * 3, p_equal->MaxShippablePackages(partial));
    EXPECT_EQ(partial, p_hybrid->MaxShippablePackages(partial));
    EXPECT_EQ(q_max, q_first->MaxShippablePackages(partial));
    EXPECT_EQ(q_max, q_equal->MaxShippablePackages(partial));
    EXPECT_EQ(q_max, q_hybrid->MaxShippablePackages(partial));

    int partial2 = 14; // all ship p_hybrid. q, others ship 12. q_equal and 
    // q_hybrid can ship all
    EXPECT_EQ(p_max * 3, p_first->MaxShippablePackages(partial2));
    EXPECT_EQ(p_max * 3, p_equal->MaxShippablePackages(partial2));
    EXPECT_EQ(partial2, p_hybrid->MaxShippablePackages(partial2));
    EXPECT_EQ(q_max, q_first->MaxShippablePackages(partial2));
    EXPECT_EQ(partial2, q_equal->MaxShippablePackages(partial2));
    EXPECT_EQ(partial2, q_hybrid->MaxShippablePackages(partial2));

    int partial3 = 15; // all ship in hybrid and p_equal
    EXPECT_EQ(p_max * 3, p_first->MaxShippablePackages(partial3));
    EXPECT_EQ(partial3, p_equal->MaxShippablePackages(partial3));
    EXPECT_EQ(partial3, p_hybrid->MaxShippablePackages(partial3));
    EXPECT_EQ(q_max, q_first->MaxShippablePackages(partial3));
    EXPECT_EQ((q_max-1) * 2, q_equal->MaxShippablePackages(partial3));
    EXPECT_EQ(partial3, q_hybrid->MaxShippablePackages(partial3));
}