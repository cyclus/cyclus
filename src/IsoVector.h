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

#define WF_U235 0.007200 // feed, natural uranium 

/**
 * we will always need Avogadro's number somewhere
 */
#define AVOGADRO 6.02e23

/**
 * we should define this numerical threshold as a simulation parameter
 * its units are kg.
 */
#define EPS_KG 1e-6

/**
 * An enumeration for different types of recipe bases
 **/
enum Basis {ATOMBASED, MASSBASED};

/**
 * A map type to represent all of the parent isotopes tracked.  The key for
 * this map type is the parent's Iso number, and the value is a pair that
 * contains the corresponding decay matrix column and decay constant
 * associated with that parent.
 */
typedef std::map< int, std::pair<int, double> > ParentMap;

/**
 * A map type to represent all of the daughter isotopes tracked.  The key for
 * this map type is the decay matrix column associated with the parent, and the
 * value is a vector of pairs of all the daughters for that parent. Each of the
 * daughters are represented by a pair that contains the daughter's Iso number
 * and its branching ratio.
 */
typedef std::map<int, std::vector<std::pair<int, double> > > DaughtersMap;

/**
 * map isotope (int) to atoms/mass (double)
 */
typedef std::map<int, double> CompMap;

/** 
 * Class Material the object used to transact material objects around the system.
 * 
 * This class keeps track of the isotopic composition of a material using both
 * the atomic fractions and the mass fractions, combined with the total number
 * of atoms and total mass.
 *
 * It is an important goal (requirement) that all material objects maintain an
 * account of the atoms that is consistent with the account of the mass.
 *
 * The default mass unit is kilograms.
 */
class IsoVector {

public:
  
  /**
   * Default constructor for the material class. Creates an empty material.
   */
  IsoVector(); 

  /**
   * a constructor for making a material object from a known recipe and size.
   *
   * @param comp a map from isotopes to number of atoms
   * @param mat_unit the units for this material
   * @param rec_name name of this recipe
   * @param scale is the size of this material
   * @param type indicates whether comp and scale are in mass or atom units
   */
  IsoVector(CompMap comp, std::string mat_unit, std::string rec_name, 
            double scale, Basis type);

  /** 
   * Default destructor does nothing.
   */
  ~IsoVector() {};

  /**
   * Returns the atomic number of the isotope with the given identifier.
   *
   * @param tope the isotope whose atomic number is being returned
   * @return the atomic number
   */
  static int getAtomicNum(int tope);

  /**
   * Returns the mass number of the isotope with the given identifier.
   *
   * @param tope the isotope whose mass number is being returned
   * @return the mass number
   */
  static int getMassNum(int tope);

  /**
   * Reads the decay information found in the 'decayInfo.dat' file into the
   * parent and daughters maps.Uses these maps to create the decay matrix.
   */
  static void loadDecayInfo();

  /**
   * get material ID
   *
   * @return ID
   */
  const int getSN(){return ID_;};

  /**
   * returns the name of the recipe
   *
   * @return recipeName
   */
  std::string getName() { return recipeName_; };

  /**
   * returns the total mass of this material object PER UNIT
   */
  const double mass() const {return total_mass_;};

  /**
   * Returns the current mass of the given isotope, or zero if 
   * that isotope isn't present.
   *
   * @param tope the isotope whose mass in the material will be returned
   * @return the mass of the given isotope within the material, or zero
   */
  const virtual double mass(int tope) const;

  /**
   * returns the total atoms in this material object 
   */
  const double atomCount() const {return total_atoms_;};

  /**
   * Returns the current number density of the given isotope, or zero if 
   * that isotope isn't present.
   *
   * @param tope the isotope whose number density will be returned
   * @return the number density of the given isotope, or zero
   */
  const virtual double atomCount(int tope) const;

  /**
   * Returns the mass of the given element in this Material.
   *
   * @param elt the element
   * @return the mass of the element (in tons)
   */
  const virtual double getEltMass(int elt) const;

  /**
   * Decays this Material object for the given change in time and updates
   * its composition map with the new number densities.
   *
   * @param time_change the number of months to decay
   */
  void decay(double time_change);
  
private:
  /**
   * Builds the decay matrix needed for the decay calculations from the parent
   * and daughters map variables.  The resulting matrix is stored in the static
   * variable decayMatrix.
   */
  static void makeDecayMatrix();

  /**
   * Converts the given mathematical Vector representation of the Material's 
   * isotopic composition back into the map representation.
   *
   * @param compVector the mathematical Vector
   */
  static void makeFromVect(const Vector & compVector, CompMap comp);
  
  /**
   * Stores the next available material ID
   */
  static int nextID_;

  /**
   *
   */
  static ParentMap parent_; 
  
  /**
   *
   */
  static DaughtersMap daughters_; 
  
  /**
   *
   */
  static Matrix decayMatrix_; 

  /**
   * Returns a mathematical Vector representation of the Material's current
   * composition map.
   *
   * @return the mathematical Vector 
   */
  Vector makeCompVector() const;

  /**
   * @brief Used to determine validity of isotope defnition.
   * @param tope 
   * @return true if isotope (number) is valid, false otherwise
   */
  void validateAtomicNumber(int tope);

  /**
   * Returns true if the given isotope's number density is for some reason 
   * negative, false otherwise. We define number densities that are negative by 
   * less than the conservation of mass tolerance as positive.
   *
   * @param tope the isotope in question
   * @return true iff nd(tope) < 0
   */
  const bool isNeg(int tope) const;

  /**
   * Returns true if the given isotope's number density is less than the 
   * conservation of mass tolerance.
   *
   * @param tope the isotope in question
   * @return true iff nd(tope) == 0
   */
  const bool isZero(int tope) const;

  /**
   * Normalizes the composition vector it is provided.
   * 
   * @param comp_map the vector to normalize
   */
  void normalize(CompMap &comp_map);

  /**
   * convert mass composition into a consitent atom composition
   */
  void rationalize_M2A();
  
  /** 
   * The serial number for this Material.
   */
  int ID_;

  /**
   * total mass of this material object PER UNIT
   */
  double total_mass_;

  /**
   * total number of atoms in this material object PER UNIT
   */
  double total_atoms_;

  /*
   * name of this recipe
   */
  std::string recipeName_;
  
  /*
   * Core isotope composition information stored here.
   */
  CompMap atom_comp_;

};

#endif
