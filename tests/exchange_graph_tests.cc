#include <gtest/gtest.h>

#include "cyc_limits.h"
#include "error.h"

#include "exchange_graph.h"

using cyclus::Arc;
using cyclus::ExchangeGraph;
using cyclus::Match;
using cyclus::ExchangeNode;
using cyclus::ExchangeNodeGroup;
using cyclus::RequestGroup;
using std::vector;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeGroups) {
  ExchangeNode::Ptr n(new ExchangeNode());
  ExchangeNodeGroup s;
  s.AddExchangeNode(n);
  EXPECT_EQ(&s, n->group);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ReqGroups) {
  double q = 1.5;
  RequestGroup r;
  EXPECT_EQ(0, r.qty());
  r = RequestGroup(q);
  EXPECT_EQ(q, r.qty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeCapThrow) {
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);
  EXPECT_THROW(Capacity(m, a), cyclus::StateError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeNoCap) {
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);
  
  ExchangeNodeGroup ugroup;
  ugroup.AddExchangeNode(m);
    
  EXPECT_DOUBLE_EQ(Capacity(m, a), std::numeric_limits<double>::max());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeCaps1) {
  double ncap = 1.0;
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);
  n->unit_capacities[a].push_back(ncap);

  double scap = 1.5;
  ExchangeNodeGroup s;
  s.AddCapacity(scap);
  s.AddExchangeNode(n);

  EXPECT_DOUBLE_EQ(scap, Capacity(n, a));  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeCaps2) {
  double qty = 1.5;
  double ucap = 1.7;
  double cap = 5;
  
  double acap[] = {10, cap, 3, 1};
  vector<double> caps (acap, acap + sizeof(acap) / sizeof(acap[0]) );
  
  double aucap[] = {2.1, ucap, .07, .01};
  vector<double> ucaps (aucap, aucap + sizeof(aucap) / sizeof(aucap[0]) );
  
  vector<double> exp;
  for (int i = 0; i < caps.size(); i++) {
    exp.push_back(caps[i] - ucaps[i] * qty);
  }

  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);
  n->unit_capacities[a] = ucaps;

  ExchangeNodeGroup s;
  for (int i = 0; i < caps.size(); i++) {
    s.AddCapacity(caps[i]);
  }
  s.AddExchangeNode(n);
  double min_exp = cap / ucap;
  EXPECT_EQ(min_exp, Capacity(n, a));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeCaps3) {
  double ncap = 1.0;
  double qty = 0.5;

  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode(0.5));
  EXPECT_EQ(n->qty, 0.5);
  
  Arc a(m, n);
  n->unit_capacities[a].push_back(ncap);

  double scap = 1.5;
  ExchangeNodeGroup s;
  s.AddCapacity(scap);
  s.AddExchangeNode(n);

  EXPECT_DOUBLE_EQ(qty, Capacity(n, a));  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ArcCap) {
  double uval = 1.0;
  double vval = 0.5;
  
  ExchangeNode::Ptr u(new ExchangeNode());
  
  ExchangeNode::Ptr v(new ExchangeNode());
  Arc a(u, v);
  
  u->unit_capacities[a].push_back(uval);
  v->unit_capacities[a].push_back(vval);
  
  ExchangeNodeGroup ugroup;
  ugroup.AddExchangeNode(u);
  double ucap = uval * 1.5;
  ugroup.AddCapacity(ucap);
  
  ExchangeNodeGroup vgroup;
  vgroup.AddExchangeNode(v);
  double vcap = vval;
  vgroup.AddCapacity(vcap);
  
  
  EXPECT_DOUBLE_EQ(Capacity(a), 1.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddReqGroup) {
  RequestGroup::Ptr prs(new RequestGroup());
  ExchangeGraph g;
  g.AddRequestGroup(prs);
  EXPECT_EQ(g.request_groups().at(0), prs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddSuppGroup) {
  ExchangeNodeGroup::Ptr pss(new ExchangeNodeGroup());
  ExchangeGraph g;
  g.AddSupplyGroup(pss);
  EXPECT_EQ(g.supply_groups().at(0), pss);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddArc1) {
  ExchangeGraph g;
  
  ExchangeNode::Ptr u(new ExchangeNode());  
  ExchangeNode::Ptr v(new ExchangeNode());

  Arc a(u, v);

  Arc arr[] = {a};
  vector<Arc> exp (arr, arr + sizeof(arr) / sizeof(arr[0]) );

  g.AddArc(a);
  EXPECT_EQ(exp, g.node_arc_map().at(u));
  EXPECT_EQ(exp, g.node_arc_map().at(v));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddArc2) {
  ExchangeGraph g;
  
  ExchangeNode::Ptr u(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());
  ExchangeNode::Ptr w(new ExchangeNode());
  ExchangeNode::Ptr x(new ExchangeNode());

  Arc a1(u, v);
  Arc a2(u, w);
  Arc a3(x, w);

  Arc arru[] = {a1, a2};
  vector<Arc> expu (arru, arru + sizeof(arru) / sizeof(arru[0]) );
  
  Arc arrv[] = {a1};
  vector<Arc> expv (arrv, arrv + sizeof(arrv) / sizeof(arrv[0]) );
  
  Arc arrw[] = {a2, a3};
  vector<Arc> expw (arrw, arrw + sizeof(arrw) / sizeof(arrw[0]) );
  
  Arc arrx[] = {a3};
  vector<Arc> expx (arrx, arrx + sizeof(arrx) / sizeof(arrx[0]) );

  g.AddArc(a1);
  g.AddArc(a2);
  g.AddArc(a3);
  
  EXPECT_EQ(expu, g.node_arc_map().at(u));
  EXPECT_EQ(expv, g.node_arc_map().at(v));
  EXPECT_EQ(expw, g.node_arc_map().at(w));
  EXPECT_EQ(expx, g.node_arc_map().at(x));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddMatch) {
  ExchangeGraph g;

  double uval = 1.0;
  double vval = 0.5;
  ExchangeNode::Ptr u(new ExchangeNode());  
  ExchangeNode::Ptr v(new ExchangeNode());
  Arc a(u, v);
  
  u->unit_capacities[a].push_back(uval);
  v->unit_capacities[a].push_back(vval);
  
  double large = 500;
  
  ExchangeNodeGroup::Ptr ugroup(new ExchangeNodeGroup());
  ugroup->AddExchangeNode(u);
  double ucap = uval * 500;
  ugroup->AddCapacity(ucap);
  
  ExchangeNodeGroup::Ptr vgroup(new ExchangeNodeGroup());
  vgroup->AddExchangeNode(v);
  double vcap = vval * 500;
  vgroup->AddCapacity(vcap);
  
  double qty = large * 0.1;
  
  Match match(std::make_pair(a, qty));
  
  g.AddMatch(a, qty);
  ASSERT_EQ(1, g.matches().size());
  EXPECT_EQ(match, g.matches().at(0));
}
