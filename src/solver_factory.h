#include <string>

class OsiSolverInterface;

/// A factory class that, given a configuration, returns a
/// Coin::OsiSolverInterface for a solver.
///
/// @warning it is the caller's responsibility to manage the member of the
/// interface
class SolverFactory {
 public:
  /// set the solver type
  inline void solver_t(std::string t) { t_ = t; }

  /// get the configured solver
  OsiSolverInterface* get();

 private:
  std::string t_;
};
