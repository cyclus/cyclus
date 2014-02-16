#include <gtest/gtest.h>

#include "OsiSolverInterface.hpp"

#include "exchange_graph.h"
#include "equality_helpers.h"
#include "prog_translator.h"
#include "solver_factory.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ProgTranslatorTests, translation) {
  int narcs = 5;
  int nfaux = 2;
  int nrows = 8;
  
  double prefs [] = {0.2, 1.2, 4, 5, 1.3, -1, -1};
  double ucaps_a_0 [] = {0.5, 0.4};
  double ucaps_a_3 [] = {0.3, 0.6};
  double ucaps_b_1 [] = {0.9};
  double ucaps_b_2 [] = {0.8};
  double ucaps_b_4 [] = {0.7};
  double ucaps_c_0 [] = {0.7};
  double ucaps_c_1 [] = {0.8};
  double ucaps_c_2 [] = {0.9};
  double ucaps_d_3 [] = {1.3, 2};
  double ucaps_d_4 [] = {1.4, 1.9};
  double ucaps_5 [] = {1.0};
  double ucaps_6 [] = {1.0};

  double dem_a [] = {5.0, 4.9};
  double dem_b [] = {4.0};
  double sup_c [] = {10.0};
  double sup_d [] = {10.1, 9.1};
  
  int excl_arcs [] = {1, 2, 4};
  double excl_flow [] = {0, 2, 2, 0, 2, 0, 0};

  std::vector<double> obj_coeffs;
  for (int i = 0; i != narcs + nfaux; i++) {
    obj_coeffs.push_back((excl_flow[i] != 0) ?
                         excl_flow[i] * prefs[i] : prefs[i]);
  }
  
  ExchangeNode::Ptr a0(new ExchangeNode());
  ExchangeNode::Ptr a1(new ExchangeNode());
  ExchangeNode::Ptr b0(new ExchangeNode());
  ExchangeNode::Ptr b1(new ExchangeNode());
  ExchangeNode::Ptr c0(new ExchangeNode());
  ExchangeNode::Ptr c1(new ExchangeNode());
  ExchangeNode::Ptr c2(new ExchangeNode());
  ExchangeNode::Ptr d0(new ExchangeNode());
  ExchangeNode::Ptr d1(new ExchangeNode());

  Arc x0(a0, c0);
  Arc x1(b0, c1, true, excl_flow[1]);
  Arc x2(b1, c2, true, excl_flow[2]);
  Arc x3(a1, d0);
  Arc x4(b1, d1, true, excl_flow[4]);

  a0->unit_capacities[x0] = std::vector<double>(
      ucaps_a_0, ucaps_a_0 + sizeof(ucaps_a_0) / sizeof(ucaps_a_0[0]) );
  a1->unit_capacities[x3] = std::vector<double>(
      ucaps_a_3, ucaps_a_3 + sizeof(ucaps_a_3) / sizeof(ucaps_a_3[0]) );
  b0->unit_capacities[x1] = std::vector<double>(
      ucaps_b_1, ucaps_b_1 + sizeof(ucaps_b_1) / sizeof(ucaps_b_1[0]) );
  b1->unit_capacities[x2] = std::vector<double>(
      ucaps_b_2, ucaps_b_2 + sizeof(ucaps_b_2) / sizeof(ucaps_b_2[0]) );
  b1->unit_capacities[x4] = std::vector<double>(
      ucaps_b_4, ucaps_b_4 + sizeof(ucaps_b_4) / sizeof(ucaps_b_4[0]) );
  c0->unit_capacities[x0] = std::vector<double>(
      ucaps_c_0, ucaps_c_0 + sizeof(ucaps_c_0) / sizeof(ucaps_c_0[0]) );
  c1->unit_capacities[x1] = std::vector<double>(
      ucaps_c_1, ucaps_c_1 + sizeof(ucaps_c_1) / sizeof(ucaps_c_1[0]) );
  c2->unit_capacities[x2] = std::vector<double>(
      ucaps_c_2, ucaps_c_2 + sizeof(ucaps_c_2) / sizeof(ucaps_c_2[0]) );
  d0->unit_capacities[x3] = std::vector<double>(
      ucaps_d_3, ucaps_d_3 + sizeof(ucaps_d_3) / sizeof(ucaps_d_3[0]) );
  d1->unit_capacities[x4] = std::vector<double>(
      ucaps_d_4, ucaps_d_4 + sizeof(ucaps_d_4) / sizeof(ucaps_d_4[0]) );

  a0->prefs[x0] = prefs[0];
  b0->prefs[x1] = prefs[1];
  b1->prefs[x2] = prefs[2];
  a1->prefs[x3] = prefs[3];
  b1->prefs[x4] = prefs[4];
  
  RequestGroup::Ptr a(new RequestGroup()); // new RequestGroup(dem_a[0])?
  a->AddExchangeNode(a0);
  a->AddExchangeNode(a1);
  a->AddCapacity(dem_a[0]);
  a->AddCapacity(dem_a[1]);
  RequestGroup::Ptr b(new RequestGroup()); // new RequestGroup(dem_b[0])?
  b->AddExchangeNode(b0);
  b->AddExchangeNode(b1);
  b->AddCapacity(dem_b[0]);
  ExchangeNodeGroup::Ptr c(new ExchangeNodeGroup());
  c->AddExchangeNode(c0);
  c->AddExchangeNode(c1);
  c->AddCapacity(sup_c[0]);
  ExchangeNodeGroup::Ptr d(new ExchangeNodeGroup());
  d->AddExchangeNode(d0);
  d->AddExchangeNode(d1);
  d->AddCapacity(sup_d[0]);
  d->AddCapacity(sup_d[1]);

  ExchangeGraph g;
  g.AddRequestGroup(a);
  g.AddRequestGroup(b);
  g.AddSupplyGroup(c);
  g.AddSupplyGroup(d);
  g.AddArc(x0);
  g.AddArc(x1);
  g.AddArc(x2);
  g.AddArc(x3);
  g.AddArc(x4);

  SolverFactory sf;
  sf.solver_t("clp");
  OsiSolverInterface* iface = sf.get();
  ProgTranslator pt(&g, iface);
  EXPECT_NO_THROW(pt.Translate());

  double inf = iface->getInfinity();
  double col_lbs [] = {0, 0, 0, 0, 0, 0, 0};
  double col_ubs [] = {inf, 1, 1, inf, 1, inf, inf};
  double row_lbs [] = {0, 0, 0, dem_a[0], dem_a[1], dem_b[0], 1, 1};
  double row_ubs [] = {sup_c[0], sup_d[0], sup_d[1], 0, 0, 0, 0, 0};
  array_double_eq(&obj_coeffs[0], &pt.ctx().obj_coeffs[0], narcs + nfaux);
  array_double_eq(col_ubs, &pt.ctx().col_ubs[0], narcs + nfaux);
  array_double_eq(col_lbs, &pt.ctx().col_lbs[0], narcs + nfaux);
  array_double_eq(row_ubs, &pt.ctx().row_ubs[0], nrows);
  array_double_eq(row_lbs, &pt.ctx().row_lbs[0], nrows);
  
  delete iface;
};

} // namespace cyclus
