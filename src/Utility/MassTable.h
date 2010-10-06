// MassTable.h

#if !defined(_MASSTABLE)
#define _MASSTABLE
#include <string>
#include "hdf5.h"

/*********************************************/
/*** MassTable Component Class and Functions ***/
/*********************************************/
using namespace std;
class MassTable;

typedef struct Nuclide
{
	int  Z;
	int	A;
	double	mass;
  string name;
} Nuclide;

class MassTable 
{
protected:
	//Protected Data
	Nuclide * nuc_data;
	int nuc_data_len;

	//Protected functions
	void initialize ();						//Initializes the constructors.
	double getMass ();

public:
	//MassTable Constructors	
	MassTable ();
	~MassTable ();

};

#endif
