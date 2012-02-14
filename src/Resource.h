// Resource.h
#if !defined(_RESOURCE_H)
#define _RESOURCE_H
#include <string>
#include <boost/intrusive_ptr.hpp>
#include "IntrusiveBase.h"

class Resource;
typedef boost::intrusive_ptr<Resource> rsrc_ptr;

/// A list of concrete types of resource
enum ResourceType { MATERIAL_RES, GENERIC_RES, LAST_RES }; 

class Resource: IntrusiveBase<Resource> {
public:

  /**
   * A boolean comparing the quality of the other resource 
   * to the quality of the base 
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if other is sufficiently equal in quality to 
   * the base, False otherwise.
   */
  virtual bool checkQuality(Resource* other) =0;

  /**
   * Returns the base unit of this resource 
   *
   * @return resource_unit_ the base unit of this resource
   */
  virtual std::string units() = 0;
    
  /**
   * Returns the total quantity of this resource in its base unit 
   *
   * @return the total quantity of this resource in its base unit
   */
  virtual double quantity() = 0;

  /**
   * Set the quantity of the resource in its default units to 'val'.
   *
   */
  virtual void setQuantity(double val) = 0;
    
  /**
   * A boolean comparing the quantity of the other resource is 
   * to the quantity of the base
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if other is sufficiently equal in quantity to 
   * the base, False otherwise.
   */
  virtual bool checkQuantityEqual(Resource* other) = 0;

  /**
   * Returns true if the quantity of the other resource is 
   * greater than the quantity of the base 
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if second is sufficiently equal in quantity to 
   * first, False otherwise.
   */
  virtual bool checkQuantityGT(Resource* other) = 0;

  /**
   * Compares the quantity and quality of the other resource  
   * to the base
   *
   * @param other The resource to evaluate
   *
   * @return True if other is sufficiently equal to the base, 
   * False otherwise.
   */
  virtual bool checkEquality(Resource* other);

  /// Returns the concrete resource type, an enum
  virtual ResourceType type() = 0;

  /// Returns a newly allocated copy of the resource
  virtual Resource* clone() = 0;

  virtual void print() = 0;

  /// return this resource's unique ID
  const int ID() {return ID_;};

protected:

  Resource();
  
  int ID_;

private:

  static int nextID_;

};

#endif
