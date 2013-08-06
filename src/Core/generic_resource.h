// generic_resource.h
#if !defined(_GENERICRESOURCE_H)
#define _GENERICRESOURCE_H

#include "resource.h"

namespace cyclus {

/**
   A Generic Resource is a general type of resource in the Cyclus 
   simulation, and is a catch-all for non-standard resources. 
    
   @section definition Defining a Generic Resource 
   Generic Resources must have a unit, and quality and a quantity. 
 */

class GenericResource : public Resource {
public:
  typedef boost::intrusive_ptr<GenericResource> Ptr;

  /**
     Constructor 
      
     @param units is a string indicating the resource unit 
     @param quality is a string indicating a quality 
     @param quantity is a double indicating the quantity 
   */
  GenericResource(std::string units, std::string quality, double quantity);
  
  /**
     Copy constructor 
      
     @param other the resource to copy 
   */
  GenericResource(const GenericResource& other);

  virtual int StateID();

  /**
     Returns a reference to a newly allocated copy of this resource 
   */ 
  virtual Resource::Ptr clone();

  /**
     Prints information about the resource 
   */ 
  virtual void Print();

  /**
     A boolean comparing the quality of the other resource 
     to the quality of the base 
      
     @param other The resource to evaluate against the base 
      
     @return True if other is sufficiently equal in quality to 
     the base, False otherwise. 
   */
  virtual bool CheckQuality(Resource::Ptr other);

  /**
     Returns the total quantity of this resource in its base unit 
      
     @return the total quantity of this resource in its base unit 
   */
  virtual double quantity() {return quantity_;};
    
  /**
     Returns the total quantity of this resource in its base unit 
      
     @return the total quantity of this resource in its base unit 
   */
  virtual std::string units() {return units_;};
    
  /**
     Sets the total quantity of this resource in its base unit 
   */ 
  void SetQuantity(double new_quantity) {quantity_ = new_quantity;};

  /**
     Gets the quality of this resource 
   */ 
  std::string quality() {return quality_;};

  /**
     Returns the concrete type of this resource 
   */ 
  virtual ResourceType type(){return GENERIC_RES;};
  
  /**
     Returns the type name of this resource 
   */ 
  virtual std::string type_name(){return "Generic Resource";}

  /**
     Return if this resource type has been recorded for the database 
   */ 
  bool is_resource_type_recorded(){return type_is_recorded_;}

  /**
     Tells this resource that it has, indeed, been recorded 
   */   
  void type_recorded(){type_is_recorded_ = true;}
  
  /**
     Absorbs the contents of the given 'other' resource into this 
     resource  
     @throws CycGenResourceIncompatible 'other' resource is of a 
   */
  virtual void Absorb(Ptr other);

  /**
     Extracts the specified mass from this resource and returns it as a 
     new generic resource object with the same quality/type. 
      
     @throws CycGenResourceOverExtract 
   */
  virtual Ptr Extract(double mass);

  virtual void AddToTable();

private:  
  /**
     The units of the resource 
   */ 
  std::string units_;

  /**
     The quality distinguishing this resource will be traded as. 
   */ 
  std::string quality_;

  /**
     The quantity of the resource 
   */ 
  double quantity_;

  bool recorded_;

  /**
     A boolean to tell if the resource has been recorded 
   */ 
  static bool type_is_recorded_;
};
} // namespace cyclus
#endif
