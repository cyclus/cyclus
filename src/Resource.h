// Resource.h
#if !defined(_RESOURCE_H)
#define _RESOURCE_H

#include <string>
#include <boost/intrusive_ptr.hpp>

#include "IntrusiveBase.h"
#include "Table.h"

class Resource;
typedef boost::intrusive_ptr<Resource> rsrc_ptr;

/// A list of concrete types of resource
enum ResourceType { MATERIAL_RES, GENERIC_RES, LAST_RES }; 

/**
   @brief
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
   * @brief A boolean comparing the quality of the other resource 
   * to the quality of the base 
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if other is sufficiently equal in quality to 
   * the base, False otherwise.
   */
  virtual bool checkQuality(rsrc_ptr other) =0;

  /**
   * @brief Returns the base unit of this resource 
   *
   * @return resource_unit_ the base unit of this resource
   */
  virtual std::string units() = 0;
    
  /**
   * @brief Returns the total quantity of this resource in its base unit 
   *
   * @return the total quantity of this resource in its base unit
   */
  virtual double quantity() = 0;

  /**
   * @brief Set the quantity of the resource in its default units to 'val'.
   *
   */
  virtual void setQuantity(double val) = 0;
    
  /**
   * @brief A boolean comparing the quantity of the other resource is 
   * to the quantity of the base
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if other is sufficiently equal in quantity to 
   * the base, False otherwise.
   */
  virtual bool checkQuantityEqual(rsrc_ptr other) = 0;

  /**
   * @brief Returns true if the quantity of the other resource is 
   * greater than the quantity of the base 
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if second is sufficiently equal in quantity to 
   * first, False otherwise.
   */
  virtual bool checkQuantityGT(rsrc_ptr other) = 0;

  /**
   * @brief Compares the quantity and quality of the other resource  
   * to the base
   *
   * @param other The resource to evaluate
   *
   * @return True if other is sufficiently equal to the base, 
   * False otherwise.
   */
  virtual bool checkEquality(rsrc_ptr other);

  /**
   * @brief Returns the concrete resource type, an enum
   */ 
  virtual ResourceType type() = 0;

  /**
   * @brief Returns a newly allocated copy of the resource
   */ 
  virtual rsrc_ptr clone() = 0;

  /**
   * @brief Prints information about the resource
   */
  virtual void print() = 0;

  /**
   * @brief return this resource's unique ID
   */ 
  const int ID() {return ID_;};

  /**
   * @brief return this resource's original ID
   */
  const int originalID() {return originalID_;};

  /**
   * @brief a resource has been split, set the id to the original resource's
   */ 
  void setOriginalID(int id);

  /**
   * @brief return this resource's originator's ID
   */ 
  const int originatorID() {return originatorID_;}
  
  /**
   * @brief set the originator. NOTE this is when resources must
   * be added to their respective tables
   */ 
  virtual void setOriginatorID(int id) = 0;

  /**
   * @brief Destructor
   */
  virtual ~Resource();

 protected:
  /**
   * @brief Constructor
   */
  Resource();

  /**
   * @brief Resource ID
   */
  int ID_;

  /**
   * @brief After a split, this is the original resource's id
   */
  int originalID_;

  /**
   * @brief the resource originator's id
   */
  int originatorID_;

 private:

  /**
   * @brief the next resource id
   */
  static int nextID_;

// -------- output database related members  -------- 
  
 public:
  /**
   * @brief the output database table which logs resources
   */
  static table_ptr resource_table;

  /**
   * @brief the output database table which logs resource types
   */
  static table_ptr resource_type_table;

  /**
   * @brief adds a resource to the resouce table
   */
  void addToTable();

  /**
   * @brief the name of the resource's type
   */
  virtual std::string type_name() = 0;

  /**
   * @brief returns if this type of resource is logged
   */
  virtual bool is_resource_type_logged() = 0;

  /**
   * @brief declares that this resource type is now logged
   */
  virtual void type_logged() = 0;
  
 private:
  /**
   * @brief defines the resource table
   */
  static void define_table();

  /**
   * @brief defines the resource type table
   */
  static void define_type_table();

  /**
   * @brief adds a resource type to the resource type table
   */
  void logNewType();

  /**
   * @brief the primary key reference to this resource
   */
  primary_key_ref pkref_;

  /**
   * @brief the primary key reference to this resource type
   */
  primary_key_ref type_pkref_;

};

#endif
