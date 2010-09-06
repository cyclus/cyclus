// StubConverter.h
#if !defined(_STUBCONVERTER_H)
#define _STUBCONVERTER_H
#include <iostream>
#include <queue>

#include "ConverterModel.h"

/**
 * The StubConverter class inherits from the ConverterModel class and is dynamically
 * loaded by the Model class when requested.
 *
 * This region will do nothing. This RegionModel is intended as a skeleton to guide
 * the implementation of new RegionModel models. 
 *
 */
class StubConverter : public ConverterModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /** 
   * Default constructor for the StubConverter class.
   */
  StubConverter() {};

  /**
   * Destructor for the StubConverter class. 
   */
  ~StubConverter() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(StubConverter* src);

  /**
   * Print information about this model
   */
  virtual void print();

/* ------------------- */ 

/* --------------------
 * all CONVERTERMODEL classes have these members
 * --------------------
 */

public:
    /**
     * The handleTick function specific to the StubConverter.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /**
     * The handleTick function specific to the StubConverter.
     *
     * @param time the time of the tock
     */
    virtual void handleTock(int time);

    /**
     * The convert function specific to the StubConverter
     * Converts between amounts of two commodities
     *
     * @param convMsg is a message concerning one convertible commodity to convert
     * @param refMsg is a message concerning the commodity to convert it to
     * 
     * @return the converted message
     */
 
    virtual Message* convert(Message* convMsg, Message* refMsg); 


/* ------------------- */ 

/* --------------------
 * _THIS_ CONVERTERMODEL class has these members
 * --------------------
 */

protected:

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new StubConverter();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* ------------------- */ 

#endif
