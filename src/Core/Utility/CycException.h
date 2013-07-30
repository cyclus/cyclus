// CycException.h
#if !defined(_GEN_EXCEPTION)
#define _GEN_EXCEPTION

#include <exception>
#include <string>


namespace cyclus {

/**
   @class CycException 
   A generic mechanism to manually manage exceptions 
 */
class CycException: public std::exception {

protected:
    /**
       The message associated with this exception. 
     */
    std::string myMessage_;
    
    /**
       A string to prepend to all message of this class. 
     */
    static std::string prepend_;
    
public:
    /**
       Constructs a new CycException with the default message. 
     */
    CycException();
    
    /**
       Constructs a new CycException with a provided message 
        
       @param msg the message 
     */
    CycException(std::string  msg);
    
    /**
       Returns the error message associated with this CycException. 
        
       @return the message 
     */
    virtual const char* what() const throw();
    
    /**
       Destroys this CycException. 
     */
    virtual ~CycException() throw();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycIndexException: public CycException {
  public: CycIndexException(std::string msg) : CycException(msg) {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycKeyException: public CycException {
  public: CycKeyException(std::string msg) : CycException(msg) {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycIOException: public CycException {
  public: CycIOException(std::string msg) : CycException(msg) {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycTypeException: public CycException {
  public: CycTypeException(std::string msg) : CycException(msg) {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycRangeException: public CycException {
  public: CycRangeException(std::string msg) : CycException(msg) {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycParseException: public CycException {
  public: CycParseException(std::string msg) : CycException(msg) {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycOverrideException: public CycException {
  public: CycOverrideException(std::string msg) : CycException(msg) {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycNegativeValueException : public CycException {
///An exception class for something nonzero attempting to be decreased below 0.
   public: 
      CycNegativeValueException(std::string msg) : CycException(msg) {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycNotRegisteredException : public CycException {
  /** 
      An exception class for a not-registration event, i.e., when 
      an object should be registered with another object but is not
  */
 public: 
 CycNotRegisteredException(std::string msg) : CycException(msg) {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycDoubleRegistrationException : public CycException {
  /** 
      An exception class for a multiple registration event, i.e., when 
      an object should only be registered with another object once but a 
      second attempt has been made
  */
 public: 
 CycDoubleRegistrationException(std::string msg) : CycException(msg) {};
};
} // namespace cyclus
#endif


