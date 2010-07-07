// Material.cpp
#include <iostream>

using namespace std;

#include "Material.h"


void Material::normalize(CompMap &comp_map)
{
    double sum_total_comp = 0;
    CompMap::iterator entry;

    for (entry = comp_map.begin(); entry != comp_map.end(); entry++)
	sum_total_comp += (*entry).second;

    for (entry = comp_map.begin(); entry != comp_map.end(); entry++)
	(*entry).second /= sum_total_comp;

}

void Material::rationalize_A2M()
{
    
    total_mass = 0;

    for(CompMap::iterator entry = atom_comp.begin();
	entry != atom_comp.end();
	entry++)
    {
	mass_comp[(*entry).first] = (*entry).second * getA((*entry).first);
	total_mass += mass_comp[(*entry).first];
    }

    total_mass *= total_atoms;

    normalize(mass_comp);

}

void Material::rationalize_M2A()
{

    total_atoms = 0;
    for(CompMap::iterator entry = mass_comp.begin();
	entry != mass_comp.end();
	entry++)
    {
	atom_comp[(*entry).first] = (*entry).second / getA((*entry).first);
	total_atoms += atom_comp[(*entry).first];
    }

    total_atoms *= total_mass;

    normalize(atom_comp);

}

Material::Material(istream &input)
{

    string comp_type;
    bool atomComp = true;
    int numIsos;
    Iso isotope;
    double sum_total_comp, entry;

    input >> comp_type >> numIsos;
    
    CompMap &comp_map = ( "atom" != comp_type ? mass_comp : atom_comp );
    double &read_total_comp = ( "atom" != comp_type ? total_mass : total_atoms);

    input >> read_total_comp;
    
    for (int isoNum=0; isoNum<numIsos; isoNum++)
    {
	input >> isotope >> entry;
	comp_map[isotope] = entry;
    }

    normalize(comp_map);

    if ( "atom" != comp_type)
	rationalize_M2A();
    else
	rationalize_A2M();

}

void Material::printComp(string header, CompMap comp_map)
{

    cout << header << endl;
    for (CompMap::iterator iso = comp_map.begin();
	 iso != comp_map.end();
	 iso++)
	cout << (*iso).first << " : " <<  (*iso).second << endl;
    
}

