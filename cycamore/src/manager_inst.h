#ifndef CYCAMORE_SRC_MANAGER_INST_H_
#define CYCAMORE_SRC_MANAGER_INST_H_

#include "cyclus.h"
#include "cycamore_version.h"

namespace cycamore {

/// @class ManagerInst
/// @section introduction Introduction
/// @section detailedBehavior Detailed Behavior
/// @warning The ManagerInst is experimental
class ManagerInst
    : public cyclus::Institution,
      public cyclus::toolkit::CommodityProducerManager,
      public cyclus::toolkit::Builder,
      public cyclus::toolkit::Position {
 public:
  /// Default constructor
  ManagerInst(cyclus::Context* ctx);

  /// Default destructor
  virtual ~ManagerInst();

  virtual std::string version() { return CYCAMORE_VERSION; }

  #pragma cyclus

  #pragma cyclus note {"doc": "An institution that owns and operates a " \
                              "manually entered list of facilities in " \
                              "the input file"}

  /// enter the simulation and register any children present
  virtual void EnterNotify();

  /// register a new child
  virtual void BuildNotify(Agent* m);

  /// unregister a child
  virtual void DecomNotify(Agent* m);

  /// write information about a commodity producer to a stream
  /// @param producer the producer
  void WriteProducerInformation(cyclus::toolkit::CommodityProducer*
                                producer);

 private:
  /// register a child
  void Register_(cyclus::Agent* agent);

  /// unregister a child
  void Unregister_(cyclus::Agent* agent);

  #pragma cyclus var { \
    "tooltip": "producer facility prototypes",                          \
    "uilabel": "Producer Prototype List",                               \
    "uitype": ["oneormore", "prototype"],                                    \
    "doc": "A set of facility prototypes that this institution can build. " \
    "All prototypes in this list must be based on an archetype that "   \
    "implements the cyclus::toolkit::CommodityProducer interface",      \
    }
  std::vector<std::string> prototypes;

  #pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Geographical latitude in degrees as a double", \
    "doc": "Latitude of the agent's geographical position. The value should " \
           "be expressed in degrees as a double." \
  }
  double latitude;

  #pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Geographical longitude in degrees as a double", \
    "doc": "Longitude of the agent's geographical position. The value should " \
           "be expressed in degrees as a double." \
  }
  double longitude;

  cyclus::toolkit::Position coordinates;

  /// Records an agent's latitude and longitude to the output db
  void RecordPosition();
};

}  // namespace cycamore

#endif  // CYCAMORE_SRC_MANAGER_INST_H_
