// Component.h
#if !defined(_COMPONENT_H)
#define _COMPONENT_H

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
 * Enum for type of component.
 */
enum ComponentType {ENV, FF, EBS, BUFFER, WP, WF};

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
 * Class Component describes a uniformly mixed component cell
 * 
 * This class is intended to describe the subcomponents of the repository
 *
 * The mixed cell conserves mass with a mass balance every month and is linked 
 * to other components by mass transfer links.
 */
class Component {

public:
  
  /**
   * Default constructor for the component class. Creates an empty component.
   */
  Component(); 

  /**
   * primary constructor reads input from XML node
   *
   * @param cur input XML node
   */
  Component(xmlNodePtr cur);

  /**
   * a constructor for making a component object from a known recipe and size.
   *
   * @param volName
   * @param temperature
   * @param inner
   * @param outer
   * @param concs a vector, per isotope 
   * @param matrix the solid, a struct
   * @param liquid the fluid, a struct
   * @param type indicates whether comp and scale are in mass or atom units
   */
  Component(string volName, Temp temperature, Radius inner, Radius outer, 
      ConcMap concs, Solid* matrix, Fluid* liquid, ComponentType type);

  
  /** 
   * Default destructor does nothing.
   */
  ~Component() {};

  /**
   * standard verbose printer includes current temp and concentrations
   */
  void print(); 
    
  /**
   * get the ID
   *
   * @return ID_
   */
  const int getSN(){return ID_;};

  /**
   * get the Name
   *
   * @return name_
   */
  const string getName(){return name_;};

  /**
   * get the list of waste objects 
   *
   * @return wastes
   */
  const vector<Material*> getWastes(){return wastes_;};

  /**
   * get the solid matrix
   *
   * @return matrix_
   */
  const Solid* getMatrix(){return matrix_;};

  /**
   * get the liquid
   *
   * @return liquid_
   */
  const Fluid* getLiquid(){return liquid_;};

  /**
   * get the Temperature
   *
   * @return temperature_
   */
  const Temp getTemp(){return temperature_;};

  /**
   * get the Inner Radius
   *
   * @return inner_radius_
   */
  const Radius getInnerRadius(){return inner_radius_;};

  /**
   * get the Outer Radius
   *
   * @return outer_radius_
   */
  const Radius getOuterRadius(){return outer_radius_;};

  /**
   * get the concentration of an isotope 
   *
   * @return concentration_
   */
  const Concentration getConcentration(Iso tope);

  /**
   * Absorbs the contents of the given Material into this Component.
   * 
   * @param matToAdd the Component to be absorbed
   */
  virtual void absorb(Material* matToAdd);

  /**
   * Extracts the contents of the given Material from this Component. Use this 
   * function for decrementing a Component's mass balance after transferring 
   * through a link. 
   *
   * @param matToRem the Material whose composition we want to decrement 
   * against this Component
   */
  virtual void extract(Material* matToRem);

protected:
  /** 
   * The serial number for this Component.
   */
  int ID_;

  /**
   * Stores the next available component ID
   */
  static int nextID_;

  /**
   * The composition history of this Component, in the form of a map whose
   * keys are integers representing the time at which this Component had a 
   * particular composition and whose values are the corresponding 
   * compositions. A composition is a map of isotopes and their 
   * corresponding number of atoms.
   */
  CompHistory vol_comp_hist_;

  /**
   * The mass history of this Component, in the form of a map whose
   * keys are integers representing the time at which this Component had a 
   * particular composition and whose values are the corresponding mass 
   * compositions. A composition is a map of isotopes and the corresponding
   * masses.
   */
  MassHistory vol_mass_hist_;

private:
  /**
   * This step mixes the material and performs a mass balance.
   */
  void mixCell();

  /**
   * The contained contaminants, a list of material objects..
   */
  vector<Material*> wastes_;

  /**
   * The name of this component, a string
   */
  string name_;

  /**
   * The inner radius of the (cylindrical) component
   */
  Radius inner_radius_;

  /**
   * The outer radius of the (cylindrical) component
   */
  Radius outer_radius_;

  /**
   * The solid matrix object which is the main constituent of this component.
   */
  Solid* matrix_;

  /**
   * The fluid liquid object which is the second constituent of this component.
   */
  Fluid* liquid_;

  /**
   * The type of component that this component represents 
   */
  ComponentType type_;

  /**
   * The temperature taken to be the homogeneous temperature of the whole 
   * component.
   */
  Temp temperature_;

  /**
   * The concentrations of contaminant isotopes in the component
   */
  ConcMap concentrations_;
};


#endif
