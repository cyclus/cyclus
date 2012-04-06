// FacilityModel.h
#if !defined(_FACILITYMODEL_H)
#define _FACILITYMODEL_H

#include <string>
#include <vector>

#include "TimeAgent.h"
#include "Communicator.h"
#include "InstModel.h"

// forward declare Material class to avoid full inclusion and dependency
class Material;

/**
   @class FacilityModel 
   The FacilityModel class is the abstract class/interface used by all 
   facility models 
    
   This is all that is known externally about facilities 
    
   @section intro Introduction 
    
   The FacilityModel type plays a primary role in Cyclus.  A 
   FacilityModel facility is where offers and requests are generated and 
   transmitted to markets and where shipments of material, issued by the 
   markets, are executed. The algorithms to determine what offers and 
   requests are issued and how material shipments are handled are the 
   primary differentiators between different FacilityModel 
   implementations.  
   Like all model implementations, there are a number of implementations 
   that are distributed as part of the core Cyclus application as well 
   as implementations contributed by third-party developers.  The links 
   below describe additional parameters necessary for the complete 
   definition of a facility of that implementation. 
    
   @section available Available Core Implementations 
    
   - SourceFacility: A facility that can produce a constant (or 
   infinite) amount of some commodity 
   - SinkFacility:  A facility that can consume a constant (or infinite) 
   amount of some commodity 
   - NullFacility: A facility that consumes a constant amount of one 
   commodity and produces a constant amount of another 
    
    
   @section anticipated Anticipated Core Implementations 
    
   Developers are encouraged to add to this list and create pages that 
   describe the detailed behavior of these models. 
    
   - RecipeReactor: A facility that consumes a fixed fresh fuel recipe 
   one a time scale governed by reactor cycle lengths and batch sizes, 
   and produced a fixed/corresponding spent fuel recipe at the same 
   frequency - SeparationsMatrixFacility: A facility that consumes a 
   fixed quantity of material of one commodity and produces many 
   different output streams with the input isotopes distributed across 
   those output streams according to a fixed matrix 
   - EnrichmentFacility: A facility that offers a fixed quantity of SWUs 
   to accomplish isotopic enrichment of material 
    
   @section thirdparty Third-party Implementations 
    
   Collaborators are encouraged to add to this list and link to external 
   pages that describe how to get the models and the detailed behavior 
 */

class FacilityModel : public TimeAgent, public Communicator {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  FacilityModel();
  
  virtual ~FacilityModel();
  
  /**
     Initalize members of FacilityModel and any other non-input
     related parameters
   */
  virtual void init() { Model::init(); }

  /**
     Initalize the FacilityModel from xml. Calls the init() function. 
     
     @param cur the current xml node pointer 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
     every model needs a method to copy one object to another 
   */
  virtual void copy(FacilityModel* src);

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
  virtual void print() { Model::print(); };

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
  /**
     There is no default FacilityModel receiver 
      
     Each derived class must implement an offer/request receiver 
   */ 
  virtual void receiveMessage(msg_ptr msg)=0;

/* ------------------- */ 


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */
 protected:
  /**
     each facility should have an institution that manages it 
   */
  std::string inst_name_;

  /**
     each facility needs a lifetime 
   */
  int fac_lifetime_;

 public:
  /**
     Sets the facility's name 
      
     @param facName is the new name of the facility 
   */
  virtual void setFacName(std::string facName) { this->setName(facName); };

  /**
     Returns the facility's name 
      
     @return fac_name_ the name of this facility, a string 
   */
  virtual std::string facName() { return this->name(); };

  /**
     Sets this facility's instutution name 
      
     @param name the name of the institution associated with this 
   */
  virtual void setInstName(std::string name){ inst_name_ = name;};

  /**
     Returns this facility's institution 
      
     @return the institution assosicated with this facility 
   */
  virtual InstModel* facInst();

  /**
     Sets the facility's lifetime 
      
     @param lifetime is the new lifetime of the facility in months 
   */
  virtual void setFacLifetime(int lifetime) { fac_lifetime_ = lifetime; };

  /**
     Returns the facility's lifetime 
      
     @return fac_lifetime_ the lifetime of this facility, an int, in 
   */
  virtual int facLifetime() { return fac_lifetime_; };

  /**
     Returns the facility's power capacity 
      
     @return 0 by default. If the facility produces power, it will use 
     its own function. 
   */
  virtual double powerCapacity() { return 0.0; };

  /**
     Each facility is prompted to do its beginning-of-life 
     stuff. 
      
   */
  virtual void handlePreHistory();

  /**
     Each facility is prompted to do its beginning-of-time-step 
     stuff at the tick of the timer. 
      
     @param time is the time to perform the tick 
   */
  virtual void handleTick(int time);

  /**
     Each facility is prompted to its end-of-time-step 
     stuff on the tock of the timer. 
      
     @param time is the time to perform the tock 
   */
  virtual void handleTock(int time);

  /**
     Each facility is prompted to do its daily tasks 
      
     @param time is the number of months since the beginning of the 
     simulation @param day is the current day in this month 
   */
  virtual void handleDailyTasks(int time, int day);

/* ------------------- */ 
  
};

#endif



