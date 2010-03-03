// ConvertModel.h
// This is a type of facility that implements Conversion

#if !defined(_CONVERTMODEL)
#define _CONVERTMODEL
#include "Facility.h"
#include "FacilityFactory.h"
#include "ConvertModelData.h"

using namespace std;

class Inst;

class ConvertModel : public Facility, public virtual ConvertData
{
	protected:
			/**
	 * Converts a month's worth of yellowcake to unenriched UF6.
	 */
	void convert();
	
	/**
	 * Begins a new cycle for this Conversion Facility.
	 *
	 * @param time the current time
	 */
	virtual void beginCycle(int time);

public:

	/**
	 * Constructs a new Conversion plant with the specified construction
	 * start time, owning Institution, and name.
	 *
	 * @param start the starting time for construction
	 * @param i the Institution that owns this Conversion plant
	 * @param name the name of this Conversion plant
	 * @param dur this simulation's duration
	 * @param sub the subtype for this Conversion plant
	 * @param SN the ID number for this Conversion plant
	 */
	ConvertModel(int start, Inst* i, string name, int SN, SubType sub, int dur);

	/**
	 * Constructs a Conversion with the specified data.
	 *
	 * @param sCon the time construction started on this Facility
	 * @param sOp the time operation started on this Facility
	 * @param i a pointer to the Institution this Conversion belongs to
	 * @param name this Conversion's name
	 * @param SN the ID number for this Conversion
	 * @param sub this type's subtype
	 * @param dur this simulation's duration
	 * @param constr the construction time of this Conversion, in months
	 * @param life the lifetime of this Conversion, in months
	 * @param cycle this Conversion's cycle time
	 * @param stat the current OpStatus of this Conversion
	 * @param capFac the current capacity factor of this Conversion
	 * @param cap the current capacity of this Conversion
	 * @param convEff the efficiency (between 0 and 1) of the conversion process,
	 * on a mass basis
	 * @param time the current time
	 * @param feeds a list of Commodity type(s) this Facility uses as raw material
	 * @param prods a list of Commoidty type(s) this Facility produces
	 */
	ConvertModel(int sCon, int sOp, Inst* i, string name, int SN, SubType sub, 
						 int dur, int constr, int life, int cycle, OpStatus stat, 
						 double capFac, double cap, double convEff, int time, 
             list<Commodity> feeds, list<Commodity> prods);

	/**
	 * Executes the order represented by the given Messages.
	 * 
	 * @param needsMat the (possibly altered) request some entity interested
	 * in taking on the Material
	 * @param hasMat the (possibly altered) offer this Facility sent in hopes 
	 * that someone would want its Material
	 * @param time the current time
	 */
	virtual void executeOrder(Message* needsMat, Message* hasMat, int time);

};
#endif

