#ifndef CYCLUS_TESTS_MATERIAL_TESTS_H_
#define CYCLUS_TESTS_MATERIAL_TESTS_H_

#include <gtest/gtest.h>

#include "composition.h"
#include "material.h"
#include "env.h"
#include "context.h"
#include "pyhooks.h"
#include "test_agents/test_facility.h"
#include "recorder.h"
#include "timer.h"


namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MaterialTest : public ::testing::Test {
 protected:
  Nuc u235_, am241_, th228_, pb208_, pu239_, sr89_;
  int one_g_;  // grams
  Composition::Ptr test_comp_, diff_comp_;
  double test_size_, fraction;
  Material::Ptr test_mat_, two_test_mat_, ten_test_mat_;
  Material::Ptr diff_mat_;
  Material::Ptr default_mat_;
  Material::Ptr tracked_mat_;
  Material::Ptr tracked_mat_no_decay_;
  long int u235_halflife_;
  int th228_halflife_;
  double u235_g_per_mol_;

  cyclus::Timer ti;
  cyclus::Timer ti_day_timestep;
  cyclus::Recorder rec;
  cyclus::Context* ctx;
  TestFacility* fac;
  cyclus::Context* ctx_no_decay;
  TestFacility* fac_no_decay;
  cyclus::Context* ctx_day_timestep;
  TestFacility* fac_day_timestep;
  // dur 100, y0 = 2015, m0=1, handle="", d="never"
  SimInfo si;
  SimInfo si_day_timestep;

  virtual void SetUp() {
    PyStart();
    ctx = new cyclus::Context(&ti, &rec);
    fac = new TestFacility(ctx);

    // Set up the one day time step context
    int one_day = 86400;
    si_day_timestep = SimInfo(100, 2015, 1, "", "manual");
    si_day_timestep.dt = one_day;
    ctx_day_timestep = new cyclus::Context(&ti_day_timestep, &rec);
    ctx_day_timestep->InitSim(si_day_timestep);
    fac_day_timestep = new TestFacility(ctx_day_timestep);

    si = SimInfo(100, 2015, 1, "", "never");
    ctx_no_decay = new cyclus::Context(&ti, &rec);
    ctx_no_decay->InitSim(si);
    fac_no_decay = new TestFacility(ctx_no_decay);
    Env::SetNucDataPath();

    // composition set up
    u235_ = 922350000;
    am241_ = 952410000;
    th228_ = 902280000;
    pb208_ = 822080000;
    sr89_ = 380890000;
    test_size_ = 10 * units::g;
    fraction = 2.0 / 3.0;

    // composition creation
    CompMap v;
    v[u235_] = 1;
    test_comp_ = Composition::CreateFromMass(v);

    v[u235_] = 1;
    v[pb208_] = 1;
    v[am241_] = 1;
    v[sr89_] = 1;
    diff_comp_ = Composition::CreateFromMass(v);

    default_mat_ = Material::CreateUntracked(0 * units::g, test_comp_);
    test_mat_ = Material::CreateUntracked(test_size_, test_comp_);
    two_test_mat_ = Material::CreateUntracked(2 * test_size_, test_comp_);
    ten_test_mat_ = Material::CreateUntracked(10 * test_size_, test_comp_);
    diff_mat_ = Material::CreateUntracked(test_size_, diff_comp_);

    // tracked material
    tracked_mat_ = Material::Create(fac, 1000, diff_comp_);
    tracked_mat_no_decay_ = Material::Create(fac_no_decay, 1000, diff_comp_);

    // test info
    u235_g_per_mol_ = 235.044;
    u235_halflife_ = 8445600000;  // approximate, in months
    th228_halflife_ = 2 * 11;  // approximate, in months
  }

  virtual void TearDown() {
    delete ctx;
    delete ctx_no_decay;
    PyStop();
  }
};

}  // namespace cyclus

#endif  // CYCLUS_TESTS_MATERIAL_TESTS_H_
