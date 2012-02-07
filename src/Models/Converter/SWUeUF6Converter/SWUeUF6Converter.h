// SWUeUF6Converter.h
#if !defined(_SWUEUF6CONVERTER_H)
#define _SWUEUF6CONVERTER_H
#include <iostream>
#include "Logger.h"
#include <queue>

#include "ConverterModel.h"

/**
   @brief The SWUeUF6Converter class inherits from the ConverterModel class 
   and is dynamically loaded by the Model class when requested.
   
   This converter model changes SWUs into amounts of material (and back?) 
   
   @section intro Introduction
   The SWUeUF6Converter is a converter type in Cyclus capable of translating 
   a message concerning some Separative Work Unit amount to enriched uranium 
   hexafluoride. It relies on a default value for natural uranium enrichment 
   and requests tails information from the SWU generating facility in order 
   to determine the parameters for the conversion.

   @section modelParams Model Parameters
   SWUeUF6Converter behavior is comprehensively defined by the following 
   parameters:
   - string in_commod : The commodity that the converter will translate 
   into the out_commod.
   - string out_commod : The commodity that the converter will translate 
   the in_commod into.

   @section optionalParams Optional Parameters
   SWUeUF6Converter behavior may also be specified with the following 
   optional parameters which have default values listed here.
   - double WF_U35 : The tails fraction to be assumed in the calculations.

   @section behavior Detailed Behavior
   The SWUeUF6Converter simply responds to requests for conversion. It 
   receives an amount and a commodity and applies the definition of the 
   separative work unit, the natural enrichment of uranium, a desired final 
   enrichment, and a provided tails fraction in order to convert between the 
   two commodities. It then sends a response to the request for conversion 
   including the new amount.
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
     * The convert function specific to the SWUeUF6Converter
     * Converts between amounts of two commodities
     *
     * @param convMsg is a message concerning one convertible commodity to convert
     * @param refMsg is a message concerning the commodity to convert it to
     * 
     * @return the converted message
     */

    virtual msg_ptr convert(msg_ptr convMsg, msg_ptr refMsg); 


    /* ------------------- */ 

    /* --------------------
     * _THIS_ CONVERTERMODEL class has these members
     * --------------------
     */

  protected:
    /**
     * The SWUeUF6Converter has one input commodity
     */
    std::string in_commod_;

    /**
     * The SWUeUF6Converter has one output commodity
     */
    std::string out_commod_;

    /* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

#endif
