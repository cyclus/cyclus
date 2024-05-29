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
    int pq_min = 3;
    int pq_max = 4;
    int r_min = 6;
    int r_max = 8;
    TransportUnit::Ptr p = TransportUnit::Create("foo", pq_min, pq_max, "first");
    TransportUnit::Ptr q = TransportUnit::Create("bar", pq_min, pq_max, "equal");
    TransportUnit::Ptr r = TransportUnit::Create("baz", r_min, r_max, "equal");

    int exp;

    int none_fit = 2;
    EXPECT_EQ(0, p->MaxShippablePackages(none_fit));
    EXPECT_EQ(0, q->MaxShippablePackages(none_fit));
    EXPECT_EQ(0, r->MaxShippablePackages(none_fit));

    int all_fit = 8;
    EXPECT_EQ(all_fit, p->MaxShippablePackages(all_fit));
    EXPECT_EQ(all_fit, q->MaxShippablePackages(all_fit));
    EXPECT_EQ(all_fit, r->MaxShippablePackages(all_fit));

    // all can ship for p/q, but will go 4-3-3, only 8 ship for r
    int partial = 10; 
    EXPECT_EQ(pq_max * 2, p->MaxShippablePackages(partial));
    EXPECT_EQ(partial, q->MaxShippablePackages(partial));
    EXPECT_EQ(r_max, r->MaxShippablePackages(partial));

    int partial2 = 14;
    EXPECT_EQ(pq_max * 3, p->MaxShippablePackages(partial2));
    EXPECT_EQ(partial2, q->MaxShippablePackages(partial2));
    EXPECT_EQ(partial2, r->MaxShippablePackages(partial2));

}