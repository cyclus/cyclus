// Material.h
#if !defined(_MATERIAL_H)
#define _MATERIAL_H

#include <map>
#include <utility>
#include <math.h>
#include <vector>

#include "InputXML.h"
#include "UseMatrixLib.h"

#define WF_U235 0.007200 // feed, natural uranium 

/**
 * type definition for isotopes
 */
typedef int Iso;

/**
 * type definition for elements
 */
typedef int Elt;

/**
 * type definition for atom count
 */
typedef double Atoms;

/**
 * type definition for mass
 */
typedef double Mass;

/**
 * Spectra for which average cross-section data are available.
 */
enum Spectrum {THERMAL, FAST};

/**
 * Represents a decay constant of a parent isotope.
 */
typedef double DecayConst;

/**
 * Represents a branching ratio of a daughter isotope.
 */
typedef double BranchRatio;

/**
 * Represents a column of the decay matrix.
 */
typedef int Col;

/**
 * A map type to represent all of the parent isotopes tracked.  The key for
 * this map type is the parent's Iso number, and the value is a pair that
 * contains the corresponding decay matrix column and decay constant
 * associated with that parent.
 */
typedef std::map< Iso, std::pair<Col, DecayConst> > ParentMap;

/**
 * A map type to represent all of the daughter isotopes tracked.  The key for
 * this map type is the decay matrix column associated with the parent, and the
 * value is a vector of pairs of all the daughters for that parent. Each of the
 * daughters are represented by a pair that contains the daughter's Iso number
 * and its branching ratio.
 */
typedef std::map<Col, std::vector<std::pair<Iso, BranchRatio> > > DaughtersMap;

/**
 * map integers to doubles: Iso => (Atoms|Mass)
 */
typedef std::map<Iso, double> CompMap;

/**
 * A map for storing the composition history of a material.
 */
typedef std::map<int, std::map<Iso, Atoms> > CompHistory;

/**
 * A map for storing the mass history of a material.
 */
typedef std::map<int, std::map<Iso, Mass> > MassHistory;

/**
 * A map for storing the facility history of a material.
 */
typedef std::map<int, std::pair<int, int> > FacHistory;

/**
 * An enumeration for different types of recipe bases
 **/
enum Basis {ATOMBASED, MASSBASED};

/**
 * we will always need Avogadro's number somewhere
 */
#define AVOGADRO 6.02e23

/**
 * we should define this numerical threshold as a simulation parameter
 * its units are kg.
 */
#define eps 1e-6

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
class Material {

public:
  
  /**
   * Default constructor for the material class. Creates an empty material.
   */
  Material(); 

  /**
   * primary constructor reads input from XML node
   *
   * @param cur input XML node
   */
  Material(xmlNodePtr cur);

  /**
   * a constructor for making a material object from a known recipe and size.
   *
   * @param comp a map from isotopes to number of atoms
   * @param mat_unit the units for this material
   * @param rec_name name of this recipe
   * @param scale is the size of this material
   * @param type indicates whether comp and scale are in mass or atom units
   */
  Material(CompMap comp, std::string mat_unit, std::string rec_name, 
            double scale, Basis type);

  
  /** 
   * Default destructor does nothing.
   */
  ~Material() {};

  /**
   * loads the recipes from the input file
   */
  static void load_recipes();
  
  /**
   * standard verbose printer includes both an atom and mass composition output
   */
  void print(); 
    
  /**
   * verbose printer for a single type of composition
   *
   * @param header is the description to precede the composition 
   * @param comp_map is the map between isotopes and mass
   */
  void printComp(std::string header, CompMap comp_map);

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
   * returns the units of the recipe, a string
   *
   * @return units
   */
  std::string getUnits() { return units_; };

  /**
   * returns the total mass of this material object PER UNIT
   */
  const Mass getTotMass() const {return total_mass_;};

  /**
   * Returns the total mass of the given composition vector.
   * 
   * @param comp the composition vector
   * @return the mass (in tons)
   */
  static double getTotMass(const CompMap& comp);

  /**
   * returns the total atoms in this material object 
   */
  const Atoms getTotAtoms() const {return total_atoms_;};

  /**
   * Returns the total atoms in the given composition vector.
   * 
   * @param comp the composition vector
   * @return the total atoms
   */
  static Atoms getTotAtoms(const CompMap& comp);

  /**
   * Returns the entire (current) mass vector for this Material.
   *
   * @return the current mass composition
   */
  virtual const CompMap getMassComp() const;

  /**
   * Returns the current mass of the given isotope, or zero if 
   * that isotope isn't present.
   *
   * @param tope the isotope whose mass in the material will be returned
   * @return the mass of the given isotope within the material, or zero
   */
  const virtual Mass getMassComp(Iso tope) const;

  /**
   * Returns the mass of the given isotope in the given composition vector.
   *
   * @param tope the atomic number of the element
   * @param comp the composition vector
   * @return the mass
   */
  static double getMassComp(Iso tope, const CompMap& comp);

  /**
   * Returns the entire (current) isotopic vector for this Material.
   *
   * @return the current composition
   */
  virtual const CompMap getAtomComp() const;

  /**
   * Returns the atoms of the given isotope in the given composition vector.
   *
   * @param tope the atomic number of the element
   * @param comp the composition vector
   * @return the number of atoms
   */
  static double getAtomComp(Iso tope, const CompMap& comp);
 
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
  const virtual Atoms getAtomComp(Iso tope) const;

  /**
   * Returns the atomic number of the isotope with the given identifier.
   *
   * @param tope the isotope whose atomic number is being returned
   * @return the atomic number
   */
  static int getAtomicNum(Iso tope);

  /**
   * Returns the mass number of the isotope with the given identifier.
   *
   * @param tope the isotope whose mass number is being returned
   * @return the mass number
   */
  static int getMassNum(Iso tope);

  /**
   * Returns the atomic mass of the isotope with the given identifier.
   *
   * @param tope the isotope whose mass is being returned
   * @return the atomic mass (g/mol) 
   */
  static Mass getAtomicMass(Iso tope);

  /**
   * Adds (or subtracts) from the number density of the given isotope by 
   * the amount.
   *
   * @param tope the isotope whose number density we want to change
   * @param change if positive, the amount to add to the given isotopes 
   * number density; if negative, the amount to subtract
   * @param time the time at which this takes place.
   */
  virtual void changeComp(Iso tope, Atoms change, int time);

  /**
   * Changes the atomic composition to a new composition
   *
   * @param newComp is the new atom composition vector
   * @param time is the time at which this takes place
   */
  virtual void changeAtomComp(CompMap newComp, int time);

  /**
   * Changes the mass composition to a new composition
   *
   * @param newComp is the new atom composition vector
   * @param time is the time at which this takes place
   */
  virtual void changeMassComp(CompMap newComp, int time);

  /**
   * Returns the mass of the given element in this Material.
   *
   * @param elt the element
   * @return the mass of the element (in tons)
   */
  const virtual double getEltMass(int elt) const;

  /**
   * Returns the mass of the given element in the given composition vector.
   *
   * @param elt the atomic number of the element
   * @param comp the composition vector
   * @return the mass (in tons)
   */
  static double getEltMass(int elt, const std::map<Iso, Atoms>& comp);

  /**
   * Returns the mass of the given isotope in this Material.
   *
   * @param tope the isotope
   * @return the mass of the element (in tons)
   */
  const virtual double getIsoMass(Iso tope) const;

  /**
   * Returns the mass of the given isotope in the given composition vector.
   *
   * @param tope the atomic number of the element
   * @param comp the composition vector
   * @return the mass (in tons)
   */
  static double getIsoMass(Iso tope, const CompMap& comp);
 
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

  /**
   * Normalizes the composition vector it is provided.
   * 
   * @param comp_map the vector to normalize
   */
  void normalize(CompMap &comp_map);

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

  /**
   * Reads the decay information found in the 'decayInfo.dat' file into the
   * parent and daughters maps.Uses these maps to create the decay matrix.
   */
  static void loadDecayInfo();

protected:
  /** 
   * The serial number for this Material.
   */
  int ID_;

  /**
   * Stores the next available material ID
   */
  static int nextID_;

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
   * corresponding number of atoms.
   */
  CompHistory compHist_;

  /**
   * The mass history of this Material, in the form of a map whose
   * keys are integers representing the time at which this Material had a 
   * particular composition and whose values are the corresponding mass 
   * compositions. A composition is a map of isotopes and the corresponding
   * masses.
   */
  MassHistory massHist_;
    
  /**
   * The facility history of this Material. The form is a map whose keys 
   * are integers representing the time of a transfer and whose values 
   * are pairs storing (first) the SN of the originating facility and 
   * (second) the SN of the destination facility.
   */
  FacHistory facHist_;

  /**
   * Builds the decay matrix needed for the decay calculations from the parent
   * and daughters map variables.  The resulting matrix is stored in the static
   * variable decayMatrix.
   */
  static void makeDecayMatrix();

  /**
   * Returns a mathematical Vector representation of the Material's current
   * composition map.
   *
   * @return the mathematical Vector 
   */
  Vector makeCompVector() const;

  /**
   * Converts the given mathematical Vector representation of the Material's 
   * isotopic composition back into the map representation.
   *
   * @param compVector the mathematical Vector
   * @return the composition map
   */
  static std::map<Iso, Atoms> makeCompMap(const Vector & compVector);

private:
  /**
   * loads a recipebook full of recipes
   *
   * @param filename the name of the recipebook file, including the 
   * extension
   * @param ns the namespace under which these recipes will reside
   * @param format the format of the recipebook
   */
  static void load_recipebook(std::string filename, std::string ns, 
                               std::string format);

  /**
   * @brief Used to determine validity of isotope defnition.
   * @param tope 
   * @return true if isotope (number) is valid, false otherwise
   */
  static bool isAtomicNumValid(Iso tope);

  /**
   * total mass of this material object PER UNIT
   */
  Mass total_mass_;

  /**
   * total number of atoms in this material object PER UNIT
   */
  Atoms total_atoms_;

  /**
   * Keeps track of whether or not the mass & atom count is still consistent
   */
  bool atomEqualsMass_;

  /**
   * units for this material
   */
  std::string units_;

  /*
   * name of this recipe
   */
  std::string recipeName_;
  
  /**
   * convert an atom composition into a consitent mass composition
   */
  void rationalize_A2M();

  /**
   * convert mass composition into a consitent atom composition
   */
  void rationalize_M2A();
  
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
};




#endif
