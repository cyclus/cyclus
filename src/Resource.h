// Resource.h
#if !defined(_RESOURCE_H)
#define _RESOURCE_H
#include <string>

/// A list of concrete types of resource
enum ResourceType { MATERIAL_RES, GENERIC_RES, LAST_RES }; 

class Resource {
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
  virtual std::string getResourceUnits() = 0;
    
  /**
   * Returns the total quantity of this resource in its base unit 
   *
   * @return the total quantity of this resource in its base unit
   */
  virtual double getQuantity() = 0;
    
  /**
   * A boolean comparing the quantity of the other resource is 
   * to the quantity of the base
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if other is sufficiently equal in quantity to 
   * the base, False otherwise.
   */
  virtual bool checkQuantityEqual(Resource* other)=0;

  /**
   * Returns true if the quantity of the other resource is 
   * greater than the quantity of the base 
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if second is sufficiently equal in quantity to 
   * first, False otherwise.
   */
  virtual bool checkQuantityGT(Resource* other)=0;

  /**
   * Compares the quantity and quality of the other resource  
   * to the base
   *
   * @param other The resource to evaluate
   *
   * @return True if other is sufficiently equal to the base, 
   * False otherwise.
   */
  virtual bool checkEquality(Resource* other){
    bool toRet;
    (this->checkQuality(other) && this->checkQuantityEqual(other)) ? toRet = true : toRet = false;
    return toRet; 
  };

  /**
   * Returns the concrete resource type, an enum
   */
  virtual ResourceType getResourceType()=0;

  /**
   * Returns a newly allocated copy of the resource
   */
  virtual Resource* clone()=0;

};

#endif
