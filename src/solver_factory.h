#ifndef CYCLUS_SRC_SOLVER_FACTORY_H_
#define CYCLUS_SRC_SOLVER_FACTORY_H_

#include <string>

class OsiSolverInterface;

namespace cyclus {

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

void SolveProg(OsiSolverInterface* si, bool verbose=false);
bool HasInt(OsiSolverInterface* si);

}  // namespace cyclus

#endif  // CYCLUS_SRC_SOLVER_FACTORY_H_
