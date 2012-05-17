// CapacityRegion.cpp
// Implements the CapacityRegion class

#include "CapacityRegion.h"


#include <sstream>
#include <iostream>
#include "Logger.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void CapacityRegion::populateSchedule(FILE *infile)
{
  int n_facs, n_periods=0;
  int i, j;
  char fac_name[20];

  // Read the total number of types of facilities which will be build
  fscanf(infile, "%d", &n_facs);

  // For each type of facility, populate the build schedul
  for (i=0;i<n_facs;i++){
    // Read in the facility name and number of periods during which builds occur
    fscanf(infile, "%s", fac_name);
    fscanf(infile, "%d", &n_periods);
    queue <pair <int, int> > schedule;

    // For each building period, populate the facility's schedule
    for (j=0;j<n_periods;j++){
      pair <int, int> time_step_to_build;
      int time, number;
      fscanf(infile, "%d %d", &time, &number);
      time_step_to_build.first = time;
      time_step_to_build.second = number;
      schedule.push(time_step_to_build);
    };
    
    // Populate the to_build_map_ for the given facility
    string fac_str (fac_name);
    to_build_map_[fac_str]=schedule;
  };
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void CapacityRegion::initBuild(xmlNodePtr cur)
{
  // Get input file
  xmlNodePtr region_node = XMLinput->get_xpath_element(cur,"model/CapacityRegion");

  const char* input_path = XMLinput->get_xpath_content(region_node,"input_file");

  FILE *input_file = fopen(input_path,"r");

  // Populate build schedule
  populateSchedule(input_file);
  // Close the files that you open
  fclose(input_file);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void CapacityRegion::initCapacity(xmlNodePtr cur)
{
  // Get input file
  LOG(LEV_DEBUG2, "none!") << "capacity region cur: " << cur;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "model/CapacityRegion/capacitydemand");
  
  // for each fuel pair, there is an in and an out commodity
  for (int i=0;i<nodes->nodeNr;i++){
    LOG(LEV_DEBUG2, "none!") << "i am in a capacity region node!";
    // get xml node
    xmlNodePtr entry_node = nodes->nodeTab[i];
    // get capacity information
    capacity_type_.push_back( XMLinput->get_xpath_content( entry_node,"capacitytype" ));
    capacity_function_.push_back( XMLinput->get_xpath_content( entry_node,"capacityfunction" ));
    nominal_value_.push_back( strtod( XMLinput->get_xpath_content( entry_node,"nominalvalue" ), NULL));
    // get replacement facility information
    ReplacementFacs replacementFacs;
    allReplacementFacs_.push_back(replacementFacs);
    Model* facility;
    xmlNodeSetPtr fac_nodes = XMLinput->get_xpath_elements(entry_node, "replacementlist");
    for (int j=0;i<fac_nodes->nodeNr;j++){
      xmlNodePtr fac_node = fac_nodes->nodeTab[j];
      // assign each item initially in storage
      facility = NULL;
      // facility
      string fac_name = XMLinput->get_xpath_content(fac_node,"replacementfacility");
      LOG(LEV_DEBUG2, "none!") << "fac_name:" << fac_name << "is on the list of repalcement facilities";
      facility = dynamic_cast<FacilityModel*>(Model::getModelByName(fac_name));

      allReplacementFacs_[i].push_back(facility);
    };
  };    
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void CapacityRegion::init(xmlNodePtr cur)
{
  // Initiate as a region model
  RegionModel::init(cur);
  // Initiate the building schedule
  initBuild(cur);
  // Initiate the capacity data
  initCapacity(cur);
  LOG(LEV_DEBUG2, "none!") << "checking stage 1";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool CapacityRegion::requestBuild(Model* fac, InstModel* inst)
{
  bool test_build;
  // Request that Institution inst build Facility fac
  test_build=inst->pleaseBuild(fac);
  // If it is not built by the inst for whatever reason, test_build returns false
  return test_build;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int CapacityRegion::nFacs() {
  // Return the total number of facilities which will be built
  return nFacs_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Model* CapacityRegion::chooseInstToBuildFac() {
  // Define the inst to build some fac
  // By default we pick the first institution in the region's list
  return children(0);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CapacityRegion::checkCurrentCapcity(string capacity_type) {
  // for now, all capacity types will be power capacity
  double capacity = 0.0;
  for(vector<Model*>::iterator inst=children_.begin();
      inst != children_.end();
      inst++){
    capacity += (dynamic_cast<InstModel*>(*inst))->powerCapacity();
  }
  return capacity;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Model* CapacityRegion::chooseFacToBuild(ReplacementFacs facs) {
  // this needs to be fleshed out
  return facs[0];
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void CapacityRegion::handleTick(int time) {
  // Overwriting RegionModel's handleTick
  // We loop through each facility that exists in the to_build_map_
  map <string, queue <pair <int,int> > >::iterator fac;

  for (fac = to_build_map_.begin(); fac != to_build_map_.end(); ++fac){
    // Define a few parameters
    /* !!!! This method is not full proof... what if multiple facilities need
			 to be built and some fail and some succeed...? !!!! */
    bool built = false; 
    string fac_name = fac->first;
    queue<pair <int,int> > fac_build_queue = fac->second;
    pair<int,int> next_fac_build = fac_build_queue.front();
    int next_build_time = next_fac_build.first;

    // Continue to loop until the facility is built
    while (built!=true){
      // If the current time = the next build time for a facility, 
      // build said facility
      if (time == next_build_time) {
	Model* inst;
	Model* fac_to_build = Model::getModelByName(fac_name);
	int num_facs_to_build = next_fac_build.second;
	int i;
	// Build the prescribed number of facilities for this time step
	for (i=0;i!=num_facs_to_build;i++){
	  inst = chooseInstToBuildFac();
	  built = requestBuild(fac_to_build,dynamic_cast<InstModel*>(inst));
	}
      }
      // If there is nothing to build at this time, consider 0 facilities built
      else {
	built=true;
      }
      
      // For now, catch any situation for which no facility is built.
      // ************* This should eventually be changed
      if (built!=true){
	std::stringstream ss1, ss2;
	ss1 << fac_name;
	ss2 << time;
	throw CycException("Facility " + ss1.str()
			   + " could not be built at time " + ss2.str() + ".");	
      }
      
    } // end build loop
    
  } // end facility loop
  
  // check current capacity
  for (int i=0;i<nCapacities();i++){
    bool build_facility = true;
    bool built = false;
    while (build_facility){
      double current_capacity = checkCurrentCapcity(capacity_type(i));
      build_facility = current_capacity < nominal_value(i);
      Model* fac_to_build;
      if (build_facility){
	Model* inst = chooseInstToBuildFac();
	fac_to_build = chooseFacToBuild( allReplacementFacs_[i] );
	built = requestBuild(fac_to_build,dynamic_cast<InstModel*>(inst));
      }
      // For now, catch any situation for which no facility is built.
      // ************* This should eventually be changed
      if (build_facility && !built){
	string fac_name = (dynamic_cast<FacilityModel*>(fac_to_build))->facName();
	std::stringstream ss1, ss2;
	ss1 << fac_name;
	ss2 << time;
	throw CycException("Facility " + ss1.str()
			   + " could not be built at time " + ss2.str() + ".");	
      }
    }
  }

  // After we finish building, call the normal handleTick for a region
  RegionModel::handleTick(time);
}


/* --------------------
   output database info
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string CapacityRegion::outputDir_ = "/capacity";


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructCapacityRegion() {
    return new CapacityRegion();
}


/* -------------------- */
