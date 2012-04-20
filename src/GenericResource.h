// GenericResource.h
#if !defined(_GENERICRESOURCE_H)
#define _GENERICRESOURCE_H

#include "Resource.h"
#include "CycException.h"

class CycGenResourceIncompatible: public CycException {
    public: CycGenResourceIncompatible(std::string msg) : CycException(msg) {};
};

class CycGenResourceOverExtract: public CycException {
    public: CycGenResourceOverExtract(std::string msg) : CycException(msg) {};
};

class GenericResource;
typedef boost::intrusive_ptr<GenericResource> gen_rsrc_ptr;

/**
   A Generic Resource is a general type of resource in the Cyclus 
   simulation, and is a catch-all for non-standard resources. 
    
   @section definition Defining a Generic Resource 
   Generic Resources must have a unit, and quality and a quantity. 
 */

class GenericResource : public Resource {
public:
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

  virtual int stateID();

  /**
     Returns a reference to a newly allocated copy of this resource 
   */ 
  virtual rsrc_ptr clone();

  /**
     Prints information about the resource 
   */ 
  virtual void print();

  /**
     A boolean comparing the quality of the other resource 
     to the quality of the base 
      
     @param other The resource to evaluate against the base 
      
     @return True if other is sufficiently equal in quality to 
     the base, False otherwise. 
   */
  virtual bool checkQuality(rsrc_ptr other);

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
  void setQuantity(double new_quantity) {quantity_ = new_quantity;};

  /**
     Gets the quality of this resource 
   */ 
  std::string quality() {return quality_;};

  /**
     Sets the quality of this resource 
   */ 
  void setQuality(std::string new_quality) {quality_ = new_quality;};
    
  /**
     A boolean comparing the quantity of the other resource is 
     to the quantity of the base 
      
     @param other The resource to evaluate against the base 
      
     @return True if other is sufficiently equal in quantity to 
     the base, False otherwise. 
   */
  virtual bool checkQuantityEqual(rsrc_ptr other);

  /**
     Returns true if the quantity of the other resource is 
     greater than the quantity of the base 
      
     @param second The resource to evaluate against the base 
      
     @return True if second is sufficiently equal in quantity to 
     first, False otherwise. 
   */
  virtual bool checkQuantityGT(rsrc_ptr second);

  /**
     Returns the concrete type of this resource 
   */ 
  virtual ResourceType type(){return GENERIC_RES;};
  
  /**
     Returns the type name of this resource 
   */ 
  virtual std::string type_name(){return "Generic Resource";}

  /**
     Return if this resource type has been logged for the database 
   */ 
  bool is_resource_type_logged(){return type_is_logged_;}

  /**
     Tells this resource that it has, indeed, been logged 
   */   
  void type_logged(){type_is_logged_ = true;}
  
  /**
     Absorbs the contents of the given 'other' resource into this 
     resource  
     @throws CycGenResourceIncompatible 'other' resource is of a 
   */
  virtual void absorb(gen_rsrc_ptr other);

  /**
     Extracts the specified mass from this resource and returns it as a 
     new generic resource object with the same quality/type. 
      
     @throws CycGenResourceOverExtract 
   */
  virtual gen_rsrc_ptr extract(double mass);

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

 private:
  /**
     A boolean to tell if the resource has been logged 
   */ 
  static bool type_is_logged_;
};

#endif
