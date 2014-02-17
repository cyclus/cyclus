#ifndef CYCLUS_SOLVER_FACTORY_H_
#define CYCLUS_SOLVER_FACTORY_H_

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
  /// @param t the solver type
  explicit SolverFactory(std::string t = "");
  
  /// get/set the solver type
  inline void solver_t(std::string t) { t_ = t; }
  inline const std::string solver_t() const { return t_; }
  inline std::string solver_t() { return t_; }

  /// get the configured solver
  OsiSolverInterface* get();

 private:
  std::string t_;
};

void Solve(OsiSolverInterface* si);
bool HasInt(OsiSolverInterface* si);

} // namespace cyclus

#endif // ifndef CYCLUS_SOLVER_FACTORY_H_
