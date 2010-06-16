// GenException.cpp
#include "GenException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenException::GenException()
{
	myMessage = "\nGENIUS exception";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenException::GenException(string msg)
{
	string def = "\nGENIUS exception: ";
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
