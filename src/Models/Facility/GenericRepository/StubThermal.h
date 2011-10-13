// StubThermal.h
#if !defined(_STUBTHERMAL_H)
#define _STUBTHERMAL_H

#include <iostream>
#include <string>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "ThermalModel.h"


/** 
 * @brief StubThermal is a skeleton component model that does nothing.

 * This disposal system component will do nothing. This Component is 
 * intended as a skeleton to guide the implementation of new Components. 
 *
 * The StubThermal model can be used to represent components of the 
 * disposal system such as the Waste Form, Waste Package, Buffer, Near Field,
 * Far Field, and Envrionment.
 */
class StubThermal : public ThermalModel {
public:
  
  /**
   * Default constructor for the component class. Creates an empty component.
   */
  StubThermal(){this->impl_name_="StubThermal";}; 

  /**
   * primary constructor reads input from XML node
   *
   * @param cur input XML node
   */
  StubThermal(xmlNodePtr cur){this->impl_name_="StubThermal";};

  /** 
   * Default destructor does nothing.
   */
  ~StubThermal() {};

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
  virtual void copy(StubThermal* src); 

  /**
   * standard verbose printer includes current temp and concentrations
   */
  virtual void print(); 

  /**
   * Get the available capacity within the component
   *
   * @param comp the component whose capacity is being queried
   */
  virtual Power getAvailCapacity(Component* comp);

  /**
   * Returns the name of this 
   *
   * @return name_ the name of the ThermalModel 
   */
  const virtual std::string getName(){return name_;};

  /**
   * return the implementation name
   *
   * @return impl_name_ the name of the ThermalModel implementation 
   */
  const std::string getImpl(){return impl_name_;}; 
  
  /**
   * gets the peak temperature that this component will experience on the 
   * boundary
   *
   * @param type the type of boundary (i.e. INNER or OUTER)
   * @param comp the component whose boundary is being queried
   */
  virtual Temp getPeakTemp(BoundaryType type, Component* comp);

  /**
   * gets the temperature average in the component
   *
   * @param comp the component whose temperature is being queried
   */
  virtual Temp getTemp(Component* comp);

protected:
  /**
   * The name of the ThermalModel implementation
   */
  std::string impl_name_;

  /**
   * The name of the ThermalModel 
   */
  std::string name_;
};
#endif
