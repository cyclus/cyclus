#include "mock_facility.h"
#include "context.h"
#include "trade.h"
#include "material.h"
#include "model.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TestSender : public MockFacility {
 public:
  TestSender(cyclus::Context* ctx, bool adjust = false)
    : MockFacility(ctx),
      cyclus::Model(ctx),
      i(0),
      adjust(adjust) { };
  
  virtual cyclus::Model* Clone() {
    TestSender* m = new TestSender(*this);
    m->InitFrom(this);
    m->i = i;
    m->adjust = adjust;
    context()->RegisterTicker(m);
    return m;
  };

  cyclus::Material::Ptr OfferMatlTrade(const cyclus::Trade<cyclus::Material>& trade) {
    ++i;
    cyclus::Material::Ptr mat = trade.bid->offer();
    if (adjust) {
      mat = mat->ExtractQty(mat->quantity() - 0.1);
    }
    return mat;
  }

  bool adjust;
  int i;
};
