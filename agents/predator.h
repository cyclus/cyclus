#ifndef CYCLUS_AGENTS_PREDATOR_H_
#define CYCLUS_AGENTS_PREDATOR_H_

#include <string>
#include <sstream>

#include "cyclus.h"

namespace cyclus {

/// @class Predator
/// This Predator facility simulates hunters on preys.
class Predator : public cyclus::Facility  {
 public:
  /// Smallest first!
  static inline bool SortById(cyclus::Bid<cyclus::Product>* l,
                              cyclus::Bid<cyclus::Product>* r) {
    return l->bidder()->manager()->id() < r->bidder()->manager()->id();
  }

  Predator(cyclus::Context* ctx);
  virtual ~Predator() {}

  #pragma cyclus

  #pragma cyclus note {"doc": "A facility that represents predators " \
                              "in the Lotka-Volterra integration tests"}

  virtual void EnterNotify();
  virtual void Decommission();
  virtual void Tick();
  virtual void Tock();

  /// @brief Predator request Product of their given
  /// commodity. Note that it is assumed the Predator operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
      GetProductRequests();

  virtual void AdjustProductPrefs(cyclus::PrefMap<cyclus::Product>::type& prefs);

  /// @brief Predator place accepted trade Materials in their Inventory
  virtual void AcceptProductTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
                                   cyclus::Product::Ptr> >& responses);

  inline std::string name() {
    std::stringstream ss;
    ss << prototype() << "_" << id();
    return ss.str();
  }

  void GiveBirth();

 private:
  #pragma cyclus var { \
    "tooltip": "predator commodity",				     \
    "doc": "commodity that the predator supplies",		     \
    "schematype": "token",					     \
    "uilabel": "Predator Commodity",				     \
    "uitype": "outcommodity" \
  }
  std::string commod;

  #pragma cyclus var { \
    "tooltip": "predator's prey",			     \
    "doc": "prey that the predator hunts",		     \
    "schematype": "token",					     \
    "uilabel": "Prey Commodity",				     \
    "uitype": "incommodity" \
  }
  std::string prey;

  /// How many prey until we're full
  #pragma cyclus var { \
    'default': 1,						  \
    "tooltip": "feast size",						\
    "doc": "how many units of prey a predator consumes until it is satisfied",\
    "uilabel": "Feast Size" 						\
  }
  double full;

  /// How often we hunt
  #pragma cyclus var {\
    'default': 1,				 \
    "tooltip": "hunting frequency",		 \
    "doc": "how often a predator needs to hunt", \
    "uilabel": "Hunting Frequency"               \
  }
  int hunt_freq;

  /// How many prey we can catch on the hunt
  #pragma cyclus var { \
    'default': 1, \
    "tooltip": "hunting yield",					  \
    "doc": "how many units of prey a predator can catch during a hunt", \
    "uilabel": "Hunting Yield" 					\
  }
  double hunt_cap;

  /// Hunting success on a scale from 0 to 1
  #pragma cyclus var {\
    'default': 1,						       \
    "tooltip": "hunting success fraction",			       \
    "doc": "fraction of hunting success on a scale from 0 to 1",      \
    "uilabel": "Hunting Success Fraction" 		\
  }
  double success;

  /// Consumption this time step
  #pragma cyclus var {\
    'default': 0, \
    "tooltip": "prey consumed",						\
    "doc": "how many units of prey consumed per time step", \
    "uilabel": "Prey Consumed" 			       \
  }
  double consumed;

  /// Whether or not to base hunt succes on relative prey/predator populations
  #pragma cyclus var { \
    'default': 0, \
    "tooltip": "hunting success factor",				\
    "doc": "whether or not to base hunting success on "			\
           "relative predator/prey populations", \
    "uilabel": "Hunting Success Factor" 	\
  }  // false
  bool hunt_factor;

  /// Age of a predator
  #pragma cyclus var {\
    'default': 0, \
    "tooltip": "predator age",						\
    "doc": "age of predator at beginning of simulation", \
    "uilabel": "Predator Age" \
  }
  int age;

  #pragma cyclus var { \
    'default': 1, \
    "tooltip": "predator lifespan",	\
    "doc": "how long a predator lives",		\
    "uilabel": "Predator Lifespan" 	\
  }
  int lifespan;

  #pragma cyclus var { \
    'default': 0, \
    "tooltip": "dead?",							\
    "doc": "flag for whether predator is currently dead", \
    "uilabel": "Predator Dead?"				  \
  }
  bool dead;

  #pragma cyclus var {				\
    'default': 1, \
    "tooltip": "number of children",				    \
    "doc": "number of predator children born at each birthing instance", \
    "uilabel": "Number Predator Children"				\
  }
  double nchildren;

  /// Whether or not an agent can give birth and die in the same timestep
  #pragma cyclus var { \
    'default': 0, \
    "tooltip": "simultaneous birth and death?",				\
    "doc": "whether or not simultaneous birth and death are allowed "	\
           "(i.e., can a facility give birth and die in the same time step?)", \
    "uilabel": "Simultaneous Birth and Death?" 			       \
  }  // false
  bool birth_and_death;

};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_PREDATOR_H_
