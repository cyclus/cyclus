// GenericResource.h
#if !defined(_GENERICRESOURCE_H)
#define _GENERICRESOURCE_H

#include "Resource.h"

class GenericResource;
typedef boost::intrusive_ptr<GenericResource> gen_rsrc_ptr;

/**
   @brief
   A Generic Resource is a general type of resource in the Cyclus
   simulation, and is a catch-all for non-standard resources.
      
   @section definition Defining a Generic Resource
   Generic Resources must have a unit, and quality and a quantity.
 */

class GenericResource : public Resource {
public:
  /**
   * @brief Constructor
   *
   * @param unit is a string indicating the resource unit 
   * @param quality is a string indicating a quality 
   * @param quantity is a double indicating the quantity
   */
  GenericResource(std::string units, std::string quality, double quantity);
  
  /**
   * @brief Copy constructor
   *
   * @param other the resource to copy
   */
  GenericResource(const GenericResource& other);

  /**
   * @brief Returns a reference to a newly allocated copy of this resource object.
   */ 
  virtual rsrc_ptr clone();

  /**
   * @brief Prints information about the resource
   */ 
  virtual void print();

  /**
   * @brief A boolean comparing the quality of the other resource 
   * to the quality of the base
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if other is sufficiently equal in quality to 
   * the base, False otherwise.
   */
  virtual bool checkQuality(rsrc_ptr other);

  /**
   * @brief Returns the total quantity of this resource in its base unit 
   *
   * @return the total quantity of this resource in its base unit
   */
  virtual double quantity() {return quantity_;};
    
  /**
   * @brief Returns the total quantity of this resource in its base unit 
   *
   * @return the total quantity of this resource in its base unit
   */
  virtual std::string units() {return units_;};
    
  /**
   * @brief Sets the total quantity of this resource in its base unit 
   */ 
  void setQuantity(double new_quantity) {quantity_ = new_quantity;};

  /**
   * @brief Gets the quality of this resource
   */ 
  std::string quality() {return quality_;};

  /**
   * @brief Sets the quality of this resource
   */ 
  void setQuality(std::string new_quality) {quality_ = new_quality;};
    
  /**
   * @brief A boolean comparing the quantity of the other resource is 
   * to the quantity of the base 
   * 
   * @param other The resource to evaluate against the base
   *
   * @return True if other is sufficiently equal in quantity to 
   * the base, False otherwise.
   */
  virtual bool checkQuantityEqual(rsrc_ptr other);

  /**
   * @brief Returns true if the quantity of the other resource is 
   * greater than the quantity of the base 
   *
   * @param other The resource to evaluate against the base
   * 
   * @return True if second is sufficiently equal in quantity to 
   * first, False otherwise.
   */
  virtual bool checkQuantityGT(rsrc_ptr second);

  /**
   * @brief Returns the concrete type of this resource
   */ 
  virtual ResourceType type(){return GENERIC_RES;};
  
  /**
   * @brief Returns the type name of this resource
   */ 
  virtual std::string type_name(){return "Generic Resource";}

  /**
   * @brief Return if this resource type has been logged for the database
   */ 
  bool is_resource_type_logged(){return type_is_logged_;}

  /**
   * @brief Tells this resource that it has, indeed, been logged
   */   
  void type_logged(){type_is_logged_ = true;}
  
  /**
   * @brief Sets the originator's id AND logs the resource
   */   
  void setOriginatorID(int id);

  /**
   * @briefAbsorbs the contents of the given resource into this resource
   *
   * @param other the resource to be absorbed
   */
  virtual void absorb(gen_rsrc_ptr other);

  /**
   * @brief Extracts a specified mass from this material creating a new material
   * object with the same isotopic ratios.
   * 
   * @param the amount (mass) of material that will be removed
   *  
   * @return the extracted material as a newly allocated material object
   */
  virtual gen_rsrc_ptr extract(double mass);

private:  
  /**
   * @brief The units of the resource
   */ 
  std::string units_;

  /**
   * @brief The quality distinguishing this resource will be traded as.
   */ 
  std::string quality_;

  /**
   * @brief The quantity of the resource
   */ 
  double quantity_;

 private:
  /**
   * @brief A boolean to tell if the resource has been logged
   */ 
  static bool type_is_logged_;
};

#endif
