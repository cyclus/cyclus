// Material.h
#if !defined(_MATERIAL_H)
# define _MATERIAL_H

#include <map>
#include <iostream>
#include <math.h>

#include "InputXML.h"

using namespace std;

/// type definition for isotopes
typedef int Iso;
/// type definition for elements
typedef int Elt;
/// type definition for atom count
typedef double Atoms;
/// type definition for mass
typedef double Mass;
/// map integers to doubles: Iso => (Atoms|Mass)
typedef map<Iso, Atoms> CompMap;
/**
 * A map for storing the composition history of a material.
 */
typedef map<int, map<Iso, Atoms> > CompHistory;

/**
 * A map for storing the facility history of a material.
 */
typedef map<int, pair<int, int> > FacHistory;


/// we will always need Avogadro's number somewhere
#define AVOGADRO 6.02e23
/// we should define this material mass threshold as a simulation parameter
#define eps 1e-6

/*! 
 * Class Material the object used to transact material objects around the system.
 * 
 * This class keeps track of the isotopic composition of a material using both
 * the atomic fractions and the mass fractions, combined with the total number
 * of atoms and total mass.
 *
 * It is an important goal (requirement) that all material objects maintain an
 * account of the atoms that is consistent with the account of the mass.
 */

class Material {

public:
  
  /// default constructor
  Material() : atomEqualsMass(true), total_mass(0), total_atoms(0) {};

  /**
   * primary constructor reads input from XML node
   *
   * @param cur input XML node
   */
  Material(xmlNodePtr cur);

  /**
   * a constructor for making a material object from scratch.
   *
   * @param comp a map from isotopes to number of atoms
   * @param mat_unit the units for this material
   * @param rec_name name of this recipe
   */
  Material(CompMap comp, string mat_unit, string rec_name);
  
  /** 
   * Default destructor does nothing.
   */
  ~Material() {};

  /**
   * loads the recipes from the input file
   */
  static void load_recipes();
  
  /// standard verbose printer includes both an atom and mass composition output
  void print() {
    printComp("Atom composition:", atom_comp);
    cout << "\tTotal atoms: " << total_atoms 
        << " per " << units << endl;
    printComp("Mass composition:", mass_comp);
    cout << "\tTotal mass: " << total_mass 
        << " kg per " << units << endl;
  }

  /**
   * verbose printer for a single type of composition
   *
   * @param header is the description of the composition 
   * @param comp_map is the map between isotopes and mass
   */
  void printComp(string header, CompMap comp_map);

  /**
   * returns the name of the recipe
   *
   * @return recipeName
   */
  string getName() { return recipeName; };

  /**
   * returns the units of the recipe, a string
   *
   * @return units
   */
  string getUnits() { return units; };

  /**
   * returns the total mass of this material object PER UNIT
   */
   const Mass getTotMass() const { return total_mass; };

  /**
   * Returns the entire (current) isotopic vector for this Material.
   *
   * @return the current composition
   */
  virtual const CompMap getComp() const;

  /**
   * Returns an isotopic vector corresponding to the given fraction of this 
   * Material's current composition.
   *
   * @param frac the fraction of this Material's composition to return (we 
   * hold the stoichiometry constant, so you can think of this as a weight 
   * fraction OR an atom fraction)
   * @return the fractional composition vector
   */
  const virtual CompMap getFracComp(double frac) const;

  /**
   * Returns the current number density of the given isotope, or zero if 
   * that isotope isn't present.
   *
   * @param tope the isotope whose number density will be returned
   * @return the number density of the given isotope, or zero
   */
  const virtual Atoms getComp(Iso tope) const;

  /**
   * Returns the mass number of the isotope with the given identifier.
   *
   * @param tope the isotope whose mass number is being returned
   * @return the mass number
   */
  static int getMassNum(Iso tope);

  /**
   * Adds (or subtracts) from the number density of the given isotope by 
   * the amount.
   *
   * @param tope the isotope whose number density we want to change
   * @param change if positive, the amount to add to the given isotopes 
   * number density; if negative, the amount to subtract
   * @param time the current time
   */
  virtual void changeComp(Iso tope, Atoms change, int time);

  /**
   * Returns the mass of the given isotope in the given composition vector.
   *
   * @param tope the atomic number of the element
   * @param comp the composition vector
   * @return the mass (in tons)
   */
  static double getIsoMass(Iso tope, const CompMap& comp);

  /**
   * Returns the total mass of the given composition vector.
   * 
   * @param comp the composition vector
   * @return the mass (in tons)
   */
  static double getTotMass(const CompMap& comp);
 
  /**
   * Absorbs the contents of the given Material into this Material and deletes 
   * the given Material. 
   * 
   * @param matToAdd the Material to be absorbed (and deleted)
   */
  virtual void absorb(Material* matToAdd);

  /**
   * Extracts the contents of the given Material from this Material. Use this 
   * function for decrementing a Facility's inventory after constructing a new 
   * Material to send to another Facility to execute an order, or for similar 
   * tasks.
   *
   * @param matToRem the Material whose composition we want to decrement 
   * against this Material
   */
  virtual void extract(Material* matToRem);


  /**
   * Extracts a material object of the given mass from the material.
   * The stoichiometry of the extracted object is the same as the parent
   * and the mass of the parent is altered to reflect the change. 
   *
   * @param extract the mass to extract.
   * @return the new material object
   */
  virtual Material* extractMass(Mass extract);
protected:
  /**
   * Returns true if the given isotope's number density is for some reason 
   * negative, false otherwise. We define number densities that are negative by 
   * less than the conservation of mass tolerance as positive.
   *
   * @param tope the isotope in question
   * @return true iff nd(tope) < 0
   */
  const bool isNeg(Iso tope) const;

  /**
   * Returns true if the given isotope's number density is less than the 
   * conservation of mass tolerance.
   *
   * @param tope the isotope in question
   * @return true iff nd(tope) == 0
   */
  const bool isZero(Iso tope) const;

  /**
   * The composition history of this Material, in the form of a map whose
   * keys are integers representing the time at which this Material had a 
   * particular composition and whose values are the corresponding 
   * compositions. A composition is a map of isotopes and their 
   * corresponding number densities.
   */
  CompHistory compHist;
    
  /**
   * The facility history of this Material. The form is a map whose keys 
   * are integers representing the time of a transfer and whose values 
   * are pairs storing (first) the SN of the originating facility and 
   * (second) the SN of the destination facility.
   */
  FacHistory facHist;

private:

  static void load_recipebook(string filename, string ns, string format);

  /// map isotopes to number of atoms
  CompMap atom_comp, 

	/// map isotope to mass
	mass_comp;

  /// total mass of this material object PER UNIT
  Mass total_mass;

  /// total number of atoms in this material object PER UNIT
  Atoms total_atoms;

  /// keep track of whether or not the mass & atom count is still consistent
  bool atomEqualsMass;

  /// units for this material
  string units;

  /// name of this recipe
  string recipeName;
  
  /// get the atomic number of an isotope
  double getZ(Iso isotope) { return isotope/10/1000; };
  /// get the mass number of an isotope
  /**
   *  This function should be replaced by a function that can
   *  lookup the exact mass of an isotope, not just its mass number
   *
   *  @param isotope is the isotope whose mass you'd like to know
   */
  double getA(Iso isotope) { return ((isotope/10)%1000); };
  
  /**
   * normalize a composition so that we have atom/mass fractions
   *
   * @param &comp_map is the composition to normalize
   */
  void normalize(CompMap &comp_map);

  /// convert an atom composition into a consitent mass composition
  void rationalize_A2M();

  /// convert mass composition into a consitent atom composition
  void rationalize_M2A();
 
};




#endif
