// Component.h
#if !defined(_COMPONENT_H)
#define _COMPONENT_H

#include <vector>
#include <map>
#include <string>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "Material.h"

class Component;

/**
 * type definition for Toxicity in units of Sv 
 */
typedef double Tox;

/**
 * type definition for Temperature in Kelvin
 */
typedef double Temp;

/**
 * type definition for Radius in meters
 */
typedef double Radius;

/**
 * type definition for Concentrations in kg/m^3
 */
typedef double Concentration;

/**
 * type definition for ConcMap 
 */
typedef std::map<Iso, Concentration> ConcMap;

/**
 * Enum for type of component.
 */
enum ComponentType {ENV, FF, NF, BUFFER, WP, WF, LAST_TYPE};

/**
 * enumerator for the component models available to the repo
 */
enum RepoComponent{STUB, LAST_COMPONENT}; 

/**
 * Enum for type of boundary.
 */
enum BoundaryType {INNER, OUTER};

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
  std::map<Iso, double> solubilities;
};

/** 
 * @brief Defines interface for subcomponents of the GenericRepository
 *
 * Components such as the Waste Form, Waste Package, Buffer, Near Field,
 * Far Field, and Envrionment will share a universal interface so that 
 * information passing concerning fluxes and other boundary conditions 
 * can be passed in and out of them.
 */
class Component {

public:
  
  /**
   * Default constructor for the component class. Creates an empty component.
   */
  Component(); 
  
  /**
   * a constructor for making a component object from a known recipe and size.
   *
   * @param name is the name of the component
   * @param temperature is the initial temperature
   * @param temperature_lim is the temperature limit
   * @param toxicity_lim is the toxicity limit
   * @param inner is the inner radius
   * @param outer is the outer radius
   * @param type indicates wf, wp, buffer, nf, ff, or env component 
   */
  Component(std::string name, Temp temperature, Temp temperature_lim, Tox toxicity_lim,
      Radius inner, Radius outer, ComponentType type);

  /** 
   * Default destructor does nothing.
   */
  ~Component() {};

  /**
   * initializes the model parameters from an xmlNodePtr
   *
   * @param cur is the current xmlNodePtr
   */
  virtual void init(xmlNodePtr cur); 

  /**
   * copies a component and its parameters from another
   *
   * @param src is the component being copied
   */
  virtual void copy(Component* src); 

  /**
   * standard verbose printer includes current temp and concentrations
   */
  virtual void print(); 

  /**
   * Absorbs the contents of the given Material into this Component.
   * 
   * @param matToAdd the Component to be absorbed
   */
  virtual void absorb(Material* matToAdd) = 0;

  /**
   * Extracts the contents of the given Material from this Component. Use this 
   * function for decrementing a Component's mass balance after transferring 
   * through a link. 
   *
   * @param matToRem the Material whose composition we want to decrement 
   * against this Component
   */
  virtual void extract(Material* matToRem) ;

  /** 
   * Loads this component with another component.
   *
   * @param type the ComponentType of this component
   * @param to_load the Component to load into this component
   */
  virtual Component* load(ComponentType type, Component* to_load);

  /** 
   * Reports true if this component may be loaded with more of whatever goes 
   * inside it and reports false if that is not the case.
   */
  virtual bool isFull();
    
  /** 
   * Enumerates a string if it is one of the named ComponentTypes
   */
  ComponentType getComponentType(std::string);

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
  const std::string getName(){return name_;};
 
  /**
   * get the component implementation name
   *
   * @return impl_name_
   */
  const virtual std::string getImpl()=0;
 
  /**
   * get the list of waste objects 
   *
   * @return wastes
   */
  const std::vector<Material*> getWastes(){return wastes_;};

  /**
   * get the maximum Temperature this object allows at its boundaries 
   *
   * @return temperature_lim_
   */
  const Temp getTempLim(){return temperature_lim_;};

  /**
   * get the maximum Toxicity this object allows at its boundaries 
   *
   * @return toxicity_lim_
   */
  const Tox getToxLim(){return toxicity_lim_;};

  /**
   * get the peak Temperature this object will experience during the simulation
   *
   * @param type indicates whether to return the inner or outer peak temp
   *
   * @return peak_temperature_
   */
  const Temp getPeakTemp(BoundaryType type){
    return (type==INNER)?peak_inner_temperature_:peak_outer_temperature_; };

  /**
   * get the peak Toxicity this object will experience during the simulation
   *
   * @return peak_toxicity_
   */
  const Tox getPeakTox(){return peak_toxicity_;};

  /**
   * get the Temperature
   *
   * @return temperature_
   */
  const Temp getTemp(){return temperature_;};

  /**
   * get a Radius of the object
   *
   * @param type indicates whether to return the inner or outer radius
   * @return inner_radius_
   */
  const Radius getRadius(BoundaryType type){
    return (type==INNER)?inner_radius_:outer_radius_; };

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
  CompHistory comp_hist_;

  /**
   * The mass history of this Component, in the form of a map whose
   * keys are integers representing the time at which this Component had a 
   * particular composition and whose values are the corresponding mass 
   * compositions. A composition is a map of isotopes and the corresponding
   * masses.
   */
  MassHistory mass_hist_;

  /**
   * The type of model implemented by this component (Stub, MixedCell, etc.) 
   */
  std::string impl_name_;

  /**
   * The immediate parent component of this component.
   */
  Component* parent_component_;

  /**
   * The immediate daughter components of this component.
   */
  std::vector<Component*> daughter_components_;

  /**
   * The contained contaminants, a list of material objects..
   */
  std::vector<Material*> wastes_;

  /**
   * The name of this component, a string
   */
  std::string name_;

  /**
   * The inner radius of the (cylindrical) component
   */
  Radius inner_radius_;

  /**
   * The outer radius of the (cylindrical) component
   */
  Radius outer_radius_;

  /**
   * The type of component that this component represents (ff, buffer, wp, etc.) 
   */
  ComponentType type_;

  /**
   * The temperature limit of this component 
   */
  Temp temperature_lim_;

  /**
   * The toxicitylimit of this component 
   */
  Tox toxicity_lim_;

  /**
   * The peak temperature achieved at the outer boundary 
   */
  Temp peak_outer_temperature_;

  /**
   * The peak temperature achieved at the inner boundary 
   */
  Temp peak_inner_temperature_;

  /**
   * The peak toxicity achieved  
   */
  Tox peak_toxicity_;

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
