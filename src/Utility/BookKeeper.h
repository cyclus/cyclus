// BookKeeper.h
#if !defined(_BOOKKEEPER)
#define _BOOKKEEPER
#include <string>
#include <map>
#include <vector>

#include "H5Cpp.h"
#include "hdf5.h"
#define BI BookKeeper::Instance()

using namespace std;
using namespace H5;

/**
 * A (singleton) class for handling I/O.
 */
class BookKeeper 
{
private:
	/**
	 * A pointer to this BookKeeper once it has been initialized.
	 */
	static BookKeeper* _instance;
		
	/**
	 * The HDF5 output database for the simulation this BookKeeper is 
	 * responsible for.
	 */
	H5File* myDB;
		
	/**
	 * Stores the final filename we'll use for the DB, since we use it in 
	 * multiple places and don't want there to be any ambiguity.
	 */
	string dbName;

  /**
   * True iff the db is open.
   */
  bool dbIsOpen;

protected:
  /**
   * The (protected) constructor for this class, which can only 
	 * be called indirectly by the client.
	 */
	BookKeeper();

	/**
	 * Opens the output database in memory space.
	 */
	void openDB();

  /**
   * Creates a group with a title.
   */
  Group* createGroup(string title);

	/**
	 * Create a property list for a dataset and set up fill values.
	 */
	DSetCreatPropList* createPropList(){};

	/**
	 * Creates a dataspace for the dataset in the file....
	 */
	DataSpace* createDataSpace(){};

	/**
	 * you'll need a dataspace... 
	 */
	void createDataSet(){};

  /** 
   * The default fill value is 0.
   * Sometimes, you'll convert it to a double.
   */
  const static int fillvalue=0;

public:
		
	/**
	 * Gives all simulation objects global access to the BookKeeper by 
	 * returning a pointer to it.
	 *
	 * @return a pointer to the BookKeeper
	 */
	static BookKeeper* Instance();
		
	/**
	 * Returns a handle to the database this BookKeeper is maintaining.
	 *
	 * @return the handle to the database
	 */
	H5File* getDB();

	/**
	 * Closes the database this BookKeeper is maintaining.
	 */
	void closeDB();
  
  /**
   * Returns whether it's open
   */
  bool isOpen(){return dbIsOpen;};

};
#endif
