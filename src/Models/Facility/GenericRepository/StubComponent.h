// StubComponent.h
#if !defined(_STUBCOMPONENT_H)
#define _STUBCOMPONENT_H

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "Component.h"


/** 
 * @brief StubComponent is a skeleton component model that does nothing.

 * This disposal system component will do nothing. This Component is 
 * intended as a skeleton to guide the implementation of new Components. 
 *
 * The StubComponent model can be used to represent components of the 
 * disposal system such as the Waste Form, Waste Package, Buffer, Near Field,
 * Far Field, and Envrionment.
 */
class StubComponent : public Component {
public:
  
  /**
   * Default constructor for the component class. Creates an empty component.
   */
  StubComponent(){this->impl_name_="StubComponent";}; 

  /**
   * primary constructor reads input from XML node
   *
   * @param cur input XML node
   */
  StubComponent(xmlNodePtr cur){this->impl_name_="StubComponent";};

  /**
   * a constructor for making a component object from c++ objects.
   *
   * @param name is the name of the component
   * @param temperature is the initial temperature
   * @param temperature_lim is the temperature limit
   * @param toxicity_lim is the toxicity limit
   * @param inner is the inner radius
   * @param outer is the outer radius
   * @param type indicates wf, wp, buffer, nf, ff, or env component 
   */
  StubComponent(std::string name, Temp temperature, Temp temperature_lim, Tox toxicity_lim,
      Radius inner, Radius outer, ComponentType type);

  /** 
   * Default destructor does nothing.
   */
  ~StubComponent() {};

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
  virtual void copy(StubComponent* src); 

  /**
   * standard verbose printer includes current temp and concentrations
   */
  virtual void print(); 

  /**
   * return the implementation name
   */
  const std::string getImpl(){return impl_name_;}; 

  /**
   * Absorbs the contents of the given Material into this StubComponent.
   * 
   * @param matToAdd the StubComponent to be absorbed
   */
  virtual void absorb(Material* matToAdd) ;

  /**
   * Extracts the contents of the given Material from this StubComponent. Use this 
   * function for decrementing a StubComponent's mass balance after transferring 
   * through a link. 
   *
   * @param matToRem the Material whose composition we want to decrement 
   * against this StubComponent
   */
  virtual void extract(Material* matToRem) ;

  /**
   * Loads the subcomponent within this component
   *
   * @param type the component type this component represents
   * @param to_load the component type being loaded
   */
  virtual Component* load(ComponentType type, Component* to_load) ;

  /** 
   * Reports true if this component may be loaded with more of whatever goes 
   * inside it and reports false if that is not the case.
   */
  virtual bool isFull();
};
#endif
