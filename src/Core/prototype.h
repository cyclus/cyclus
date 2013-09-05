#ifndef PROTOTYPE_H_
#define PROTOTYPE_H_

namespace cyclus {

/**
   Prototypes are Models in the simulation that are created through
   the process of cloning. All prototypes have a subset of their
   members initialized before being registered. These majority of
   these members are likely to be static members in the derived
   prototype class, and are therefore accessible by all cloned
   prototype instances. Some, however, may be mutable by each instance
   of a cloned prototype. Such members must be copied during the clone
   operation. The cloning process returns a "fresh" prototype ready to
   be entered into the simulation.

   Some prototypes may require an additional intialization or
   construction step after being connected into the simulation. Such
   functionality is provided through the virtual
   PrototypeConstructor() function.

   Note!!! To date, *all* classes that inherite from Prototype must
   also inherit from Model. This is required in order to remove the
   prototype from the registry (we need its name). This may change
   at a later date if/when we introduce a SimulationEntity-type class.
 */
class Prototype {
 public:
  Prototype() {};

  virtual ~Prototype() {};

  /**
     Return a newly created/allocated prototype that is an exact copy of this.
   */
  virtual Prototype* clone() = 0;

  /**
     a function to be overridden if a prototype needs a constructing
     function once it has been set up to enter the simulation
   */
  virtual void PrototypeConstructor() {};
};
} // namespace cyclus
#endif
