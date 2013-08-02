#ifndef PROTOTYPE_H
#define PROTOTYPE_H

#include <map>
#include <string>

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
   prototypeConstructor() function.

   Note!!! To date, *all* classes that inherite from Prototype must
   also inherit from Model. This is required in order to remove the
   prototype from the registry (we need its name). This may change
   at a later date if/when we introduce a SimulationEntity-type class.
 */
class Prototype {
 public:
  /** 
      add a prototype to the registry
      @param name the prototype's name
      @param p a pointer to the Prototype
   */
  static void registerPrototype(std::string name, Prototype* p);

  /**
     get a registered prototype
     @param name the name of the prototype
   */
  static Prototype* getRegisteredPrototype(std::string name);

  /// constructor
  Prototype() {};

  /// virtual destructor
  virtual ~Prototype() {};

  /**
     prototypes are required to provide the capacity to copy their
     initialized members
   */
  virtual Prototype* clone() = 0;

  /**
     a function to be overridden if a prototype needs a constructing
     function once it has been set up to enter the simulation
   */
  virtual void prototypeConstructor() {};

 private:
  /// the set of registered prototyeps
  static std::map<std::string,Prototype*> prototype_registry_;
};
} // namespace cyclus
#endif
