#ifndef CYCLUS_TESTS_EXCHANGE_TEST_CASES_H_
#define CYCLUS_TESTS_EXCHANGE_TEST_CASES_H_

#include <string>

namespace cyclus {

class ExchangeGraph;
  
/// ExchangeCase, a class that provides a virtual interface for constructing
/// and testing a variety of ExchangeGraph/ExchangeSolver test instances.
///
/// To add test case support of a new solver type, add to the solver_type
/// if-then statement for the test case of your choosing.
class ExchangeCase {
public:
  virtual ~ExchangeCase() {}

  /// constructs an instance of an exchange graph for the test case
  virtual void Construct(ExchangeGraph* g) = 0;

  /// Tests any supported solver_types for correctness
  virtual void Test(std::string solver_type, ExchangeGraph* g) = 0;
};

/// Case0, an empty graph
/// no matches should exist
class Case0: public ExchangeCase {
 public:
  virtual ~Case0() {}
  virtual void Construct(ExchangeGraph* g);
  virtual void Test(std::string solver_type, ExchangeGraph* g);
};

/// Case1a, a single request group
/// no matches should exist
class Case1a: public ExchangeCase {
 public:
  virtual ~Case1a() {}
  virtual void Construct(ExchangeGraph* g);
  virtual void Test(std::string solver_type, ExchangeGraph* g);
};

/// Case1b, a single supply group
/// no matches should exist
class Case1b: public ExchangeCase {
 public:
  virtual ~Case1b() {}
  virtual void Construct(ExchangeGraph* g);
  virtual void Test(std::string solver_type, ExchangeGraph* g);
};

/// Case 2:
/// 1 supply node with capacity, c, and unit capacity, u_s
/// 1 request node with request quantity, q, and unit capacity, u_r
/// flow from s->r = f
class Case2: public ExchangeCase {
 public:
  virtual ~Case2() {}
  virtual void Construct(ExchangeGraph* g); 
  virtual void Test(std::string solver_type, ExchangeGraph* g);
  
 protected:
  double qty, unit_cap_req, capacity, unit_cap_sup, flow;
};

/// Case2a:
/// q < c, u_s = 1
/// f = q
class Case2a: public Case2 {
 public:
  virtual ~Case2a() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case2b:
///q = c, u_s = 1
///f = c
class Case2b: public Case2 {
 public:
  virtual ~Case2b() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case2c:
///q > c, u_s = 1, u_r = 1
///f = c
class Case2c: public Case2 {
 public:
  virtual ~Case2c() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case2d:
///q > c, u_s < 1, c/u_s > q, u_r = 1
///f = q
class Case2d: public Case2 {
 public:
  virtual ~Case2d() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case2e:
///q = c, u_s < 1 ( => c/u_s > q ), u_r = 1
///f = q
class Case2e: public Case2 {
 public:
  virtual ~Case2e() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case2f:
/// q = c, u_s > 1 ( => c/u_s < q ), u_r = 1
/// f = c / u_s
class Case2f: public Case2 {
 public:
  virtual ~Case2f() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case2g:
// q = c, u_s = 1, u_r < 1 ( => q/u_r > c )
// f = c
class Case2g: public Case2 {
 public:
  virtual ~Case2g() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case2h:
/// q = c, u_s = 1, u_r > 1 ( => q/u_r < c )
/// f = q / u_r
class Case2h: public Case2 {
 public:
  virtual ~Case2h() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case 3:
/// 2 suppliers (2 nodes) with capacity, c1 & c2
/// 1 requester (1 node) with request quantity, q
/// requester pref for s1 := p1
/// requester pref for s2 := p2
/// flow from s1 -> r := f1
/// flow from s2 -> r := f2
class Case3: public ExchangeCase {
 public:
  virtual ~Case3() {}
  virtual void Construct(ExchangeGraph* g); 
  virtual void Test(std::string solver_type, ExchangeGraph* g);
  
 protected:
  double q, c1, c2;
  double p1, p2;
  double f1, f2;
};

/// Case3a:
/// q = c1
/// f1 = c1, f2 DNE
class Case3a: public Case3 {
 public:
  virtual ~Case3a() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case3b:
/// q - c1 < c2
/// f1 = c1, f2 = q - c1
class Case3b: public Case3 {
 public:
  virtual ~Case3b() {}
  virtual void Construct(ExchangeGraph* g); 
  /// virtual void Test(std::string solver_type, ExchangeGraph* g);
};

/// Case3c:
/// q - c1 = c2
/// f1 = c1, f2 = c2
class Case3c: public Case3 {
 public:
  virtual ~Case3c() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case3d:
/// q - c1 > c2
/// f1 = c1, f2 = c2
class Case3d: public Case3 {
 public:
  virtual ~Case3d() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case3e:
/// q = c2
/// p2 > p1
/// f1 DNE, f2 = c2
class Case3e: public Case3 {
 public:
  virtual ~Case3e() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case3f:
/// q > c2, q - c2 < c1
/// p2 > p1
/// f1 = q - c2, f2 = c2
class Case3f: public Case3 {
 public:
  virtual ~Case3f() {}
  virtual void Construct(ExchangeGraph* g);

  /// overrides case3 Test
  virtual void Test(std::string solver_type, ExchangeGraph* g); 
};

/// Case 4:
/// 1 suppliers (2 nodes) with capacity, c
/// 2 requester (2 nodes) with request quantities, q1 & q2
/// flow from s -> r1 := f1
/// flow from s -> r2 := f2
class Case4: public ExchangeCase {
 public:
  virtual ~Case4() {}
  virtual void Construct(ExchangeGraph* g); 
  virtual void Test(std::string solver_type, ExchangeGraph* g);

 protected:
  double q1, q2, c;
  double f1, f2;
};

/// Case 4a
/// q1 > c
/// f1 = c, f2 DNE
class Case4a: public Case4 {
 public:
  virtual ~Case4a() {}
  virtual void Construct(ExchangeGraph* g); 
  virtual void Test(std::string solver_type, ExchangeGraph* g);
};

/// Case 4b
/// q1 = c
/// f1 = c, f2 DNE
class Case4b: public Case4 {
 public:
  virtual ~Case4b() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case 4c
/// q1 < c, q2 > c - q1
/// f1 = q1, f2 = c - q1
class Case4c: public Case4 {
 public:
  virtual ~Case4c() {}
  virtual void Construct(ExchangeGraph* g); 
  virtual void Test(std::string solver_type, ExchangeGraph* g);
};

/// Case 4d
/// q1 < c, q2 = c - q1
/// f1 = q1, f2 = c - q1
class Case4d: public Case4 {
 public:
  virtual ~Case4d() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case 4e
/// q1 < c, q2 < c - q1
/// f1 = q1, f2 = q2
class Case4e: public Case4 {
 public:
  virtual ~Case4e() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case 5:
/// 2 suppliers (2 nodes) with capacities, c1 & c2
/// 1 requester (2 nodes) with request quantities, q
/// flow from s1 -> r := f1
/// flow from s2 -> r := f2
class Case5: public ExchangeCase {
 public:
  virtual ~Case5() {}
  virtual void Construct(ExchangeGraph* g); 
  virtual void Test(std::string solver_type, ExchangeGraph* g);

 protected:
  double q, c1, c2;
  double f1, f2;
};

/// Case5a:
/// q < c1
/// f1 = q, f2 DNE
class Case5a: public Case5 {
 public:
  virtual ~Case5a() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case5b:
/// q = c1
/// f1 = c1, f2 DNE
class Case5b: public Case5 {
 public:
  virtual ~Case5b() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case5c:
/// q > c1, c2 > q - c1
/// f1 = c1, f2 = q - c1
class Case5c: public Case5 {
 public:
  virtual ~Case5c() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case5d:
/// q > c1, c2 = q - c1
/// f1 = c1, f2 = c2
class Case5d: public Case5 {
 public:
  virtual ~Case5d() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case5e:
/// q > c1, c2 < q - c1
/// f1 = c1, f2 = c2
class Case5e: public Case5 {
 public:
  virtual ~Case5e() {}
  virtual void Construct(ExchangeGraph* g); 
};

/// Case 6:
/// 2 suppliers (2 nodes ea.) with capacitoes, c1 & c2
/// 2 requester (2 nodes eq.) with request quantities, q1 & q2
/// flow from s1 -> r1 := f1
/// flow from s2 -> r1 := f2
/// flow from s1 -> r2 := f3
/// flow from s2 -> r2 := f4
class Case6: public ExchangeCase {
 public:
  virtual ~Case6() {}
  virtual void Construct(ExchangeGraph* g); 
  virtual void Test(std::string solver_type, ExchangeGraph* g) = 0;

 protected:
  double q1, q2, c1, c2;
  double f1, f2, f3, f4;
};

/// Case6a:
/// q1 > c1, c2 > q1 - c1, q2 < c2 - (q1 - c1)
/// f1 = c1, f2 = q1 - c1, f3 DNE, f4 = q2
class Case6a: public Case6 {
 public:
  virtual ~Case6a() {}
  virtual void Construct(ExchangeGraph* g); 
  virtual void Test(std::string solver_type, ExchangeGraph* g);
};

/// Case6b:
/// q1 < c1, q2 > c1 - q1, c2 > q2 - (c1 - q1)
/// f1 = q1, f2 DNE, f3 = c1 - q1, f4 = q2 - (c1 - q1)
class Case6b: public Case6 {
 public:
  virtual ~Case6b() {}
  virtual void Construct(ExchangeGraph* g); 
  virtual void Test(std::string solver_type, ExchangeGraph* g);
};

/// Case 7:
/// N supply node with capacity, q/N
/// 1 request node with request quantity, q
/// N flows from s->r = q/N
class Case7: public ExchangeCase {
 public:
  virtual ~Case7() {}
  virtual void Construct(ExchangeGraph* g); 
  virtual void Test(std::string solver_type, ExchangeGraph* g);

 protected:
  double qty, flow;
  int N;
};

} // namespace cyclus

#endif // ifndef CYCLUS_TESTS_EXCHANGE_TEST_CASES_H_
