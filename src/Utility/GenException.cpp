// GenException.cpp
// Implements the GenException class
#include <sstream>

#include "GenException.h"

string itoa(int i)    { stringstream out; out << i; return out.str(); };
string dtoa(double d) { stringstream out; out << d; return out.str(); };



string GenException::prepend = "cyclus exception";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenException::GenException()
{
	myMessage = prepend;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenException::GenException(string msg)
{
	myMessage = prepend + ": " + msg;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* GenException::what() const throw()
{
	//const char* toRet = myMessage;
	//	return toRet;
	return myMessage.c_str();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenException::~GenException() throw() 
{

}
