#include "building_manager.h"

#include "CoinPackedVector.hpp"

namespace cyclus {
namespace toolkit {

BuildOrder::BuildOrder(int n, Builder* b, CommodityProducer* cp)
    : number(n), builder(b), producer(cp) {}

std::vector<BuildOrder> BuildingManager::MakeBuildDecision(Commodity& commodity,
                                                           double demand) {
  std::vector<BuildOrder> orders;
  if (demand > 0) {
    OsiCbcSolverInterface iface;
    ProgTranslator::Context ctx;
    std::map<CommodityProducer*, Builder*> p_to_b;
    std::map<int, CommodityProducer*> idx_to_p;
    SetUp_(iface, ctx, p_to_b, idx_to_p, commodity, demand);
    Solve_(iface, ctx, p_to_b, idx_to_p, orders);
  }
  return orders;
}

void BuildingManager::SetUp_(OsiCbcSolverInterface& iface,
                             ProgTranslator::Context& ctx,
                             std::map<CommodityProducer*, Builder*>& p_to_b,
                             std::map<int, CommodityProducer*>& idx_to_p,
                             Commodity& commodity,
                             double demand) {
  CoinPackedVector caps;
  std::set<Builder*>::iterator bit;
  std::set<CommodityProducer*>::iterator pit;
  CommodityProducer* p;
  Builder* b;
  int idx = 0;
  double inf = iface.getInfinity();
  for (bit = builders_.begin(); bit != builders_.end(); ++bit) {
    b = *bit;
    for (pit = b->producers().begin(); pit != b->producers().end(); ++pit) {
      p = *pit;
      if (p->Produces(commodity)) {
        ctx.obj_coeffs.push_back(p->Cost(commodity));
        caps.insert(idx, p->Capacity(commodity));
        ctx.col_lbs.push_back(0);
        ctx.col_ubs.push_back(inf);
        p_to_b[p] = b;
        idx_to_p[idx] = p;
        idx++;
      }
    }
  }
  ctx.row_ubs.push_back(inf);
  ctx.row_lbs.push_back(demand);
  ctx.m.setDimensions(0, ctx.col_ubs.size());
  ctx.m.appendRow(caps);    
}

void BuildingManager::Solve_(OsiCbcSolverInterface& iface,
                             ProgTranslator::Context& ctx,
                             std::map<CommodityProducer*, Builder*>& p_to_b,
                             std::map<int, CommodityProducer*>& idx_to_p,
                             std::vector<BuildOrder>& orders) {
  int nvar = ctx.col_ubs.size();
  iface.setObjSense(1.0); // minimize
  iface.loadProblem(ctx.m, &ctx.col_lbs[0], &ctx.col_ubs[0],
                    &ctx.obj_coeffs[0], &ctx.row_lbs[0], &ctx.row_ubs[0]);
  for (int i = 0; i != nvar; i++) {
    iface.setInteger(i);
  }
  iface.initialSolve();
  iface.branchAndBound();
  
  const double* sol = iface.getColSolution();
  int n;
  CommodityProducer* p;
  Builder* b;
  for (int i = 0; i != nvar; i++) {
    n = static_cast<int>(sol[i]);
    if (n > 0) {
      p = idx_to_p[i];
      b = p_to_b[p];
      orders.push_back(BuildOrder(n, b, p));
    }
  }        
}

} // namespace toolkit
} // namespace cyclus
