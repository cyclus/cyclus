// GenException.h
#if !defined(_GEN_EXCEPTION)
#define _GEN_EXCEPTION
#include <iostream>
#include <exception>
#include <string>

using namespace std;


/**
 *  A generic mechanism to manually manage exceptions
 */
class GenException: public exception
{

protected:

    /// The message associated with this exception.
    string myMessage_;
    
    /// A string to prepend to all message of this class.
    static string prepend_;
    
public:
    
    /// Constructs a new GenException with the default message.
    GenException();
    
    /**
     * @brief Constructs a new GenException with a provided message
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
