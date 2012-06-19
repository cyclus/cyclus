// CycException.cpp
// Implements the CycException class
#include <sstream>

#include "CycException.h"

using namespace std;

string itoa(int i)    { stringstream out; out << i; return out.str(); };
string dtoa(double d) { stringstream out; out << d; return out.str(); };

string CycException::prepend_ = "cyclus exception";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CycException::CycException() {
	myMessage_ = prepend_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CycException::CycException(std::string msg) {
	myMessage_ = prepend_ + ": " + msg;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* CycException::what() const throw() {
	//const char* toRet = myMessage_;
	//	return toRet;
	return myMessage_.c_str();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CycException::~CycException() throw()  { }

