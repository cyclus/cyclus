// Material.cpp
#include <iostream>

using namespace std;

#include "Material.h"

#include "Logician.h"
#include "GenException.h"


Material::Material(xmlNodePtr cur)
{
    
    recipeName = XMLinput->get_xpath_content(cur,"name");

    string comp_type = XMLinput->get_xpath_content(cur,"basis");
    CompMap &comp_map = ( "atom" != comp_type ? mass_comp : atom_comp );
    double &total_comp = ( "atom" != comp_type ? total_mass : total_atoms);

    units = XMLinput->get_xpath_content(cur,"unit");
    
    total_comp = atoi(XMLinput->get_xpath_content(cur,"total"));

    xmlNodeSetPtr isotopes = XMLinput->get_xpath_elements(cur,"isotope");

    for (int i=0;i<isotopes->nodeNr;i++)
    {
	xmlNodePtr iso_node = isotopes->nodeTab[i];
	Iso isotope = atoi(XMLinput->get_xpath_content(iso_node,"id"));
	comp_map[isotope] = atof(XMLinput->get_xpath_content(iso_node,"comp"));
    }
    
    normalize(comp_map);

    if ( "atom" != comp_type)
	rationalize_M2A();
    else
	rationalize_A2M();
}


void Material::load_recipes()
{

    /// load recipes from file
    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/recipe");
    
    for (int i=0;i<nodes->nodeNr;i++)
	LI->addRecipe(XMLinput->getCurNS() + XMLinput->get_xpath_content(nodes->nodeTab[i], "name"),
		      new Material(nodes->nodeTab[i]));

    /// load recipes from databases
    nodes = XMLinput->get_xpath_elements("/*/recipebook");

    for (int i=0;i<nodes->nodeNr;i++)
	load_recipebook(XMLinput->get_xpath_content(nodes->nodeTab[i], "filename"),
			XMLinput->get_xpath_content(nodes->nodeTab[i], "namespace"),
			XMLinput->get_xpath_content(nodes->nodeTab[i], "format"));

}

void Material::load_recipebook(string filename, string ns, string format)
{
    XMLinput->extendCurNS(ns);

    if ("xml" == format)
	XMLinput->load_recipebook(filename);
    else
	throw GenException(format + "is not a supported recipebook format.");

    XMLinput->stripCurNS();
}


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

    total_mass *= total_atoms/AVOGADRO;

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

    total_atoms *= total_mass*AVOGADRO;

    normalize(atom_comp);

}


void Material::printComp(string header, CompMap comp_map)
{

    cout << "\t" << header << endl;
    for (CompMap::iterator iso = comp_map.begin();
	 iso != comp_map.end();
	 iso++)
	cout << "\t" << (*iso).first << " : " <<  (*iso).second << endl;
    
}

