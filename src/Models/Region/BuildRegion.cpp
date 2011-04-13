// BuildRegion.cpp
// Implements the BuildRegion class

#include "BuildRegion.h"



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::populateSchedule(FILE *infile)
{
  int n_facs, n_periods;
  int i, j;
  char *fac_name;

  // Read the total number of types of facilities which will be build
  fscanf(infile, "%d", &n_facs);

  // For each type of facility, populate the build schedul
  for (i=0;i<n_facs-1;i++){
    // Read in the facility name and number of periods during which builds occur
    fscanf(infile, "%s", fac_name);
    fscanf(infile, "%d", &n_periods);
    map<int,int> schedule;

    // ??
    //      next_build[fac_name][i] = new int [n_periods];

    // For each building period, populate the facility's schedule
    for (j=0;j<n_periods-1;j++){
      int time, number;
      fscanf(infile, "%d %d", &time, &number);
      schedule[time]=number;
      next_build[fac_name][j]=time;
    };
    // Initialize the next_build_index
    next_build_index[fac_name]=0;
    build_schedule[fac_name]=schedule;
  };
  // So, each time step, we check (for each fac) ,
  // if (time == next_build[next_build_index[fac]]), then
  //    build build_schedule[fac][time] # of facilities
  //    next_build_schedule[fac]++
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::init(xmlNodePtr cur)
{
  // Initiate as a region model
  RegionModel::init(cur);

  // Get input file
  xmlNodeSetPtr region_node = XMLinput->get_xpath_elements(cur,"model/BuildRegion");
  
  string input_path = XMLinput->get_xpath_content(region_node,"input_file");

  FILE *input_file = fopen(input_path,"r");

  // Populate build schedule
  populateSchedule(input_file);
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
void BuildRegion::handleTick(int time){
  // Override RegionModel's handleTick
  for(vector<Model*>::iterator inst=institutions.begin();
      inst != institutions.end();
      inst++){

    map<char*,bool> built;
    char* keys[nFacs()];

    // This region is "greedy" and simiply rifles through each institution
    // to choose a builder
    for (map<char*,int*>::iterator fac=next_build.begin();
	 fac != next_build.end();
	 fac++){
      
      // Initialize that the facility is not built
      built[fac.key()]=false;

      if (time == next_build[fac.key()][next_build_index[fac.key()]]){
	// Request that this institution build this facility
	Model* fac_to_build=LI->getFacilityByName(fac.key());
	built[fac.key()]=requestBuild(fac_to_build1,(InstModel*)(*inst));
      }
      else {
	// Nothing should be built this turn for this facility
	built[fac.key()]=true;
      }
    }
    
    // Check to make sure that every facility that should have been built was.
    // If not, throw an exception.
    int i;
    for (i=0;i<nFacs();i++){
      bool test = built[keys[i]];
      if (!test){
	std::stringstream ss1, ss2;
	ss1 << keys[i];
	ss2 << time;
	throw GenException("Facility " + ss1.str()
			   + " could not be built at time " + ss2.str() + ".");	
      }
    }

    // Pass the handleTick onto each institution
    ((InstModel*)(*inst))->handleTick(time);
  }
}
