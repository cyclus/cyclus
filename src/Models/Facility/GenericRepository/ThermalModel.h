// ThermalModel.h
#if !defined(_THERMALMODEL_H)
#define _THERMALMODEL_H

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "Material.h"

/**  
 * type definition for Temperature in Kelvin
 */
typedef double Temp;

/**
 * type definition for Power in Watts
 */
typedef double Power;

/**
 * enumerator for the component models available to the repo
 */
enum ThermalModelType{LLNL_THERMAL, LUMP_THERMAL, SINDA_THERMAL, STUB_THERMAL, LAST_THERMAL};  

/** 
 * @brief Defines interface for thermal models to be used in the GenericRepository
 *
 * ThermalModels such as LumpedHeat, Sinda, LLNL, etc,
 * will share this virtual interface so that they can be interchanged within the  
 * GenericRepository.
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
  virtual Power getAvailCapacity() = 0;

  /**
   * transports the heat
   */
  virtual void transportHeat() =0;

  /**
   * get the component implementation type
   */
  const virtual ThermalModelType getThermalModelType()=0;

  /**
   * get the peak Temperature this object will experience during the simulation
   *
   * @param comp is the Component being queried
   *
   */
  virtual Temp getPeakTemp() = 0;

  /**
   * get the Temperature
   *
   * @param comp the component whose temperature we're getting
   * @return temperature_
   */
  virtual Temp getTemp()=0;

};


#endif
