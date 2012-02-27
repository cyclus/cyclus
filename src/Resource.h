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
  virtual bool checkQuality(rsrc_ptr other) =0;

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
  virtual bool checkQuantityEqual(rsrc_ptr other) = 0;

  /**
   * Returns true if the quantity of the other resource is 
   * greater than the quantity of the base 
   *
   * @param other The resource to evaluate against the base
   *
   * @return True if second is sufficiently equal in quantity to 
   * first, False otherwise.
   */
  virtual bool checkQuantityGT(rsrc_ptr other) = 0;

  /**
   * Compares the quantity and quality of the other resource  
   * to the base
   *
   * @param other The resource to evaluate
   *
   * @return True if other is sufficiently equal to the base, 
   * False otherwise.
   */
  virtual bool checkEquality(rsrc_ptr other);

  /// Returns the concrete resource type, an enum
  virtual ResourceType type() = 0;

  /// Returns a newly allocated copy of the resource
  virtual rsrc_ptr clone() = 0;

  virtual void print() = 0;

  /// return this resource's unique ID
  const int ID() {return ID_;};

  /// return this resource's original ID
  const int originalID() {return originalID_;};

  /// a resource has been split, set the id to the original resource's
  void setOriginalID(int id);

  /// return this resource's originator's ID
  const int originatorID() {return originatorID_;}
  
  /// set the originator. NOTE this is when resources must
  /// be added to their respective tables
  virtual void setOriginatorID(int id) = 0;

  virtual ~Resource();

 protected:
  
  Resource();
  
  int ID_;

  int originalID_;
  
  int originatorID_;

 private:
  
  static int nextID_;

  // -------------------------------------------------------------
  /*!
    output database related members
   */
  
 public:
  // the database table and related information
  static table_ptr resource_table;
  static table_ptr resource_type_table;

  // add a resource to table
  void addToTable();

  // all resource types must have a unit
  virtual std::string type_name() = 0;
  // all resource types must say if they have been logged
  virtual bool is_resource_type_logged() = 0;
  virtual void type_logged() = 0;
  
 private:
  /*!
    Define the database table
   */
  static void define_table();
  static void define_type_table();
  void logNewType();

  /*!
    Store information about the resource's primary key
   */
  primary_key_ref pkref_;
  primary_key_ref type_pkref_;

};

#endif
