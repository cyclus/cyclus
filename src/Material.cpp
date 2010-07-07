// Material.cpp
#include <iostream>

using namespace std;

#include "Material.h"


Material::Material(istream &input)
{

    string comp_type;
    bool atomComp = true;
    int numIsos;
    Iso isotope;
    Atoms atom_dens;
    Mass mass_dens;

    input >> comp_type >> numIsos;

    if ("atom" != comp_type)
	atomComp = false;

    for (int isoNum=0; isoNum<numIsos; isoNum++)
    {
	input >> isotope;
	input >> (atomComp ? atom_comp[isotope] : mass_comp[isotope]);
    }

    if (atomComp)
	atom2mass();
    else
	mass2atom();

}

void Material::printAtomComp()
{

    cout << "Atomic composition:" << endl;
    for (AtomComp::iterator iso = atom_comp.begin();
	 iso != atom_comp.end();
	 iso++)
	cout << (*iso).first << " : " <<  (*iso).second << endl;
    
}

void Material::printMassComp()
{

    cout << "Mass composition:" << endl;
    for (MassComp::iterator iso = mass_comp.begin();
	 iso != atom_comp.end();
	 iso++)
	cout << (*iso).first << " : " <<  (*iso).second << endl;

}
