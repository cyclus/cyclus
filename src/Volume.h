// Volume.h
#if !defined(_VOLUME_H)
#define _VOLUME_H

#include <vector>
#include <map>

#include "InputXML.h"
#include "Material.h"
#include "Timer.h"

using namespace std;

/**
 * type definition for Temperature 
 */
typedef double Temp;

/**
 * type definition for Radius 
 */
typedef double Radius;

/**
 * type definition for Concentrations 
 */
typedef double Concentration;

/**
 * type definition for ConcMap 
 */
typedef map<Iso, Concentration> ConcMap;

/**
 * Enum for type of volume.
 */
enum Component {env, ff, ebs, buffer, wp, wf};

/** 
 * A struct to describe solids
 */
struct Solid{
  double density;
  double porosity;
  double mass;
  double vol;
  double conductivity;
};

/** 
 * A struct to describe fluids
 */
struct Fluid{
  double viscosity;
  double diffusivity;
  double mass;
  map<Iso, double> solubilities;
};

/** 
 * Class Volume describes a uniformly mixed volume cell
 * 
 * This class is intended to describe the subcomponents of the repository
 *
 * The mixed cell conserves mass with a mass balance every month and is linked 
 * to other volumes by mass transfer links.
 */
class Volume {

public:
  
  /**
   * Default constructor for the volume class. Creates an empty volume.
   */
  Volume(); 

  /**
   * primary constructor reads input from XML node
   *
   * @param cur input XML node
   */
  Volume(xmlNodePtr cur);

  /**
   * a constructor for making a volume object from a known recipe and size.
   *
   * @param volName
   * @param temp
   * @param inner
   * @param outer
   * @param concs a vector, per isotope 
   * @param matrix the solid, a struct
   * @param liquid the fluid, a struct
   * @param type indicates whether comp and scale are in mass or atom units
   */
  Volume(string volName, Temp temperature, Radius inner, Radius outer, 
      ConcMap concs, Solid* matrix, Fluid* liquid, Component type);

  
  /** 
   * Default destructor does nothing.
   */
  ~Volume() {};

  /**
   * standard verbose printer includes current temp and concentrations
   */
  void print(); 
    
  /**
   * get the ID
   *
   * @return ID
   */
  const int getSN(){return ID;};

  /**
   * get the Name
   *
   * @return name
   */
  const string getName(){return name;};

  /**
   * get the list of waste objects 
   *
   * @return wastes
   */
  const vector<Material*> getWastes(){return wastes;};

  /**
   * get the solid matrix
   *
   * @return matrix
   */
  const Solid* getMatrix(){return matrix;};

  /**
   * get the liquid
   *
   * @return liquid
   */
  const Fluid* getLiquid(){return liquid;};

  /**
   * get the Temperature
   *
   * @return temperature
   */
  const Temp getTemp(){return temperature;};

  /**
   * get the Inner Radius
   *
   * @return inner
   */
  const Radius getInnerRadius(){return inner_radius;};

  /**
   * get the Outer Radius
   *
   * @return outer
   */
  const Radius getOuterRadius(){return outer_radius;};

  /**
   * get the concentration of an isotope 
   *
   * @return concentration
   */
  const Concentration getConcentration(Iso tope);

  /**
   * Absorbs the contents of the given Material into this Volume.
   * 
   * @param matToAdd the Volume to be absorbed
   */
  virtual void absorb(Material* matToAdd);

  /**
   * Extracts the contents of the given Material from this Volume. Use this 
   * function for decrementing a Volume's mass balance after transferring 
   * through a link. 
   *
   * @param matToRem the Material whose composition we want to decrement 
   * against this Volume
   */
  virtual void extract(Material* matToRem);

protected:
  /** 
   * The serial number for this Volume.
   */
  int ID;

  /**
   * Stores the next available volume ID
   */
  static int nextID;

  /**
   * The composition history of this Volume, in the form of a map whose
   * keys are integers representing the time at which this Volume had a 
   * particular composition and whose values are the corresponding 
   * compositions. A composition is a map of isotopes and their 
   * corresponding number of atoms.
   */
  CompHistory volCompHist;

  /**
   * The mass history of this Volume, in the form of a map whose
   * keys are integers representing the time at which this Volume had a 
   * particular composition and whose values are the corresponding mass 
   * compositions. A composition is a map of isotopes and the corresponding
   * masses.
   */
  MassHistory volMassHist;

private:
  /**
   * This step mixes the material and performs a mass balance.
   */
  void mixCell();

  /**
   * The contained contaminants, a list of material objects..
   */
  vector<Material*> wastes;

  /**
   * The name of this volume, a string
   */
  string name;

  /**
   * The inner radius of the (cylindrical) volume
   */
  Radius inner_radius;

  /**
   * The outer radius of the (cyllindrical) volume
   */
  Radius outer_radius;

  /**
   * The solid matrix object which is the main constituent of this volume.
   */
  Solid* matrix;

  /**
   * The fluid liquid object which is the second constituent of this volume.
   */
  Fluid* liquid;

  /**
   * The type of component that this volume represents 
   */
  Component vol_type;

  /**
   * The temperature taken to be the homogeneous temperature of the whole 
   * volume.
   */
  Temp temperature;

  /**
   * The concentrations of contaminant isotopes in the volume
   */
  ConcMap concentrations;
};


#endif
