// GenException.h
#if !defined(_GEN_EXCEPTION)
#define _GEN_EXCEPTION

#include <exception>
#include <string>

/**
 *  A generic mechanism to manually manage exceptions
 */
class GenException: public std::exception {

protected:

    /// The message associated with this exception.
    std::string myMessage_;
    
    /// A string to prepend to all message of this class.
    static std::string prepend_;
    
public:
    
    /// Constructs a new GenException with the default message.
    GenException();
    
    /**
     * @brief Constructs a new GenException with a provided message
     *
     * @param msg the message
     */
    GenException(std::string  msg);
    
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

