// Market.cpp
#include "Market.h"
#include "Commodity.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Market::printMyName(){
	cout << "this is the printMyName function in Market.cpp"<<endl; 
};


Market::Market(MarketType type, string mktName, int SN) : 
	myType(type), name(mktName), ID(SN)
{

	myCommods = vector<Commodity*>();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Market::addCommod(string s, int SN, Market* mkt, bool fissile, bool sepMat)
{
	Commodity* newCommod = new Commodity(s, SN, this, fissile, sepMat);
	myCommods.push_back(newCommod);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Market::addCommod(Commod* c)
{
	myCommods.push_back(c);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Region::getInst(int SN)
{
	vector<Commodity*>::iterator iter;
	for(iter = myCommods.begin(); iter != myCommods.end(); iter ++)
		if ((*iter)->getSN() == SN)
			return *iter;
	throw GenException("Error: tried to access Inst not present in this Region");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pair<vector<Commodity*>::iterator, vector<Commodity*>::iterator> Market::getCommods()
{
	return make_pair(myCommods.begin(), myCommods.end());
}

}
