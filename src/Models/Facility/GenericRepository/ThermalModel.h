// ThermalModel.h
#if !defined(_THERMALMODEL_H)
#define _THERMALMODEL_H

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "Material.h"
#include "Component.h"

class ThermalModel;

/**
 * type definition for Temperature in Kelvin
 */
typedef double Temp;

/**
 * type definition for Power in Watts
 */
typedef double Power;

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
 * enumerator for the component models available to the repo
 */
enum ThermalModelType{LUMP, SINDA, LLNL, LAST_THERMAL}; 

/** 
 * @brief Defines interface for subcomponents of the GenericRepository
 *
 * ThermalModels such as the Waste Form, Waste Package, Buffer, Near Field,
 * Far Field, and Envrionment will share a universal interface so that 
 * information passing concerning fluxes and other boundary conditions 
 * can be passed in and out of them.
 */
class ThermalModel {

public:
  
  /**
   * initializes the model parameters from an xmlNodePtr
   *
   * @param cur is the current xmlNodePtr
   */
  virtual void init(xmlNodePtr cur)=0; 

  /**
   * copies a component and its parameters from another
   *
   * @param src is the component being copied
   */
  virtual void copy(ThermalModel* src)=0; 

  /**
   * standard verbose printer includes current temp and concentrations
   */
  virtual void print()=0; 

  /**
   * Reports the peak thermal source, in Watts, that a component can contain
   *
   * @param comp the component whose capacity is being analyzed
   */
  virtual Power getAvailCapacity(Component* comp) = 0;

  /**
   * get the Name
   *
   * @return name_
   */
  const virtual std::string getName()=0;
 
  /**
   * get the component implementation name
   *
   * @return impl_name_
   */
  const virtual std::string getImpl()=0;

  /**
   * get the peak Temperature this object will experience during the simulation
   *
   * @param type indicates whether to return the inner or outer peak temp
   *
   * @return peak_temperature_
   */
  virtual Temp getPeakTemp(BoundaryType type, Component* comp) = 0;

  /**
   * get the Temperature
   *
   * @param comp the component whose temperature we're getting
   * @return temperature_
   */
  virtual Temp getTemp(Component* comp)=0;

};


#endif
