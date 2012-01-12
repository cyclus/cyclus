// SWUeUF6Converter.cpp
// Implements the SWUeUF6Converter class
#include <iostream>
#include "Logger.h"

#include "SWUeUF6Converter.h"

#include "Logician.h"
#include "CycException.h"
#include "InputXML.h"
#include "GenericResource.h"
using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SWUeUF6Converter::init(xmlNodePtr cur)
{ 
  ConverterModel::init(cur);
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SWUeUF6Converter");

  // all converters require commodities - possibly many
  in_commod_ = XMLinput->get_xpath_content(cur,"incommodity");
  
  out_commod_ = XMLinput->get_xpath_content(cur,"outcommodity");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SWUeUF6Converter::copy(SWUeUF6Converter* src)
{

  ConverterModel::copy(src);

  in_commod_ = src->in_commod_;
  out_commod_ = src->out_commod_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SWUeUF6Converter::copyFreshModel(Model* src)
{
  copy((SWUeUF6Converter*)src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SWUeUF6Converter::print() 
{ 
  ConverterModel::print(); 
  LOG(LEV_DEBUG2) << "converts offers of commodity {"
      << in_commod_
      << "} into offers of commodity {"
      << out_commod_
      << "}.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Message* SWUeUF6Converter::convert(Message* convMsg, Message* refMsg)
{
  // Figure out what you're converting to and from
  in_commod_ = convMsg->commod();
  out_commod_ = refMsg->commod();
  Model* enr;
  Model* castEnr;
  Message* toRet;
  Material* mat;

  double P;
  double xp;
  double xf;
  double xw;
  double SWUs;
  double massProdU;
  IsoVector comp;


  // determine which direction we're converting
  if (in_commod_ == "SWUs" && out_commod_ == "eUF6"){
    // the enricher is the supplier in the convMsg
    enr = convMsg->getSupplier();
    if (0 == enr){
      throw CycException("SWUs offered by non-Model");
    }
    SWUs = convMsg->getResource()->getQuantity();
    try {
      mat = dynamic_cast<Material*>(refMsg->getResource());
      comp = mat->comp();
    } catch (exception& e) {
      string err = "The Resource sent to the SWUeUF6Converter must be a \
                    Material type resource.";
      throw CycException(err);
    }
  } else if (in_commod_ == "eUF6" && out_commod_ == "SWUs") {
    // the enricher is the supplier in the refMsg
    enr = refMsg->getSupplier();
    if (0 == enr) {
      throw CycException("SWUs offered by non-Model");
    }
    try{
      mat = dynamic_cast<Material*>(convMsg->getResource());
      comp = mat->comp();
    } catch (exception& e) {
      string err = "The Resource sent to the SWUeUF6Converter must be a \
                    Material type resource.";
      throw CycException(err);
    }
  }
  
  // Figure out xp the enrichment of the UF6 object
  P = comp.eltMass(92);
  xp = comp.mass(922350) / P; 

  // Figure out xf, the enrichment of the feed material
  // xf = castEnr->getFeedFrac();
  xf = WF_U235;

  // Figure out xw, the enrichment of the tails
  // xw = castEnr->getTailsFrac();
  xw = 0.0025;

  // Now, calculate
  double term1 = (2 * xp - 1) * log(xp / (1 - xp));
	double term2 = (2 * xw - 1) * log(xw / (1 - xw)) * (xp - xf) / (xf - xw);
	double term3 = (2 * xf - 1) * log(xf / (1 - xf)) * (xp - xw) / (xf - xw);
    
  massProdU = SWUs/(term1 + term2 - term3);
  SWUs = massProdU*(term1 + term2 - term3);

  if (out_commod_ == "eUF6"){
    comp.setMass(massProdU);
    mat = new Material(comp);
    toRet = convMsg->clone();
    toRet->setResource(mat);
  } else if (out_commod_ == "SWUs") {
    toRet = convMsg->clone();
    GenericResource* conv_res = new GenericResource(out_commod_, out_commod_, SWUs);
    toRet->setResource(conv_res);
  }
  
  toRet->setCommod(out_commod_);

  return toRet;
}    

extern "C" Model* constructSWUeUF6Converter() {
    return new SWUeUF6Converter();
}

extern "C" void destructSWUeUF6Converter(Model* p) {
    delete p;
}

/* ------------------- */ 

