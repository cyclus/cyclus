// GenException.cpp
// Implements the GenException class
#include <sstream>

#include "GenException.h"

string itoa(int i)    { stringstream out; out << i; return out.str(); };
string dtoa(double d) { stringstream out; out << d; return out.str(); };



string GenException::prepend_ = "cyclus exception";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenException::GenException()
{
	myMessage_ = prepend_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenException::GenException(string msg)
{
	myMessage_ = prepend_ + ": " + msg;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* GenException::what() const throw()
{
	//const char* toRet = myMessage_;
	//	return toRet;
	return myMessage_.c_str();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenException::~GenException() throw() 
{

}
