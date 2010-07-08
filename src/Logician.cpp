// Logician.cpp
// Implements the Logician class.
#include <math.h>
#include "Logician.h"
#include "GenException.h"

Logician* Logician::_instance = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician* Logician::Instance() {
	// If we haven't created a Logician yet, create and return it.
	if (0 == _instance)
		_instance = new Logician();
	
	return _instance;
}

Model* Logician::getModelByName(ModelList list, const string search_name)
{
    Model* found_model = NULL;

    for(ModelList::iterator model=list.begin();
	model != list.end();
	model++)
	if (search_name == (*model)->getName())
	    found_model = *model;

    return found_model;

}

void Logician::printRecipes()
{

    for (RecipeList::iterator recipe=recipes.begin();
	 recipe != recipes.end();
	 recipe++)
    {
	cout << "Recipe " << (*recipe).first << endl;
	(*recipe).second->print();
    }
}



