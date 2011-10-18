// StubNuclide.h
#if !defined(_STUBNUCLIDE_H)
#define _STUBNUCLIDE_H

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "NuclideModel.h"


/** 
 * @brief StubNuclide is a skeleton nuclide model model that does nothing.

 * This disposal system nuclide model will do nothing. This NuclideModel is 
 * intended as a skeleton to guide the implementation of new NuclideModels. 
 *
 * The StubNuclide model can be used to represent nuclide models of the 
 * disposal system such as the Waste Form, Waste Package, Buffer, Near Field,
 * Far Field, and Envrionment.
 */
class StubNuclide : public NuclideModel {
public:
  
  /**
   * Default constructor for the nuclide model class. Creates an empty nuclide model.
   */
  StubNuclide(){ }; 

  /**
   * primary constructor reads input from XML node
   *
   * @param cur input XML node
   */
  StubNuclide(xmlNodePtr cur){};

  /** 
   * Default destructor does nothing.
   */
  ~StubNuclide() {};

  /**
   * initializes the model parameters from an xmlNodePtr
   *
   * @param cur is the current xmlNodePtr
   */
  virtual void init(xmlNodePtr cur); 

  /**
   * copies a nuclide model and its parameters from another
   *
   * @param src is the nuclide model being copied
   */
  virtual void copy(NuclideModel* src); 

  /**
   * standard verbose printer includes current temp and concentrations
   */
  virtual void print(); 

  /**
   * Absorbs the contents of the given Material into this StubNuclide.
   * 
   * @param matToAdd the StubNuclide to be absorbed
   */
  virtual void absorb(Material* matToAdd) ;

  /**
   * Extracts the contents of the given Material from this StubNuclide. Use this 
   * function for decrementing a StubNuclide's mass balance after transferring 
   * through a link. 
   *
   * @param matToRem the Material whose composition we want to decrement 
   * against this StubNuclide
   */
  virtual void extract(Material* matToRem) ;

  /**
   * Transports nuclides from the inner to the outer boundary 
   */
  virtual void transportNuclides();

  /**
   * Transports nuclides from the inner to the outer boundary 
   */
  virtual NuclideModelType getNuclideModelType(){return STUB_NUCLIDE;};

  /**
   * returns the peak Toxicity this object will experience during the 
   * simulation.
   *
   * @return peak_toxicity
   */
  const virtual Tox getPeakTox(){};

  /**
   * returns the concentration map for this component at the time specified
   *
   * @param time the time to query the concentration map
   */
  virtual ConcMap getConcMap(int time){};
};
#endif
