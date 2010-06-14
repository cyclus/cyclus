// Material.h

#if !defined(_MATERIAL)
#define _MATERIAL
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <iostream>

#include "Commodity.h"
#include "UseMatrixLib.h"
#include "streamOps.hpp"

using namespace std;

/**
 * Enumerative types to specify the chemical form of this material (gas, 
 * liquid, or solid).
 */
enum ChemForm {gas, liq, sol};

/**
 * Spectra for which average cross-section data are available.
 */
enum Spectrum {thermal, fast};

/**
 * An integer type to represent an isotope. Recently changed to be calculated 
 * from the following formula:
 *
 * Iso = (Z * 1000 + A) * 10 + I
 *
 * where Z = atomic number, A = mass number, I = isomeric state code
 */
typedef int Iso;

/**
 * The atomic number of an element.
 */
typedef int Elt;

/**
 * The name of a commodity in the CommodityMap
 */
typedef string CommodType;

/**
 * The ID of a commodity in the CommodityMap
 */
typedef int ComID;

/**
 * A double type to represent a number density (of a particular isotope).
 */
typedef double NumDens;

/**
 * A double type to represent a decay constant of a parent isotope.
 */
typedef double DecayConst;

/**
 * A double type to represent a branching ratio of a daughter isotope.
 */
typedef double BranchRatio;

/**
 * An integer type to represent a column of the decay matrix.
 */
typedef int Col;

/**
 * A map type to represent all of the parent isotopes tracked.  The key for
 * this map type is the parent's Iso number, and the value is a pair that
 * contains the corresponding decay matrix column and decay constant
 * associated with that parent.
 */
typedef map< Iso, pair<Col, DecayConst> > ParentMap;

/**
 * A map type to represent all of the daughter isotopes tracked.  The key for
 * this map type is the decay matrix column associated with the parent, and the
 * value is a vector of pairs of all the daughters for that parent. Each of the
 * daughters are represented by a pair that contains the daughter's Iso number
 * and its branching ratio.
 */
typedef map< Col, vector< pair<Iso, BranchRatio> > > DaughtersMap;

/**
 * A map for storing the composition history of a material.
 */
typedef map<int, map<Iso, NumDens> > CompHistory;

/**
 * A map for storing the facility history of a material.
 */
typedef map<int, pair<int, int> > FacHistory;

// Define some macros for common constants we'll need.

#define AV_NUM 6.02E23 // Avagadro's number

// Molecular weights:
#define MW_U 238.03
#define MW_O 16.00

// Natural weight fractions:

// Not including U-234 (approximation):
#define WF_235 0.007200
#define WF_238 0.992800

// The conservation of mass tolerance. That is, an "epsilon" for testing the 
// equality of doubles representing masses in tons.
#define eps 1e-6 // = 1 g

// Non-physical isotopes that track "other" categories. We use an effective mass 
// number, and atomic numbers of 999 for multi-element collections. The isomeric 
// code ("one's digit) for these other collections is always 9.
#define OTHER_FP 8881159 // "other": fission products
#define OTHER_ACT 9992409 // "other": actinides
#define OTHER_CS 551339 // "other": cesium
#define OTHER_KR 360849 // "other": krypton
#define OTHER_C 60129 // "other": carbon
#define OTHER_SR 380889 // "other": strontium
#define OTHER_I 531279 // "other" : iodine
#define OTHER_TC 430989 // "other": technetium

/**
 * A structure to specify what kind of commodity a Material is. "all" 
 * and "fissile" are reserved for when we need to refer to multiple types of 
 * Material. They should not be assigned to actual Material objects.
 */
struct Commodity {
	static int nextID;
	CommodType name;
	ComID ID;
	Market* marketPtr;
	bool treatmentA;
	bool treatmentB;
	} cake, uUF6, eUF6, dUF6, eUoxFuel, uUoxFuel, moxFuel, truFuel, 
									 fuel, usedFuel, waste, sepU, sepPuPlus, sepMAct1, sepMAct2, 
									 sepFP, sepCsSr, sepI, sepTc, sepFGas, sepMox, fissile, all, 
									 none;	

/**
 * A map of commodity types being used in this simulation. 
 * This will hopefully replace the above enumeration in a more extensible fasion.
 * The keys to this map are ComIDs, which are typedef string
 * The elements are the Commodity data structures themselves.
 */
map<CommodType, Commodity> CommodityMap
/**
 * The parent class for all materials (fuel assemblies, etc.).
 */
class Material 
{
private:

	/**
	 *
	 */
	static ParentMap parent; 
	
	/**
	 *
	 */
	static DaughtersMap daughters; 
	
	/**
	 *
	 */
	static Matrix decayMatrix; 

	/**
	 * Averaged capture cross sections by spectrum and isotope.
	 */
	static map<Spectrum, map<Iso, double> > avgCapXSects;

	/**
	 * Averaged fission cross sections by spectrum and isotope.
	 */
	static map<Spectrum, map<Iso, double> > avgFisXSects;

	/**
	 * Averaged nu values by spectrum and isotope.
	 */
	static map<Spectrum, map<Iso, double> > avgNus;

	/**
	 * Initializes averaged fission cross section for the given
	 * spectrum.
	 *
	 * @param the spectrum we want to average over
	 */
	static void initAvgFisXSects(Spectrum s);

	/**
	 * Initializes averaged capture cross section for the given
	 * spectrum.
	 *
	 * @param the spectrum we want to average over
	 */
	static void initAvgCapXSects(Spectrum s);

	/**
	 * Initializes averaged nu values for the given spectrum.
	 *
	 * @param the spectrum we want to average over
	 */
	static void initAvgNus(Spectrum s);
protected: 
		
	/**
	 * The serial number for this Material.
	 */
	long ID;

	/**
	 * The chemical form of this Material (sol, liq, or gas).
	 */
	ChemForm myForm;

	/**
	 * The Commodity type of this Material.
	 */
	Commodity myType;
	
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
	static map<Iso, NumDens> makeCompMap(const Vector & compVector);

public:
		
	/**
	 * Constructs a default Material.
	 */
	Material();

	/**
	 * Constructs a new Material object with the given composition and
	 * chemical form.
	 *
	 * @param comp a map where the keys are isotope identifiers and the 
	 * values  are the "number densities" (number of atoms) of the 
	 * corresponding isotopes
	 * @param form the chemical form of this Material
	 * @param commod the Commodity type being created
	 */
	Material(map<Iso, NumDens> comp, ChemForm form, Commodity commod);

	/**
	 * Constructs a Material of the given Commodity type and amount. What 
	 * "amount" and "par" mean varies by Commodity type:
	 *
	 * cake:  amount --  metric ton U, par -- weight fraction U
	 * uUF6:  amount --  metric ton U, par -- N/A
	 * eUF6:  amount --  metric ton U, par -- U-235 enrichment (betw 0 and 1)
	 *
	 * @param commod the type of Commodity being created
	 * @param amount some measure of the quantity being created
	 * @param par some other paramater necessary for getting the stoich right
	 */
	Material(Commodity commod, double amount, double par);

	/**
	 * Destroys this Material (virtual destructor).
	 */
	virtual ~Material();

	/**
	 * Adds (or subtracts) from the number density of the given isotope by 
	 * the amount.
	 *
	 * @param tope the isotope whose number density we want to change
	 * @param change if positive, the amount to add to the given isotopes 
	 * number density; if negative, the amount to subtract
	 * @param time the current time
	 */
	virtual void changeComp(Iso tope, NumDens change, int time);

	/**
	 * Changes the current composition of this Material to the given 
	 * composition.
	 *
	 * @param newComp the new isotopic composition of this Material
	 * @param time the current time
	 */
	virtual void changeComp(map<Iso, NumDens> newComp, int time);

	/**
	 * Returns the entire (current) isotopic vector for this Material.
	 *
	 * @return the current composition
	 */
	virtual const map<Iso, NumDens> getComp() const;

	/**
	 * Returns an isotopic vector corresponding to the given fraction of this 
	 * Material's current composition.
	 *
	 * @param frac the fraction of this Material's composition to return (we 
	 * hold the stoichiometry constant, so you can think of this as a weight 
	 * fraction OR an atom fraction)
	 * @return the fractional composition vector
	 */
	const virtual map<Iso, NumDens> getFracComp(double frac) const;

	/**
	 * Returns the current number density of the given isotope, or zero if 
	 * that isotope isn't present.
	 *
	 * @param tope the isotope whose number density will be returned
	 * @return the number density of the given isotope, or zero
	 */
	const virtual NumDens getComp(Iso tope) const;

	/**
	 * Returns the "total number density" for a given element. That is, sums and 
	 * returns the number density of each isotope of the given element.
	 *
	 * @param elt the element's atomic number
	 * @return the element's number density
	 */
	const virtual NumDens getEltComp(int elt) const;

	/**
	 * Returns the total mass of this Material.
	 *
	 * @return the total mass (in tons)
	 */
	const virtual double getTotMass() const;

	/**
	 * Returns the total number of atoms in this Material.
	 *
	 * @return the total number of atoms.
	 */
	const virtual double getTotNumDens() const;

	/**
	 * Returns the mass of the given element in this Material.
	 *
	 * @param elt the element
	 * @return the mass of the element (in tons)
	 */
	const virtual double getEltMass(int elt) const;

	/**
	 * Returns the mass of the given isotope in this Material.
	 *
	 * @param tope the isotope
	 * @return the mass of the element (in tons)
	 */
	const virtual double getIsoMass(Iso tope) const;

	/**
	 * Returns this Material's serial number.
	 *
	 * @return the serial number
	 */
	const virtual long getSN() const;

	/**
	 * Returns this Material's Commodity type.
	 *
	 * @return the Commodity type
	 */
	virtual Commodity getCommod() const;

	/**
	 * Returns this Material's ChemForm type.
	 *
	 * @return the ChemForm type
	 */
	virtual ChemForm getForm() const;

	/**
	 * Logs a transfer of this Material between two Facilities.
	 *
	 * @param time the current time
	 * @param fromFac the originating Facility's ID number
	 * @param toFac the destination Facility's ID number
	 */
	 virtual void logTrans(int time, int fromFac, int toFac);
		 
	 /**
	  * Writes this Material's history to the SQLite database.
	  */
	 virtual void writeHist();
		 
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
	 * Changes the commodity type of this Material. Throws an exception if
	 * the given type is the same as the current type.
	 * 
	 * @param newCommod the new Commodity type
	 */
	virtual void changeCommod(Commodity newCommod);

	/**
	 * Changes the chemical form of this Material. Throws an excpetion if 
	 * the given form is the same as the current form.
	 *
	 * @param newForm the new ChemForm type
	 */
	virtual void changeChemForm(ChemForm newForm);

	/**
	 * Returns a string representation of this Material object.
	 *
	 * @return the string representation
	 */
	virtual string toString();

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
	 * Returns the mass of the given element in the given composition vector.
	 *
	 * @param elt the atomic number of the element
	 * @param comp the composition vector
	 * @return the mass (in tons)
	 */
	static double getEltMass(int elt, const map<Iso, NumDens>& comp);

	/**
	 * Returns the mass of the given isotope in the given composition vector.
	 *
	 * @param tope the atomic number of the element
	 * @param comp the composition vector
	 * @return the mass (in tons)
	 */
	static double getIsoMass(Iso tope, const map<Iso, NumDens>& comp);

	/**
	 * Returns the total mass of the given composition vector.
	 * 
	 * @param comp the composition vector
	 * @return the mass (in tons)
	 */
	static double getTotMass(const map<Iso, NumDens>& comp);

	/**
	 * Returns the total number of atoms of the given composition vector.
	 * 
	 * @param comp the composition vector
	 * @return the number of atoms
	 */
	static double getTotNumDens(const map<Iso, NumDens>& comp);


	/**
	 * Like the other getFracComp, except it's a static function that works on a 
	 * composition vector you give it rather than on the composition of a 
	 * particular Material object.
	 *
	 * @param frac the fraction of this Material's composition to return (we 
	 * hold the stoichiometry constant, so you can think of this as a weight 
	 * fraction OR an atom fraction)
	 * @param comp the composition vector
	 * @return the fractional composition vector
	 */
	static map<Iso, NumDens> getFracComp(double frac, 
																						 const map<Iso, NumDens>& comp);

	/**
	 * Insertion stream operator for a Material.
	 */
	friend ostream& operator<<(ostream &os, const Material& m);

	/**
	 * Insertion stream operator for a pointer to a Material.
	 */
	friend ostream& operator<<(ostream &os, const Material* m);

	/**
	 * Returns a stringized version of the given Commodity enumeration.
	 *
	 * @param commod the Commodity to un-enumerate
	 * @return the string representation
	 */
	static string unEnumerateCommod(Commodity commod);

	/**
	 * Returns the Commodity enumeration of the given string.
	 *
	 * @param s the string representation of the Commodity
	 * @return the corresponding Commodity
	 */
	static Commodity enumerateCommod(string s);

	/**
	 * Returns true if this Material object contains appreciable (> eps) 
	 * actinides, false otherwise.
	 *
	 * @return true iff this Material contains appreciable actinides
	 */
	virtual bool containsActinides();

	/**
	 * Returns true if this Material object contains appreciable (> eps) 
	 * fission products (defined Zn through Lu), false otherwise.
	 *
	 * @return true iff this Material contains appreciable fission products
	 */
	virtual bool containsFissionProducts();

	/**
	 * Extracts a Material object of the given mass from this Material. The 
	 * stoichiometry of the new Material object is the same as the parent (i.e., 
	 * isotope ratios are held constant).
	 *
	 * @param mass the mass to extract
	 * @return the new Material object
	 */
	virtual Material* extractMass(double mass);

	/**
	 * Returns the given composition map with the number densities converted to 
	 * masses in tons. This is useful if you're trying to optimize on composition 
	 * and want to scale the values appropriately.
	 *
	 * @param compToConv the composition map you want to convert
	 * @return the converted map
	 */ 
	static map<Iso, double> convNumDensToMass(const map<Iso, NumDens>& compToConv);

	/**
	 * Reads the decay information found in the 'decayInfo.dat' file into the
	 * parent and daughters maps.  Uses these maps to create the decay matrix.
	 */
	static void loadDecayInfo();
	
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
	 * Returns the averaged fission cross sections for the given
	 * spectrum.
	 *
	 * @param the spectrum we want to average over
	 * @return the capture cross-section data
	 */
	static const map<Iso, double>& getAvgFisXSects(Spectrum s = thermal);

	/**
	 * Returns the averaged capture cross section for the given
	 * spectrum.
	 *
	 * @param the spectrum we want to average over
	 * @return the fission cross-section data
	 */
	static const map<Iso, double>& getAvgCapXSects(Spectrum s = thermal);

	/**
	 * Returns the averaged nu values for the given spectrum.
	 *
	 * @param the spectrum we want to average over
	 * @return the nu value data
	 */
	static const map<Iso, double>& getAvgNus(Spectrum s = thermal);

	/**
	 * This function handles assigning a neutronics-based weighting
	 * for a given lot of fuel.
	 *
	 * @param the recipe
	 * @param the spectrum we'll average over
	 * @return the weighting value
	 */
	static double computeNeutWeight(const map<Iso, NumDens> &comp, 
																 Spectrum s = thermal);

	/**
	 * Assigns a neutronics-based weighting for this Material object.
	 *
	 * @param the spectrum we'll average over
	 * @return the weighting value
	 */

	virtual double computeNeutWeight(Spectrum s = thermal) const;


};
#endif
