// Material.cpp
// The Material Class
include "Material.h"
#include "BookKeeper.h"
#include <math.h>


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material::Material() 
{
  throw MatException("The default material constructor is not for you!");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material::Material(map<Iso, NumDens> comp, ChemForm form, Commodity commod)
: myForm(form), myCommod(commod)
  // Record the serial number into the BookKeeper
	ID = BI->assignMaterialSN(); 
	// Record the comp history using the Timer object . . . 
  compHist[TI->getTime()] = comp;
	// Record the facility history. 
  facHist = FacHistory();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material::Material(Commodity commod, double amount, double par){
	throw MatException("I haven't set up commodity bast vector specification yet.")
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material::~Material(){
	// Only write a history if the Material goes somewhere.
	// Otherwise it was probably created for book keeping.
	if (!facHist.empty()){
		writeHist();
	}
}

void Material::changeComp(map<Iso, NumDens> newComp, int time)
{
	compHist[time] = newComp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const map<Iso, NumDens> Material::getComp() const
{
	// This creates a map between isotopes and number densities in a mat
	CompHistory::const_reverse_iterator it = compHist.rbegin();
	return it ->second;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const NumDens Material::getComp(Iso tope) const
{ 
	map<Iso, NumDens> currComp = this->getComp();
	//

	
