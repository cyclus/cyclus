// SWUeUF6Converter.h
#if !defined(_SWUEUF6CONVERTER_H)
#define _SWUEUF6CONVERTER_H
#include <iostream>
#include <queue>

#include "ConverterModel.h"

/**
 * The SWUeUF6Converter class inherits from the ConverterModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This converter model changes SWUs into amounts of material (and back?) 
 *
 */
class SWUeUF6Converter : public ConverterModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /** 
   * Default constructor for the SWUeUF6Converter class.
   */
  SWUeUF6Converter() {};

  /**
   * Destructor for the SWUeUF6Converter class. 
   */
  ~SWUeUF6Converter() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(SWUeUF6Converter* src);

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /**
   * Print information about this model
   */
  virtual void print();

/* ------------------- */ 

public:

/* -------------------- */

/* --------------------
 * all CONVERTERMODEL classes have these members
 * --------------------
 */

public:
    /**
     * The handleTick function specific to the SWUeUF6Converter.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /**
     * The handleTick function specific to the SWUeUF6Converter.
     *
     * @param time the time of the tock
     */
    virtual void handleTock(int time);

    /**
     * The convert function specific to the SWUeUF6Converter
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
    /**
     * The SWUeUF6Converter has one input commodity
     */
    Commodity* in_commod;

    /**
     * The SWUeUF6Converter has one output commodity
     */
    Commodity* out_commod;

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new SWUeUF6Converter();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* ------------------- */ 

#endif
