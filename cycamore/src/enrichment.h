#ifndef CYCAMORE_SRC_ENRICHMENT_H_
#define CYCAMORE_SRC_ENRICHMENT_H_

#include <string>

#include "cyclus.h"
#include "cycamore_version.h"

namespace cycamore {

/// @class SWUConverter
///
/// @brief The SWUConverter is a simple Converter class for material to
/// determine the amount of SWU required for their proposed enrichment
class SWUConverter : public cyclus::Converter<cyclus::Material> {
 public:
  SWUConverter(double feed_commod, double tails) : feed_(feed_commod),
    tails_(tails) {}
  virtual ~SWUConverter() {}

  /// @brief provides a conversion for the SWU required
  virtual double convert(
      cyclus::Material::Ptr m,
      cyclus::Arc const * a = NULL,
      cyclus::ExchangeTranslationContext<cyclus::Material>
          const * ctx = NULL) const {
    cyclus::toolkit::Assays assays(feed_, cyclus::toolkit::UraniumAssayMass(m),
                                   tails_);
    return cyclus::toolkit::SwuRequired(m->quantity(), assays);
  }

  /// @returns true if Converter is a SWUConverter and feed and tails equal
  virtual bool operator==(Converter& other) const {
    SWUConverter* cast = dynamic_cast<SWUConverter*>(&other);
    return cast != NULL &&
    feed_ == cast->feed_ &&
    tails_ == cast->tails_;
  }

 private:
  double feed_, tails_;
};

/// @class NatUConverter
///
/// @brief The NatUConverter is a simple Converter class for material to
/// determine the amount of natural uranium required for their proposed
/// enrichment
class NatUConverter : public cyclus::Converter<cyclus::Material> {
 public:
  NatUConverter(double feed_commod, double tails) : feed_(feed_commod),
    tails_(tails) {}
  virtual ~NatUConverter() {}

  virtual std::string version() { return CYCAMORE_VERSION; }

  /// @brief provides a conversion for the amount of natural Uranium required
  virtual double convert(
      cyclus::Material::Ptr m,
      cyclus::Arc const * a = NULL,
      cyclus::ExchangeTranslationContext<cyclus::Material>
          const * ctx = NULL) const {
    cyclus::toolkit::Assays assays(feed_, cyclus::toolkit::UraniumAssayMass(m),
                                   tails_);
    cyclus::toolkit::MatQuery mq(m);
    std::set<cyclus::Nuc> nucs;
    nucs.insert(922350000);
    nucs.insert(922380000);

    double natu_frac = mq.mass_frac(nucs);
    double natu_req = cyclus::toolkit::FeedQty(m->quantity(), assays);
    return natu_req / natu_frac;
  }

  /// @returns true if Converter is a NatUConverter and feed and tails equal
  virtual bool operator==(Converter& other) const {
    NatUConverter* cast = dynamic_cast<NatUConverter*>(&other);
    return cast != NULL &&
    feed_ == cast->feed_ &&
    tails_ == cast->tails_;
  }

 private:
  double feed_, tails_;
};

///  The Enrichment facility is a simple Agent that enriches natural
///  uranium in a Cyclus simulation. It does not explicitly compute
///  the physical enrichment process, rather it calculates the SWU
///  required to convert an source uranium recipe (ie. natural uranium)
///  into a requested enriched recipe (ie. 4% enriched uranium), given
///  the natural uranium inventory constraint and its SWU capacity
///  constraint.
///
///  The Enrichment facility requests an input commodity and associated recipe
///  whose quantity is its remaining inventory capacity.  All facilities
///  trading the same input commodity (even with different recipes) will
///  offer materials for trade.  The Enrichment facility accepts any input
///  materials with enrichments less than its tails assay, as long as some
///  U235 is present, and preference increases with U235 content.  If no
///  U235 is present in the offered material, the trade preference is set
///  to -1 and the material is not accepted.  Any material components other
///  other than U235 and U238 are sent directly to the tails buffer.
///
///  The Enrichment facility will bid on any request for its output commodity
///  up to the maximum allowed enrichment (if not specified, default is 100%)
///  It bids on either the request quantity, or the maximum quanity allowed
///  by its SWU constraint or natural uranium inventory, whichever is lower.
///  If multiple output commodities with different enrichment levels are
///  requested and the facility does not have the SWU or quantity capacity
///  to meet all requests, the requests are fully, then partially filled
///  in unspecified but repeatable order.
///
///  The Enrichment facility also offers its tails as an output commodity with
///  no associated recipe.  Bids for tails are constrained only by total
///  tails inventory.

class Enrichment
  : public cyclus::Facility,
    public cyclus::toolkit::Position {
#pragma cyclus note {   	  \
  "niche": "enrichment facility",				  \
  "doc":								\
  "The Enrichment facility is a simple agent that enriches natural "	 \
  "uranium in a Cyclus simulation. It does not explicitly compute "	\
  "the physical enrichment process, rather it calculates the SWU "	\
  "required to convert an source uranium recipe (i.e. natural uranium) " \
  "into a requested enriched recipe (i.e. 4% enriched uranium), given " \
  "the natural uranium inventory constraint and its SWU capacity " \
  "constraint."							\
  "\n\n"								\
  "The Enrichment facility requests an input commodity and associated " \
  "recipe whose quantity is its remaining inventory capacity.  All " \
  "facilities trading the same input commodity (even with different " \
  "recipes) will offer materials for trade.  The Enrichment facility " \
  "accepts any input materials with enrichments less than its tails assay, "\
  "as long as some U235 is present, and preference increases with U235 " \
  "content.  If no U235 is present in the offered material, the trade " \
  "preference is set to -1 and the material is not accepted.  Any material " \
  "components other than U235 and U238 are sent directly to the tails buffer."\
  "\n\n"								\
  "The Enrichment facility will bid on any request for its output commodity "\
  "up to the maximum allowed enrichment (if not specified, default is 100%) "\
  "It bids on either the request quantity, or the maximum quanity allowed " \
  "by its SWU constraint or natural uranium inventory, whichever is lower. " \
  "If multiple output commodities with different enrichment levels are " \
  "requested and the facility does not have the SWU or quantity capacity " \
  "to meet all requests, the requests are fully, then partially filled " \
  "in unspecified but repeatable order."				\
  "\n\n"								\
  "Accumulated tails inventory is offered for trading as a specifiable " \
  "output commodity.", \
}
 public:
  // --- Module Members ---
  ///    Constructor for the Enrichment class
  ///    @param ctx the cyclus context for access to simulation-wide parameters
  Enrichment(cyclus::Context* ctx);

  ///     Destructor for the Enrichment class
  virtual ~Enrichment();

  virtual std::string version() { return CYCAMORE_VERSION; }

  #pragma cyclus

  ///     Print information about this agent
  virtual std::string str();
  // ---

  // --- Facility Members ---
  /// perform module-specific tasks when entering the simulation
  virtual void Build(cyclus::Agent* parent);
  // ---

  // --- Agent Members ---
  ///  Each facility is prompted to do its beginning-of-time-step
  ///  stuff at the tick of the timer.

  ///  @param time is the time to perform the tick
  virtual void Tick();

  ///  Each facility is prompted to its end-of-time-step
  ///  stuff on the tock of the timer.

  ///  @param time is the time to perform the tock
  virtual void Tock();

  /// @brief The Enrichment request Materials of its given
  /// commodity.
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests();

  /// @brief The Enrichment adjusts preferences for offers of
  /// natural uranium it has received to maximize U-235 content
  /// Any offers that have zero U-235 content are not accepted
  virtual void AdjustMatlPrefs(cyclus::PrefMap<cyclus::Material>::type& prefs);

  /// @brief The Enrichment place accepted trade Materials in their
  /// Inventory
  virtual void AcceptMatlTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
      cyclus::Material::Ptr> >& responses);

  /// @brief Responds to each request for this facility's commodity.  If a given
  /// request is more than this facility's inventory or SWU capacity, it will
  /// offer its minimum of its capacities.
  virtual std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
    GetMatlBids(cyclus::CommodMap<cyclus::Material>::type&
    commod_requests);

  /// @brief respond to each trade with a material enriched to the appropriate
  /// level given this facility's inventory
  ///
  /// @param trades all trades in which this trader is the supplier
  /// @param responses a container to populate with responses to each trade
  virtual void GetMatlTrades(
    const std::vector< cyclus::Trade<cyclus::Material> >& trades,
    std::vector<std::pair<cyclus::Trade<cyclus::Material>,
    cyclus::Material::Ptr> >& responses);
  // ---

  ///  @brief Determines if a particular material is a valid request to respond
  ///  to.  Valid requests must contain U235 and U238 and must have a relative
  ///  U235-to-U238 ratio less than this facility's tails_assay().
  ///  @return true if the above description is met by the material
  bool ValidReq(const cyclus::Material::Ptr mat);

   inline void SetMaxInventorySize(double size) {
    max_feed_inventory = size;
    inventory.capacity(size);
  }

  inline void SwuCapacity(double capacity) {
    swu_capacity = capacity;
    current_swu_capacity = swu_capacity;
  }

  inline double SwuCapacity() const { return swu_capacity; }

  inline const cyclus::toolkit::ResBuf<cyclus::Material>& Tails() const {
    return tails;
  }

 private:
  ///   @brief adds a material into the natural uranium inventory
  ///   @throws if the material is not the same composition as the feed_recipe
  void AddMat_(cyclus::Material::Ptr mat);

  ///   @brief generates a request for this facility given its current state.
  ///   Quantity of the material will be equal to remaining inventory size.
  cyclus::Material::Ptr Request_();

  ///  @brief Generates a material offer for a given request. The response
  ///  composition will be comprised only of U235 and U238 at their relative
  ///  ratio in the requested material. The response quantity will be the
  ///  same as the requested commodity.
  ///
  ///  @param req the requested material being responded to
  cyclus::Material::Ptr Offer_(cyclus::Material::Ptr req);

  cyclus::Material::Ptr Enrich_(cyclus::Material::Ptr mat, double qty);

  ///  @brief calculates the feed assay based on the unenriched inventory
  double FeedAssay();

  ///  @brief records and enrichment with the cyclus::Recorder
  void RecordEnrichment_(double natural_u, double swu);

  /// Records an agent's latitude and longitude to the output db
  void RecordPosition();

  #pragma cyclus var { \
    "tooltip": "feed commodity",					\
    "doc": "feed commodity that the enrichment facility accepts",	\
    "uilabel": "Feed Commodity",                                    \
    "uitype": "incommodity" \
  }
  std::string feed_commod;

  #pragma cyclus var { \
    "tooltip": "feed recipe",						\
    "doc": "recipe for enrichment facility feed commodity",		\
    "uilabel": "Feed Recipe",                                   \
    "uitype": "inrecipe" \
  }
  std::string feed_recipe;

  #pragma cyclus var { \
    "tooltip": "product commodity",					\
    "doc": "product commodity that the enrichment facility generates",	 \
    "uilabel": "Product Commodity",                                     \
    "uitype": "outcommodity" \
  }
  std::string product_commod;

  #pragma cyclus var {							\
    "tooltip": "tails commodity",					\
    "doc": "tails commodity supplied by enrichment facility",		\
    "uilabel": "Tails Commodity",                                   \
    "uitype": "outcommodity" \
  }
  std::string tails_commod;

  #pragma cyclus var {							\
    "default": 0.003, "tooltip": "tails assay",				\
    "uilabel": "Tails Assay",                             \
    "uitype": "range",                               \
    "range": [0.0, 0.003],                              \
    "doc": "tails assay from the enrichment process",       \
  }
  double tails_assay;

  #pragma cyclus var {							\
    "default": 0, "tooltip": "initial uranium reserves (kg)",		\
    "uilabel": "Initial Feed Inventory",				\
    "doc": "amount of natural uranium stored at the enrichment "	\
    "facility at the beginning of the simulation (kg)"			\
  }
  double initial_feed;

  #pragma cyclus var {							\
    "default": 1e299, "tooltip": "max inventory of feed material (kg)", \
    "uilabel": "Maximum Feed Inventory", \
    "uitype": "range", \
    "range": [0.0, 1e299], \
    "doc": "maximum total inventory of natural uranium in "		\
           "the enrichment facility (kg)"     \
  }
  double max_feed_inventory;

  #pragma cyclus var { \
    "default": 1.0,						\
    "tooltip": "maximum allowed enrichment fraction",		\
    "doc": "maximum allowed weight fraction of U235 in product", \
    "uilabel": "Maximum Allowed Enrichment", \
    "uitype": "range", \
    "range": [0.0,1.0], \
    "schema": '<optional>'				     	   \
        '          <element name="max_enrich">'			   \
        '              <data type="double">'			   \
        '                  <param name="minInclusive">0</param>'   \
        '                  <param name="maxInclusive">1</param>'   \
        '              </data>'					   \
        '          </element>'					   \
        '      </optional>'					   \
  }
  double max_enrich;

  #pragma cyclus var { \
    "default": 1,		       \
    "userlevel": 10,							\
    "tooltip": "Rank Material Requests by U235 Content",		\
    "uilabel": "Prefer feed with higher U235 content", \
    "doc": "turn on preference ordering for input material "		\
           "so that EF chooses higher U235 content first" \
  }
  bool order_prefs;

  #pragma cyclus var {						       \
    "default": 1e299,						       \
    "tooltip": "SWU capacity (kgSWU/month)",			       \
    "uilabel": "SWU Capacity",                                         \
    "uitype": "range",                                                  \
    "range": [0.0, 1e299],                                               \
    "doc": "separative work unit (SWU) capacity of enrichment "		\
           "facility (kgSWU/timestep) "                                     \
  }
  double swu_capacity;

  double current_swu_capacity;

  #pragma cyclus var { 'capacity': 'max_feed_inventory' }
  cyclus::toolkit::ResBuf<cyclus::Material> inventory;  // natural u
  #pragma cyclus var {}
  cyclus::toolkit::ResBuf<cyclus::Material> tails;  // depleted u

  // used to total intra-timestep swu and natu usage for meeting requests -
  // these help enable time series generation.
  double intra_timestep_swu_;
  double intra_timestep_feed_;

  friend class EnrichmentTest;
  // ---

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
};

}  // namespace cycamore

#endif // CYCAMORE_SRC_ENRICHMENT_FACILITY_H_
