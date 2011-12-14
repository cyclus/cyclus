// Resource.h
#if !defined(_RESOURCE_H)
#define _RESOURCE_H
#include <string>

class Resource {
public:
  /**
   * A boolean comparing the quality of the second resource 
   * to the quality of the first 
   *
   * @param first The base resource
   * @param second The resource to evaluate
   *
   * @return True if second is sufficiently equal in quality to 
   * first, False otherwise.
   */
  virtual bool checkQuality(Resource* first, Resource* second)=0;

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
   * Sets the total quantity of this resource in its base unit 
   */
  virtual void setQuantity(double new_quantity) = 0;
    
  /**
   * A boolean comparing the quantity of the second resource is 
   * to the quantity of the first 
   *
   * @param first The base resource
   * @param second The resource to evaluate
   *
   * @return True if second is sufficiently equal in quantity to 
   * first, False otherwise.
   */
  virtual bool checkQuantityEqual(Resource* first, Resource* second)=0;

  /**
   * Returns true if the quantity of the second resource is 
   * greater than the quantity of the first 
   *
   * @param first The base resource
   * @param second The resource to evaluate
   *
   * @return True if second is sufficiently equal in quantity to 
   * first, False otherwise.
   */
  virtual bool checkQuantityGT(Resource* first, Resource* second)=0;

  /**
   * Compares the quantity and quality of the second resource  
   * to the first 
   *
   * @param first The base resource
   * @param second The resource to evaluate
   *
   * @return True if second is sufficiently equal to the first, 
   * False otherwise.
   */
  virtual bool checkEquality(Resource* first, Resource* second){
    bool toRet;
    (checkQuality(first,second) && checkQuantityEqual(first,second)) ? toRet = true : toRet = false;
    return toRet; };

};

#endif
