// Timer.h
#if !defined(_TIMER)
# define _TIMER

#include <utility>

#define TI Timer::Instance()

using namespace std;

//class Manager;

/**
 * A (singleton) timer to control a simulation with a one-month time step.
 */
class Timer
{
private:
		
	/**
	 * The Manager for which this Timer is keeping time.
	 */
//	Manager* myManager;
		
	/**
	 * A pointer to this Timer once it has been initialized.
	 */
	static Timer* _instance;

	/**
	 * The current time, measured in months from when the simulation 
	 * started.
	 */
	int time;
		
	/**
	 * The time at which the simulation started.
	 */
	int time0;
		
	/**
	 * The duration of this simulation, in months.
	 */
	int simDur;

	/**
	 * The number of the month (Jan = 1, etc.) corresponding to t = 0 for the 
	 * scenario being run.
	 */
	int month0;

	/**
	 * The year corresponding to t = 0 for the scenario being run.
	 */
	int year0;


protected:
		
	/**
	 * Constructs a new Timer for this simulation.
	 */
	Timer();
	
public:

	/**
	 * Gives all simulation objects global access to the Timer by 
	 * returning a pointer to it.
	 *
	 * @return a pointer to the Timer
	 */
	static Timer* Instance();

	/**
	 * Initialize this Timer by setting the specs for the simulation.
	 *
	 * @param dur the duration of this simulation, in months
	 * @param m0 the month the simulation starts: Jan. = 1, ..., Dec. = 12
	 * (default = 1)
	 * @param y0 the year the simulation starts (default = 2010)
	 * @param start the GENIUS time representing the first month of the simulation 
	 * (default = 0)
	 */
	void initialize(int dur, int m0 = 1, int y0 = 2010, int start = 0);

	/**
	 * Runs the simulation.
	 */ 
	void runSim();

	/**
	 * Returns the current time, in months since the simulation started.
	 *
	 * @return the current time
	 */
	int getTime();

	/**
	 * Returns the duration of the simulation this Timer's timing.
	 *
	 * @return the duration, in months
	 */
	int getSimDur();

	/**
	 * Converts the given date into a GENIUS time.
	 *
	 * @param month the month corresponding to the date (Jan = 1, etc.)
	 * @param year the year corresponding to the date
	 * @return the GENIUS date
	 * 
	 */
	int convertDate(int month, int year);

	/**
	 * Converts the given GENIUS time into a (month, year) pair.
	 */
	pair<int, int> convertDate(int time);


};
#endif
