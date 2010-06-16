// GenException.h

#if !defined(_GEN_EXCEPTION)
#define _GEN_EXCEPTION
#include <iostream>
#include <exception>
#include <string>
using namespace std;

/**
 * An exception parent class for GENIUS-specific Exceptions.
 */
class GenException: public exception
{

protected:

	/**
	 * The message associated with this exception.
	 */
	string myMessage;

public:

	/**
	 * Constructs a new GenException with the default message.
	 */
	GenException();

	/**
	 * Constructs a new GenException with the given message appended to the
	 * default message.
	 *
	 * @param msg the message
	 */
	GenException(string  msg);

	/**
	 * Returns the error message associated with this GenException.
	 *
	 * @return the message
	 */
	virtual const char* what() const throw();

	/**
	 * Destroys this GenException.
	 */
	virtual ~GenException() throw();

};
#endif
