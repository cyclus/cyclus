#pragma once

// @TODO: find what to include

#include "cyclus.h"
#include "cycamore_version.h"

// forward declarations
namespace cycamore {
class GIS;
} // namespace cycamore

// forward includes
#include "gis_tests.h"

namespace cycamore {

///@class GIS
///
/// The GIS class is a basic class that stores the geographic 
/// location of each agents in longditude and latitude (subject
/// to change).
///
/// @TODO Imprementation of GIS information. Possibly determine 
/// if the use of Geohash is benefitial.

class GIS : public cyclus::Region {
		friend class GISTests
	public:
	/// The default constructor for GIS
	GIS();
	GIS(cyclus::Context* ctx);
	
	/// The default destructor for GIS
	~GIS();
	
	virtual std::string version() { return CYCAMORE_VERSION; }
	
	/// returns the distance between two GIS objects in kilometers
	double get_distance (GIS a, GIS b);
	
	/// returns a list of agents that are within a specified distance 
	/// (in kilometers) from an agent in ascending order
	Agent[] nearby (GIS reference, double range);
	
	/// HELPER sorts the array of agents by distance in ascending order
	double * sort (double &list[]);

	private:
	double *coordinates;
};
} // namespace cycamore
