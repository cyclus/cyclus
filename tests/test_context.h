// test_context.h
#ifndef CYCLUS_TESTS_TEST_CONTEXT_H_
#define CYCLUS_TESTS_TEST_CONTEXT_H_

#include "context.h"
#include "recorder.h"
#include "timer.h"
#include "bid.h"
#include "request.h"

#include "resource_helpers.h"
#include "test_agents/test_facility.h"

using cyclus::Bid;
using cyclus::Request;
using test_helpers::get_mat;

namespace cyclus {

class FakeContext: public Context {
 public:
  FakeContext(cyclus::Timer* ti, cyclus::Recorder* rec)
    : cyclus::Context(ti, rec), t_(0) {};

  virtual int time() { return t_; };
  virtual void time(int t) { t_ = t; };

 private:
  int t_;
};

class TestContext {
 public:
  TestContext() {
    ctx_ = new FakeContext(&ti_, &rec_);
    trader_ = new TestFacility(ctx_);
    mat_ = get_mat();
  }

  ~TestContext() {
    std::vector<Bid<Material>*>::iterator bit;
    for (bit = bids_.begin(); bit != bids_.end(); ++bit) {
      delete *bit;
    }
    std::vector<Request<Material>*>::iterator rit;
    for (rit = reqs_.begin(); rit != reqs_.end(); ++rit) {
      delete *rit;
    }
    delete trader_;
    delete ctx_;
  }

  Request<Material>* NewReq(TestFacility* trader = NULL,
                            std::string commod = "") {
    trader = (trader == NULL) ? trader_ : trader;
    Request<Material>* req = Request<Material>::Create(mat_, trader, commod);
    reqs_.push_back(req);
    return req;
  }

  Bid<Material>* NewBid(Request<Material>* req = NULL,
                        TestFacility* trader = NULL) {
    trader = (trader == NULL) ? trader_ : trader;
    req = (req == NULL) ? NewReq(trader) : req;
    Bid<Material>* bid = Bid<Material>::Create(req, mat_, trader);
    bids_.push_back(bid);
    return bid;
  }

  FakeContext* get() {return ctx_;}
  Timer* timer() {return &ti_;}
  Recorder* recorder() {return &rec_;}
  TestFacility* trader() {return trader_;}
  Material::Ptr mat() {return mat_;}

 private:
  Timer ti_;
  Recorder rec_;
  FakeContext* ctx_;
  TestFacility* trader_;
  Material::Ptr mat_;
  std::vector<Request<Material>*> reqs_;
  std::vector<Bid<Material>*> bids_;
};

}  // namespace cyclus

#endif  // CYCLUS_TESTS_TEST_CONTEXT_H_
