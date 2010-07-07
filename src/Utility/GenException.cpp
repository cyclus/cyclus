// GenException.cpp
// Implements the GenException class
#include "GenException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenException::GenException()
{
	myMessage = "\ncyclus exception";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenException::GenException(string msg)
{
	string def = "\ncyclus exception: ";
	myMessage = def + msg;
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
