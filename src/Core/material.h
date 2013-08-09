#if !defined(_MATERIAL_H)
#define _MATERIAL_H

#include <list>
#include <boost/shared_ptr.hpp>

#include "composition.h"
#include "cyc_limits.h"
#include "resource.h"
#include "res_tracker.h"

namespace cyclus {

namespace units {
const double kg = 1.0;
const double g =  kg* .001;
const double mg = kg* .000001;
const double ug = kg* .000000001;
} // namespace units

class Material: public Resource {
 public:
  typedef boost::shared_ptr<Material> Ptr;
  static const ResourceType kType;

  virtual ~Material();

  static Ptr Create(double quantity, Composition::Ptr c);
  static Ptr CreateUntracked(double quantity, Composition::Ptr c);

  const int id() const;

  virtual int state_id() const;

  virtual const ResourceType type() const;

  virtual Resource::Ptr Clone() const;

  virtual void RecordSpecial() const;

  /// returns "kg"
  virtual std::string units() const;

  /// returns the mass of this material in kg.
  virtual double quantity() const;

  virtual Resource::Ptr ExtractRes(double qty);

  Ptr ExtractQty(double qty);

  Ptr ExtractComp(double qty, Composition::Ptr c, double threshold=eps_rsrc());

  void Absorb(Ptr mat);

  void Transmute(Composition::Ptr c);

  void Decay(int curr_time);

  static void DecayAll(int curr_time);

  Composition::Ptr comp() const;

 protected:
  Material(double quantity, Composition::Ptr c);

 private:
  static std::map<Material*, bool> all_mats_;

  double qty_;
  Composition::Ptr comp_;
  int prev_decay_time_;
  ResTracker tracker_;

};

} // namespace cyclus

#endif
