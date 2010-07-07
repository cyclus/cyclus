// Material.h
#if !defined(_MATERIAL_H)
# define _MATERIAL_H

#include <map>

typedef int Iso;
typedef int Ele;
typedef double Atoms;
typedef double Mass;
typedef map<int,double> CompMap;

class Material
{

private:

    CompMap atom_comp, mass_comp;
    Mass total_mass;
    Atoms total_atoms;
    
    double getZ(Iso isotope) { return isotope/10/1000; };
    double getA(Iso isotope) { return ((isotope/10)%1000); };
    
    void normalize(CompMap &comp_map);
    void rationalize_A2M();
    void rationalize_M2A();

    bool atomEqualsMass;
    
public:
    
    Material() {};
    Material(std::istream &input);
    
    void print() {
        printComp("Atom composition:", atom_comp); 
	cout << "Total atoms: " << total_atoms << endl;
	printComp("Mass composition:", mass_comp);
	cout << "Total mass: " << total_mass << endl;
        }
    void printComp(string header, CompMap comp_map);
    
};




#endif
