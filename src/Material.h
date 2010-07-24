// Material.h
#if !defined(_MATERIAL_H)
# define _MATERIAL_H

#include <map>

#include "InputXML.h"

using namespace std;

/// type definition for isotopes
typedef int Iso;
/// type definition for elements
typedef int Ele;
/// type definition for atom count
typedef double Atoms;
/// type definition for mass
typedef double Mass;
/// map integers to doubles: Iso => (Atoms|Mass)
typedef map<int,double> CompMap;

/// we will always need Avogadro's number somewhere
#define AVOGADRO 6.02e23

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

  /// primary constructor reads input from XML node
  Material(xmlNodePtr cur);

  ~Material() {};

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

  /// verbose printer for a single type of composition
  void printComp(string header, CompMap comp_map);

  string getName() { return recipeName; };
  string getUnits() { return units; };

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
   */
  double getA(Iso isotope) { return ((isotope/10)%1000); };
  
  /// normalize a composition so that we have atom/mass fractions
  void normalize(CompMap &comp_map);
  /// convert an atom composition into a consitent mass composition
  void rationalize_A2M();
  /// convert mass composition into a consitent atom composition
  void rationalize_M2A();
 
};




#endif
