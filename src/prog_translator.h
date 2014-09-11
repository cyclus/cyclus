#ifndef CYCLUS_SRC_PROG_TRANSLATOR_H_
#define CYCLUS_SRC_PROG_TRANSLATOR_H_

#include <vector>

#include "CoinPackedMatrix.hpp"
#include "CoinPackedVector.hpp"

class OsiSolverInterface;

namespace cyclus {

class ExchangeGraph;
class ExchangeNodeGroup;

/// a helper class to translate a product exchange into a mathematical
/// program.
///
/// use as follows:
/// @code
/// // given a graph, g, and interface, iface
/// ProgTranslator t(g, iface);
/// t->ToProg();
/// // solve the program via the interface
/// t->FromProg();
/// @endcode
class ProgTranslator {
 public:
  /// @brief struct to hold all problem instance state
  struct Context {
    std::vector<double> obj_coeffs;
    std::vector<double> row_ubs;
    std::vector<double> row_lbs;
    std::vector<double> col_ubs;
    std::vector<double> col_lbs;
    CoinPackedMatrix m;
    double inf;
  };

  /// a helper function to translate capacities
  ///
  /// @param grp the exchange group
  /// @param request whether this is a request or not
  /// @param faux_id a false arc id for surplus supply
  /// @param cap_rows the capacity rows which will be appended to
  /// @param ctx the translation context
  static void XlateCaps(ExchangeNodeGroup* grp,
                        bool request,
                        int faux_id,
                        std::vector<CoinPackedVector>& cap_rows,
                        ProgTranslator::Context& ctx);
  
  /// constructor
  ///
  /// @param g the exchange graph
  /// @param iface the solver interface
  /// @param exclusive whether or not to include binary-valued arcs
  /// @param pseudo_cost the cost to use for faux arcs
  ProgTranslator(ExchangeGraph* g, OsiSolverInterface* iface);
  ProgTranslator(ExchangeGraph* g, OsiSolverInterface* iface, bool exclusive);
  ProgTranslator(ExchangeGraph* g, OsiSolverInterface* iface,
                 double pseudo_cost);
  ProgTranslator(ExchangeGraph* g, OsiSolverInterface* iface,
                 bool exclusive, double pseudo_cost);

  /// @brief translates the graph, filling the translators Context
  void Translate();

  /// @brief populates the solver interface with values from the translators
  /// Context
  void Populate();

  /// @brief translates graph into mathematic program via iface. This method is
  /// equivalent to calling Translate(), then Populate().
  void ToProg();

  /// @brief translates solution from iface back into graph matches
  void FromProg();

  const ProgTranslator::Context& ctx() const { return ctx_; }

 private:
  void Init();
 
  /// perform all translation for a node group
  /// @param grp a pointer to the node group
  /// @param req a boolean flag, true if grp is a request group
  void XlateGrp_(ExchangeNodeGroup* grp, bool req);

  ExchangeGraph* g_;
  OsiSolverInterface* iface_;
  bool excl_;
  int arc_offset_;
  ProgTranslator::Context ctx_;
  double pseudo_cost_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_PROG_TRANSLATOR_H_
