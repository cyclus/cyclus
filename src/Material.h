// Material.h
#if !defined(_MATERIAL_H)
#define _MATERIAL_H

#include <map>
#include <utility>
#include <math.h>
#include <vector>
#include <string>
#include <libxml/tree.h>

#include "UseMatrixLib.h"
#include "Resource.h"
#include "IsoVector.h"

#define WF_U235 0.007200 // feed, natural uranium 

class Material;
typedef boost::intrusive_ptr<Material> mat_rsrc_ptr;

/**
   @class Material

   @brief The Cyclus Material class provides the data structure that supports 
   the isotopic histories of materials passed around in a Cyclus simulation 
   and the functions that comprise the interface with which models interact 
   with materials.

   @section intro Introduction
   This class keeps track of the isotopic composition of a material using both
   the atomic fractions and the mass fractions, combined with the total number
   of atoms and total mass.
   
   It is an important goal (requirement) that all material objects maintain an
   account of the atoms that is consistent with the account of the mass.
   
   The default mass unit is kilograms.

   @section necessaryParams Necessary Parameters
   A fully defined material must have:
   - Isotopic Composition in mass or atom units
   
   @section optionalParams Optional Parameters
   Optional Parameters
   - rec_name
   - units
   - total_atoms
   - total_mass

   @section atomTracking Atomic Tracking
   The history of the isotope resolution atomic composition of the material 
   is kept in a map of compositions over time. The composition at each time 
   is a vector of pairs of Isotope identifiers and the number of Atoms of 
   each isotope. The total atoms in a material are kept track of via a loop 
   over each the composition at that time.

   @section massTracking Mass Tracking
   The history of the mass composition of the material is kept in a map of 
   compositions over time. The composition at each time is a vector of pairs 
   of Isotope identifiers and the mass it contains of each isotope. The total 
   mass of a material is kept track of via a loop over each the composition 
   at that time

   @section recipes Recipes
   Recipes can be in either mass or atom units and define an often referenced 
   materal composition.

   @section dataTables Data Tables
   Material data can be added with a Table class. This is currently under 
   development for the Mass Table.

   @subsection interface Interface
   Models in a Cyclus simulation must interact with materials in order to 
   perform their fuel cycle tasks. Some models will need to create, destroy, 
   query and alter materials and their compositions. In order to do this, the 
   Material class provides an interface that supports these actions by any 
   model that includes its header file.
   
   @section deleteMaterials Deleting Materials
   Only in very rare cases should this be done. In order to conserve mass and 
   atoms in this simulation models must maintain mass an atomic conservation. 
   So, materials in the real world typically do not vanish. Rather, they are 
   transmuted. This behavior is best mimicked by the implementation of your 
   model.

   That said, destruction of a material in Cyclus is done by changing the 
   composition to zero. The composition history of the material will reflect 
   this change and the material object will persist "under the hood" but its 
   composition for the extent of the simulation time following this change 
   will reflect that it was reduced to a void.
   
   @section queryMaterial Querying Material Objects
   Public data access functions of the material class are used to query 
   material composition, total mass, atoms, atom fractions, etc.

   Furthmore, access to nuclide data is also done through the Material Class.

   @section changingComp Changing Material Compositions
   Methods for adding and subtracting mass and isotopes from material objects 
   are provided by many public member functions of the MaterialClass.
*/
class Material : public Resource {

public:
  Material();

  /**
   * a constructor for making a material object from a known recipe and size.
   *
   * @param comp isotopic makeup of this material object
   */
  Material(IsoVector comp);

  /// copy constructor
  Material(const Material& other);
  
  /// Default destructor does nothing.
  ~Material() {};

  
  /**
   * standard verbose printer includes both an atom and mass composition output
   */
  void print(); 
    
  /**
   * Change/set the mass of the resource object. Note that this does make
   * matter (dis)appear and should only be used on objects that are not part of
   * any actual tracked inventory.
   */
  void setQuantity(double quantity) {iso_vector_.setMass(quantity);};

  /**
   * Resource class method
   */
  double quantity() {return iso_vector_.mass();};

  /**
   * Resource class method
   */
  std::string units() {return "kg";};

  /**
   * Resource class method
   */
  bool checkQuality(rsrc_ptr other);

  /**
   * Resource class method
   */
  bool checkQuantityEqual(rsrc_ptr other);

  /**
   * Resource class method
   */
  bool checkQuantityGT(rsrc_ptr other);

  /**
   * Resource class method
   */
  ResourceType type() {return MATERIAL_RES;};

  /**
   * Resource class method
   */
  mat_rsrc_ptr clone();

  /**
   * Absorbs the contents of the given Material into this Material and deletes 
   * the given Material. 
   * 
   * @param matToAdd the Material to be absorbed (and deleted)
   */
  virtual void absorb(mat_rsrc_ptr matToAdd);

  /**
   * Extracts from this material a composition specified by the given IsoVector
   *
   * @param rem_comp the composition/amount of material that will be removed
   * against this Material
   * 
   * @return the extracted material as a newly allocated material object
   */
  virtual mat_rsrc_ptr extract(IsoVector rem_comp);

  /**
   * Extracts a specified mass from this material creating a new material
   * object with the same isotopic ratios.
   *
   * @param the amount (mass) of material that will be removed
   * 
   * @return the extracted material as a newly allocated material object
   */
  virtual mat_rsrc_ptr extract(double mass);

  /**
   * Decays this Material object for the given number of months and updates
   * its composition map with the new number densities.
   *
   * @param months the number of months to decay
   */
  void decay(double months);
  
  /**
   * Decays this Material object for however many months have passed since the 
   * last entry in the material history.
   *
   */
  void decay();

  /*!
  Returns a copy of this material's isotopic composition
  */
  IsoVector isoVector() {return iso_vector_;}

  /**
   * Decays all of the materials if decay is on
   *
   * @todo should be private (khuff/rcarlsen)
   *
   * @param time is the simulation time of the tick
   */
  static void decayMaterials(int time);
  
  /*
   * sets the decay boolean and the interval
   */
  static void setDecay(int dec);

private:

  /// last time this material object's state was accurate (e.g. time of last
  /// decay, etc.)
  int last_update_time_;

  /// all isotopic details of this material object
  IsoVector iso_vector_;

  /// list of materials
  static std::vector<mat_rsrc_ptr> materials_;

  /// true if decay should occur, false if not.
  static bool decay_wanted_;

  /// how many months between decay calculations
  static int decay_interval_;

};

#endif
