// ConverterModel.h
#if !defined(_CONVERTERMODEL_H)
#define _CONVERTERMODEL_H
#include <string>
#include <list>

#include "Model.h"
#include "Message.h"

using namespace std;

/// forward declaration to resolve recursion
class Commodity;
// forward declare Material class to avoid full inclusion and dependency
class Material;

//-----------------------------------------------------------------------------
/**
 * The ConverterModel class is the abstract class/interconve used by all
 * converter models
 * 
*/
//-----------------------------------------------------------------------------
class ConverterModel : public Model {

public:
  /// Default constructor for ConverterModel Class
  ConverterModel() {
    setModelType("Converter");
  };

  /// every model should be destructable
  virtual ~ConverterModel() {};
  
  // every model needs a method to initialize from XML
  virtual void init(xmlNodePtr cur);

  // every model needs a method to copy one object to another
  virtual void copy(ConverterModel* src);

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src)=0;

  // every model should be able to print a verbose description
  virtual void print()              { Model::print(); };

protected:
  /// each instance of a converter needs a name
  string conv_name;

  /// each converter needs a list of equivalent commodities it converts
  list<Commodity*> commodities; 

public:

  /**
   * Sets the converter's name 
   *
   * @param convName is the new name of the converter
   */
  void setConvName(string convName) { conv_name = convName; };

  /// Returns the converter's name
  string getConvName() { return conv_name; };

  /**
   * Converts between amounts of two commodities
   *
   * @param convMsg is a message concerning one convertible commodity to convert
   * @param refMsg is a message concerning the commodity to convert it to
   * 
   * @return the converted message
   */
  virtual Message* convert(Message* convMsg, Message* refMsg); 

    /**
   * Each converter may be prompted to do its beginning-of-time-step
   * stuff at the tick of the timer.
   *
   * @param time is the time to perform the tick
   */
  virtual void handleTick(int time);

  /**
   * Each converter may be prompted to its end-of-time-step
   * stuff on the tock of the timer.
   * 
   * @param time is the time to perform the tock
   */
  virtual void handleTock(int time);

/* ------------------- */ 
  
};

#endif



