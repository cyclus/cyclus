#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <list>
#include <boost/shared_ptr.hpp>

#include "composition.h"
#include "cyc_limits.h"
#include "resource.h"
#include "res_tracker.h"

namespace cyclus {

class Context;

namespace units {
const double kg = 1.0;
const double g =  kg* .001;
const double mg = kg* .000001;
const double ug = kg* .000000001;
} // namespace units

/// The material class is primarily responsible for enabling basic material
/// manipulation while helping enforce mass conservation.  It also provides the
/// ability to easily decay a material up to the current simulation time; it
/// does not perform any decay related logic itself.
///
/// There are four basic operations that can be performed on materials: create,
/// transmute (change material composition - e.g. fission by reactor), absorb
/// (combine materials), extract (split a material). All material
/// handling/manipulation will be performed using these operations - and all
/// operations performed will be tracked and recorded. Usage examples:
///
/// * A mining facility that "creates" new material
///
///   @code
///   Composition::Ptr nat_u = ...
///   double qty = 10.0;
///
///   Material::Ptr m = Material::Create(qty, nat_u, ctx);
///   @endcode
///
/// * A conversion facility mixing uranium and flourine:
///
///   @code
///   Material::Ptr uf6 = uranium_buf.Pop();
///   Material::Ptr f = flourine_buf.Pop();
///
///   uf6.Absorb(f);
///   @endcode
///
/// * A reactor transmuting fuel:
///
///   @code
///   Composition::Ptr burned_comp = ... // fancy code to calculate burned isotopics
///   Material::Ptr assembly = core_fuel.Pop();
///
///   assembly.Transmute(burned_comp);
///   @endcode
///
/// * A separations plant extracting stuff from spent fuel:
///
///   @code
///   Composition::Ptr comp = ... // fancy code to calculate extraction isotopics
///   Material::Ptr bucket = spent_fuel.Pop();
///   double qty = 3.0;
///
///   Material::Ptr mox = bucket.ExtractComp(qty, comp);
///   @endcode
///
class Material: public Resource {
 public:
  typedef boost::shared_ptr<Material> Ptr;
  static const ResourceType kType;

  virtual ~Material();

  /// Creates a new material resource that is "live" and tracked. creator is a
  /// pointer to the model creating the resource (usually will be the caller's
  /// "this" pointer). All future output data recorded will be done using the
  /// creator's context.
  static Ptr Create(Model* creator, double quantity, Composition::Ptr c);

  /// Creates a new material resource that does not actually exist as part of
  /// the simulation and is untracked.
  static Ptr CreateUntracked(double quantity, Composition::Ptr c);

  /// Creates a new material resource that does not actually exist as part of
  /// the simulation, is untracked, and does not have a known composition.
  static Ptr CreateBlank(double quantity);

  /// Returns the id of the material's internal nuclide composition.
  virtual int state_id() const;

  /// Returns Material::kType.
  virtual const ResourceType type() const;

  virtual Resource::Ptr Clone() const;

  /// Records the internal nuclide composition of this resource.
  virtual void Record(Context* ctx) const;

  /// Returns "kg"
  virtual std::string units() const;

  /// Returns the mass of this material in kg.
  virtual double quantity() const;

  virtual Resource::Ptr ExtractRes(double qty);

  /// Same as ExtractComp with c = this->comp().
  Ptr ExtractQty(double qty);

  /// Creates a new material by extracting from this one.
  ///
  /// @param qty the mass quantity to extract
  /// @param c the composition the extracted/returned material
  /// @param threshold an absolute mass cutoff below which constituent nuclide
  /// quantities of the remaining unextracted material are set to zero.
  /// @return a new material with quantity qty and composition c
  Ptr ExtractComp(double qty, Composition::Ptr c, double threshold = eps_rsrc());

  /// Combines material mat with this one.  mat's quantity becomes zero.
  void Absorb(Ptr mat);

  /// Changes the material's composition to c without changing its mass.  Use
  /// this method for things like converting fresh to spent fuel via burning in
  /// a reactor.
  void Transmute(Composition::Ptr c);

  /// A special case of Transmute where the new composition is calculated by
  /// performing a decay calculation on the material's composition.  The time
  /// delta is calculated as the difference between curr_time and the last time
  /// Decay was invoked.
  void Decay(int curr_time);

  /// Calls Decay for all materials currently existing in the simulation.
  static void DecayAll(int curr_time);

  /// Returns the nuclide composition of this material.
  Composition::Ptr comp() const;

 protected:
  Material(Context* ctx, double quantity, Composition::Ptr c);

 private:
  static std::map<Material*, bool> all_mats_;

  Context* ctx_;
  double qty_;
  Composition::Ptr comp_;
  int prev_decay_time_;
  ResTracker tracker_;

};

} // namespace cyclus

#endif
