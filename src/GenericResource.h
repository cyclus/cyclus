// GenericResource.h
#if !defined(_GENERICRESOURCE_H)
#define _GENERICRESOURCE_H
#include "Resource.h"

class GenericResource;
typedef boost::intrusive_ptr<GenericResource> gen_rsrc_ptr;

class GenericResource : public Resource {
public:
  /**
   * Constructor
   *
   * @param unit is a string indicating the resource unit 
   * @param quality is a string indicating a quality 
   * @param quantity is a double indicating the quantity
   */
  GenericResource(std::string units, std::string quality, double quantity);

  /// copy constructor
  GenericResource(const GenericResource& other);

  /**
   * Returns a reference to a newly allocated copy of this resource object.
   */
  virtual rsrc_ptr clone();

  virtual void print();

  /**
   * A boolean comparing the quality of the other resource 
   * to the quality of the base
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if other is sufficiently equal in quality to 
   * the base, False otherwise.
   */
  virtual bool checkQuality(rsrc_ptr other);

  /**
   * Returns the total quantity of this resource in its base unit 
   *
   * @return the total quantity of this resource in its base unit
   */
  virtual double quantity() {return quantity_;};
    
  /**
   * Returns the total quantity of this resource in its base unit 
   *
   * @return the total quantity of this resource in its base unit
   */
  virtual std::string units() {return units_;};
    
  /**
   * Sets the total quantity of this resource in its base unit 
   */
  void setQuantity(double new_quantity) {quantity_ = new_quantity;};

  /// Gets the quality of this resource
  std::string quality() {return quality_;};

  /// Sets the quality of this resource
  void setQuality(std::string new_quality) {quality_ = new_quality;};
    
  /**
   * A boolean comparing the quantity of the other resource is 
   * to the quantity of the base 
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if other is sufficiently equal in quantity to 
   * the base, False otherwise.
   */
  virtual bool checkQuantityEqual(rsrc_ptr other);

  /**
   * Returns true if the quantity of the other resource is 
   * greater than the quantity of the base 
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if second is sufficiently equal in quantity to 
   * first, False otherwise.
   */
  virtual bool checkQuantityGT(rsrc_ptr second);

  /**
   * Returns the concrete type of this resource
   */
  virtual ResourceType type(){return GENERIC_RES;};

protected:
  /**
   * The quality distinguishing this resource will be traded as.
   */
  std::string units_;

  /**
   * The quality distinguishing this resource will be traded as.
   */
  std::string quality_;

  /**
   * The quality distinguishing this resource will be traded as.
   */
  double quantity_;

};

#endif
