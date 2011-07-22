// Volume.cpp
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#include "Volume.h"
#include "BookKeeper.h"
#include "GenException.h"
#include "Logician.h"

// Static variables to be initialized.
int Volume::nextID = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Volume::Volume(): temperature(0), inner_radius(0), outer_radius(0)
{
  name = "";
  ID=nextID++;
  BI->registerVolChange(this);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Volume::Volume(xmlNodePtr cur)
{
  ID=nextID++;
  
  name = XMLinput->get_xpath_content(cur,"name");

  string vol_type = XMLinput->get_xpath_content(cur,"basis");

  volCompHist = CompHistory() ;
  BI->registerVolChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Volume::Volume(string volName, Temp temp, Radius inner, Radius outer, 
    ConcMap concs, Solid* mat, Fluid* liq, Component type)  
{
  
  ID=nextID++;
  name = name;
  temperature = temp;
  inner_radius = inner;
  outer_radius = outer;
  matrix = mat;
  liquid = liq;
  concentrations = concs;
  vol_type = type;

  volCompHist = CompHistory() ;
  BI->registerVolChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Volume::print(){
    cout << "Volume: " << this->getName() << endl;
    cout << "Contains Materials:" << endl;
    for(int i=0; i<this->getWastes().size() ; i++){
      cout << wastes[i];
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Volume::absorb(Material* matToAdd)
{
  // Get the given Volume's contaminant material.
  // add the material to it with the material absorb function.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Volume::extract(Material* matToRem)
{
  // Get the given Volume's contaminant material.
  // subtract the material from it with the material extract function.
}

