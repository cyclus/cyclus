// Material.h
#if !defined(_MATERIAL_H)
# define _MATERIAL_H

#include <map>

typedef int Iso;
typedef int Ele;
typedef double Atoms;
typedef double Mass;
typedef map<Iso,Atoms> AtomComp;
typedef map<Iso,Mass> MassComp;

class Material
{

private:

    AtomComp atom_comp;
    MassComp mass_comp;
    
    void atom2mass();
    void mass2atom();

    bool atomEqualsMass;
    
public:

    Material() {};
    Material(std::istream &input);

    void print() { printAtomComp(); printMassComp();};
    void printAtomComp();
    void printMassComp();

};




#endif
