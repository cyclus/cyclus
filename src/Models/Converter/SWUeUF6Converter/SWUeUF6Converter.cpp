// SWUeUF6Converter.cpp
// Implements the SWUeUF6Converter class
#include <iostream>
#include "Logger.h"

#include "SWUeUF6Converter.h"

#include "CycException.h"
#include "InputXML.h"
#include "GenericResource.h"
#include "Material.h"
#include "IsoVector.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SWUeUF6Converter::init(xmlNodePtr cur) {
  ConverterModel::init(cur);
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SWUeUF6Converter");

  // all converters require commodities - possibly many
  in_commod_ = XMLinput->get_xpath_content(cur,"incommodity");
  
  out_commod_ = XMLinput->get_xpath_content(cur,"outcommodity");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SWUeUF6Converter::copy(SWUeUF6Converter* src) {

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
std::string SWUeUF6Converter::str() { 
  std::string s = ConverterModel::str(); 
  s += "converts commodity '" + in_commod_;
  s += "' into commodity '" + out_commod_ + "'.";
  return s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
msg_ptr SWUeUF6Converter::convert(msg_ptr convMsg, msg_ptr refMsg)
{
  // Figure out what you're converting to and from
  in_commod_ = convMsg->trans().commod();
  out_commod_ = refMsg->trans().commod();
  Model* enr;
  Model* castEnr;
  msg_ptr toRet;
  mat_rsrc_ptr mat;

  double P;
  double xp;
  double xf;
  double xw;
  double SWUs;
  double massProdU;
  IsoVector iso_vector;


  // determine which direction we're converting
  if (in_commod_ == "SWUs" && out_commod_ == "eUF6"){
    // the enricher is the supplier in the convMsg
    enr = convMsg->trans().supplier();
    if (0 == enr){
      throw CycException("SWUs offered by non-Model");
    }
    SWUs = convMsg->trans().resource()->quantity();
    try {
      mat = boost::dynamic_pointer_cast<Material>(refMsg->trans().resource());
      iso_vector = mat->isoVector();
    } catch (exception& e) {
      string err = "The Resource sent to the SWUeUF6Converter must be a \
                    Material type resource.";
      throw CycException(err);
    }
  } else if (in_commod_ == "eUF6" && out_commod_ == "SWUs") {
    // the enricher is the supplier in the refMsg
    enr = refMsg->trans().supplier();
    if (0 == enr) {
      throw CycException("SWUs offered by non-Model");
    }
    try{
      mat = boost::dynamic_pointer_cast<Material>(convMsg->trans().resource());
      iso_vector = mat->isoVector();
    } catch (exception& e) {
      string err = "The Resource sent to the SWUeUF6Converter must be a \
                    Material type resource.";
      throw CycException(err);
    }
  }
  
  // Figure out xp the enrichment of the UF6 object
  xp = iso_vector.massFraction(922350); 

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
    mat = mat_rsrc_ptr(new Material(iso_vector));
    mat->setQuantity(massProdU);
    toRet = convMsg->clone();
    toRet->trans().setResource(mat);
  } else if (out_commod_ == "SWUs") {
    toRet = convMsg->clone();
    gen_rsrc_ptr conv_res = gen_rsrc_ptr(new GenericResource(out_commod_, out_commod_, SWUs));
    toRet->trans().setResource(conv_res);
  }
  
  toRet->trans().setCommod(out_commod_);

  return toRet;
}    

extern "C" Model* constructSWUeUF6Converter() {
    return new SWUeUF6Converter();
}


/* ------------------- */ 

