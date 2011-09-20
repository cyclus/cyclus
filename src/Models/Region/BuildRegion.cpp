// BuildRegion.cpp
// Implements the BuildRegion class

#include "BuildRegion.h"

#include <sstream>
#include <iostream>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::populateSchedule(FILE *infile)
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
    
    // Populate the to_build_map for the given facility
    string fac_str (fac_name);
    to_build_map[fac_str]=schedule;
  };
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::init(xmlNodePtr cur)
{
  // Initiate as a region model
  RegionModel::init(cur);

  // Get input file
  xmlNodePtr region_node = XMLinput->get_xpath_element(cur,"model/BuildRegion");

  const char* input_path = XMLinput->get_xpath_content(region_node,"input_file");

  FILE *input_file = fopen(input_path,"r");

  // Populate build schedule
  populateSchedule(input_file);
  // Close the files that you open
  fclose(input_file);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool BuildRegion::requestBuild(Model* fac, InstModel* inst)
{
  bool test_build;
  // Request that Institution inst build Facility fac
  test_build=inst->pleaseBuild(fac);
  // If it is not built by the inst for whatever reason, test_build returns false
  return test_build;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int BuildRegion::nFacs()
{
  // Return the total number of facilities which will be built
  return _nFacs;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Model* BuildRegion::chooseInstToBuildFac()
{
  // Define the inst to build some fac
  // By default we pick the first institution in the region's list
  return institutions[0];
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::handleTick(int time)
{
  // Overwriting RegionModel's handleTick
  // We loop through each facility that exists in the to_build_map
  map <string, queue <pair <int,int> > >::iterator fac;

  for (fac = to_build_map.begin(); fac != to_build_map.end(); ++fac){
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
	Model* fac_to_build = LI->getModelByName(fac_name, FACILITY);
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
	throw GenException("Facility " + ss1.str()
			   + " could not be built at time " + ss2.str() + ".");	
      }
      
    } // end build loop
    
  } // end facility loop
  
  // After we finish building, call the normal handleTick for a region
  RegionModel::handleTick(time);
}
