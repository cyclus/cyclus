// MassTable.h

#if !defined(_MASSTABLE)
#define _MASSTABLE
#include <string>
#include "hdf5.h"

#include "Material.h"

/*********************************************/
/*** MassTable Component Class and Functions ***/
/*********************************************/
using namespace std;
class MassTable;

/*
 * Type definition for masses
 */
typedef double Mass;

/*
 * Type definition for isotopes.
 */
typedef int Iso;

class MassTable : public Material{


protected:
	//Protected Data
  typedef struct nuclide_t
  {
    int  Z;
    int	A;
    double	mass;
  } nuclide_t;

	nuclide_t nuclide;
	int nuc_data_len;

	//Protected functions
	void initialize();						//Initializes the constructors.

public:
	//MassTable Constructors	
	MassTable();
	~MassTable();

  //MassTable Functions
  int getAtomicNum(Iso tope);
  Mass getMass(Iso tope);
  string getName(Iso tope);
};

#endif
