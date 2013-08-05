// Material.h
#if !defined(_MATERIAL_H)
#define _MATERIAL_H

#include <map>
#include <utility>
#include <math.h>
#include <vector>
#include <list>
#include <string>

#include "UseMatrixLib.h"
#include "Resource.h"
#include "IsoVector.h"

namespace cyclus {

/* -- Typedefs -- */
class Material;
typedef boost::intrusive_ptr<Material> mat_rsrc_ptr;
/* -- */

// A type definition for mass units
enum MassUnit { KG, G };

/*!
   @class Material
  
    The Cyclus Material class provides the data structure that supports 
   the isotopic composition of materials passed around in a Cyclus simulation 
   and the functions as the interface with which models interact 
   with materials.
  
   @section intro Introduction
  
   This class keeps track of the isotopic composition of a material.
   Composition can be queried as either atom-based (moles) or mass-based (kg).
   The material state is book-kept each time a material object is transacted
   between simulation agents.
  
   @section dataTables Data Tables
   Material data can be added with a Table class. This is currently under 
   development for the MassTable.
  
   @subsection interface Interface Models in a Cyclus simulation must interact
   with materials in order to perform their fuel cycle tasks. Some models will
   need to create, destroy, query and alter materials and their compositions.
   In order to do this, the Material class, in concert with the IsoVector
   class, provides an interface that supports these actions by any model that
   includes its header file.
  
   @section modifyMaterials Modifying Materials
  
   Most interaction with material objects will take the form of checking their
   composition properties by querying the material object's stored IsoVector.
   The only methods available to modify the material object are the 'absorb'
   and 'extract' methods.  Changing a material object's composition is not
   possible.  If the composition needs to be changed, a new material object
   will need to be created with the desired composition.  In general, the
   setQuantity method should NOT be used to change the mass of a material
   object. An IsoVector containing the desired composition and mass properties
   should be configured prior to the material object creation 
*/
class Material : public Resource {
public:
  /**
     default constructor 
   */
  Material();

  /**
     a constructor for making a material object 
     from a known recipe. 
      
     @param comp isotopic makeup of this material object 
   */
  Material(CompMapPtr comp);

  /**
     a constructor for making a material object 
     from a known isovector. 
      
     @param vec isotopic makeup of this material object 
   */
  Material(IsoVector vec);

  /**
     a constructor for making a material object 
     from another material object 
      
     @param other the material object to copy from 
   */
  Material(const Material& other);
  
  /**
     default destructor 
   */
  ~Material();

  /**
     standard verbose printer includes both an 
     atom and mass composition output 
   */
  void print(); 

  /**
     Returns a boolean indicating whether these materials are equivalent

     @param other the material to compare to this one

     @return equal true if compositions and size are equal. false otherwise.
    */
  virtual bool operator==(const mat_rsrc_ptr other);

  /**
     Returns a boolean indicating whether these materials are equivalent

     @param other the material to compare to this one
     @param threshold the smallest amount considered equal when comparing comps

     @return equal true if equal within the threshold. false otherwise.
    */
  virtual bool almostEqual(const mat_rsrc_ptr other, double threshold) const;

  /**
     Change/set the mass of the resource object. 
     Note that this does make matter (dis)appear and 
     should only be used on objects that are not part of 
     any actual tracked inventory. 
   */
  void setQuantity(double quantity);

  /**
     Change/set the mass of the resource object. 
     Note that this does make matter (dis)appear and 
     should only be used on objects that are not part of 
     any actual tracked inventory. 

     @param quantity the new mass, in units of the unit provided
     @param unit the unit of the mass provided, choose kg, g..
    */
  void setQuantity(double quantity, MassUnit unit);

  /**
     Resource class method 

     @return the mass in kg
   */
  double quantity();


  /**
     Resource class method 
   */
  std::string units() {return "kg";};

  /**
     Resource class method 
   */
  bool checkQuality(rsrc_ptr other);

  /**
     Resource class method 
   */
  ResourceType type() {return MATERIAL_RES;};

  /**
     Resource class method 
   */
  rsrc_ptr clone();

  /**
     Calls the resource class method, but checks 
     the units.

     @param unit is the mass unit. Choose kg, g...
   */
  double mass(MassUnit unit);

  /**
     returns the mass (in kg) of a certain isotope contained 
     in the material.

     @param tope is the isotope identifier. (e.g. 92235)
   */
  double mass(Iso tope);

  /**
     returns the mass of a certain isotope contained 
     in the material.

     @param tope is the isotope identifier. (e.g. 92235)
     @param unit is the mass unit. Choose kg, g...
   */
  double mass(Iso tope, MassUnit unit);

  /** 
     conversion from kg to some other unit

     @param kg the mass in kg to convert
     @param to_unit the unit to convert it to
     */
  double convertFromKg(double kg, MassUnit to_unit);

  /** 
     conversion from kg to some other unit

     @param mass the mass in kg to convert
     @param from_unit the unit to convert it from
     */
  double convertToKg(double mass, MassUnit from_unit);

  /**
     returns the number of atoms, in moles in the material.

    */
  double moles();

  /**
     returns the number of atomes (in moles) of a certain isotope in a material

     @param tope is the isotope identifier. (e.g. 92235)
    */
  double moles(Iso tope);

  /**
     Absorbs the contents of the given 
     Material into this Material and deletes 
     the given Material. 
      
     @param matToAdd the Material to be absorbed (and deleted) 
   */
  virtual void absorb(mat_rsrc_ptr matToAdd);
  /**
     Reports the difference between this material and another material

     @param other the other material

     @return diff a map of isotope ids to amounts (in the MassUnit of unit)
     */
  virtual std::map<Iso, double> diff(const mat_rsrc_ptr other);

  /**
     Reports the difference between this material and a CompMap
      
     @param other the material to compare to the original material
     @param other_amt the amount associated with 
     @param unit the MassUnit with which to interperet the CompMap 

     @return comp_diff a map of isotope ids to amounts (in the MassUnit of unit) 
   */
  virtual std::map<Iso, double> diff(const CompMapPtr other, double other_amt, MassUnit unit=KG);

  /**
     Returns the vec, less the elements whose absolute value are less than the threshold.

     @param vec the map of isos and amounts to which to apply the threshold
     @param threshold the smallest value considered nonzero
     
     @throws CycNegValueError if the threshold provided is negative.
     @returns to_ret, the vector less elements whose abs(val) is less than threshhold
     */
  virtual std::map<Iso, double> applyThreshold(std::map<Iso, double> vec, double threshold);

  /**
     Extracts from this material a composition
     specified by the given CompMapPtr. This operation will change
     the quantity_ and iso_vector_ members.
      
     @param comp_to_rem the composition of material that will be removed against this Material. 
     @param amt_to_rem the amount in *unit* of material that will be removed against this Material. 
     @param unit the MassUnit to do the extraction operation in. Default is KG. 
     @param threshold is the smallest amount considered negligible in this extraction.

     @throws ValueError for overextraction events
     @return the extracted material as a newly allocated material object
   */
  virtual mat_rsrc_ptr extract(const CompMapPtr comp_to_rem, double amt_to_rem, 
      MassUnit unit=KG, double threshold=eps_rsrc());

  /**
     Extracts a specified mass from this material creating a new 
     material object with the same isotopic ratios. 
      
     @param mass the amount (mass) of material that will be removed 
     @throws ValueError for overextraction events
     @return the extracted material as a newly allocated material object 
   */
  virtual mat_rsrc_ptr extract(double mass);

  /**
     Decays this Material object for the amount of time that has passed since
     decay was last called.

     Calling decay effectively updates the material decay to the current
     simulation time-step.
   */
  virtual void decay();

  /**
     Returns a copy of this material's isotopic composition 

     @return a copy of the isovector
   */
  IsoVector& isoVector() {return iso_vector_;}

  /**
     Decays all of the materials if decay is on 
      
     @todo should be private (khuff/rcarlsen) 
   */
  static void decayMaterials();

  /**
     returns true if the resource pointer points to a material resource
  */
  static bool isMaterial(rsrc_ptr rsrc);

  /**
     This scales the composition by the amount of moles or kg, depending on the 
     basis provided. It returns an unnormalized CompMapPtr

     @param basis MASS or ATOMS
     @param unit if the basis is mass, give a unit (KG or G) to calculate in
     */
  CompMapPtr unnormalizeComp(Basis basis, MassUnit unit=KG);

protected:
  /**
     Decays this Material object for the given number of months and 
     updates its composition map with the new number densities. 
      
     @param months the number of months to decay a material 
   */
  void decay(double months);
  

private:
  /**
     used by print() to 'hide' print code when recording is not desired 
   */
  std::string detail(); 

  /**
     last time this material object's state 
     was accurate (e.g. time of last decay, etc.) 
   */
  int last_update_time_;

  /**
     all isotopic details of this material object 
   */
  IsoVector iso_vector_;

  /**
     list of materials 
   */
  static std::list<Material*> materials_;

  /**
     true if decay should occur, false if not. 
   */
  static bool decay_wanted_;

  /**
     how many months between decay calculations 
   */
  static int decay_interval_;

// -------- resource class related members  -------- 
 public:
  /**
     the material class resouce type 
   */
  std::string type_name() {return "material";}

  /**
     resouce type recording state 
   */
  bool is_resource_type_recorded() {return type_is_recorded_;}

  /**
     tells the simulation this resource type is recorded 
   */
  void type_recorded() {type_is_recorded_ = true;}

 private:
  /**
     the state of recording for this resource type 
   */
  static bool type_is_recorded_;
// -------- resource class related members  -------- 


 public:

  /**
     add a material to table 
   */
  virtual void addToTable();

  /**
     return the state id for the iso vector 
   */
  virtual int stateID() {return iso_vector_.comp()->ID();}

};
} // namespace cyclus
#endif
