
#include <gtest/gtest.h>

#include "iso_vector.h"
#include "material.h"
#include "resource_buff.h"
#include "error.h"
#include "cyc_limits.h"
#include "logger.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class ResourceBuffTest : public ::testing::Test {
protected:
  int oxygen, u235, u238, pu240;
  double m_oxygen, m_u235, m_u238, m_pu240;
  cyclus::IsoVector vect1_, vect2_;
  
  cyclus::Resource::Ptr mat1_, mat2_;
  double mass1, mass2;
  cyclus::Manifest mats;
  
  cyclus::ResourceBuff store_; // default constructed mat store
  cyclus::ResourceBuff filled_store_;
  
  double neg_cap, zero_cap, cap, low_cap;
  double exact_qty; // mass in filled_store_
  double exact_qty_under; // mass in filled_store - 0.9*cyclus::eps_rsrc()
  double exact_qty_over; // mass in filled_store + 0.9*cyclus::eps_rsrc()
  double over_qty;  // mass in filled_store - 1.1*cyclus::eps_rsrc()
  double under_qty; // mass in filled_store + 1.1*cyclus::eps_rsrc()
  double overeps, undereps;

  virtual void SetUp() {
    using cyclus::CompMap;
    using cyclus::CompMapPtr;
    using cyclus::IsoVector;
    using cyclus::Material;
    using cyclus::Resource;
    try {
      // composition
      CompMapPtr comp;
      oxygen = 8001;
      u235 = 92235;
      u238 = 92238;
      pu240 = 94240;
      m_oxygen = 1;
      m_u235 = 10;
      m_u238 = 100;
      m_pu240 = 100;

      // vectors
      vect1_ = IsoVector();
      comp = vect1_.comp();
      (*comp)[oxygen] = m_oxygen;
      (*comp)[u235] = m_u235;
      (*comp)[u238] = m_u238;
      vect2_ = IsoVector();
      comp = vect2_.comp();
      (*comp)[oxygen] = m_oxygen;
      (*comp)[u235] = m_u235;
      (*comp)[pu240] = m_pu240;

      // materials
      mass1 = 111;
      mat1_ = Resource::Ptr(new Material(vect1_));
      mat1_->SetQuantity(mass1);
      mass2 = 222;
      mat2_ = Resource::Ptr(new Material(vect2_));
      mat2_->SetQuantity(mass2);
      mats.push_back(mat1_);
      mats.push_back(mat2_);

      neg_cap = -1;
      zero_cap = 0;
      cap = mat1_->quantity()+mat2_->quantity()+1; // should be higher than mat1+mat2 masses
      low_cap = mat1_->quantity()+mat2_->quantity()-1; // should be lower than mat1_mat2 masses

      undereps = 0.9 * cyclus::eps_rsrc();
      overeps = 1.1 * cyclus::eps_rsrc();
      exact_qty = mat1_->quantity();
      exact_qty_under = exact_qty - undereps;
      exact_qty_over = exact_qty + undereps;
      under_qty = exact_qty - overeps;
      over_qty = exact_qty + overeps;

      filled_store_.SetCapacity(cap);
      filled_store_.PushOne(mat1_);
      filled_store_.PushOne(mat2_);
    } catch (std::exception err) {
      LOG(cyclus::LEV_ERROR, "MSTest") << err.what();
      FAIL() << "An exception was thrown in the fixture SetUp.";
    }
  }
};
