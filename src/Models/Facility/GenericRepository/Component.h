/** \file Component.h
 * \brief Declares the Component class
 */
#if !defined(_COMPONENT_H)
#define _COMPONENT_H

#include <vector>
#include <map>
#include <string>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "Material.h"
#include "ThermalModel.h"
#include "NuclideModel.h"

/*!
A map for storing the composition history of a material.

@TODO IsoVector should be used instead of this
*/
typedef std::map<int, std::map<int, double> > CompHistory;

/*!
A map for storing the mass history of a material.

@TODO IsoVector should be used instead of this
*/
typedef std::map<int, std::map<int, double> > MassHistory;

/// type definition for Radius in meters
typedef double Radius;

/// type definition for Toxicity in units of Sv 
typedef double Tox;

/// type definition for Concentrations in kg/m^3
typedef double Concentration;

/// type definition for ConcMap 
typedef std::map<int, Concentration> ConcMap;

/// type definition for Temperature in Kelvin map for storing the composition history of a material.
typedef double Temp;

/// type definition for Power in Watts
typedef double Power;

/// Enum for type of engineered barrier component.
enum ComponentType {BUFFER, ENV, FF, NF, WF, WP, LAST_EBS};

/// Enum for type of boundary.
enum BoundaryType {INNER, OUTER};

/// This struct stores the cylindrical Component geometry 
typedef struct geometry_t{
  Radius inner_radius_; /**<Radius of the inner surface. 0 for solid objects. */
  Radius outer_radius_; /**<Radius of the outer surface. NULL for infinite objects. */
  double x_; /**<The x coordinate of the centroid */
  double y_; /**<The y coordinate of the centroid */
  double z_; /**<The z coordinate of the centroid */
}geometry_t;

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
   * Default destructor does nothing.
   */
  ~Component() {};

  /**
   * initializes the model parameters from an xmlNodePtr
   *
   * @param cur is the current xmlNodePtr
   */
  void init(xmlNodePtr cur); 

  /**
   * copies a component and its parameters from another
   *
   * @param src is the component being copied
   */
  void copy(Component* src); 

  /**
   * standard verbose printer includes current temp and concentrations
   */
  void print(); 

  /**
   * Absorbs the contents of the given Material into this Component.
   * 
   * @param matToAdd the Component to be absorbed
   */
  void absorb(Material* matToAdd){nuclide_model_->absorb(matToAdd);};

  /**
   * Extracts the contents of the given Material from this Component. Use this 
   * function for decrementing a Component's mass balance after transferring 
   * through a link. 
   *
   * @param matToRem the Material whose composition we want to decrement 
   * against this Component
   */
  void extract(Material* matToRem){nuclide_model_->extract(matToRem);};

  /**
   * Transports heat from the inner boundary to the outer boundary in this 
   * component
   */
  void transportHeat(){thermal_model_->transportHeat();};

  /**
   * Transports nuclides from the inner boundary to the outer boundary in this 
   * component
   */
  void transportNuclides(){nuclide_model_->transportNuclides();};

  /** 
   * Loads this component with another component.
   *
   * @param type the ComponentType of this component
   * @param to_load the Component to load into this component
   */
  Component* load(ComponentType type, Component* to_load);

  /// registers this component with the BookKeeper.
  void registerComponent() ;

  /**
   * Reports true if the component is full of whatever goes inside it.
   *
   * @return TRUE if the component is full and FALSE if there is no more room 
   */
  bool isFull() ;

  /**
   * Returns the ComponentType of this component
   *
   * @return type_ the ComponentType of this component
   */
  ComponentType getComponentType(){return type_;};

  /**
   * Enumerates a string if it is one of the named ComponentTypes
   * 
   * @param type the name of the ComponentType (i.e. FF)
   * @return the ComponentType enum associated with this string by the 
   * component_type_names_ list 
   */
  ComponentType getComponentType(std::string type);
  
  /**
   * Enumerates a string if it is one of the named ThermalModelTypes
   *
   * @param type the name of the ThermalModelType (i.e. StubThermal)
   */
  ThermalModelType getThermalModelType(std::string type);

  /** 
   * Enumerates a string if it is one of the named NuclideModelTypes
   *
   * @param type the name of the NuclideModelType (i.e. StubNuclide)
   */
  NuclideModelType getNuclideModelType(std::string type);

  /** 
   * Returns a new thermal model of the string type xml node pointer
   *
   * @param cur the xml node pointer defining the thermal model
   * @return thermal_model_ a pointer to the ThermalModel that was created
   */
  ThermalModel* getThermalModel(xmlNodePtr cur);

  /** 
   * Returns a new nuclide model of the string type and xml node pointer
   *
   * @param cur the xml node pointer defining the nuclide model
   * @return nuclide_model_ a pointer to the NuclideModel that was created
   */
  NuclideModel* getNuclideModel(xmlNodePtr cur);

  /** 
   * Returns a new thermal model that is a copy of the src model
   *
   * @param src a pointer to the thermal model to copy
   * @return a pointer to the ThermalModel that was created
   */
  ThermalModel* copyThermalModel(ThermalModel* src);

  /** 
   * Returns atd::s new nuclide model that is a copy of the src model
   */
  NuclideModel* copyNuclideModel(NuclideModel* src);

  /**
   * get the ID
   *
   * @return ID_
   */
  const int ID(){return ID_;};

  /**
   * get the Name
   *
   * @return name_
   */
  const std::string name(){return name_;};
 
  /**
   * get the list of daughter components 
   *
   * @return components
   */
  const std::vector<Component*> getDaughters(){return daughter_components_;};

  /**
   * get the parent component 
   *
   * @return component
   */
  Component* getParent(){return parent_component_;};

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
   * @return inner_radius__ or outer_radius__
   */
  const Radius getRadius(BoundaryType type){
    return (type==INNER)?geom_.inner_radius_:geom_.outer_radius_; };

  /// get the x component of the centroid position vector of the object
  const double getX(){
    return geom_.x_;};

  /// get the y component of the centroid position vector of the object
  const double getY(){
    return geom_.y_;};

  /// get the z component of the centroid position vector of the object
  const double getZ(){
    return geom_.z_;};

  /**
   * set the parent component 
   *
   * @param parent is the component that should be set as the parent
   */
  void setParent(Component* parent){parent_component_ = parent;};

  /**
   * set the placement of the object
   *
   * @param x is the x dimension of the centroid position vector
   * @param y is the y dimension of the centroid position vector
   * @param z is the z dimension of the centroid position vector
   */
  void* setPlacement(double x, double y, double z){
    geom_.x_=x, geom_.y_=y, geom_.z_=z; };

protected:
  /** 
   * The serial number for this Component.
   */
  int ID_;

  /**
   * Stores the next available component ID
   */
  static int nextID_;

  /// ThermalModleType names list
  static std::string thermal_type_names_[LAST_THERMAL];

  /// NuclideModelType names list
  static std::string nuclide_type_names_[LAST_NUCLIDE];

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
   * The type of thermal model implemented by this component
   */
  ThermalModel* thermal_model_;

  /**
   * The type of nuclide model implemented by this component
   */
  NuclideModel* nuclide_model_;

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
   * The geometry of the cylindrical component
   */
  geometry_t geom_;

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
