#include "exchange_translator.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeTranslator::ToProg(ExchangeGraph* g, OsiSolverInterface* iface) {
  iface->setObjSense(-1.0); // maximize
  const std::vector<Arc>& arcs = g->arcs();
  std::vector<Arcs>::const_iterator a_it;
  for (a_it = arcs->begin(); a_it != arcs->end(); ++a_it) {
    double id = g->arc_ids()[*a_it];
    double pref = a_it->unode->prefs[*a_it];
    if (a_it->exclusive) {
      iface->setObjCoeff(id, pref * a_it->excl_arc);
      ExclArcToProg(g, iface, *a_it);
    } else {
      iface->setObjCoeff(id, pref);
      ArcToProg(g, iface, *a_it);
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeTranslator::ArcToProg(ExchangeGraph* g, OsiSolverInterface* iface,
                                   const Arc& a) {
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeTranslator::ExclArcToProg(ExchangeGraph* g, OsiSolverInterface* iface,
                                       const Arc& a) {
}
