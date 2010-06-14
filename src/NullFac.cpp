#include <iostream>
#include <map>
#include <string>
#include "NullFac.h"

void NullFac::printMyName() 
{ 
	cout << "My Name : Null Fac and bob is "<< bob << endl;
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NullFac::NullFac() : Facility()
{	
	bob = 0;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NullFac::NullFac(double a, double b) : Facility()
{	
	bob = a+b;
};

namespace {
  class nullCreator : public FacFactory
  {
		public:
     nullCreator()
     {
        get_fac_map()["null"]=this;
		 }
    virtual Facility * Create()const
    {
			return new NullFac();
    }
  }facType;

/**
 * This is how we overload constructors when we would like to put them
 * in the nickname to pointer map in the anonymous namespace
 */
  class nullCreator2 : public FacFactory
  {
		public:
     nullCreator2()
     {
        get_fac_map()["null2"]=this;
		 }
    virtual Facility * Create()const
    {
			double a,b;
			cout << "\n" << "give me an a: "; cin >> a; 
			cout << "\n" << "give me a b: "; cin >> b; 
			return new NullFac(a,b);
    }
  }facType2;

	/**
	 * This is a test of material mapping in the anonymous namespace. 
	 * I don't think it's quite right
	 * so I'm going to leave it commented
	class NullFac : public Material
	{
		public:
			get_com_map()[1]="null";
	}myFeed;
	*/

}



