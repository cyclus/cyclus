// MassTable.h
#if !defined(_MASSTABLE)
#define _MASSTABLE

#include <string>
#include <vector>
#include <map>

#define MT MassTable::Instance()

/**
   @class MassTable 
   The MassTable class provides an interface to the mass.sqlite 
   database, providing a robust and correct mass lookup by isotope 
 */
class MassTable {
private:
  /**
     A pointer to this MassTable once it has been initialized. 
   */
  static MassTable* instance_;

public:
  /**
     Gives all simulation objects global access to the Env by 
     returning a pointer to it. 
      
     @return a pointer to the MassTable 
   */
  static MassTable* Instance();

  /**
     Default constructor for the MassTable class. 
     Initializes the data from the provided mass.h5 file. 
   */
  MassTable();

  /**
     Destructor for the NullFacility class. 
     Makes certain to delete all appropriate data on the stack. 
   */
  ~MassTable();

  /**
     get the Atomic Number of an isotope according to its 
     identifier. 
      
     @param tope is the isotope identifier of type Iso, which is an int 
     typedef  
     @return int the atomic number of the tope isotope. 
   */
  int getAtomicNum(int tope);

  /**
     get the Mass, a double, of an isotope according to its 
     identifier. 
      
     @param tope is the isotope identifier of type Iso, which is an int 
     typedef  
     @return the mass, a double, of the tope isotope. 
   */
   double gramsPerMol(int tope);

protected:
  /**
     Defines the structure of data associated with each row entry in the 
     mass database. Right now, strings are a little funky, so the names 
   */
  typedef struct nuclide_t
  {
    int  Z; /**< an integer indicating the atomic (proton) number of an atom >**/
    int  A; /**< an integer indicating the A (mass A=N+Z) number of an atom.  >**/
    double  mass; /**< a double indicating the mass of an atom >**/
  } nuclide_t;

  /**
     The integer length (number of rows) of the mass.h5/ame03/nuclide/ 
   */
  int nuclide_len_;

  /**
     The vector of nuclide structs that holds the data in the mass table 
   */
  std::vector<nuclide_t> nuclide_vec_;

  /** 
     a map for index lookup in the nuclide vector. 
   */
  std::map<int, int> isoIndex_;

  /** 
     a function to initialize a large array of nuclide_t structs via the 
     SQLite/C++ API 
   */
  void initializeSQL();

};

#endif
