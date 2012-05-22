// ConverterModel.h
#if !defined(_CONVERTERMODEL_H)
#define _CONVERTERMODEL_H

#include <string>
#include <list>

#include "Message.h"
#include "Model.h"

// forward declare Material class to avoid full inclusion and dependency
class Material;

/**
   The ConverterModel class is the abstract class/interconve used by all 
   converter models. 
    
   @section introduction Introduction 
   The ConverterModel type is used to translate between commodity types 
   in Cyclus. A ConverterModel receives a message concerning an amount 
   of one commodity, A, and translates that amount into another 
   commodity B. It then returns the message with the amount and 
   commodity type altered to reflect this translation. The algorithm to 
   determine the method of translation between commodities is the 
   primary differentiator between different ConverterModel 
   implementations.  
   Like all model implementations, there are a number of implementations 
   that are distributed as part of the core Cyclus application as well 
   as implementations contributed by third-party developers. The links 
   below describe additional parameters necessary for the complete 
   definition of a facility of that implementation. 
    
   @section availableCoreImpl Available Core Implementations 
   (None) 
    
   @section anticipatedCoreImpl Anticipated Core Implementations 
   (Developers are encouraged to add to this list and create pages 
   that describe the detailed behavior of these models.) 
   - SWUeUF6Converter: A facility that can convert between SWU 
   amounts and enriched uranium hexafluoride. 
    
   @section thirdPartyImpl Third Party Implementations 
   (Developers are encouraged to add to this list and create 
   pages that describe the detailed behavior of these models.) 
    
   (None) 
 */

class ConverterModel : public Model {
 public:
  /**
     Default constructor for ConverterModel Class 
   */
  ConverterModel();

  /**
     every model should be destructable 
   */
  virtual ~ConverterModel() {};

  /**
     A method to initialize the model 
      
     @param cur the pointer to the xml input for the model to initialize 
   */
  virtual void init(xmlNodePtr cur);

  /**
     every model needs a method to copy one object to another 
   */
  virtual void copy(ConverterModel* src);

  /**
     This drills down the dependency tree to initialize all relevant 
     parameters/containers.  
     Note that this function must be defined only in the specific model 
     in question and not in any inherited models preceding it. 
      
     @param src the pointer to the original (initialized ?) model to be 
   */
  virtual void copyFreshModel(Model* src)=0;

  /**
     every model should be able to print a verbose description 
   */
  virtual std::string str();

protected:
  /**
     each instance of a converter needs a name 
   */
  std::string conv_name_;

  /**
     each converter needs a list of equivalent commodities it converts 
   */
  std::list<std::string> commodities_; 

public:
  /**
     Sets the converter's name 
      
     @param convName is the new name of the converter 
   */
  void setConvName(std::string convName) { conv_name_ = convName; };

  /**
     Returns the converter's name 
   */
  std::string getConvName() { return conv_name_; };

  /**
     Converts between amounts of two commodities 
      
     @param convMsg is a message concerning one convertible commodity to 
     convert @param refMsg is a message concerning the commodity to 
     convert it to  
     @return the converted message 
   */
  virtual msg_ptr convert(msg_ptr convMsg, msg_ptr refMsg); 
  
};

#endif



