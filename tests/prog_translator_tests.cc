#include <gtest/gtest.h>

#include <utility>

#include "CoinModel.hpp"
#include "CoinPackedMatrix.hpp"
#include "OsiClpSolverInterface.hpp"
#include "OsiSolverInterface.hpp"

#include "coin_helpers.h"
#include "equality_helpers.h"
#include "exchange_graph.h"
#include "logger.h"
#include "prog_translator.h"
#include "solver_factory.h"

namespace cyclus {

TEST(ProgTranslatorTests, xlatecaps) {
  int ncaps = 2;
  std::vector<CoinPackedVector> cap_rows;
  cap_rows.resize(2);
  
  ProgTranslator::Context ctx;
  int faux_id = 0;
  bool request = false;
  ExchangeNodeGroup::Ptr grp(new ExchangeNodeGroup());
  grp->AddCapacity(5, LTEQ);
  grp->AddCapacity(6, GTEQ);

  ProgTranslator::XlateCaps(grp.get(), request, faux_id, cap_rows, ctx);
  EXPECT_EQ(ctx.row_lbs[0], 0);
  EXPECT_EQ(ctx.row_ubs[0], 5);
  EXPECT_EQ(ctx.row_lbs[1], 6);
  EXPECT_EQ(ctx.row_ubs[1], ctx.inf);
}

TEST(ProgTranslatorTests, translation) {
  // Logger::ReportLevel() = Logger::ToLogLevel("LEV_DEBUG2");
  SolverFactory sf("cbc");
  OsiSolverInterface* iface = sf.get();
  CoinMessageHandler h;
  h.setLogLevel(0);
  iface->passInMessageHandler(&h);
  double inf = iface->getInfinity();

  int narcs = 5;
  int nfaux = 2;
  int nrows = 8;
  int nexcl = 3;

  double prefs[] = {0.2, 1.2, 4, 5, 1.3};
  double ucaps_a_0[] = {0.5, 0.4};
  double ucaps_a_3[] = {0.3, 0.6};
  double ucaps_b_1[] = {0.9};
  double ucaps_b_2[] = {0.8};
  double ucaps_b_4[] = {0.7};
  double ucaps_c_0[] = {0.7};
  double ucaps_c_1[] = {0.8};
  double ucaps_c_2[] = {0.9};
  double ucaps_d_3[] = {1.3, 2};
  double ucaps_d_4[] = {1.4, 1.9};
  double ucaps_5[] = {1.0};
  double ucaps_6[] = {1.0};

  double dem_a[] = {5.0, 4.9};
  double dem_b[] = {4.0};
  double sup_c[] = {10.0};
  double sup_d[] = {10.1, 9.1};

  int excl_arcs[] = {1, 2, 4};
  double excl_flow[] = {0, 2, 2, 0, 2, 0, 0};

  std::vector<double> obj_coeffs;
  for (int i = 0; i != narcs; i++) {
    obj_coeffs.push_back((excl_flow[i] != 0) ?
                         excl_flow[i] / prefs[i] : 1 / prefs[i]);
  }

  
  double cost_add = 1;
  double max_obj_coeff = 1 / 0.2;  // 1 / prefs[0]
  double min_row_coeff = 0.3;  // ucaps_a_3
  double max_cost = max_obj_coeff / min_row_coeff + cost_add;
  for (int i = 0; i != nfaux; i++) {
    obj_coeffs.push_back(max_cost);
  }

  ExchangeNode::Ptr a0(new ExchangeNode());
  ExchangeNode::Ptr a1(new ExchangeNode());
  ExchangeNode::Ptr b0(new ExchangeNode(excl_flow[1], true));
  ExchangeNode::Ptr b1(new ExchangeNode(excl_flow[1], true));
  ExchangeNode::Ptr c0(new ExchangeNode());
  ExchangeNode::Ptr c1(new ExchangeNode());
  ExchangeNode::Ptr c2(new ExchangeNode());
  ExchangeNode::Ptr d0(new ExchangeNode());
  ExchangeNode::Ptr d1(new ExchangeNode());

  Arc x0(a0, c0);
  Arc x1(b0, c1);
  Arc x2(b1, c2);
  Arc x3(a1, d0);
  Arc x4(b1, d1);

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

  RequestGroup::Ptr a(new RequestGroup());  // new RequestGroup(dem_a[0])?
  a->AddExchangeNode(a0);
  a->AddExchangeNode(a1);
  a->AddCapacity(dem_a[0]);
  a->AddCapacity(dem_a[1]);
  RequestGroup::Ptr b(new RequestGroup());  // new RequestGroup(dem_b[0])?
  b->AddExchangeNode(b0);
  b->AddExchangeNode(b1);
  b->AddCapacity(dem_b[0]);
  ExchangeNodeGroup::Ptr c(new ExchangeNodeGroup());
  c->AddExchangeNode(c0);
  c->AddExchangeNode(c1);
  c->AddExchangeNode(c2);
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

  bool excl = true;
  ProgTranslator pt(&g, iface, excl, max_cost);
  ASSERT_NO_THROW(pt.Translate());

  // test non-coin xlate members
  double col_lbs[] = {0, 0, 0, 0, 0, 0, 0};
  double col_ubs[] = {inf, 1, 1, inf, 1, inf, inf};
  double row_lbs[] = {0, 0, 0, dem_a[0], dem_a[1], dem_b[0], 0, 0};
  double row_ubs[] = {sup_c[0], sup_d[0], sup_d[1], inf, inf, inf, 1, 1};
  array_double_eq(&obj_coeffs[0], &pt.ctx().obj_coeffs[0], narcs + nfaux, "obj");
  array_double_eq(col_ubs, &pt.ctx().col_ubs[0], narcs + nfaux, "col_ub");
  array_double_eq(col_lbs, &pt.ctx().col_lbs[0], narcs + nfaux, "col_lb");
  array_double_eq(row_ubs, &pt.ctx().row_ubs[0], nrows, "row_ub");
  array_double_eq(row_lbs, &pt.ctx().row_lbs[0], nrows, "row_lb");

  for (int i = 0; i != 7; i++) {
    EXPECT_DOUBLE_EQ(col_lbs[i], pt.ctx().col_lbs[i]);
  }

  // test coin xlate members
  CoinPackedMatrix m(false, 0, 0);
  m.setDimensions(0, narcs + nfaux);

  int row_ind_0[] = {0, 1, 2};
  double row_val_0[] = {ucaps_c_0[0],
                        ucaps_c_1[0] * excl_flow[1],
                        ucaps_c_2[0] * excl_flow[2]};
  m.appendRow(3, row_ind_0, row_val_0);

  int row_ind_1[] = {3, 4};
  double row_val_1[] = {ucaps_d_3[0],
                        ucaps_d_4[0] * excl_flow[4]};
  m.appendRow(2, row_ind_1, row_val_1);

  int row_ind_2[] = {3, 4};
  double row_val_2[] = {ucaps_d_3[1],
                        ucaps_d_4[1] * excl_flow[4]};
  m.appendRow(2, row_ind_2, row_val_2);

  int row_ind_3[] = {0, 3, 5};
  double row_val_3[] = {ucaps_a_0[0], ucaps_a_3[0], 1};
  m.appendRow(3, row_ind_3, row_val_3);

  int row_ind_4[] = {0, 3, 5};
  double row_val_4[] = {ucaps_a_0[1], ucaps_a_3[1], 1};
  m.appendRow(3, row_ind_4, row_val_4);

  int row_ind_5[] = {1, 2, 4, 6};
  double row_val_5[] = {ucaps_b_1[0] * excl_flow[1],
                        ucaps_b_2[0] * excl_flow[2],
                        ucaps_b_4[0] * excl_flow[4],
                        1};
  m.appendRow(4, row_ind_5, row_val_5);

  int row_ind_6[] = {1};
  double row_val_6[] = {1};
  m.appendRow(1, row_ind_6, row_val_6);

  int row_ind_7[] = {2, 4};
  double row_val_7[] = {1, 1};
  m.appendRow(2, row_ind_7, row_val_7);

  EXPECT_TRUE(m.isEquivalent2(pt.ctx().m));

  // test population
  EXPECT_NO_THROW(pt.Populate());

  for (int i = 0; i != nexcl; i++) {
    EXPECT_TRUE(iface->isInteger(excl_arcs[i]));
  }

  // verify problem instance
  OsiClpSolverInterface checkface;
  checkface.loadProblem(m, &col_lbs[0], &col_ubs[0],
                        &obj_coeffs[0], &row_lbs[0], &row_ubs[0]);
  for (int i = 0; i != nexcl; i++) {
    checkface.setInteger(excl_arcs[i]);
  }
  EXPECT_EQ(0, differentAgent(*iface, checkface));
  differentAgent(*iface, checkface);

  checkface.passInMessageHandler(&h);
  checkface.setObjSense(1.0);
  checkface.initialSolve();
  checkface.branchAndBound();

  // verify solution
  EXPECT_NO_THROW(SolveProg(iface));
  const double* soln = iface->getColSolution();
  const double* check = checkface.getColSolution();
  array_double_eq(soln, check, narcs + nfaux, "soln");

  // validate solution
  double x1_flow = excl_flow[1];
  double x2_flow = excl_flow[2];
  double x3_flow = sup_d[1] / ucaps_d_3[1];  // 4.55
  double x4_flow = 0;
  double x0_flow = (dem_a[0] - ucaps_a_3[0] * x3_flow) /  ucaps_a_0[0];  // 7.27

  // first faux arc
  double x5_flow = 0;
  // second faux arc
  double x6_flow = dem_b[0] -
                   ucaps_b_1[0] * excl_flow[1] -
                   ucaps_b_2[0] * excl_flow[2];  // 0.6;

  EXPECT_DOUBLE_EQ(soln[0], x0_flow);
  EXPECT_EQ(soln[1], 1);
  EXPECT_EQ(soln[2], 1);
  EXPECT_DOUBLE_EQ(soln[3], x3_flow);
  EXPECT_EQ(soln[4], 0);
  EXPECT_DOUBLE_EQ(soln[5], 0);
  EXPECT_DOUBLE_EQ(soln[6], x6_flow);

  // check back translation
  ASSERT_NO_THROW(pt.FromProg());
  const std::vector<Match>& matches = g.matches();
  ASSERT_EQ(4, matches.size());
  pair_double_eq(matches[0], std::pair<Arc, double>(x0, x0_flow));
  pair_double_eq(matches[1], std::pair<Arc, double>(x1, x1_flow));
  pair_double_eq(matches[2], std::pair<Arc, double>(x2, x2_flow));
  pair_double_eq(matches[3], std::pair<Arc, double>(x3, x3_flow));

  delete iface;
}

}  // namespace cyclus
