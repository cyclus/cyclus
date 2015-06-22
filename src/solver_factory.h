#ifndef CYCLUS_SRC_SOLVER_FACTORY_H_
#define CYCLUS_SRC_SOLVER_FACTORY_H_

#include <string>

#include "CbcEventHandler.hpp"

class OsiSolverInterface;

namespace cyclus {
 
/// this is taken exactly from driver4.cpp in the Cbc examples
static int CbcCallBack(CbcModel * model, int from);
  
/// An event handler that records the time that a better solution is found  
class ObjValueHandler: public CbcEventHandler {
 public:
  ObjValueHandler(double obj, double time, bool found);
  explicit ObjValueHandler(double obj);
  virtual ~ObjValueHandler();
  ObjValueHandler(const ObjValueHandler& other);
  ObjValueHandler& operator=(const ObjValueHandler& other);
  virtual CbcEventHandler* clone();
  virtual CbcEventHandler::CbcAction event(CbcEvent e);
  inline double time() const { return time_; }
  inline double obj() const { return obj_; }
  inline bool found() const { return found_; }
    
 private:
  double obj_, time_;
  bool found_;
};

/// A factory class that, given a configuration, returns a
/// Coin::OsiSolverInterface for a solver.
///
/// @warning it is the caller's responsibility to manage the member of the
/// interface
class SolverFactory {
 public:
  /// currently supported solver types are 'clp' and 'cbc'
  /// @param t the solver type
  /// @param tmax the maximum solution time
  SolverFactory();
  explicit SolverFactory(std::string t);
  SolverFactory(std::string t, double tmax);

  /// get/set the solver type
  inline void solver_t(std::string t) { t_ = t; }
  inline const std::string solver_t() const { return t_; }
  inline std::string solver_t() { return t_; }
  
  /// get the configured solver
  OsiSolverInterface* get();
  
 private:
  std::string t_;
  double tmax_;
};

void SolveProg(OsiSolverInterface* si);
void SolveProg(OsiSolverInterface* si, bool verbose);
void SolveProg(OsiSolverInterface* si, double greedy_obj);
void SolveProg(OsiSolverInterface* si, double greedy_obj, bool verbose);
bool HasInt(OsiSolverInterface* si);

}  // namespace cyclus

#endif  // CYCLUS_SRC_SOLVER_FACTORY_H_
