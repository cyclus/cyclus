// Resource.h
#if !defined(_RESOURCE_H)
#define _RESOURCE_H

#include <string>

#include "Table.h"

class Resource;
typedef boost::intrusive_ptr<Resource> rsrc_ptr;

/* -- Defines -- */
#define EPS_RSRC 1e-6
/* -- */

/**
   A list of concrete types of resource
*/
enum ResourceType { MATERIAL_RES, GENERIC_RES, LAST_RES }; 

/**
   A resource is the base class for items that are passed between 
   agents in a Cyclus simulation. 
    
   @section intro Introduction 
   A resource must have knowlege of what is and how it was created. 
   Accordingly, it keeps track of its units, quality, quantity, the 
   id of its creator, and if it was spawned from the splitting of a 
   different resource. 
    
   @section output Database Output 
   The Resource class keeps track of two Tables in the Cyclus output 
   database: Resource Types and Resources. For each new type of resource 
   introduced in the simulation, a row is added to the Resource Types 
   Table. When a new resource is created in the simulation, a row is 
   added to the Resources table. 
 */

class Resource: IntrusiveBase<Resource> {
public:
  /**
     A boolean comparing the quality of the other resource 
     to the quality of the base 
      
     @param other The resource to evaluate against the base 
      
     @return True if other is sufficiently equal in quality to 
     the base, False otherwise. 
   */
  virtual bool checkQuality(rsrc_ptr other) =0;

  /**
     Returns the base unit of this resource 
      
     @return resource_unit_ the base unit of this resource 
   */
  virtual std::string units() = 0;
    
  /**
     Returns the total quantity of this resource in its base unit 
      
     @return the total quantity of this resource in its base unit 
   */
  virtual double quantity() = 0;

  /**
     Set the quantity of the resource in its default units to 'val'. 
      
   */
  virtual void setQuantity(double val) = 0;
    
  /**
     A boolean comparing the quantity of the other resource is 
     to the quantity of the base 
      
     @param other The resource to evaluate against the base 
      
     @return True if other is sufficiently equal in quantity to 
     the base, False otherwise. 
   */
  virtual bool checkQuantityEqual(rsrc_ptr other) = 0;

  /**
     Returns true if the quantity of the other resource is 
     greater than the quantity of the base 
      
     @param other The resource to evaluate against the base 
      
     @return True if second is sufficiently equal in quantity to 
     first, False otherwise. 
   */
  virtual bool checkQuantityGT(rsrc_ptr other) = 0;

  /**
     Compares the quantity and quality of the other resource 
     to the base 
      
     @param other The resource to evaluate 
      
     @return True if other is sufficiently equal to the base, 
     False otherwise. 
   */
  virtual bool checkEquality(rsrc_ptr other);

  /**
  The current state of the resource object.

  This can be used to prevent writing of redundant information into the output
  database.  e.g. concrete resources only get a new stateID if they enter a
  'state' that has not yet been recorded in the output db. Note that new states
  should be pulled from the static Resource::nextStateID() method;
  **/
  virtual int stateID() = 0;

  /**
     Returns the concrete resource type, an enum 
   */ 
  virtual ResourceType type() = 0;

  /**
     Returns a newly allocated copy of the resource 
   */ 
  virtual rsrc_ptr clone() = 0;

  /**
     Prints information about the resource 
   */
  virtual void print() = 0;

  /**
     return this resource's unique ID 
   */ 
  const int ID() {return ID_;};

  /**
     return this resource's original ID 
   */
  const int originalID() {return originalID_;};

  /**
     a resource has been split, set the id to the original resource's 
   */ 
  void setOriginalID(int id);

  /**
     Destructor 
   */
  virtual ~Resource();

 protected:
  /**
     Constructor 
   */
  Resource();

  /**
     amount of a resource
   */
  double quantity_;

  /**
     Resource ID 
   */
  int ID_;

  /**
     After a split, this is the original resource's id 
   */
  int originalID_;

 private:

  /**
     the next resource id 
   */
  static int nextID_;

// -------- output database related members  -------- 
  
 public:

  /**
     the output database table which logs resources 
   */
  static table_ptr resource_table;

  /**
     the output database table which logs resource types 
   */
  static table_ptr resource_type_table;

  /**
     adds a resource to the resouce table 
   */
  virtual void addToTable();

  /**
     the name of the resource's type 
   */
  virtual std::string type_name() = 0;

  /**
     returns if this type of resource is logged 
   */
  virtual bool is_resource_type_logged() = 0;

  /**
     declares that this resource type is now logged 
   */
  virtual void type_logged() = 0;
  
 private:
  /**
     defines the resource table 
   */
  static void define_table();

  /**
     defines the resource type table 
   */
  static void define_type_table();

  /**
     adds a resource type to the resource type table 
   */
  void logNewType();

  /**
     the primary key reference to this resource 
   */
  primary_key_ref pkref_;

  /**
     the primary key reference to this resource type 
   */
  primary_key_ref type_pkref_;

  /**
  True if this resource object has already been recorded to the output db
  */
  bool book_kept_;

};

#endif
