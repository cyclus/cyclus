// This file is composed of the following original files:

//   license.txt
//   cpp/pyne.cpp
//   cpp/state_map.cpp
//   cpp/nucname.cpp
//   cpp/rxname.cpp
//   cpp/data.cpp
//   cpp/jsoncpp.cpp

// PyNE amalgated source http://pyne.io/
#include "pyne.h"

//
// start of license.txt
//
// Copyright 2011-2014, the PyNE Development Team. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
//    1. Redistributions of source code must retain the above copyright notice, this list of
//       conditions and the following disclaimer.
// 
//    2. Redistributions in binary form must reproduce the above copyright notice, this list
//       of conditions and the following disclaimer in the documentation and/or other materials
//       provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE PYNE DEVELOPMENT TEAM ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// The views and conclusions contained in the software and documentation are those of the
// authors and should not be interpreted as representing official policies, either expressed
// or implied, of the stakeholders of the PyNE project or the employers of PyNE developers.
// //
// end of license.txt
//


//
// start of cpp/pyne.cpp
//
// General Library

#ifndef PYNE_IS_AMALGAMATED
#include "pyne.h"
#endif


// PyNE Globals

std::string pyne::PYNE_DATA = "";
std::string pyne::NUC_DATA_PATH = "";

void pyne::pyne_start() {
#if defined __WIN_MSVC__
  char * tmpPYNE_DATA;
  size_t lenPYNE_DATA;
  errno_t errPYNE_DATA = _dupenv_s(&tmpPYNE_DATA, &lenPYNE_DATA, "PYNE_DATA");
  if (errPYNE_DATA)
    tmpPYNE_DATA = (char *) "<NOT_FOUND>";
  PYNE_DATA = (std::string) tmpPYNE_DATA;

  char * tmpNUC_DATA_PATH;
  size_t lenNUC_DATA_PATH;
  errno_t errNUC_DATA_PATH = _dupenv_s(&tmpNUC_DATA_PATH, &lenNUC_DATA_PATH, "NUC_DATA_PATH");
  if (errPYNE_DATA)
    tmpNUC_DATA_PATH = (char *) "<NOT_FOUND>";
  NUC_DATA_PATH = (std::string) tmpNUC_DATA_PATH;
#else
  char * tmppath;
  tmppath = getenv("PYNE_DATA");
  if (tmppath == NULL)
      tmppath = (char *) "<NOT_FOUND>";
  PYNE_DATA = std::string(tmppath);

  tmppath = getenv("NUC_DATA_PATH");
  if (tmppath == NULL)
      tmppath = (char *) "<NOT_FOUND>";
  NUC_DATA_PATH = std::string(tmppath);
#endif
  return;
};



// String Transformations
std::string pyne::to_str (int t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}

std::string pyne::to_str(unsigned int t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}

std::string pyne::to_str (double t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}

std::string pyne::to_str (bool t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}


int pyne::to_int (std::string s) {
  return atoi( s.c_str() );
}

double pyne::to_dbl (std::string s) {
  return strtod( s.c_str(), NULL );
}

double pyne::endftod (char * s) {
  // Converts string from ENDF to float64.
  int pos, mant, exp;
  double v, dbl_exp;

  mant = exp = 0;
  if (s[2] == '.') {
    // Convert an ENDF float
    if (s[9] == '+' or s[9] == '-') {
      // All these factors of ten are from place values.
      mant = s[8] + 10 * s[7] + 100 * s[6] + 1000 * s[5] + 10000 * s[4] + \
             100000 * s[3] + 1000000 * s[1] - 1111111 * '0';
      exp = s[10] - '0';
      // Make the right power of 10.
      dbl_exp = exp & 01? 10.: 1;
      dbl_exp *= (exp >>= 1) & 01? 100.: 1;
      dbl_exp *= (exp >>= 1) & 01? 1.0e4: 1;
      dbl_exp *= (exp >>= 1) & 01? 1.0e8: 1;
      // Adjust for powers of ten from treating mantissa as an integer.
      dbl_exp = (s[9] == '-'? 1/dbl_exp: dbl_exp) * 1.0e-6;
      // Get mantissa sign, apply exponent.
      v = mant * (s[0] == '-'? -1: 1) * dbl_exp;
    }
    else {
      mant = s[7] + 10 * s[6] + 100 * s[5] + 1000 * s[4] + 10000 * s[3] + \
             100000 * s[1] - 111111 * '0';
      exp = s[10] + 10 * s[9] - 11 * '0';
      dbl_exp = exp & 01? 10.: 1;
      dbl_exp *= (exp >>= 1) & 01? 100.: 1;
      dbl_exp *= (exp >>= 1) & 01? 1.0e4: 1;
      dbl_exp *= (exp >>= 1) & 01? 1.0e8: 1;
      dbl_exp *= (exp >>= 1) & 01? 1.0e16: 1;
      dbl_exp *= (exp >>= 1) & 01? 1.0e32: 1;
      dbl_exp *= (exp >>= 1) & 01? 1.0e64: 1;
      dbl_exp = (s[8] == '-'? 1/dbl_exp: dbl_exp) * 1.0e-5;
      v = mant * (s[0] == '-'? -1: 1) * dbl_exp;
    }
  }

  // Convert an ENDF int to float; we start from the last char in the field and
  // move forward until we hit a non-digit.
  else {
    v = 0;
    mant = 1; // Here we use mant for the place value about to be read in.
    pos = 10;
    while (s[pos] != '-' and s[pos] != '+' and s[pos] != ' ' and pos > 0) {
      v += mant * (s[pos] - '0');
      mant *= 10;
      pos--;
    }
    v *= (s[pos] == '-'? -1: 1);
  }
  return v;
}

std::string pyne::to_upper(std::string s) {
  // change each element of the string to upper case.
  for(unsigned int i = 0; i < s.length(); i++)
    s[i] = toupper(s[i]);
  return s;
}

std::string pyne::to_lower(std::string s) {
  // change each element of the string to lower case
  for(unsigned int i = 0; i < s.length(); i++)
    s[i] = tolower(s[i]);
  return s;
}


std::string pyne::capitalize(std::string s) {
  unsigned int slen = s.length();
  if (slen == 0)
    return s;
  // uppercase the first character
  s[0] = toupper(s[0]);
  // change each subsequent element of the string to lower case
  for(unsigned int i = 1; i < slen; i++)
    s[i] = tolower(s[i]);
  return s;
}


std::string pyne::get_flag(char line[], int max_l) {
  char tempflag [10];
  for (int i = 0; i < max_l; i++)
  {
    if (line[i] == '\t' || line[i] == '\n' || line[i] == ' ' || line[i] == '\0')
    {
      tempflag[i] = '\0';
      break;
    }
    else
      tempflag[i] = line[i];
  }
  return std::string (tempflag);
}



std::string pyne::remove_substring(std::string s, std::string substr) {
  // Removes a substring from the string s
  int n_found = s.find(substr);
  while ( 0 <= n_found ) {
    s.erase( n_found , substr.length() );
    n_found = s.find(substr);
  }
  return s;
}


std::string pyne::remove_characters(std::string s, std::string chars) {
  // Removes all characters in the string chars from the string s
  for (int i = 0; i < chars.length(); i++ ) {
    s = remove_substring(s, chars.substr(i, 1) );
  }
  return s;
}


std::string pyne::replace_all_substrings(std::string s, std::string substr, std::string repstr) {
  // Replaces all instance of substr in s with the string repstr
  int n_found = s.find(substr);
  while ( 0 <= n_found ) {
    s.replace( n_found , substr.length(), repstr );
    n_found = s.find(substr);
  }
  return s;
};



std::string pyne::last_char(std::string s) {
    // Returns the last character in a string.
    return s.substr(s.length()-1, 1);
}


std::string pyne::slice_from_end(std::string s, int n, int l) {
  // Returns the slice of a string using negative indices.
  return s.substr(s.length()+n, l);
}


bool pyne::ternary_ge(int a, int b, int c) {
  // Returns true id a <= b <= c and flase otherwise.
  return (a <= b && b <= c);
}


bool pyne::contains_substring(std::string s, std::string substr) {
  // Returns a boolean based on if the sub is in s.
  int n = s.find(substr);
  return ( 0 <= n && n < s.length() );
}


std::string pyne::natural_naming(std::string name) {
  // Calculates a version on the string name that is a valid
  // variable name, ie it uses only word characters.
  std::string nat_name (name);

  // Replace Whitespace characters with underscores
  nat_name = pyne::replace_all_substrings(nat_name, " ",  "_");
  nat_name = pyne::replace_all_substrings(nat_name, "\t", "_");
  nat_name = pyne::replace_all_substrings(nat_name, "\n", "_");

  // Remove non-word characters
  int n = 0;
  while ( n < nat_name.length() ) {
    if ( pyne::words.find(nat_name[n]) == std::string::npos )
      nat_name.erase(n, 1);
    else
      n++;
  }

  // Make sure that the name in non-empty before continuing
  if (nat_name.length() == 0)
    return nat_name;

  // Make sure that the name doesn't begin with a number.
  if ( pyne::digits.find(nat_name[0]) != std::string::npos)
    nat_name.insert(0, "_");

  return nat_name;
};


//
// Math Helpers
//

double pyne::slope(double x2, double y2, double x1, double y1) {
  // Finds the slope of a line.
  return (y2 - y1) / (x2 - x1);
};


double pyne::solve_line(double x, double x2, double y2, double x1, double y1) {
  return (slope(x2,y2,x1,y1) * (x - x2)) + y2;
};


double pyne::tanh(double x) {
  return std::tanh(x);
};

double pyne::coth(double x) {
  return 1.0 / std::tanh(x);
};



// File Helpers

bool pyne::file_exists(std::string strfilename) {
  // Thank you intarwebz for this function!
  // Sepcifically: http://www.techbytes.ca/techbyte103.html
  struct stat stFileInfo; 
  bool blnReturn; 
  int intStat; 

  // Attempt to get the file attributes 
  intStat = stat(strfilename.c_str(), &stFileInfo); 

  if(intStat == 0) { 
    // We were able to get the file attributes 
    // so the file obviously exists. 
    blnReturn = true; 
  } 
  else { 
    // We were not able to get the file attributes. 
    // This may mean that we don't have permission to 
    // access the folder which contains this file. If you 
    // need to do that level of checking, lookup the 
    // return values of stat which will give you 
    // more details on why stat failed. 
    blnReturn = false; 
  } 
   
  return(blnReturn); 
};
//
// end of cpp/pyne.cpp
//


//
// start of cpp/state_map.cpp
//
//Mapping file for state ids to nuc ids
//This File was autogenerated!!
#ifndef PYNE_4HFU6PUEQJB3ZJ4UIFLVU4SPCM
#define PYNE_4HFU6PUEQJB3ZJ4UIFLVU4SPCM
namespace pyne {
namespace nucname {
#define TOTAL_STATE_MAPS 885
std::map<int, int> state_id_map;
int map_nuc_ids [TOTAL_STATE_MAPS] = {110240001,
130240001,
130260001,
130320002,
170340001,
170380001,
190380001,
190380015,
210420002,
210430001,
210440004,
230440001,
210450001,
210460002,
230460001,
210500001,
250500001,
250520001,
260520041,
260530022,
270540001,
210560001,
210560004,
250580001,
270580001,
270580002,
250600001,
270600001,
250620001,
270620001,
250640002,
260650003,
260670002,
290670023,
300690001,
290700001,
290700003,
350700006,
280710002,
300710001,
320710002,
300730001,
300730002,
320730002,
340730001,
360730004,
310740002,
350740002,
290750001,
290750002,
300750001,
320750002,
330750004,
280760004,
290760001,
350760002,
300770002,
320770001,
330770004,
340770001,
350770001,
300780004,
310780004,
350780004,
370780003,
390780001,
320790001,
330790007,
340790001,
350790001,
360790001,
350800002,
390800001,
390800003,
320810001,
340810001,
360810002,
370810001,
330820001,
340820015,
350820001,
370820001,
410820003,
340830001,
360830002,
380830002,
390830001,
310840001,
350840001,
360840019,
360840061,
370840002,
390840002,
410840007,
360850001,
380850002,
390850001,
400850002,
370860002,
390860002,
410860001,
410860002,
380870001,
390870001,
400870002,
370880019,
390880002,
390880003,
400880013,
390890001,
400890001,
410890001,
420890002,
430890001,
370900001,
390900002,
400900003,
410900002,
410900007,
430900001,
430900006,
390910001,
400910040,
410910001,
420910001,
430910001,
440910001,
450910001,
410920001,
450920001,
390930002,
410930001,
420930016,
430930001,
440930001,
470940001,
470940002,
390970001,
390970029,
410970001,
430970001,
450970001,
370980001,
390980005,
410980001,
450980001,
410990001,
430990002,
450990001,
470990002,
371000001,
391000004,
411000001,
411000009,
411000012,
431000002,
431000004,
451000004,
471000001,
471010002,
411020001,
431020001,
451020005,
471020001,
441030005,
451030001,
471030002,
491030001,
411040004,
451040003,
471040001,
491040003,
451050001,
471050001,
491050001,
451060001,
471060001,
491060001,
431070000,
461070002,
471070001,
491070001,
401080003,
461090002,
471090001,
491090001,
491090021,
451100000,
451100001,
471100002,
491100001,
461110002,
471110001,
491110001,
451120000,
451120001,
491120001,
491120004,
491120010,
471130001,
481130001,
491130001,
501130001,
451140005,
491140001,
491140005,
531140005,
461150001,
471150001,
481150001,
491150001,
521150001,
451160001,
471160001,
471160004,
511160003,
551160001,
471180004,
491180001,
491180003,
511180007,
531180002,
551180001,
471190000,
471190001,
481190002,
491190001,
501190002,
511190072,
521190002,
551190001,
471200002,
491200001,
491200002,
511200001,
531200013,
551200001,
571200000,
461210001,
481210002,
491210001,
501210001,
521210002,
551210001,
471220001,
471220002,
491220001,
491220005,
511220005,
511220006,
551220007,
551220008,
481230003,
491230001,
501230001,
521230002,
551230005,
491240002,
501240016,
511240001,
511240002,
551240025,
481250001,
491250001,
501250001,
521250002,
541250002,
571250005,
491260001,
511260001,
511260002,
481270006,
491270001,
491270009,
501270001,
521270002,
541270002,
561270002,
571270001,
581270001,
491280003,
501280003,
511280001,
571280001,
491290001,
501290001,
511290011,
511290012,
521290001,
541290002,
561290001,
571290002,
581290001,
591290001,
491300001,
491300002,
491300003,
501300002,
511300001,
531300001,
551300004,
561300030,
591300002,
491310001,
491310004,
501310001,
521310001,
521310033,
541310002,
561310002,
571310006,
581310001,
591310002,
501320006,
511320001,
521320006,
521320022,
531320003,
541320030,
571320004,
581320030,
491330001,
521330002,
531330016,
531330059,
531330065,
541330001,
561330002,
581330001,
591330003,
601330001,
611330005,
621330000,
511340002,
521340003,
531340005,
541340007,
601340017,
611340000,
611340001,
521350010,
541350002,
551350010,
561350002,
581350004,
591350004,
601350001,
611350000,
611350003,
531360006,
551360001,
561360005,
611360000,
611360001,
631360001,
561370002,
581370002,
601370004,
551380003,
581380005,
591380005,
581390002,
601390002,
611390001,
621390004,
641390001,
591400003,
591400015,
601400009,
611400008,
631400004,
601410002,
621410002,
631410001,
641410004,
651410001,
591420001,
591420024,
611420012,
631420031,
641420019,
641420020,
651420003,
621430002,
621430043,
641430002,
651430001,
661430003,
551440004,
591440001,
651440004,
651440006,
651440007,
671440003,
641450002,
651450004,
661450002,
681450002,
571460001,
631460013,
651460022,
651460026,
661460008,
651470001,
661470002,
681470002,
691470001,
591480000,
591480001,
611480000,
611480003,
651480001,
671480001,
671480012,
681480008,
651490001,
661490027,
671490001,
681490002,
631500001,
651500002,
671500001,
691500005,
581510001,
621510012,
631510002,
651510003,
671510001,
681510021,
691510001,
691510012,
701510001,
701510005,
701510010,
611520004,
611520014,
631520001,
631520016,
651520006,
671520001,
691520006,
691520018,
691520019,
701520006,
621530006,
641530003,
641530008,
651530003,
671530001,
691530001,
601540003,
611540000,
631540013,
651540001,
651540002,
711540015,
721540006,
641550006,
661550009,
671550002,
691550001,
711550001,
711550004,
611560002,
651560002,
651560004,
671560001,
671560012,
711560001,
721560004,
641570012,
661570005,
651580003,
651580019,
671580001,
671580007,
711580000,
621590006,
641590002,
661590009,
671590003,
671600001,
671600006,
691600002,
711600001,
671610002,
681610014,
691610001,
711610004,
671620003,
691620020,
711620008,
711620009,
751620001,
671630003,
751630001,
671640003,
691640001,
771640001,
661650002,
751650001,
771650001,
671660001,
691660006,
711660001,
711660002,
681670003,
711670001,
751670001,
671680001,
711680013,
771680001,
701690001,
711690001,
751690001,
771690001,
671700001,
711700008,
771700001,
711710001,
721710001,
771710001,
781710002,
711720001,
711720005,
751720001,
771720002,
791720001,
771730000,
771730029,
791730001,
711740003,
771740001,
701750007,
711750053,
791750001,
701760005,
711760001,
731760012,
731760090,
791760001,
791760002,
691770000,
701770006,
711770029,
711770203,
721770048,
721770107,
791770002,
711780003,
721780005,
721780109,
731780000,
731780059,
731780094,
731780139,
711790006,
721790005,
721790046,
731790117,
741790002,
751790137,
791790007,
811790001,
711800010,
721800007,
731800002,
721810025,
721810078,
761810001,
811810002,
721820009,
721820026,
731820001,
731820029,
751820001,
761820029,
741830007,
751830058,
761830002,
781830001,
811830002,
721840005,
751840005,
771840007,
781840034,
791840003,
741850006,
781850002,
791850001,
801850004,
811850003,
751860004,
771860001,
811860000,
811860005,
831860001,
791870002,
801870001,
811870002,
821870001,
831870002,
751880007,
811880001,
761890001,
771890006,
771890084,
791890003,
801890002,
811890001,
821890001,
831890002,
831890003,
731900002,
741900006,
751900003,
761900032,
771900002,
771900037,
791900014,
811900000,
811900001,
811900006,
831900000,
831900001,
761910001,
771910003,
771910071,
791910004,
801910035,
811910002,
821910002,
831910002,
751920002,
751920003,
761920047,
761920112,
771920003,
771920015,
791920004,
791920015,
811920002,
811920008,
821920011,
821920014,
821920017,
821920020,
821920021,
831920001,
841920006,
851920000,
851920001,
771930002,
781930005,
791930004,
801930003,
811930002,
821930001,
831930001,
851930001,
851930002,
751940001,
771940007,
771940012,
791940003,
791940008,
811940001,
831940001,
831940002,
851940000,
851940001,
761950003,
771950002,
781950007,
791950004,
801950003,
811950002,
821950002,
831950001,
841950002,
851950001,
861950001,
751960001,
771960004,
791960003,
791960054,
811960006,
831960002,
831960003,
841960015,
761970001,
771970002,
781970009,
791970004,
801970004,
811970002,
821970002,
831970001,
841970002,
851970001,
861970001,
761980006,
761980010,
771980001,
791980050,
811980007,
811980012,
831980001,
831980003,
851980001,
781990008,
791990006,
801990007,
811990003,
821990003,
831990001,
841990002,
861990001,
812000010,
832000001,
832000003,
852000001,
852000003,
802010013,
812010003,
822010004,
832010001,
842010003,
862010001,
872010001,
882010000,
782020003,
822020014,
852020001,
852020002,
872020001,
822030006,
822030053,
832030006,
842030005,
862030001,
882030001,
812040029,
822040021,
832040008,
832040038,
852040001,
872040001,
872040002,
802050008,
822050009,
842050010,
842050017,
882050001,
812060045,
832060016,
872060001,
872060002,
892060001,
812070002,
822070003,
832070036,
842070014,
862070007,
882070001,
802080004,
832080018,
802100002,
802100005,
832100002,
822110014,
832110021,
842110015,
852110076,
872110013,
872110019,
832120005,
832120012,
842120030,
852120004,
882130005,
852140006,
862140004,
862140005,
872140001,
902140004,
832150003,
872160001,
892170010,
902170001,
912170001,
872180002,
922180001,
892220001,
912340002,
922350001,
932360001,
952360001,
942370003,
922380101,
932380128,
942380041,
942380044,
952380001,
942390090,
942390094,
952390011,
932400001,
942400102,
952400057,
962400002,
962400003,
942410106,
942410107,
952410075,
962410007,
932420007,
942420044,
942420045,
952420002,
952420141,
962420004,
962420005,
972420002,
972420003,
942430074,
952430029,
962430030,
972430004,
942440032,
952440001,
952440112,
952440113,
962440009,
962440013,
962440014,
972440004,
982440002,
942450024,
952450021,
962450061,
972450003,
1012450001,
952460001,
952460008,
972460000,
982460002,
992460000,
1012460000,
1012460001,
1002470001,
1002470002,
972480001,
992500001,
1002500001,
1002500002,
1022500001,
1022510002,
1002530008,
1022530003,
1022530030,
1022530031,
1022530032,
1032530000,
1032530001,
992540002,
1012540000,
1012540001,
1022540011,
1032550001,
1032550027,
992560001,
1002560022,
1042570002,
1052570002,
1012580001,
1052580001,
1042610001,
1072620001,
1062630003,
1062650001,
1082650001,
1082670002,
1102700001,
1102710001,
};int map_metastable [TOTAL_STATE_MAPS] = {1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
3,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
3,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
2,
1,
1,
1,
1,
1,
1,
2,
1,
2,
3,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
2,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
2,
1,
2,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
2,
3,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
2,
3,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
2,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
2,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
2,
3,
1,
2,
1,
2,
1,
1,
1,
2,
3,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
2,
1,
2,
1,
1,
2,
1,
1,
2,
1,
2,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
2,
1,
1,
1,
2,
1,
2,
3,
4,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
2,
3,
1,
1,
1,
2,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
2,
1,
1,
2,
3,
1,
2,
1,
1,
2,
1,
1,
1,
1,
1,
1,
2,
1,
2,
1,
2,
1,
2,
1,
2,
1,
2,
3,
4,
5,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
2,
1,
2,
1,
1,
2,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
2,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
2,
1,
1,
1,
2,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
2,
1,
2,
2,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
2,
1,
1,
2,
1,
2,
1,
2,
1,
2,
1,
2,
1,
1,
1,
1,
1,
1,
2,
3,
1,
2,
3,
1,
1,
1,
1,
1,
1,
1,
1,
2,
1,
1,
1,
1,
2,
2,
1,
1,
1,
1,
2,
1,
1,
1,
1,
2,
3,
4,
1,
1,
1,
1,
2,
1,
1,
2,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
};
}
}
#endif//
// end of cpp/state_map.cpp
//


//
// start of cpp/nucname.cpp
//
// Converts between naming conventions for nuclides.
// zzaaam is for numerals only (923350).
// name is for letters  as well (U-235).
// MCNP is for numerals without the meta-stable flag (92235), as used in MCNP.

#ifndef PYNE_IS_AMALGAMATED
#include "nucname.h"
#include "state_map.cpp"
#endif


/*** Constructs the LL to zz Dictionary ***/
pyne::nucname::name_zz_t pyne::nucname::get_name_zz() {
  pyne::nucname::name_zz_t lzd;

  lzd["Be"] = 04;
  lzd["Ba"] = 56;
  lzd["Bh"] = 107;
  lzd["Bi"] = 83;
  lzd["Bk"] = 97;
  lzd["Br"] = 35;
  lzd["Ru"] = 44;
  lzd["Re"] = 75;
  lzd["Rf"] = 104;
  lzd["Rg"] = 111;
  lzd["Ra"] = 88;
  lzd["Rb"] = 37;
  lzd["Rn"] = 86;
  lzd["Rh"] = 45;
  lzd["Tm"] = 69;
  lzd["H"] = 01;
  lzd["P"] = 15;	
  lzd["Ge"] = 32;
  lzd["Gd"] = 64;
  lzd["Ga"] = 31;
  lzd["Os"] = 76;
  lzd["Hs"] = 108;
  lzd["Zn"] = 30;
  lzd["Ho"] = 67;
  lzd["Hf"] = 72;
  lzd["Hg"] = 80;
  lzd["He"] = 02;
  lzd["Pr"] = 59;
  lzd["Pt"] = 78;
  lzd["Pu"] = 94;
  lzd["Pb"] = 82;
  lzd["Pa"] = 91;
  lzd["Pd"] = 46;
  lzd["Po"] = 84;
  lzd["Pm"] = 61;
  lzd["C"] = 6;
  lzd["K"] = 19;
  lzd["O"] = 8;
  lzd["S"] = 16;
  lzd["W"] = 74;
  lzd["Eu"] = 63;
  lzd["Es"] = 99;
  lzd["Er"] = 68;
  lzd["Md"] = 101;
  lzd["Mg"] = 12;
  lzd["Mo"] = 42;
  lzd["Mn"] = 25;
  lzd["Mt"] = 109;
  lzd["U"] = 92;
  lzd["Fr"] = 87;
  lzd["Fe"] = 26;
  lzd["Fm"] = 100;
  lzd["Ni"] = 28;
  lzd["No"] = 102;
  lzd["Na"] = 11;
  lzd["Nb"] = 41;
  lzd["Nd"] = 60;
  lzd["Ne"] = 10;
  lzd["Zr"] = 40;
  lzd["Np"] = 93;
  lzd["B"] = 05;
  lzd["Co"] = 27;
  lzd["Cm"] = 96;
  lzd["F"] = 9;
  lzd["Ca"] = 20;
  lzd["Cf"] = 98;
  lzd["Ce"] = 58;
  lzd["Cd"] = 48;
  lzd["V"] = 23;
  lzd["Cs"] = 55;
  lzd["Cr"] = 24;
  lzd["Cu"] = 29;
  lzd["Sr"] = 38;
  lzd["Kr"] = 36;
  lzd["Si"] = 14;
  lzd["Sn"] = 50;
  lzd["Sm"] = 62;
  lzd["Sc"] = 21;
  lzd["Sb"] = 51;
  lzd["Sg"] = 106;
  lzd["Se"] = 34;
  lzd["Yb"] = 70;
  lzd["Db"] = 105;
  lzd["Dy"] = 66;
  lzd["Ds"] = 110;
  lzd["La"] = 57;
  lzd["Cl"] = 17;
  lzd["Li"] = 03;
  lzd["Tl"] = 81;
  lzd["Lu"] = 71;
  lzd["Lr"] = 103;
  lzd["Th"] = 90;
  lzd["Ti"] = 22;
  lzd["Te"] = 52;
  lzd["Tb"] = 65;
  lzd["Tc"] = 43;
  lzd["Ta"] = 73;
  lzd["Ac"] = 89;
  lzd["Ag"] = 47;
  lzd["I"] = 53;
  lzd["Ir"] = 77;
  lzd["Am"] = 95;
  lzd["Al"] = 13;
  lzd["As"] = 33;
  lzd["Ar"] = 18;
  lzd["Au"] = 79;
  lzd["At"] = 85;
  lzd["In"] = 49;
  lzd["Y"] = 39;
  lzd["N"] = 07;
  lzd["Xe"] = 54;
  lzd["Cn"] = 112;
  lzd["Fl"] = 114;
  lzd["Lv"] = 116;

  return lzd;
};
pyne::nucname::name_zz_t pyne::nucname::name_zz = pyne::nucname::get_name_zz();


/*** Constructs zz to LL dictionary **/
pyne::nucname::zzname_t pyne::nucname::get_zz_name()
{
  zzname_t zld;
  for (name_zz_iter i = name_zz.begin(); i != name_zz.end(); i++)
  {
    zld[i->second] = i->first;
  }
  return zld;
};
pyne::nucname::zzname_t pyne::nucname::zz_name = pyne::nucname::get_zz_name();



/******************************************/
/*** Define useful elemental group sets ***/
/******************************************/

pyne::nucname::zz_group pyne::nucname::name_to_zz_group(pyne::nucname::name_group eg)
{
  zz_group zg;
  for (name_group_iter i = eg.begin(); i != eg.end(); i++)
    zg.insert(name_zz[*i]);
  return zg;
};

// Lanthanides
pyne::nucname::name_t pyne::nucname::LAN_array[15] = {"La", "Ce", "Pr", "Nd", 
  "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu"};
pyne::nucname::name_group pyne::nucname::LAN (pyne::nucname::LAN_array, 
                                              pyne::nucname::LAN_array+15);
pyne::nucname::zz_group pyne::nucname::lan = \
  pyne::nucname::name_to_zz_group(pyne::nucname::LAN);

// Actinides
pyne::nucname::name_t pyne::nucname::ACT_array[15] = {"Ac", "Th", "Pa", "U", 
  "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr"};
pyne::nucname::name_group pyne::nucname::ACT (pyne::nucname::ACT_array, pyne::nucname::ACT_array+15);
pyne::nucname::zz_group pyne::nucname::act = pyne::nucname::name_to_zz_group(pyne::nucname::ACT);

// Transuarnics
pyne::nucname::name_t pyne::nucname::TRU_array[22] = {"Np", "Pu", "Am", "Cm", 
  "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", 
  "Ds", "Rg", "Cn", "Fl", "Lv"};
pyne::nucname::name_group pyne::nucname::TRU (pyne::nucname::TRU_array, 
                                              pyne::nucname::TRU_array+22);
pyne::nucname::zz_group pyne::nucname::tru = \
  pyne::nucname::name_to_zz_group(pyne::nucname::TRU);

//Minor Actinides
pyne::nucname::name_t pyne::nucname::MA_array[10] = {"Np", "Am", "Cm", "Bk", 
  "Cf", "Es", "Fm", "Md", "No", "Lr"};
pyne::nucname::name_group pyne::nucname::MA (pyne::nucname::MA_array, 
                                             pyne::nucname::MA_array+10);
pyne::nucname::zz_group pyne::nucname::ma = \
  pyne::nucname::name_to_zz_group(pyne::nucname::MA);

//Fission Products
pyne::nucname::name_t pyne::nucname::FP_array[88] = {"Ag", "Al", "Ar", "As", 
  "At", "Au", "B",  "Ba", "Be", "Bi", "Br", "C",  "Ca", "Cd", "Ce", "Cl", "Co",
  "Cr", "Cs", "Cu", "Dy", "Er", "Eu", "F",  "Fe", "Fr", "Ga", "Gd", "Ge", "H",  
  "He", "Hf", "Hg", "Ho", "I",  "In", "Ir", "K",  "Kr", "La", "Li", "Lu", "Mg", 
  "Mn", "Mo", "N",  "Na", "Nb", "Nd", "Ne", "Ni", "O",  "Os", "P",  "Pb", "Pd", 
  "Pm", "Po", "Pr", "Pt", "Ra", "Rb", "Re", "Rh", "Rn", "Ru", "S",  "Sb", "Sc", 
  "Se", "Si", "Sm", "Sn", "Sr", "Ta", "Tb", "Tc", "Te", "Ti", "Tl", "Tm", "V",  
  "W",  "Xe", "Y",  "Yb", "Zn", "Zr"};
pyne::nucname::name_group pyne::nucname::FP (pyne::nucname::FP_array, 
                                             pyne::nucname::FP_array+88);
pyne::nucname::zz_group pyne::nucname::fp = \
  pyne::nucname::name_to_zz_group(pyne::nucname::FP);


/***************************/
/*** isnuclide functions ***/
/***************************/

bool pyne::nucname::isnuclide(std::string nuc) {
  int n;
  try {
    n = id(nuc);
  }
  catch(NotANuclide) {
    return false;
  }
  catch(IndeterminateNuclideForm) {
    return false;
  };
  return isnuclide(n);
};

bool pyne::nucname::isnuclide(char * nuc) {
  return isnuclide(std::string(nuc));
};

bool pyne::nucname::isnuclide(int nuc) {
  int n;
  try {
    n = id(nuc);
  }
  catch(NotANuclide) {
    return false;
  }
  catch(IndeterminateNuclideForm) {
    return false;
  };
  if (n <= 10000000)
    return false;
  int zzz = n / 10000000;
  int aaa = (n % 10000000) / 10000;
  if (aaa == 0)
    return false;  // is element
  else if (aaa < zzz)
    return false;
  return true;
};



/********************/
/*** id functions ***/
/********************/
int pyne::nucname::id(int nuc) {
  if (nuc < 0)
    throw NotANuclide(nuc, "");

  int newnuc;
  int zzz = nuc / 10000000;     // ZZZ ?
  int aaassss = nuc % 10000000; // AAA-SSSS ?
  int aaa = aaassss / 10000;    // AAA ?
  // Nuclide must already be in id form
  if (0 < zzz && zzz <= aaa && aaa <= zzz * 7) {
    // Normal nuclide
    return nuc;
  } else if (aaassss == 0 && 0 < zz_name.count(zzz)) {
    // Natural elemental nuclide:  ie for Urnaium = 920000000
    return nuc;
  } else if (nuc < 1000 && 0 < zz_name.count(nuc))
    //  Gave Z-number
    return nuc * 10000000;

  // Not in id form, try  ZZZAAAM form.
  zzz = nuc / 10000;     // ZZZ ?
  aaassss = nuc % 10000; // AAA-SSSS ?
  aaa = aaassss / 10;    // AAA ?
  if (zzz <= aaa && aaa <= zzz * 7) {
    // ZZZAAAM nuclide
    return (zzz*10000000) + (aaa*10000) + (nuc%10);
  } else if (aaa <= zzz && zzz <= aaa * 7 && 0 < zz_name.count(aaa)) {
    // Cinder-form (aaazzzm), ie 2350920
    return (aaa*10000000) + (zzz*10000) + (nuc%10);
  }
  //else if (aaassss == 0 && 0 == zz_name.count(nuc/1000) && 0 < zz_name.count(zzz))
  else if (aaassss == 0 && 0 < zz_name.count(zzz)) {
    // zzaaam form natural nuclide
    return zzz * 10000000;
  }

  if (nuc >= 1000000){
    // From now we assume no metastable info has been given.
    throw IndeterminateNuclideForm(nuc, "");
  };

  // Nuclide is not in zzaaam form, 
  // Try MCNP form, ie zzaaa
  // This is the same form as SZA for the 0th state.
  zzz = nuc / 1000;
  aaa = nuc % 1000; 
  if (zzz <= aaa) {
    if (aaa - 400 < 0) {
      if (nuc == 95242)
        return nuc * 10000 + 1;  // special case MCNP Am-242m
      else
        return nuc * 10000;  // Nuclide in normal MCNP form
    } else {
      // Nuclide in MCNP metastable form
      if (nuc == 95642)
        return (95642 - 400)*10000;  // special case MCNP Am-242
      nuc = ((nuc - 400) * 10000) + 1;
      while (3.0 < (float ((nuc/10000)%1000) / float (nuc/10000000)))
        nuc -= 999999;
      return nuc;
    }
  } else if (aaa == 0 && 0 < zz_name.count(zzz)) {
    // MCNP form natural nuclide
    return zzz * 10000000;
  } else if (zzz > 1000) {
    // SZA form with a metastable state (sss != 0)
    int sss = zzz / 1000;
    int newzzz = zzz % 1000;
    
    return newzzz * 10000000 + aaa * 10000 + sss;
  }

  // Not a normal nuclide, might be a 
  // Natural elemental nuclide.  
  // ie 92 for Urnaium = 920000
  if (0 < zz_name.count(nuc))
    return nuc * 10000000;
  throw IndeterminateNuclideForm(nuc, "");
};

int pyne::nucname::id(char * nuc) {
  std::string newnuc (nuc);
  return id(newnuc);
};

int pyne::nucname::id(std::string nuc) {
  if (nuc.empty())
    throw NotANuclide(nuc, "<empty>");
  int newnuc;
  std::string elem_name;
  
  if(nuc.length()>=5) { //nuc must be at least 4 characters or greater if it is in ZZLLAAAM form.
    if((pyne::contains_substring(nuc.substr(1, 3), "-")) && (pyne::contains_substring(nuc.substr(4, 5), "-")) ){
       // Nuclide most likely in ZZLLAAAM Form, only form that contains two "-"'s.
       int dashIndex = nuc.find("-"); 
       std::string zz = nuc.substr(0, dashIndex);
       std::string ll_aaa_m = nuc.substr(dashIndex+1);
       int dash2Index = ll_aaa_m.find("-");
       std::string ll = ll_aaa_m.substr(0, dash2Index);
       int zz_int;
       std::stringstream s_str(zz);
       s_str >> zz_int;
       if(znum(ll)==zz_int ) {    // Verifying that the LL and ZZ point to the same element as secondary
	  			  // verification that nuc is in ZZLLAAAM form.
         return zzllaaam_to_id(nuc);
       }
    }
  }

  // Get the string into a regular form
  std::string nucstr = pyne::to_upper(nuc);
  nucstr = pyne::remove_substring(nucstr, "-");
  int nuclen = nucstr.length();

  if (pyne::contains_substring(pyne::digits, nucstr.substr(0, 1))) {
    if (pyne::contains_substring(pyne::digits, nucstr.substr(nuclen-1, nuclen))) {
      // Nuclide must actually be an integer that 
      // just happens to be living in string form.
      newnuc = pyne::to_int(nucstr);
      newnuc = id(newnuc);
    } else {
      // probably in NIST-like form (242Am)
      // Here we know we have both digits and letters
      std::string anum_str = pyne::remove_characters(nucstr, pyne::alphabet);
      newnuc = pyne::to_int(anum_str) * 10000;

      // Add the Z-number
      elem_name = pyne::remove_characters(nucstr, pyne::digits);
      elem_name = pyne::capitalize(elem_name);
      if (0 < name_zz.count(elem_name))
        newnuc = (10000000 * name_zz[elem_name]) + newnuc;
      else
        throw NotANuclide(nucstr, newnuc);
    };
  } else if (pyne::contains_substring(pyne::alphabet, nucstr.substr(0, 1))) {
    // Nuclide is probably in name form, or some variation therein
    std::string anum_str = pyne::remove_characters(nucstr, pyne::alphabet);

    // natural element form, a la 'U' -> 920000000
    if (anum_str.empty()) {
      elem_name = pyne::capitalize(nucstr);    
      if (0 < name_zz.count(elem_name))
        return 10000000 * name_zz[elem_name]; 
    }

    int anum = pyne::to_int(anum_str);

    // bad form
    if (anum < 0)
      throw NotANuclide(nucstr, anum); 

    // Figure out if we are meta-stable or not
    std::string end_char = pyne::last_char(nucstr);
    if (end_char == "M")
      newnuc = (10000 * anum) + 1;
    else if (pyne::contains_substring(pyne::digits, end_char))
      newnuc = (10000 * anum);
    else
      throw NotANuclide(nucstr, newnuc);

    // Add the Z-number
    elem_name = pyne::remove_characters(nucstr.substr(0, nuclen-1), pyne::digits);
    elem_name = pyne::capitalize(elem_name);
    if (0 < name_zz.count(elem_name))
      newnuc = (10000000 * name_zz[elem_name]) + newnuc;
    else
      throw NotANuclide(nucstr, newnuc);
  } else {
    // Clearly not a nuclide
    throw NotANuclide(nuc, nucstr);
  }
  return newnuc;  
};



/**********************/
/*** name functions ***/
/**********************/
std::string pyne::nucname::name(int nuc) {
  int nucid = id(nuc);
  std::string newnuc = "";

  int zzz = nucid / 10000000;
  int ssss = nucid % 10000;
  int aaassss = nucid % 10000000;
  int aaa = aaassss / 10000;

  // Make sure the LL value is correct
  if (0 == zz_name.count(zzz))
    throw NotANuclide(nuc, nucid);

  // Add LL
  newnuc += zz_name[zzz];

  // Add A-number
  if (0 < aaa)
    newnuc += pyne::to_str(aaa);

  // Add meta-stable flag
  if (0 < ssss)
    newnuc += "M";

  return newnuc;
};



std::string pyne::nucname::name(char * nuc) {
  std::string newnuc (nuc);
  return name(newnuc);
}


std::string pyne::nucname::name(std::string nuc) {
  return name(id(nuc));
}


/**********************/
/*** znum functions ***/
/**********************/
int pyne::nucname::znum(int nuc) {
  return id(nuc) / 10000000;
};

int pyne::nucname::znum(char * nuc) {
  return id(nuc) / 10000000;
};

int pyne::nucname::znum(std::string nuc) {
  return id(nuc) / 10000000;
};

/**********************/
/*** anum functions ***/
/**********************/
int pyne::nucname::anum(int nuc) {
  return (id(nuc) / 10000) % 1000;
};

int pyne::nucname::anum(char * nuc) {
  return (id(nuc) / 10000) % 1000;
};

int pyne::nucname::anum(std::string nuc) {
  return (id(nuc) / 10000) % 1000;
};

/**********************/
/*** snum functions ***/
/**********************/
int pyne::nucname::snum(int nuc) {
  return id(nuc) % 10000;
};

int pyne::nucname::snum(char * nuc) {
  return id(nuc) % 10000;
};

int pyne::nucname::snum(std::string nuc) {
  return id(nuc) % 10000;
};

/************************/
/*** zzaaam functions ***/
/************************/
int pyne::nucname::zzaaam(int nuc) {
  int nucid = id(nuc);
  int zzzaaa = nucid / 10000;
  int ssss = nucid % 10000;
  if (10 <= ssss)
    ssss = 9;
  return zzzaaa*10 + ssss;
};


int pyne::nucname::zzaaam(char * nuc) {
  std::string newnuc (nuc);
  return zzaaam(newnuc);
};


int pyne::nucname::zzaaam(std::string nuc) {
  return zzaaam(id(nuc));
};


int pyne::nucname::zzaaam_to_id(int nuc) {
  return (nuc/10)*10000 + (nuc%10);
};


int pyne::nucname::zzaaam_to_id(char * nuc) {
  return zzaaam_to_id(std::string(nuc));
};


int pyne::nucname::zzaaam_to_id(std::string nuc) {
  return zzaaam_to_id(pyne::to_int(nuc));
};

/************************/
/*** zzzaaa functions ***/
/************************/
int pyne::nucname::zzzaaa(int nuc) {
  int nucid = id(nuc);
  int zzzaaa = nucid/10000;

  return zzzaaa;
};


int pyne::nucname::zzzaaa(char * nuc) {
  std::string newnuc (nuc);
  return zzzaaa(newnuc);
};


int pyne::nucname::zzzaaa(std::string nuc) {
  return zzzaaa(id(nuc));
};


int pyne::nucname::zzzaaa_to_id(int nuc) {
  return (nuc)*10000;
};


int pyne::nucname::zzzaaa_to_id(char * nuc) {
  return zzzaaa_to_id(std::string(nuc));
};


int pyne::nucname::zzzaaa_to_id(std::string nuc) {
  return zzzaaa_to_id(pyne::to_int(nuc));
};

/*************************/
/*** zzllaaam functions ***/
/*************************/
std::string pyne::nucname::zzllaaam(int nuc) {
  int nucid = id(nuc);
  std::string newnuc = "";

  int ssss = nucid % 10000;
  int aaassss = nucid % 10000000;
  int zzz = nucid / 10000000;
  int aaa = aaassss / 10000;

  // Make sure the LL value is correct
  if (0 == zz_name.count(zzz))
    throw NotANuclide(nuc, nucid);
  //Adding ZZ
  newnuc += pyne::to_str(zzz);
  newnuc += "-";
  // Add LL
  newnuc += zz_name[zzz];
  // Add required dash
  newnuc += "-";
  // Add AAA
  if (0 < aaassss)
    newnuc += pyne::to_str(aaa);
  // Add meta-stable flag
  if (0 < ssss)
    newnuc += "m";
  return newnuc;
};


std::string pyne::nucname::zzllaaam(char * nuc) {
  std::string newnuc (nuc);
  return zzllaaam(newnuc);
};


std::string pyne::nucname::zzllaaam(std::string nuc) {
  return zzllaaam(id(nuc));
};


int pyne::nucname::zzllaaam_to_id(char * nuc) {
  return zzllaaam_to_id(std::string(nuc));
};


int pyne::nucname::zzllaaam_to_id(std::string nuc) {
  if (nuc.empty())
    throw NotANuclide(nuc, "<empty>");
  int nucid;
  std::string elem_name;

  // Get the string into a regular form
  std::string nucstr = pyne::to_upper(nuc);
  // Removing first two characters (redundant), for 1 digit nuclides, such
  // as 2-He-4, the first slash will be removed, and the second attempt to
  // remove the second slash will do nothing.  
  nucstr.erase(0,2);
  nucstr = pyne::remove_substring(nucstr, "-");
  // Does nothing if nuclide is short, otherwise removes the second "-" instance
  nucstr = pyne::remove_substring(nucstr, "-");
  int nuclen = nucstr.length();

  // Nuclide is probably in name form, or some variation therein
  std::string anum_str = pyne::remove_characters(nucstr, pyne::alphabet);

  // natural element form, a la 'U' -> 920000000
  if (anum_str.empty() || pyne::contains_substring(nucstr, "NAT")) {
    elem_name = pyne::capitalize(pyne::remove_substring(nucstr, "NAT")); 
    if (0 < name_zz.count(elem_name))
      return 10000000 * name_zz[elem_name]; 
  }
  int anum = pyne::to_int(anum_str);

  // Figure out if we are meta-stable or not
  std::string end_char = pyne::last_char(nucstr);
  if (end_char == "M")
    nucid = (10000 * anum) + 1;
  else if (pyne::contains_substring(pyne::digits, end_char))
    nucid = (10000 * anum);
  else
    throw NotANuclide(nucstr, nucid);

  // Add the Z-number
  elem_name = pyne::remove_characters(nucstr.substr(0, nuclen-1), pyne::digits);
  elem_name = pyne::capitalize(elem_name);
  if (0 < name_zz.count(elem_name))
    nucid = (10000000 * name_zz[elem_name]) + nucid;
  else
    throw NotANuclide(nucstr, nucid);
  return nucid;
};

/**********************/
/*** mcnp functions ***/
/**********************/
int pyne::nucname::mcnp(int nuc) {
  nuc = id(nuc);
  int ssss = nuc % 10000;
  int newnuc = nuc / 10000;

  // special case Am242(m)
  if (newnuc == 95242 && ssss < 2)
    ssss = (ssss + 1) % 2;

  // Handle the crazy MCNP meta-stable format
  if (0 != ssss && ssss < 10) 
    newnuc += 300 + (ssss * 100);

  return newnuc;
};



int pyne::nucname::mcnp(char * nuc) {
  std::string newnuc (nuc);
  return mcnp(newnuc);
};



int pyne::nucname::mcnp(std::string nuc) {
  return mcnp(id(nuc));
};

//
// MCNP -> id
//
int pyne::nucname::mcnp_to_id(int nuc) {
  int zzz = nuc / 1000;
  int aaa = nuc % 1000; 
  if (zzz <= aaa) {
    if (aaa - 400 < 0) {
      if (nuc == 95242)
        return nuc * 10000 + 1;  // special case MCNP Am-242m
      else
        return nuc * 10000;  // Nuclide in normal MCNP form
    } else {
      // Nuclide in MCNP metastable form
      if (nuc == 95642)
        return (95642 - 400)*10000;  // special case MCNP Am-242
      nuc = ((nuc - 400) * 10000) + 1;
      while (3.0 < (float ((nuc/10000)%1000) / float (nuc/10000000)))
        nuc -= 999999;
      return nuc;
    }
  } else if (aaa == 0)
    // MCNP form natural nuclide
    return zzz * 10000000;
  throw IndeterminateNuclideForm(nuc, "");
};


int pyne::nucname::mcnp_to_id(char * nuc) {
  return mcnp_to_id(std::string(nuc));
};


int pyne::nucname::mcnp_to_id(std::string nuc) {
  return mcnp_to_id(pyne::to_int(nuc));
};


/*************************/
/*** serpent functions ***/
/*************************/
std::string pyne::nucname::serpent(int nuc) {
  int nucid = id(nuc);
  std::string newnuc = "";

  int ssss = nucid % 10000;
  int aaassss = nucid % 10000000;
  int zzz = nucid / 10000000;
  int aaa = aaassss / 10000;

  // Make sure the LL value is correct
  if (0 == zz_name.count(zzz))
    throw NotANuclide(nuc, nucid);

  // Add LL
  std::string llupper = pyne::to_upper(zz_name[zzz]);
  std::string lllower = pyne::to_lower(zz_name[zzz]);
  newnuc += llupper[0];
  for (int l = 1; l < lllower.size(); l++)
    newnuc += lllower[l];  

  // Add required dash
  newnuc += "-";

  // Add A-number
  if (0 < aaassss)
    newnuc += pyne::to_str(aaa);
  else if (0 == aaassss)
    newnuc += "nat";

  // Add meta-stable flag
  if (0 < ssss)
    newnuc += "m";

  return newnuc;
};


std::string pyne::nucname::serpent(char * nuc) {
  std::string newnuc (nuc);
  return serpent(newnuc);
};


std::string pyne::nucname::serpent(std::string nuc) {
  return serpent(id(nuc));
};

//
// Serpent -> id
//
//int pyne::nucname::serpent_to_id(int nuc)
//{
// Should be ZAID
//};


int pyne::nucname::serpent_to_id(char * nuc) {
  return serpent_to_id(std::string(nuc));
};


int pyne::nucname::serpent_to_id(std::string nuc) {
  if (nuc.empty())
    throw NotANuclide(nuc, "<empty>");
  int nucid;
  std::string elem_name;

  // Get the string into a regular form
  std::string nucstr = pyne::to_upper(nuc);
  nucstr = pyne::remove_substring(nucstr, "-");
  int nuclen = nucstr.length();

  // Nuclide is probably in name form, or some variation therein
  std::string anum_str = pyne::remove_characters(nucstr, pyne::alphabet);

  // natural element form, a la 'U' -> 920000000
  if (anum_str.empty() || pyne::contains_substring(nucstr, "NAT")) {
    elem_name = pyne::capitalize(pyne::remove_substring(nucstr, "NAT")); 
    if (0 < name_zz.count(elem_name))
      return 10000000 * name_zz[elem_name]; 
  }
  int anum = pyne::to_int(anum_str);

  // Figure out if we are meta-stable or not
  std::string end_char = pyne::last_char(nucstr);
  if (end_char == "M")
    nucid = (10000 * anum) + 1;
  else if (pyne::contains_substring(pyne::digits, end_char))
    nucid = (10000 * anum);
  else
    throw NotANuclide(nucstr, nucid);

  // Add the Z-number
  elem_name = pyne::remove_characters(nucstr.substr(0, nuclen-1), pyne::digits);
  elem_name = pyne::capitalize(elem_name);
  if (0 < name_zz.count(elem_name))
    nucid = (10000000 * name_zz[elem_name]) + nucid;
  else
    throw NotANuclide(nucstr, nucid);
  return nucid;
};


/**********************/
/*** nist functions ***/
/**********************/
std::string pyne::nucname::nist(int nuc) {
  int nucid = id(nuc);
  std::string newnuc = "";

  int zzz = nucid / 10000000;
  int ssss = nucid % 10000;
  int aaassss = nucid % 10000000;
  int aaa = aaassss / 10000;

  // Make sure the LL value is correct
  if (0 == zz_name.count(zzz))
    throw NotANuclide(nuc, nucid);

  // Add A-number
  if (0 < aaassss)
    newnuc += pyne::to_str(aaa);

  // Add name
  std::string name_upper = pyne::to_upper(zz_name[zzz]);
  std::string name_lower = pyne::to_lower(zz_name[zzz]);
  newnuc += name_upper[0];
  for (int l = 1; l < name_lower.size(); l++)
    newnuc += name_lower[l];  

  // Add meta-stable flag
  // No metastable flag for NIST, 
  // but could add star, by uncommenting below
  //if (0 < mod_10)
  //  newnuc += "*";

  return newnuc;
};


std::string pyne::nucname::nist(char * nuc) {
  std::string newnuc (nuc);
  return nist(newnuc);
};


std::string pyne::nucname::nist(std::string nuc) {
  return nist(id(nuc));
};


//
// NIST -> id
//
//int pyne::nucname::nist_to_id(int nuc)
//{
// NON-EXISTANT
//};

int pyne::nucname::nist_to_id(char * nuc) {
  return nist_to_id(std::string(nuc));
};

int pyne::nucname::nist_to_id(std::string nuc) {
  if (nuc.empty())
    throw NotANuclide(nuc, "<empty>");
  int nucid;
  nuc = pyne::to_upper(nuc);
  std::string elem_name;
  int nuclen = nuc.length();

  // Nuclide is probably in name form, or some variation therein
  std::string anum_str = pyne::remove_characters(nuc, pyne::alphabet);

  // natural element form, a la 'U' -> 920000000
  if (anum_str.empty()) {
    elem_name = pyne::capitalize(nuc);
    if (0 < name_zz.count(elem_name))
      return 10000000 * name_zz[elem_name]; 
  }
  nucid = pyne::to_int(anum_str) * 10000;

  // Add the Z-number
  elem_name = pyne::remove_characters(nuc, pyne::digits);
  elem_name = pyne::capitalize(elem_name);
  if (0 < name_zz.count(elem_name))
    nucid = (10000000 * name_zz[elem_name]) + nucid;
  else
    throw NotANuclide(nuc, nucid);
  return nucid;
};


/************************/
/*** cinder functions ***/
/************************/
int pyne::nucname::cinder(int nuc) {
  // cinder nuclides of form aaazzzm
  int nucid = id(nuc);
  int zzz = nucid / 10000000;
  int ssss = nucid % 10000;
  int aaassss = nucid % 10000000;
  int aaa = aaassss / 10000;
  if (10 <= ssss)
    ssss = 9;
  return (aaa*10000) + (zzz*10) + ssss;
};



int pyne::nucname::cinder(char * nuc) {
  std::string newnuc (nuc);
  return cinder(newnuc);
};



int pyne::nucname::cinder(std::string nuc) {
  return cinder(id(nuc));
};

//
// Cinder -> Id
//
int pyne::nucname::cinder_to_id(int nuc) {
  int ssss = nuc % 10;
  int aaazzz = nuc / 10;
  int zzz = aaazzz % 1000;
  int aaa = aaazzz / 1000;
  return (zzz * 10000000) + (aaa * 10000) + ssss;
};


int pyne::nucname::cinder_to_id(char * nuc) {
  return cinder_to_id(std::string(nuc));
};


int pyne::nucname::cinder_to_id(std::string nuc) {
  return cinder_to_id(pyne::to_int(nuc));
};




/**********************/
/*** ALARA functions ***/
/**********************/
std::string pyne::nucname::alara(int nuc) {
  int nucid = id(nuc);
  std::string newnuc = "";
  std::string ll = "";

  int zzz = nucid / 10000000;
  int ssss = nucid % 10000;
  int aaassss = nucid % 10000000;
  int aaa = aaassss / 10000;

  // Make sure the LL value is correct
  if (0 == zz_name.count(zzz))
    throw NotANuclide(nuc, nucid);

  // Add LL, in lower case
  ll += zz_name[zzz];

  for(int i = 0; ll[i] != '\0'; i++)
    ll[i] = tolower(ll[i]);
  newnuc += ll;

  // Add A-number
  if (0 < aaassss){
    newnuc += ":";
    newnuc += pyne::to_str(aaa);
  }

  // Note, ALARA input format does not use metastable flag
  return newnuc;
};


std::string pyne::nucname::alara(char * nuc) {
  std::string newnuc (nuc);
  return alara(newnuc);
}


std::string pyne::nucname::alara(std::string nuc) {
  return alara(id(nuc));
}


//
// Cinder -> Id
//
//int pyne::nucname::alara_to_id(int nuc)
//{
// Not Possible
//};


int pyne::nucname::alara_to_id(char * nuc) {
  return alara_to_id(std::string(nuc));
};


int pyne::nucname::alara_to_id(std::string nuc) {
  if (nuc.empty())
    throw NotANuclide(nuc, "<empty>");
  int nucid;
  nuc = pyne::to_upper(pyne::remove_characters(nuc, ":"));
  std::string elem_name;
  int nuclen = nuc.length();

  // Nuclide is probably in name form, or some variation therein
  std::string anum_str = pyne::remove_characters(nuc, pyne::alphabet);

  // natural element form, a la 'U' -> 920000000
  if (anum_str.empty()) {
    elem_name = pyne::capitalize(nuc);
    if (0 < name_zz.count(elem_name))
      return 10000000 * name_zz[elem_name]; 
  }
  nucid = pyne::to_int(anum_str) * 10000;

  // Add the Z-number
  elem_name = pyne::remove_characters(nuc, pyne::digits);
  elem_name = pyne::capitalize(elem_name);
  if (0 < name_zz.count(elem_name))
    nucid = (10000000 * name_zz[elem_name]) + nucid;
  else
    throw NotANuclide(nuc, nucid);
  return nucid;
};




/***********************/
/***  SZA functions  ***/
/***********************/
int pyne::nucname::sza(int nuc) {
  int nucid = id(nuc);
  int zzzaaa = nucid / 10000;
  int sss = nucid % 10000;
  return sss * 1000000 + zzzaaa;
}


int pyne::nucname::sza(char * nuc) {
  std::string newnuc (nuc);
  return sza(newnuc);
}


int pyne::nucname::sza(std::string nuc) {
  return sza(id(nuc));
}


int pyne::nucname::sza_to_id(int nuc) {
  int sss = nuc / 1000000;
  int zzzaaa = nuc % 1000000;
  return zzzaaa * 10000 + sss;
}


int pyne::nucname::sza_to_id(char * nuc) {
  std::string newnuc (nuc);
  return sza_to_id(newnuc);
}


int pyne::nucname::sza_to_id(std::string nuc) {
  return sza_to_id(pyne::to_int(nuc));
}



/*******************************/
/***  Groundstate functions  ***/
/*******************************/
int pyne::nucname::groundstate(int nuc) {
  int nucid = id(nuc);
  int nostate = (nucid / 10000 ) * 10000;
  return nostate;
}


int pyne::nucname::groundstate(char * nuc) {
  std::string newnuc (nuc);
  return groundstate(newnuc);
}


int pyne::nucname::groundstate(std::string nuc) {
  return groundstate(id(nuc));
}


void pyne::nucname::_load_state_map(){
    for (int i = 0; i < TOTAL_STATE_MAPS; ++i) {
       state_id_map[map_nuc_ids[i]] = map_metastable[i];
    }
}

int pyne::nucname::state_id_to_id(int state) {
    int zzzaaa = (state / 10000) * 10000;
    
    std::map<int, int>::iterator nuc_iter, nuc_end;

    nuc_iter = state_id_map.find(state);
    nuc_end = state_id_map.end();
    if (nuc_iter != nuc_end){ 
     int m = (*nuc_iter).second;
     return zzzaaa + m;
    }        

    if (state_id_map.empty())  {
      _load_state_map();
      return state_id_to_id(state);
    }
    throw IndeterminateNuclideForm(state, "no matching metastable state");
}


int pyne::nucname::id_to_state_id(int nuc_id) {
    int zzzaaa = (nuc_id / 10000) * 10000;
    int state = nuc_id % 10000;
    
    std::map<int, int>::iterator nuc_iter, nuc_end, it;
    
    nuc_iter = state_id_map.lower_bound(nuc_id);
    nuc_end = state_id_map.upper_bound(nuc_id + 10000);
    for (it = nuc_iter; it!= nuc_end; ++it){
        if (state == it->second) {
          return it->first;
        }
    }
    int m = (*nuc_iter).second;
    
    if (state_id_map.empty())  {
      _load_state_map();
      return id_to_state_id(nuc_id);
    }
    throw IndeterminateNuclideForm(state, "no matching state id");
}//
// end of cpp/nucname.cpp
//


//
// start of cpp/rxname.cpp
//
#ifndef PYNE_IS_AMALGAMATED
#include "rxname.h"
#endif

std::string pyne::rxname::_names[NUM_RX_NAMES] = {
  "total",
  "scattering",
  "elastic",
  "nonelastic",
  "n",
  "misc",
  "continuum",
  "z_2nd",
  "z_2n",
  "z_2n_0",
  "z_2n_1",
  "z_2n_2",
  "z_3n",
  "z_3n_0",
  "z_3n_1",
  "z_3n_2",
  "fission",
  "fission_first",
  "fission_second",
  "fission_third",
  "na",
  "na_0",
  "na_1",
  "na_2",
  "n3a",
  "z_2na",
  "z_3na",
  "absorption",
  "np",
  "np_0",
  "np_1",
  "np_2",
  "npd",
  "n2a",
  "z_2n2a",
  "nd",
  "nd_0",
  "nd_1",
  "nd_2",
  "nt",
  "nt_0",
  "nt_1",
  "nt_2",
  "nHe3",
  "nHe3_0",
  "nHe3_1",
  "nHe3_2",
  "nd3a",
  "nt2a",
  "z_4n",
  "z_4n_0",
  "z_4n_1",
  "fission_fourth",
  "z_2np",
  "z_3np",
  "n2p",
  "npa",
  "n_0",
  "n_1",
  "n_2",
  "n_3",
  "n_4",
  "n_5",
  "n_6",
  "n_7",
  "n_8",
  "n_9",
  "n_10",
  "n_11",
  "n_12",
  "n_13",
  "n_14",
  "n_15",
  "n_16",
  "n_17",
  "n_18",
  "n_19",
  "n_20",
  "n_21",
  "n_22",
  "n_23",
  "n_24",
  "n_25",
  "n_26",
  "n_27",
  "n_28",
  "n_29",
  "n_30",
  "n_31",
  "n_32",
  "n_33",
  "n_34",
  "n_35",
  "n_36",
  "n_37",
  "n_38",
  "n_39",
  "n_40",
  "n_continuum",
  "disappearance",
  "gamma",
  "gamma_0",
  "gamma_1",
  "gamma_2",
  "p",
  "d",
  "t",
  "He3",
  "a",
  "z_2a",
  "z_3a",
  "z_2p",
  "z_2p_0",
  "z_2p_1",
  "z_2p_2",
  "pa",
  "t2a",
  "d2a",
  "pd",
  "pt",
  "da",
  "resonance_parameters",
  "n_total",
  "gamma_total",
  "p_total",
  "d_total",
  "t_total",
  "He3_total",
  "a_total",
  "pionp",
  "pion0",
  "pionm",
  "muonp",
  "muonm",
  "kaonp",
  "kaon0_long",
  "kaon0_short",
  "kaonm",
  "antip",
  "antin",
  "mubar",
  "epsilon",
  "y",
  "erel_total",
  "erel_elastic",
  "erel_nonelastic",
  "erel_n",
  "erel_misc",
  "erel_continuum",
  "erel_2nd",
  "erel_2n",
  "erel_3n",
  "erel_fission",
  "erel_fission_first",
  "erel_fission_second",
  "erel_fission_third",
  "erel_na",
  "erel_n3a",
  "erel_2na",
  "erel_3na",
  "erel_absorption",
  "erel_np",
  "erel_n2a",
  "erel_2n2a",
  "erel_nd",
  "erel_nt",
  "erel_nHe3",
  "erel_nd3a",
  "erel_nt2a",
  "erel_4n",
  "erel_fission_fourth",
  "erel_2np",
  "erel_3np",
  "erel_n2p",
  "erel_npa",
  "erel_n_0",
  "erel_n_1",
  "erel_n_2",
  "erel_n_3",
  "erel_n_4",
  "erel_n_5",
  "erel_n_6",
  "erel_n_7",
  "erel_n_8",
  "erel_n_9",
  "erel_n_10",
  "erel_n_11",
  "erel_n_12",
  "erel_n_13",
  "erel_n_14",
  "erel_n_15",
  "erel_n_16",
  "erel_n_17",
  "erel_n_18",
  "erel_n_19",
  "erel_n_20",
  "erel_n_21",
  "erel_n_22",
  "erel_n_23",
  "erel_n_24",
  "erel_n_25",
  "erel_n_26",
  "erel_n_27",
  "erel_n_28",
  "erel_n_29",
  "erel_n_30",
  "erel_n_31",
  "erel_n_32",
  "erel_n_33",
  "erel_n_34",
  "erel_n_35",
  "erel_n_36",
  "erel_n_37",
  "erel_n_38",
  "erel_n_39",
  "erel_n_40",
  "erel_n_continuum",
  "erel_disappearance",
  "erel_gamma",
  "erel_p",
  "erel_d",
  "erel_t",
  "erel_He3",
  "erel_a",
  "erel_2a",
  "erel_3a",
  "erel_2p",
  "erel_pa",
  "erel_t2a",
  "erel_d2a",
  "erel_pd",
  "erel_pt",
  "erel_da",
  "damage",
  "heading",
  "nubar",
  "fission_product_yield_independent",
  "nubar_delayed",
  "nubar_prompt",
  "decay",
  "energy_per_fission",
  "fission_product_yield_cumulative",
  "gamma_delayed",
  "stopping_power",
  "photon_total",
  "photon_coherent",
  "photon_incoherent",
  "scattering_factor_imag",
  "scattering_factor_real",
  "pair_prod_elec",
  "pair_prod",
  "pair_prod_nuc",
  "absorption_photoelectric",
  "photoexcitation",
  "scattering_electroatomic",
  "bremsstrahlung",
  "excitation_electroatomic",
  "atomic_relaxation",
  "k_photoelectric",
  "l1_photoelectric",
  "l2_photoelectric",
  "l3_photoelectric",
  "m1_photoelectric",
  "m2_photoelectric",
  "m3_photoelectric",
  "m4_photoelectric",
  "m5_photoelectric",
  "n1_photoelectric",
  "n2_photoelectric",
  "n3_photoelectric",
  "n4_photoelectric",
  "n5_photoelectric",
  "n6_photoelectric",
  "n7_photoelectric",
  "o1_photoelectric",
  "o2_photoelectric",
  "o3_photoelectric",
  "o4_photoelectric",
  "o5_photoelectric",
  "o6_photoelectric",
  "o7_photoelectric",
  "o8_photoelectric",
  "o9_photoelectric",
  "p1_photoelectric",
  "p2_photoelectric",
  "p3_photoelectric",
  "p4_photoelectric",
  "p5_photoelectric",
  "p6_photoelectric",
  "p7_photoelectric",
  "p8_photoelectric",
  "p9_photoelectric",
  "p10_photoelectric",
  "p11_photoelectric",
  "q1_photoelectric",
  "q2_photoelectric",
  "q3_photoelectric",
  "p_0",
  "p_1",
  "p_2",
  "p_3",
  "p_4",
  "p_5",
  "p_6",
  "p_7",
  "p_8",
  "p_9",
  "p_10",
  "p_11",
  "p_12",
  "p_13",
  "p_14",
  "p_15",
  "p_16",
  "p_17",
  "p_18",
  "p_19",
  "p_20",
  "p_21",
  "p_22",
  "p_23",
  "p_24",
  "p_25",
  "p_26",
  "p_27",
  "p_28",
  "p_29",
  "p_30",
  "p_31",
  "p_32",
  "p_33",
  "p_34",
  "p_35",
  "p_36",
  "p_37",
  "p_38",
  "p_39",
  "p_40",
  "p_41",
  "p_42",
  "p_43",
  "p_44",
  "p_45",
  "p_46",
  "p_47",
  "p_48",
  "p_continuum",
  "d_0",
  "d_1",
  "d_2",
  "d_3",
  "d_4",
  "d_5",
  "d_6",
  "d_7",
  "d_8",
  "d_9",
  "d_10",
  "d_11",
  "d_12",
  "d_13",
  "d_14",
  "d_15",
  "d_16",
  "d_17",
  "d_18",
  "d_19",
  "d_20",
  "d_21",
  "d_22",
  "d_23",
  "d_24",
  "d_25",
  "d_26",
  "d_27",
  "d_28",
  "d_29",
  "d_30",
  "d_31",
  "d_32",
  "d_33",
  "d_34",
  "d_35",
  "d_36",
  "d_37",
  "d_38",
  "d_39",
  "d_40",
  "d_41",
  "d_42",
  "d_43",
  "d_44",
  "d_45",
  "d_46",
  "d_47",
  "d_48",
  "d_continuum",
  "t_0",
  "t_1",
  "t_2",
  "t_3",
  "t_4",
  "t_5",
  "t_6",
  "t_7",
  "t_8",
  "t_9",
  "t_10",
  "t_11",
  "t_12",
  "t_13",
  "t_14",
  "t_15",
  "t_16",
  "t_17",
  "t_18",
  "t_19",
  "t_20",
  "t_21",
  "t_22",
  "t_23",
  "t_24",
  "t_25",
  "t_26",
  "t_27",
  "t_28",
  "t_29",
  "t_30",
  "t_31",
  "t_32",
  "t_33",
  "t_34",
  "t_35",
  "t_36",
  "t_37",
  "t_38",
  "t_39",
  "t_40",
  "t_41",
  "t_42",
  "t_43",
  "t_44",
  "t_45",
  "t_46",
  "t_47",
  "t_48",
  "t_continuum",
  "He3_0",
  "He3_1",
  "He3_2",
  "He3_3",
  "He3_4",
  "He3_5",
  "He3_6",
  "He3_7",
  "He3_8",
  "He3_9",
  "He3_10",
  "He3_11",
  "He3_12",
  "He3_13",
  "He3_14",
  "He3_15",
  "He3_16",
  "He3_17",
  "He3_18",
  "He3_19",
  "He3_20",
  "He3_21",
  "He3_22",
  "He3_23",
  "He3_24",
  "He3_25",
  "He3_26",
  "He3_27",
  "He3_28",
  "He3_29",
  "He3_30",
  "He3_31",
  "He3_32",
  "He3_33",
  "He3_34",
  "He3_35",
  "He3_36",
  "He3_37",
  "He3_38",
  "He3_39",
  "He3_40",
  "He3_41",
  "He3_42",
  "He3_43",
  "He3_44",
  "He3_45",
  "He3_46",
  "He3_47",
  "He3_48",
  "He3_continuum",
  "a_0",
  "a_1",
  "a_2",
  "a_3",
  "a_4",
  "a_5",
  "a_6",
  "a_7",
  "a_8",
  "a_9",
  "a_10",
  "a_11",
  "a_12",
  "a_13",
  "a_14",
  "a_15",
  "a_16",
  "a_17",
  "a_18",
  "a_19",
  "a_20",
  "a_21",
  "a_22",
  "a_23",
  "a_24",
  "a_25",
  "a_26",
  "a_27",
  "a_28",
  "a_29",
  "a_30",
  "a_31",
  "a_32",
  "a_33",
  "a_34",
  "a_35",
  "a_36",
  "a_37",
  "a_38",
  "a_39",
  "a_40",
  "a_41",
  "a_42",
  "a_43",
  "a_44",
  "a_45",
  "a_46",
  "a_47",
  "a_48",
  "a_continuum",
  "lumped_covar",
  "excited",
  "bminus",
  "bplus",
  "ec",
  "bminus_n",
  "bminus_a",
  "it",
  "bplus_a",
  "ec_bplus",
  "bplus_p",
  "bminus_2n",
  "bminus_3n",
  "bminus_4n",
  "ecp",
  "eca",
  "bplus_2p",
  "ec_2p",
  "decay_2bminus",
  "bminus_p",
  "decay_14c",
  "bplus_3p",
  "sf",
  "decay_2bplus",
  "decay_2ec",
  };
std::set<std::string> pyne::rxname::names(pyne::rxname::_names, 
                                          pyne::rxname::_names+NUM_RX_NAMES);


std::map<std::string, unsigned int> pyne::rxname::altnames;
std::map<unsigned int, std::string> pyne::rxname::id_name;
std::map<std::string, unsigned int> pyne::rxname::name_id;
std::map<unsigned int, unsigned int> pyne::rxname::id_mt;
std::map<unsigned int, unsigned int> pyne::rxname::mt_id;
std::map<unsigned int, std::string> pyne::rxname::labels;
std::map<unsigned int, std::string> pyne::rxname::docs;
std::map<std::pair<std::string, int>, unsigned int> pyne::rxname::offset_id;
std::map<std::pair<std::string, unsigned int>, int> pyne::rxname::id_offset;

void * pyne::rxname::_fill_maps() {
  using std::make_pair;
  std::string rx;
  unsigned int rxid;
  unsigned int _mts [NUM_RX_NAMES] = {
    1,
    0,
    2,
    3,
    4,
    5,
    10,
    11,
    16,
    0,
    0,
    0,
    17,
    0,
    0,
    0,
    18,
    19,
    20,
    21,
    22,
    0,
    0,
    0,
    23,
    24,
    25,
    27,
    28,
    0,
    0,
    0,
    0,
    29,
    30,
    32,
    0,
    0,
    0,
    33,
    0,
    0,
    0,
    34,
    0,
    0,
    0,
    35,
    36,
    37,
    0,
    0,
    38,
    41,
    42,
    44,
    45,
    50,
    51,
    52,
    53,
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    62,
    63,
    64,
    65,
    66,
    67,
    68,
    69,
    70,
    71,
    72,
    73,
    74,
    75,
    76,
    77,
    78,
    79,
    80,
    81,
    82,
    83,
    84,
    85,
    86,
    87,
    88,
    89,
    90,
    91,
    101,
    102,
    0,
    0,
    0,
    103,
    104,
    105,
    106,
    107,
    108,
    109,
    111,
    0,
    0,
    0,
    112,
    113,
    114,
    115,
    116,
    117,
    151,
    201,
    202,
    203,
    204,
    205,
    206,
    207,
    208,
    209,
    210,
    211,
    212,
    213,
    214,
    215,
    216,
    217,
    218,
    251,
    252,
    253,
    301,
    302,
    303,
    304,
    305,
    310,
    311,
    316,
    317,
    318,
    319,
    320,
    321,
    322,
    323,
    324,
    325,
    327,
    328,
    329,
    330,
    332,
    333,
    334,
    335,
    336,
    337,
    338,
    341,
    342,
    344,
    345,
    350,
    351,
    352,
    353,
    354,
    355,
    356,
    357,
    358,
    359,
    360,
    361,
    362,
    363,
    364,
    365,
    366,
    367,
    368,
    369,
    370,
    371,
    372,
    373,
    374,
    375,
    376,
    377,
    378,
    379,
    380,
    381,
    382,
    383,
    384,
    385,
    386,
    387,
    388,
    389,
    390,
    391,
    401,
    402,
    403,
    404,
    405,
    406,
    407,
    408,
    409,
    411,
    412,
    413,
    414,
    415,
    416,
    417,
    444,
    451,
    452,
    454,
    455,
    456,
    457,
    458,
    459,
    460,
    500,
    501,
    502,
    504,
    505,
    506,
    515,
    516,
    517,
    522,
    523,
    526,
    527,
    528,
    533,
    534,
    535,
    536,
    537,
    538,
    539,
    540,
    541,
    542,
    543,
    544,
    545,
    546,
    547,
    548,
    549,
    550,
    551,
    552,
    553,
    554,
    555,
    556,
    557,
    558,
    559,
    560,
    561,
    562,
    563,
    564,
    565,
    566,
    567,
    568,
    569,
    570,
    571,
    572,
    600,
    601,
    602,
    603,
    604,
    605,
    606,
    607,
    608,
    609,
    610,
    611,
    612,
    613,
    614,
    615,
    616,
    617,
    618,
    619,
    620,
    621,
    622,
    623,
    624,
    625,
    626,
    627,
    628,
    629,
    630,
    631,
    632,
    633,
    634,
    635,
    636,
    637,
    638,
    639,
    640,
    641,
    642,
    643,
    644,
    645,
    646,
    647,
    648,
    649,
    650,
    651,
    652,
    653,
    654,
    655,
    656,
    657,
    658,
    659,
    660,
    661,
    662,
    663,
    664,
    665,
    666,
    667,
    668,
    669,
    670,
    671,
    672,
    673,
    674,
    675,
    676,
    677,
    678,
    679,
    680,
    681,
    682,
    683,
    684,
    685,
    686,
    687,
    688,
    689,
    690,
    691,
    692,
    693,
    694,
    695,
    696,
    697,
    698,
    699,
    700,
    701,
    702,
    703,
    704,
    705,
    706,
    707,
    708,
    709,
    710,
    711,
    712,
    713,
    714,
    715,
    716,
    717,
    718,
    719,
    720,
    721,
    722,
    723,
    724,
    725,
    726,
    727,
    728,
    729,
    730,
    731,
    732,
    733,
    734,
    735,
    736,
    737,
    738,
    739,
    740,
    741,
    742,
    743,
    744,
    745,
    746,
    747,
    748,
    749,
    750,
    751,
    752,
    753,
    754,
    755,
    756,
    757,
    758,
    759,
    760,
    761,
    762,
    763,
    764,
    765,
    766,
    767,
    768,
    769,
    770,
    771,
    772,
    773,
    774,
    775,
    776,
    777,
    778,
    779,
    780,
    781,
    782,
    783,
    784,
    785,
    786,
    787,
    788,
    789,
    790,
    791,
    792,
    793,
    794,
    795,
    796,
    797,
    798,
    799,
    800,
    801,
    802,
    803,
    804,
    805,
    806,
    807,
    808,
    809,
    810,
    811,
    812,
    813,
    814,
    815,
    816,
    817,
    818,
    819,
    820,
    821,
    822,
    823,
    824,
    825,
    826,
    827,
    828,
    829,
    830,
    831,
    832,
    833,
    834,
    835,
    836,
    837,
    838,
    839,
    840,
    841,
    842,
    843,
    844,
    845,
    846,
    847,
    848,
    849,
    851,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  };
  std::string _labels[NUM_RX_NAMES] = {
    "(z,total)",
    "(z,scattering)",
    "(z,elastic)",
    "(z,nonelastic)",
    "(z,n)",
    "(misc)",
    "(z,continuum)",
    "(z,2nd)",
    "(z,2n)",
    "(z,2n0)",
    "(z,2n1)",
    "(z,2n2)",
    "(z,3n)",
    "(z,3n0)",
    "(z,3n1)",
    "(z,3n2)",
    "(z,fission)",
    "(z,f)",
    "(z,nf)",
    "(z,2nf)",
    "(z,n+a)",
    "(z,n+a0)",
    "(z,n+a1)",
    "(z,n+a2)",
    "(z,n+3a)",
    "(z,2n+a)",
    "(z,3n+a)",
    "(z,abs) Absorption",
    "(z,n+p)",
    "(z,n+p0)",
    "(z,n+p1)",
    "(z,n+p2)",
    "(z,n+p+d)",
    "(z,n+2a)",
    "(z,2n+2a)",
    "(z,nd)",
    "(z,nd0)",
    "(z,nd1)",
    "(z,nd2)",
    "(z,nt)",
    "(z,nt0)",
    "(z,nt1)",
    "(z,nt2)",
    "(z,n+He3)",
    "(z,n+He3-0)",
    "(z,n+He3-1)",
    "(z,n+He3-2)",
    "(z,n+d+3a)",
    "(z,n+t+2a)",
    "(z,4n)",
    "(z,4n0)",
    "(z,4n1)",
    "(z,3nf)",
    "(z,2n+p)",
    "(z,3n+p)",
    "(z,n+2p)",
    "(z,npa)",
    "(z,n0)",
    "(z,n1)",
    "(z,n2)",
    "(z,n3)",
    "(z,n4)",
    "(z,n5)",
    "(z,n6)",
    "(z,n7)",
    "(z,n8)",
    "(z,n9)",
    "(z,n10)",
    "(z,n11)",
    "(z,n12)",
    "(z,n13)",
    "(z,n14)",
    "(z,n15)",
    "(z,n16)",
    "(z,n17)",
    "(z,n18)",
    "(z,n19)",
    "(z,n20)",
    "(z,n21)",
    "(z,n22)",
    "(z,n23)",
    "(z,n24)",
    "(z,n25)",
    "(z,n26)",
    "(z,n27)",
    "(z,n28)",
    "(z,n29)",
    "(z,n30)",
    "(z,n31)",
    "(z,n32)",
    "(z,n33)",
    "(z,n34)",
    "(z,n35)",
    "(z,n36)",
    "(z,n37)",
    "(z,n38)",
    "(z,n39)",
    "(z,n40)",
    "(z,nc)",
    "(z,disap) Neutron disappearance",
    "(z,gamma)",
    "(z,gamma0)",
    "(z,gamma1)",
    "(z,gamma2)",
    "(z,p)",
    "(z,d)",
    "(z,t)",
    "(z,3He)",
    "(z,a)",
    "(z,2a)",
    "(z,3a)",
    "(z,2p)",
    "(z,2p0)",
    "(z,2p1)",
    "(z,2p2)",
    "(z,pa)",
    "(z,t2a)",
    "(z,d2a)",
    "(z,pd)",
    "(z,pt)",
    "(z,da)",
    "Resonance Parameters",
    "(z,Xn)",
    "(z,Xgamma)",
    "(z,Xp)",
    "(z,Xd)",
    "(z,Xt)",
    "(z,X3He)",
    "(z,Xa)",
    "(z,Xpi+) Total pi+ meson production",
    "(z,Xpi0) Total pi0 meson production",
    "(z,Xpi-) Total pi- meson production",
    "(z,Xmu+) Total anti-muon production",
    "(z,Xmu-) Total muon production",
    "(z,Xk+) Total positive kaon production",
    "(z,Xk0long) Total long-lived neutral kaon production",
    "(z,Xk0short) Total short-lived neutral kaon production",
    "(z,Xk-) Total negative kaon production",
    "(z,Xp-) Total anti-proton production",
    "(z,Xn-) Total anti-neutron production",
    "Average cosine of scattering angle",
    "Average logarithmic energy decrement",
    "Average xi^2/(2*xi)",
    "Energy Release from (z,total)",
    "Energy Release from (z,elastic)",
    "Energy Release from (z,nonelastic)",
    "Energy Release from (z,inelastic)",
    "Energy Release from (misc)",
    "Energy Release from (z,continuum)",
    "Energy Release from (z,2nd)",
    "Energy Release from (z,2n)",
    "Energy Release from (z,3n)",
    "Energy Release from (z,fission)",
    "Energy Release from (z,f)",
    "Energy Release from (z,nf)",
    "Energy Release from (z,2nf)",
    "Energy Release from (z,n+a)",
    "Energy Release from (z,n+3a)",
    "Energy Release from (z,2n+a)",
    "Energy Release from (z,3n+a)",
    "Energy Release from (z,abs) Absorption",
    "Energy Release from (z,n+p)",
    "Energy Release from (z,n+2a)",
    "Energy Release from (z,2n+2a)",
    "Energy Release from (z,nd)",
    "Energy Release from (z,nt)",
    "Energy Release from (z,n+He3)",
    "Energy Release from (z,n+d+3a)",
    "Energy Release from (z,n+t+2a)",
    "Energy Release from (z,4n)",
    "Energy Release from (z,3nf)",
    "Energy Release from (z,2n+p)",
    "Energy Release from (z,3n+p)",
    "Energy Release from (z,n+2p)",
    "Energy Release from (z,npa)",
    "Energy Release from (z,n0)",
    "Energy Release from (z,n1)",
    "Energy Release from (z,n2)",
    "Energy Release from (z,n3)",
    "Energy Release from (z,n4)",
    "Energy Release from (z,n5)",
    "Energy Release from (z,n6)",
    "Energy Release from (z,n7)",
    "Energy Release from (z,n8)",
    "Energy Release from (z,n9)",
    "Energy Release from (z,n10)",
    "Energy Release from (z,n11)",
    "Energy Release from (z,n12)",
    "Energy Release from (z,n13)",
    "Energy Release from (z,n14)",
    "Energy Release from (z,n15)",
    "Energy Release from (z,n16)",
    "Energy Release from (z,n17)",
    "Energy Release from (z,n18)",
    "Energy Release from (z,n19)",
    "Energy Release from (z,n20)",
    "Energy Release from (z,n21)",
    "Energy Release from (z,n22)",
    "Energy Release from (z,n23)",
    "Energy Release from (z,n24)",
    "Energy Release from (z,n25)",
    "Energy Release from (z,n26)",
    "Energy Release from (z,n27)",
    "Energy Release from (z,n28)",
    "Energy Release from (z,n29)",
    "Energy Release from (z,n30)",
    "Energy Release from (z,n31)",
    "Energy Release from (z,n32)",
    "Energy Release from (z,n33)",
    "Energy Release from (z,n34)",
    "Energy Release from (z,n35)",
    "Energy Release from (z,n36)",
    "Energy Release from (z,n37)",
    "Energy Release from (z,n38)",
    "Energy Release from (z,n39)",
    "Energy Release from (z,n40)",
    "Energy Release from (z,nc)",
    "Energy Release from (z,disap) Neutron disappearance",
    "Energy Release from (z,gamma)",
    "Energy Release from (z,p)",
    "Energy Release from (z,d)",
    "Energy Release from (z,t)",
    "Energy Release from (z,3He)",
    "Energy Release from (z,a)",
    "Energy Release from (z,2a)",
    "Energy Release from (z,3a)",
    "Energy Release from (z,2p)",
    "Energy Release from (z,pa)",
    "Energy Release from (z,t2a)",
    "Energy Release from (z,d2a)",
    "Energy Release from (z,pd)",
    "Energy Release from (z,pt)",
    "Energy Release from (z,da)",
    "(damage)",
    "Descriptive Data",
    "Total Neutrons per Fission",
    "Independent fission product yield",
    "Delayed Neutron Data",
    "Prompt Neutrons per Fission",
    "Radioactive Decay Data",
    "Energy Release Due to Fission",
    "Cumulative Fission Product Yield",
    "Delayed Photon Data",
    "Total charged-particle stopping power",
    "Total photon interaction",
    "Photon coherent scattering",
    "Photon incoherent scattering",
    "Imaginary scattering factor",
    "Real scattering factor",
    "Pair production, electron field",
    "Total pair production",
    "Pair production, nuclear field",
    "Photoelectric absorption",
    "Photo-excitation cross section",
    "Electro-atomic scattering",
    "Electro-atomic bremsstrahlung",
    "Electro-atomic excitation cross section",
    "Atomic relaxation data",
    "K (1s1/2) subshell",
    "L1 (2s1/2) subshell",
    "L2 (2p1/2) subshell",
    "L3 (2p3/2) subshell",
    "M1 (3s1/2) subshell",
    "M2 (3p1/2) subshell",
    "M3 (3p3/2) subshell",
    "M4 (3d1/2) subshell",
    "M5 (3d1/2) subshell",
    "N1 (4s1/2) subshell",
    "N2 (4p1/2) subshell",
    "N3 (4p3/2) subshell",
    "N4 (4d3/2) subshell",
    "N5 (4d5/2) subshell",
    "N6 (4f5/2) subshell",
    "N7 (4f7/2) subshell",
    "O1 (5s1/2) subshell",
    "O2 (5p1/2) subshell",
    "O3 (5p3/2) subshell",
    "O4 (5d3/2) subshell",
    "O5 (5d5/2) subshell",
    "O6 (5f5/2) subshell",
    "O7 (5f7/2) subshell",
    "O8 (5g7/2) subshell",
    "O9 (5g9/2) subshell",
    "P1 (6s1/2) subshell",
    "P2 (6p1/2) subshell",
    "P3 (6p3/2) subshell",
    "P4 (6d3/2) subshell",
    "P5 (6d5/2) subshell",
    "P6 (6f5/2) subshell",
    "P7 (6f7/2) subshell",
    "P8 (6g7/2) subshell",
    "P9 (6g9/2) subshell",
    "P10 (6h9/2) subshell",
    "P11 (6h11/2) subshell",
    "Q1 (7s1/2) subshell",
    "Q2 (7p1/2) subshell",
    "Q3 (7p3/2) subshell",
    "(z,p0)",
    "(z,p1)",
    "(z,p2)",
    "(z,p3)",
    "(z,p4)",
    "(z,p5)",
    "(z,p6)",
    "(z,p7)",
    "(z,p8)",
    "(z,p9)",
    "(z,p10)",
    "(z,p11)",
    "(z,p12)",
    "(z,p13)",
    "(z,p14)",
    "(z,p15)",
    "(z,p16)",
    "(z,p17)",
    "(z,p18)",
    "(z,p19)",
    "(z,p20)",
    "(z,p21)",
    "(z,p22)",
    "(z,p23)",
    "(z,p24)",
    "(z,p25)",
    "(z,p26)",
    "(z,p27)",
    "(z,p28)",
    "(z,p29)",
    "(z,p30)",
    "(z,p31)",
    "(z,p32)",
    "(z,p33)",
    "(z,p34)",
    "(z,p35)",
    "(z,p36)",
    "(z,p37)",
    "(z,p38)",
    "(z,p39)",
    "(z,p40)",
    "(z,p41)",
    "(z,p42)",
    "(z,p43)",
    "(z,p44)",
    "(z,p45)",
    "(z,p46)",
    "(z,p47)",
    "(z,p48)",
    "(z,pc)",
    "(z,d0)",
    "(z,d1)",
    "(z,d2)",
    "(z,d3)",
    "(z,d4)",
    "(z,d5)",
    "(z,d6)",
    "(z,d7)",
    "(z,d8)",
    "(z,d9)",
    "(z,d10)",
    "(z,d11)",
    "(z,d12)",
    "(z,d13)",
    "(z,d14)",
    "(z,d15)",
    "(z,d16)",
    "(z,d17)",
    "(z,d18)",
    "(z,d19)",
    "(z,d20)",
    "(z,d21)",
    "(z,d22)",
    "(z,d23)",
    "(z,d24)",
    "(z,d25)",
    "(z,d26)",
    "(z,d27)",
    "(z,d28)",
    "(z,d29)",
    "(z,d30)",
    "(z,d31)",
    "(z,d32)",
    "(z,d33)",
    "(z,d34)",
    "(z,d35)",
    "(z,d36)",
    "(z,d37)",
    "(z,d38)",
    "(z,d39)",
    "(z,d40)",
    "(z,d41)",
    "(z,d42)",
    "(z,d43)",
    "(z,d44)",
    "(z,d45)",
    "(z,d46)",
    "(z,d47)",
    "(z,d48)",
    "(z,dc)",
    "(z,t0)",
    "(z,t1)",
    "(z,t2)",
    "(z,t3)",
    "(z,t4)",
    "(z,t5)",
    "(z,t6)",
    "(z,t7)",
    "(z,t8)",
    "(z,t9)",
    "(z,t10)",
    "(z,t11)",
    "(z,t12)",
    "(z,t13)",
    "(z,t14)",
    "(z,t15)",
    "(z,t16)",
    "(z,t17)",
    "(z,t18)",
    "(z,t19)",
    "(z,t20)",
    "(z,t21)",
    "(z,t22)",
    "(z,t23)",
    "(z,t24)",
    "(z,t25)",
    "(z,t26)",
    "(z,t27)",
    "(z,t28)",
    "(z,t29)",
    "(z,t30)",
    "(z,t31)",
    "(z,t32)",
    "(z,t33)",
    "(z,t34)",
    "(z,t35)",
    "(z,t36)",
    "(z,t37)",
    "(z,t38)",
    "(z,t39)",
    "(z,t40)",
    "(z,t41)",
    "(z,t42)",
    "(z,t43)",
    "(z,t44)",
    "(z,t45)",
    "(z,t46)",
    "(z,t47)",
    "(z,t48)",
    "(z,tc)",
    "(z,3He0)",
    "(z,3He1)",
    "(z,3He2)",
    "(z,3He3)",
    "(z,3He4)",
    "(z,3He5)",
    "(z,3He6)",
    "(z,3He7)",
    "(z,3He8)",
    "(z,3He9)",
    "(z,3He10)",
    "(z,3He11)",
    "(z,3He12)",
    "(z,3He13)",
    "(z,3He14)",
    "(z,3He15)",
    "(z,3He16)",
    "(z,3He17)",
    "(z,3He18)",
    "(z,3He19)",
    "(z,3He20)",
    "(z,3He21)",
    "(z,3He22)",
    "(z,3He23)",
    "(z,3He24)",
    "(z,3He25)",
    "(z,3He26)",
    "(z,3He27)",
    "(z,3He28)",
    "(z,3He29)",
    "(z,3He30)",
    "(z,3He31)",
    "(z,3He32)",
    "(z,3He33)",
    "(z,3He34)",
    "(z,3He35)",
    "(z,3He36)",
    "(z,3He37)",
    "(z,3He38)",
    "(z,3He39)",
    "(z,3He40)",
    "(z,3He41)",
    "(z,3He42)",
    "(z,3He43)",
    "(z,3He44)",
    "(z,3He45)",
    "(z,3He46)",
    "(z,3He47)",
    "(z,3He48)",
    "(z,3Hec)",
    "(z,a0)",
    "(z,a1)",
    "(z,a2)",
    "(z,a3)",
    "(z,a4)",
    "(z,a5)",
    "(z,a6)",
    "(z,a7)",
    "(z,a8)",
    "(z,a9)",
    "(z,a10)",
    "(z,a11)",
    "(z,a12)",
    "(z,a13)",
    "(z,a14)",
    "(z,a15)",
    "(z,a16)",
    "(z,a17)",
    "(z,a18)",
    "(z,a19)",
    "(z,a20)",
    "(z,a21)",
    "(z,a22)",
    "(z,a23)",
    "(z,a24)",
    "(z,a25)",
    "(z,a26)",
    "(z,a27)",
    "(z,a28)",
    "(z,a29)",
    "(z,a30)",
    "(z,a31)",
    "(z,a32)",
    "(z,a33)",
    "(z,a34)",
    "(z,a35)",
    "(z,a36)",
    "(z,a37)",
    "(z,a38)",
    "(z,a39)",
    "(z,a40)",
    "(z,a41)",
    "(z,a42)",
    "(z,a43)",
    "(z,a44)",
    "(z,a45)",
    "(z,a46)",
    "(z,a47)",
    "(z,a48)",
    "(z,ac)",
    "Lumped Covariances",
    "Any Excited State",
    "(z,b-)",
    "(z,b+)",
    "(z,ec)",
    "(z,b-n)",
    "(z,b-a)",
    "(z,it)",
    "(z,b+a)",
    "(z,ec+b+)",
    "(z,b+p)",
    "(z,b-2n)",
    "(z,b-3n)",
    "(z,b-4n)",
    "(z,ecp)",
    "(z,eca)",
    "(z,b+2p)",
    "(z,ec2p)",
    "(z,2b-)",
    "(z,b-p)",
    "(z,14c)",
    "(z,b+3p)",
    "(z,sf)",
    "(z,2b+)",
    "(z,2ec)",
  };
  std::string _docs[NUM_RX_NAMES] = {
    "(n,total) Neutron total",
    "Total scattering",
    "(z,z0) Elastic scattering",
    "(z,nonelas) Nonelastic neutron",
    "(z,n) One neutron in exit channel",
    "(z,anything) Miscellaneous",
    "(z,contin) Total continuum reaction",
    "(z,2nd) Production of 2n and d",
    "(z,2n) Production of 2n",
    "(z,2n0) Production of 2n, ground state",
    "(z,2n1) Production of 2n, 1st excited state",
    "(z,2n2) Production of 2n, 2nd excited state",
    "(z,3n) Production of 3n",
    "(z,3n0) Production of 3n, ground state",
    "(z,3n1) Production of 3n, 1st excited state",
    "(z,3n2) Production of 3n, 2nd excited state",
    "(z,fiss) Particle-induced fission",
    "(z,f) First-chance fission",
    "(z,nf) Second chance fission",
    "(z,2nf) Third-chance fission",
    "(z,na) Production of n and alpha",
    "(z,na0) Production of n and alpha, ground state",
    "(z,na1) Production of n and alpha, 1st excited state",
    "(z,na2) Production of n and alpha, 2nd excited state",
    "(z,n3a) Production of n and 3 alphas",
    "(z,2na) Production of 2n and alpha",
    "(z,3na) Production of 3n and alpha",
    "(n,abs) Absorption",
    "(z,np) Production of n and p",
    "(z,np0) Production of n and p, ground state",
    "(z,np1) Production of n and p, 1st excited state",
    "(z,np2) Production of n and p, 2nd excited state",
    "(z,npd) Production of n, p, and d",
    "(z,n2a) Production of n and 2 alphas",
    "(z,2n2a) Production of 2n and 2 alphas",
    "(z,nd) Production of n and d",
    "(z,nd0) Production of n and d, ground state",
    "(z,nd1) Production of n and d, 1st excited state",
    "(z,nd2) Production of n and d, 2nd excited state",
    "(z,nt) Production of n and t",
    "(z,nt0) Production of n and t, ground state",
    "(z,nt1) Production of n and t, 1st excited state",
    "(z,nt2) Production of n and t, 2nd excited state",
    "(z,n3He) Production of n and He-3",
    "(z,n3He-0) Production of n and He-3, ground state",
    "(z,n3He-1) Production of n and He-3, 1st excited state",
    "(z,n3He-2) Production of n and He-3, 2nd excited state",
    "(z,nd2a) Production of n, d, and alpha",
    "(z,nt2a) Production of n, t, and 2 alphas",
    "(z,4n) Production of 4n",
    "(z,4n0) Production of 4n, ground state",
    "(z,4n1) Production of 4n, 1st excited state",
    "(z,3nf) Fourth-chance fission",
    "(z,2np) Production of 2n and p",
    "(z,3np) Production of 3n and p",
    "(z,n2p) Production of n and 2p",
    "(z,npa) Production of n, p, and alpha",
    "(z,n0) Production of n, ground state",
    "(z,n1) Production of n, 1st excited state",
    "(z,n2) Production of n, 2nd excited state",
    "(z,n3) Production of n, 3rd excited state",
    "(z,n4) Production of n, 4th excited state",
    "(z,n5) Production of n, 5th excited state",
    "(z,n6) Production of n, 6th excited state",
    "(z,n7) Production of n, 7th excited state",
    "(z,n8) Production of n, 8th excited state",
    "(z,n9) Production of n, 9th excited state",
    "(z,n10) Production of n, 10th excited state",
    "(z,n11) Production of n, 11th excited state",
    "(z,n12) Production of n, 12th excited state",
    "(z,n13) Production of n, 13th excited state",
    "(z,n14) Production of n, 14th excited state",
    "(z,n15) Production of n, 15th excited state",
    "(z,n16) Production of n, 16th excited state",
    "(z,n17) Production of n, 17th excited state",
    "(z,n18) Production of n, 18th excited state",
    "(z,n19) Production of n, 19th excited state",
    "(z,n20) Production of n, 20th excited state",
    "(z,n21) Production of n, 21st excited state",
    "(z,n22) Production of n, 22nd excited state",
    "(z,n23) Production of n, 23rd excited state",
    "(z,n24) Production of n, 24th excited state",
    "(z,n25) Production of n, 25th excited state",
    "(z,n26) Production of n, 26th excited state",
    "(z,n27) Production of n, 27th excited state",
    "(z,n28) Production of n, 28th excited state",
    "(z,n29) Production of n, 29th excited state",
    "(z,n30) Production of n, 30th excited state",
    "(z,n31) Production of n, 31st excited state",
    "(z,n32) Production of n, 32nd excited state",
    "(z,n33) Production of n, 33rd excited state",
    "(z,n34) Production of n, 34th excited state",
    "(z,n35) Production of n, 35th excited state",
    "(z,n36) Production of n, 36th excited state",
    "(z,n37) Production of n, 37th excited state",
    "(z,n38) Production of n, 38th excited state",
    "(z,n39) Production of n, 39th excited state",
    "(z,n40) Production of n, 40th excited state",
    "(z,nc) Production of n in continuum",
    "(n,disap) Neutron disappearance",
    "(z,gamma) Radiative capture",
    "(z,gamma0) Radiative capture, ground state",
    "(z,gamma1) Radiative capture, 1st excited state",
    "(z,gamma2) Radiative capture, 2st excited state",
    "(z,p) Production of p",
    "(z,d) Production of d",
    "(z,t) Production of t",
    "(z,3He) Production of He-3",
    "(z,a) Production of alpha",
    "(z,2a) Production of 2 alphas",
    "(z,3a) Production of 3 alphas",
    "(z,2p) Production of 2p",
    "(z,2p0) Production of 2p, ground state",
    "(z,2p1) Production of 2p, 1st excited state",
    "(z,2p2) Production of 2p, 2nd excited state",
    "(z,pa) Production of p and alpha",
    "(z,t2a) Production of t and 2 alphas",
    "(z,d2a) Production of d and 2 alphas",
    "(z,pd) Production of p and d",
    "(z,pt) Production of p and t",
    "(z,da) Production of d and a",
    "Resonance Parameters",
    "(z,Xn) Total neutron production",
    "(z,Xgamma) Total gamma production",
    "(z,Xp) Total proton production",
    "(z,Xd) Total deuteron production",
    "(z,Xt) Total triton production",
    "(z,X3He) Total He-3 production",
    "(z,Xa) Total alpha production",
    "(z,Xpi+) Total pi+ meson production",
    "(z,Xpi0) Total pi0 meson production",
    "(z,Xpi-) Total pi- meson production",
    "(z,Xmu+) Total anti-muon production",
    "(z,Xmu-) Total muon production",
    "(z,Xk+) Total positive kaon production",
    "(z,Xk0long) Total long-lived neutral kaon production",
    "(z,Xk0short) Total short-lived neutral kaon production",
    "(z,Xk-) Total negative kaon production",
    "(z,Xp-) Total anti-proton production",
    "(z,Xn-) Total anti-neutron production",
    "Average cosine of scattering angle",
    "Average logarithmic energy decrement",
    "Average xi^2/(2*xi)",
    "Energy Release from (n,total) Neutron total",
    "Energy Release from (z,z0) Elastic scattering",
    "Energy Release from (z,nonelas) Nonelastic neutron",
    "Energy Release from (z,n) One neutron in exit channel",
    "Energy Release from (z,anything) Miscellaneous",
    "Energy Release from (z,contin) Total continuum reaction",
    "Energy Release from (z,2nd) Production of 2n and d",
    "Energy Release from (z,2n) Production of 2n",
    "Energy Release from (z,3n) Production of 3n",
    "Energy Release from (z,fiss) Particle-induced fission",
    "Energy Release from (z,f) First-chance fission",
    "Energy Release from (z,nf) Second chance fission",
    "Energy Release from (z,2nf) Third-chance fission",
    "Energy Release from (z,na) Production of n and alpha",
    "Energy Release from (z,n3a) Production of n and 3 alphas",
    "Energy Release from (z,2na) Production of 2n and alpha",
    "Energy Release from (z,3na) Production of 3n and alpha",
    "Energy Release from (n,abs) Absorption",
    "Energy Release from (z,np) Production of n and p",
    "Energy Release from (z,n2a) Production of n and 2 alphas",
    "Energy Release from (z,2n2a) Production of 2n and 2 alphas",
    "Energy Release from (z,nd) Production of n and d",
    "Energy Release from (z,nt) Production of n and t",
    "Energy Release from (z,n3He) Production of n and He-3",
    "Energy Release from (z,nd2a) Production of n, d, and alpha",
    "Energy Release from (z,nt2a) Production of n, t, and 2 alphas",
    "Energy Release from (z,4n) Production of 4n",
    "Energy Release from (z,3nf) Fourth-chance fission",
    "Energy Release from (z,2np) Production of 2n and p",
    "Energy Release from (z,3np) Production of 3n and p",
    "Energy Release from (z,n2p) Production of n and 2p",
    "Energy Release from (z,npa) Production of n, p, and alpha",
    "Energy Release from (z,n0) Production of n, ground state",
    "Energy Release from (z,n1) Production of n, 1st excited state",
    "Energy Release from (z,n2) Production of n, 2nd excited state",
    "Energy Release from (z,n3) Production of n, 3rd excited state",
    "Energy Release from (z,n4) Production of n, 4th excited state",
    "Energy Release from (z,n5) Production of n, 5th excited state",
    "Energy Release from (z,n6) Production of n, 6th excited state",
    "Energy Release from (z,n7) Production of n, 7th excited state",
    "Energy Release from (z,n8) Production of n, 8th excited state",
    "Energy Release from (z,n9) Production of n, 9th excited state",
    "Energy Release from (z,n10) Production of n, 10th excited state",
    "Energy Release from (z,n11) Production of n, 11th excited state",
    "Energy Release from (z,n12) Production of n, 12th excited state",
    "Energy Release from (z,n13) Production of n, 13th excited state",
    "Energy Release from (z,n14) Production of n, 14th excited state",
    "Energy Release from (z,n15) Production of n, 15th excited state",
    "Energy Release from (z,n16) Production of n, 16th excited state",
    "Energy Release from (z,n17) Production of n, 17th excited state",
    "Energy Release from (z,n18) Production of n, 18th excited state",
    "Energy Release from (z,n19) Production of n, 19th excited state",
    "Energy Release from (z,n20) Production of n, 20th excited state",
    "Energy Release from (z,n21) Production of n, 21st excited state",
    "Energy Release from (z,n22) Production of n, 22nd excited state",
    "Energy Release from (z,n23) Production of n, 23rd excited state",
    "Energy Release from (z,n24) Production of n, 24th excited state",
    "Energy Release from (z,n25) Production of n, 25th excited state",
    "Energy Release from (z,n26) Production of n, 26th excited state",
    "Energy Release from (z,n27) Production of n, 27th excited state",
    "Energy Release from (z,n28) Production of n, 28th excited state",
    "Energy Release from (z,n29) Production of n, 29th excited state",
    "Energy Release from (z,n30) Production of n, 30th excited state",
    "Energy Release from (z,n31) Production of n, 31st excited state",
    "Energy Release from (z,n32) Production of n, 32nd excited state",
    "Energy Release from (z,n33) Production of n, 33rd excited state",
    "Energy Release from (z,n34) Production of n, 34th excited state",
    "Energy Release from (z,n35) Production of n, 35th excited state",
    "Energy Release from (z,n36) Production of n, 36th excited state",
    "Energy Release from (z,n37) Production of n, 37th excited state",
    "Energy Release from (z,n38) Production of n, 38th excited state",
    "Energy Release from (z,n39) Production of n, 39th excited state",
    "Energy Release from (z,n40) Production of n, 40th excited state",
    "Energy Release from (z,nc) Production of n in continuum",
    "Energy Release from (n,disap) Neutron disappearance",
    "Energy Release from (z,gamma) Radiative capture",
    "Energy Release from (z,p) Production of p",
    "Energy Release from (z,d) Production of d",
    "Energy Release from (z,t) Production of t",
    "Energy Release from (z,3He) Production of He-3",
    "Energy Release from (z,a) Production of alpha",
    "Energy Release from (z,2a) Production of 2 alphas",
    "Energy Release from (z,3a) Production of 3 alphas",
    "Energy Release from (z,2p) Production of 2p",
    "Energy Release from (z,pa) Production of p and alpha",
    "Energy Release from (z,t2a) Production of t and 2 alphas",
    "Energy Release from (z,d2a) Production of d and 2 alphas",
    "Energy Release from (z,pd) Production of p and d",
    "Energy Release from (z,pt) Production of p and t",
    "Energy Release from (z,da) Production of d and a",
    "(damage)",
    "Descriptive Data",
    "Total Neutrons per Fission",
    "Independent fission product yield",
    "Delayed Neutron Data",
    "Prompt Neutrons per Fission",
    "Radioactive Decay Data",
    "Energy Release Due to Fission",
    "Cumulative Fission Product Yield",
    "Delayed Photon Data",
    "Total charged-particle stopping power",
    "Total photon interaction",
    "Photon coherent scattering",
    "Photon incoherent scattering",
    "Imaginary scattering factor",
    "Real scattering factor",
    "Pair production, electron field",
    "Total pair production",
    "Pair production, nuclear field",
    "Photoelectric absorption",
    "Photo-excitation cross section",
    "Electro-atomic scattering",
    "Electro-atomic bremsstrahlung",
    "Electro-atomic excitation cross section",
    "Atomic relaxation data",
    "K (1s1/2) subshell",
    "L1 (2s1/2) subshell",
    "L2 (2p1/2) subshell",
    "L3 (2p3/2) subshell",
    "M1 (3s1/2) subshell",
    "M2 (3p1/2) subshell",
    "M3 (3p3/2) subshell",
    "M4 (3d1/2) subshell",
    "M5 (3d1/2) subshell",
    "N1 (4s1/2) subshell",
    "N2 (4p1/2) subshell",
    "N3 (4p3/2) subshell",
    "N4 (4d3/2) subshell",
    "N5 (4d5/2) subshell",
    "N6 (4f5/2) subshell",
    "N7 (4f7/2) subshell",
    "O1 (5s1/2) subshell",
    "O2 (5p1/2) subshell",
    "O3 (5p3/2) subshell",
    "O4 (5d3/2) subshell",
    "O5 (5d5/2) subshell",
    "O6 (5f5/2) subshell",
    "O7 (5f7/2) subshell",
    "O8 (5g7/2) subshell",
    "O9 (5g9/2) subshell",
    "P1 (6s1/2) subshell",
    "P2 (6p1/2) subshell",
    "P3 (6p3/2) subshell",
    "P4 (6d3/2) subshell",
    "P5 (6d5/2) subshell",
    "P6 (6f5/2) subshell",
    "P7 (6f7/2) subshell",
    "P8 (6g7/2) subshell",
    "P9 (6g9/2) subshell",
    "P10 (6h9/2) subshell",
    "P11 (6h11/2) subshell",
    "Q1 (7s1/2) subshell",
    "Q2 (7p1/2) subshell",
    "Q3 (7p3/2) subshell",
    "(n,p0)",
    "(n,p1)",
    "(n,p2)",
    "(n,p3)",
    "(n,p4)",
    "(n,p5)",
    "(n,p6)",
    "(n,p7)",
    "(n,p8)",
    "(n,p9)",
    "(n,p10)",
    "(n,p11)",
    "(n,p12)",
    "(n,p13)",
    "(n,p14)",
    "(n,p15)",
    "(n,p16)",
    "(n,p17)",
    "(n,p18)",
    "(n,p19)",
    "(n,p20)",
    "(n,p21)",
    "(n,p22)",
    "(n,p23)",
    "(n,p24)",
    "(n,p25)",
    "(n,p26)",
    "(n,p27)",
    "(n,p28)",
    "(n,p29)",
    "(n,p30)",
    "(n,p31)",
    "(n,p32)",
    "(n,p33)",
    "(n,p34)",
    "(n,p35)",
    "(n,p36)",
    "(n,p37)",
    "(n,p38)",
    "(n,p39)",
    "(n,p40)",
    "(n,p41)",
    "(n,p42)",
    "(n,p43)",
    "(n,p44)",
    "(n,p45)",
    "(n,p46)",
    "(n,p47)",
    "(n,p48)",
    "(n,pc)",
    "(n,d0)",
    "(n,d1)",
    "(n,d2)",
    "(n,d3)",
    "(n,d4)",
    "(n,d5)",
    "(n,d6)",
    "(n,d7)",
    "(n,d8)",
    "(n,d9)",
    "(n,d10)",
    "(n,d11)",
    "(n,d12)",
    "(n,d13)",
    "(n,d14)",
    "(n,d15)",
    "(n,d16)",
    "(n,d17)",
    "(n,d18)",
    "(n,d19)",
    "(n,d20)",
    "(n,d21)",
    "(n,d22)",
    "(n,d23)",
    "(n,d24)",
    "(n,d25)",
    "(n,d26)",
    "(n,d27)",
    "(n,d28)",
    "(n,d29)",
    "(n,d30)",
    "(n,d31)",
    "(n,d32)",
    "(n,d33)",
    "(n,d34)",
    "(n,d35)",
    "(n,d36)",
    "(n,d37)",
    "(n,d38)",
    "(n,d39)",
    "(n,d40)",
    "(n,d41)",
    "(n,d42)",
    "(n,d43)",
    "(n,d44)",
    "(n,d45)",
    "(n,d46)",
    "(n,d47)",
    "(n,d48)",
    "(n,dc)",
    "(z,t0)",
    "(z,t1)",
    "(z,t2)",
    "(z,t3)",
    "(z,t4)",
    "(z,t5)",
    "(z,t6)",
    "(z,t7)",
    "(z,t8)",
    "(z,t9)",
    "(z,t10)",
    "(z,t11)",
    "(z,t12)",
    "(z,t13)",
    "(z,t14)",
    "(z,t15)",
    "(z,t16)",
    "(z,t17)",
    "(z,t18)",
    "(z,t19)",
    "(z,t20)",
    "(z,t21)",
    "(z,t22)",
    "(z,t23)",
    "(z,t24)",
    "(z,t25)",
    "(z,t26)",
    "(z,t27)",
    "(z,t28)",
    "(z,t29)",
    "(z,t30)",
    "(z,t31)",
    "(z,t32)",
    "(z,t33)",
    "(z,t34)",
    "(z,t35)",
    "(z,t36)",
    "(z,t37)",
    "(z,t38)",
    "(z,t39)",
    "(z,t40)",
    "(z,t41)",
    "(z,t42)",
    "(z,t43)",
    "(z,t44)",
    "(z,t45)",
    "(z,t46)",
    "(z,t47)",
    "(z,t48)",
    "(n,tc)",
    "(n,3He0)",
    "(n,3He1)",
    "(n,3He2)",
    "(n,3He3)",
    "(n,3He4)",
    "(n,3He5)",
    "(n,3He6)",
    "(n,3He7)",
    "(n,3He8)",
    "(n,3He9)",
    "(n,3He10)",
    "(n,3He11)",
    "(n,3He12)",
    "(n,3He13)",
    "(n,3He14)",
    "(n,3He15)",
    "(n,3He16)",
    "(n,3He17)",
    "(n,3He18)",
    "(n,3He19)",
    "(n,3He20)",
    "(n,3He21)",
    "(n,3He22)",
    "(n,3He23)",
    "(n,3He24)",
    "(n,3He25)",
    "(n,3He26)",
    "(n,3He27)",
    "(n,3He28)",
    "(n,3He29)",
    "(n,3He30)",
    "(n,3He31)",
    "(n,3He32)",
    "(n,3He33)",
    "(n,3He34)",
    "(n,3He35)",
    "(n,3He36)",
    "(n,3He37)",
    "(n,3He38)",
    "(n,3He39)",
    "(n,3He40)",
    "(n,3He41)",
    "(n,3He42)",
    "(n,3He43)",
    "(n,3He44)",
    "(n,3He45)",
    "(n,3He46)",
    "(n,3He47)",
    "(n,3He48)",
    "(n,3Hec)",
    "(z,a0)",
    "(z,a1)",
    "(z,a2)",
    "(z,a3)",
    "(z,a4)",
    "(z,a5)",
    "(z,a6)",
    "(z,a7)",
    "(z,a8)",
    "(z,a9)",
    "(z,a10)",
    "(z,a11)",
    "(z,a12)",
    "(z,a13)",
    "(z,a14)",
    "(z,a15)",
    "(z,a16)",
    "(z,a17)",
    "(z,a18)",
    "(z,a19)",
    "(z,a20)",
    "(z,a21)",
    "(z,a22)",
    "(z,a23)",
    "(z,a24)",
    "(z,a25)",
    "(z,a26)",
    "(z,a27)",
    "(z,a28)",
    "(z,a29)",
    "(z,a30)",
    "(z,a31)",
    "(z,a32)",
    "(z,a33)",
    "(z,a34)",
    "(z,a35)",
    "(z,a36)",
    "(z,a37)",
    "(z,a38)",
    "(z,a39)",
    "(z,a40)",
    "(z,a41)",
    "(z,a42)",
    "(z,a43)",
    "(z,a44)",
    "(z,a45)",
    "(z,a46)",
    "(z,a47)",
    "(z,a48)",
    "(n,ac)",
    "Lumped-Reaction Covariances",
    "production of any excited state nucleus",
    "(z,b-)",
    "(z,b+)",
    "(z,ec)",
    "(z,b-n)",
    "(z,b-a)",
    "(z,it)",
    "(z,b+a)",
    "(z,ec+b+)",
    "(z,b+p)",
    "(z,b-2n)",
    "(z,b-3n)",
    "(z,b-4n)",
    "(z,ecp)",
    "(z,eca)",
    "(z,b+2p)",
    "(z,ec2p)",
    "(z,2b-)",
    "(z,b-p)",
    "(z,14c)",
    "(z,b+3p)",
    "(z,sf)",
    "(z,2b+)",
    "(z,2ec)",
  };

  // fill the maps
  for (int i = 0; i < NUM_RX_NAMES; i++) {
    rx = _names[i];
    rxid = pyne::rxname::hash(rx);
    id_name[rxid] = rx;
    name_id[rx] = rxid;
    if (0 < _mts[i]) {
      id_mt[rxid] = _mts[i];
      mt_id[_mts[i]] = rxid;
    };
    labels[rxid] = _labels[i];
    docs[rxid] = _docs[i];
  };

  // set alternative names
  altnames["tot"] = name_id["total"];
  altnames["s"] = name_id["scattering"];
  altnames["scat"] = name_id["scattering"];
  altnames["e"] = name_id["elastic"];
  altnames["elas"] = name_id["elastic"];
  altnames["i"] = name_id["n"];
  altnames["inel"] = name_id["n"];
  altnames["inelastic"] = name_id["n"];
  altnames["abs"] = name_id["absorption"];
  altnames["fis"] = name_id["fission"];
  altnames["fiss"] = name_id["fission"];
  altnames["alpha"] = name_id["a"];
  altnames["deut"] = name_id["d"];
  altnames["deuteron"] = name_id["d"];
  altnames["deuterium"] = name_id["d"];
  altnames["trit"] = name_id["t"];
  altnames["triton"] = name_id["t"];
  altnames["tritium"] = name_id["t"];
  altnames["proton"] = name_id["p"];
  altnames["h"] = name_id["He3"];  // 'h' stands for helion
  altnames["he3"] = name_id["He3"];
  altnames["HE3"] = name_id["He3"];
  altnames["3HE"] = name_id["He3"];
  altnames["3He"] = name_id["He3"];
  altnames["3he"] = name_id["He3"];
  altnames["he-3"] = name_id["He3"];
  altnames["HE-3"] = name_id["He3"];
  altnames["*"] = name_id["excited"];
  altnames["2n"] = name_id["z_2n"];
  altnames["2p"] = name_id["z_2p"];
  altnames["3h"] = name_id["t"];
  altnames["g"] = name_id["it"];
  altnames["b-"] = name_id["bminus"];
  altnames["b+"] = name_id["bplus"];
  altnames["b-n"] = name_id["bminus_n"];
  altnames["b-a"] = name_id["bminus_a"];
  altnames["b+a"] = name_id["bplus_a"];
  altnames["ec+b+"] = name_id["ec_bplus"];
  altnames["b+p"] = name_id["bplus_p"];
  altnames["b-2n"] = name_id["bminus_2n"];
  altnames["b-3n"] = name_id["bminus_3n"];
  altnames["b-4n"] = name_id["bminus_4n"];
  altnames["b+2p"] = name_id["bplus_2p"];
  altnames["ec2p"] = name_id["ec_2p"];
  altnames["2b-"] = name_id["decay_2bminus"];
  altnames["b-p"] = name_id["bminus_p"];
  altnames["14c"] = name_id["decay_14c"];
  altnames["b+3p"] = name_id["bplus_3p"];
  altnames["2b+"] = name_id["decay_2bplus"];
  altnames["2ec"] = name_id["decay_2ec"];
  

  // set the nuclide difference mappings, offset_id
  // offset_id[incident particle type "n", "p", ...][delta Z num][delta A num][rxid]
  // offset_id mapping may be ambiquious so they must come before the id_offsets!
  // the following should be sorted by (dz, da, ds)
  // neutrons:
  offset_id[make_pair("n", offset(-4, -8))] = name_id["n2a"];
  offset_id[make_pair("n", offset(-4, -7))] = name_id["z_2a"];
  offset_id[make_pair("n", offset(-2, -5))] = name_id["z_2na"];
  offset_id[make_pair("n", offset(-2, -4))] = name_id["na"];
  offset_id[make_pair("n", offset(-2, -4, 1))] = name_id["na_1"];
  offset_id[make_pair("n", offset(-2, -4, 2))] = name_id["na_2"];
  offset_id[make_pair("n", offset(-2, -3))] = name_id["a"];
  offset_id[make_pair("n", offset(-2, -3, 1))] = name_id["a_1"];
  offset_id[make_pair("n", offset(-2, -3, 2))] = name_id["a_2"];
  offset_id[make_pair("n", offset(-2, -2))] = name_id["He3"];
  offset_id[make_pair("n", offset(-2, -2, 1))] = name_id["He3_1"];
  offset_id[make_pair("n", offset(-2, -2, 2))] = name_id["He3_2"];
  offset_id[make_pair("n", offset(-2, -1))] = name_id["z_2p"];
  offset_id[make_pair("n", offset(-2, -1, 1))] = name_id["z_2p_1"];
  offset_id[make_pair("n", offset(-2, -1, 2))] = name_id["z_2p_2"];
  offset_id[make_pair("n", offset(-1, -3))] = name_id["nt"];
  offset_id[make_pair("n", offset(-1, -3, 1))] = name_id["nt_1"];
  offset_id[make_pair("n", offset(-1, -3, 2))] = name_id["nt_2"];
  offset_id[make_pair("n", offset(-1, -2))] = name_id["t"];
  offset_id[make_pair("n", offset(-1, -2, 1))] = name_id["t_1"];
  offset_id[make_pair("n", offset(-1, -2, 2))] = name_id["t_2"];
  offset_id[make_pair("n", offset(-1, -1))] = name_id["d"];
  offset_id[make_pair("n", offset(-1, -1, 1))] = name_id["d_1"];
  offset_id[make_pair("n", offset(-1, -1, 2))] = name_id["d_2"];
  offset_id[make_pair("n", offset(-1, 0))] = name_id["p"];
  offset_id[make_pair("n", offset(-1, 0, 1))] = name_id["p_1"];
  offset_id[make_pair("n", offset(-1, 0, 2))] = name_id["p_2"];
  offset_id[make_pair("n", offset(0, -3))] = name_id["z_4n"];
  offset_id[make_pair("n", offset(0, -3, 1))] = name_id["z_4n_1"];
  offset_id[make_pair("n", offset(0, -2))] = name_id["z_3n"];
  offset_id[make_pair("n", offset(0, -2, 1))] = name_id["z_3n_1"];
  offset_id[make_pair("n", offset(0, -2, 2))] = name_id["z_3n_2"];
  offset_id[make_pair("n", offset(0, -1))] = name_id["z_2n"];
  offset_id[make_pair("n", offset(0, -1, 1))] = name_id["z_2n_1"];
  offset_id[make_pair("n", offset(0, -1, 2))] = name_id["z_2n_2"];
  offset_id[make_pair("n", offset(0, 0))] = name_id["scattering"];
  offset_id[make_pair("n", offset(0, 0, 1))] = name_id["n_1"];
  offset_id[make_pair("n", offset(0, 0, 2))] = name_id["n_2"];
  offset_id[make_pair("n", offset(0, 1))] = name_id["absorption"];
  offset_id[make_pair("n", offset(0, 1, 1))] = name_id["gamma_1"];
  offset_id[make_pair("n", offset(0, 1, 2))] = name_id["gamma_2"];
  // proton:
  offset_id[make_pair("p", offset(0, 0))] = name_id["scattering"];
  offset_id[make_pair("p", offset(1, 1))] = name_id["absorption"];
  offset_id[make_pair("p", offset(1, 0))] = name_id["n"];
  offset_id[make_pair("p", offset(1, -1))] = name_id["z_2n"];
  offset_id[make_pair("p", offset(1, -2))] = name_id["z_3n"];
  offset_id[make_pair("p", offset(1, -3))] = name_id["z_4n"];
  offset_id[make_pair("p", offset(-1, -1))] = name_id["z_2p"];
  offset_id[make_pair("p", offset(0, -1))] = name_id["d"];
  offset_id[make_pair("p", offset(0, -2))] = name_id["t"];
  offset_id[make_pair("p", offset(-1, -2))] = name_id["He3"];
  offset_id[make_pair("p", offset(-1, -3))] = name_id["a"];
  // deuterium:
  offset_id[make_pair("d", offset(0, 0))] = name_id["scattering"];
  offset_id[make_pair("d", offset(1, 2))] = name_id["absorption"];
  offset_id[make_pair("d", offset(1, 1))] = name_id["n"];
  offset_id[make_pair("d", offset(1, 0))] = name_id["z_2n"];
  offset_id[make_pair("d", offset(1, -1))] = name_id["z_3n"];
  offset_id[make_pair("d", offset(1, -2))] = name_id["z_4n"];
  offset_id[make_pair("d", offset(0, 1))] = name_id["p"];
  offset_id[make_pair("d", offset(-1, 0))] = name_id["z_2p"];
  offset_id[make_pair("d", offset(0, -1))] = name_id["t"];
  offset_id[make_pair("d", offset(-1, -1))] = name_id["He3"];
  offset_id[make_pair("d", offset(-1, -2))] = name_id["a"];
  // tritium:
  offset_id[make_pair("t", offset(0, 0))] = name_id["scattering"];
  offset_id[make_pair("t", offset(1, 3))] = name_id["absorption"];
  offset_id[make_pair("t", offset(1, 2))] = name_id["n"];
  offset_id[make_pair("t", offset(1, 1))] = name_id["z_2n"];
  offset_id[make_pair("t", offset(1, 0))] = name_id["z_3n"];
  offset_id[make_pair("t", offset(1, -1))] = name_id["z_4n"];
  offset_id[make_pair("t", offset(0, 2))] = name_id["p"];
  offset_id[make_pair("t", offset(-1, 1))] = name_id["z_2p"];
  offset_id[make_pair("t", offset(0, 1))] = name_id["d"];
  offset_id[make_pair("t", offset(-1, 0))] = name_id["He3"];
  offset_id[make_pair("t", offset(-1, -1))] = name_id["a"];
  // He3:
  offset_id[make_pair("He3", offset(0, 0))] = name_id["scattering"];
  offset_id[make_pair("He3", offset(2, 3))] = name_id["absorption"];
  offset_id[make_pair("He3", offset(2, 2))] = name_id["n"];
  offset_id[make_pair("He3", offset(2, 1))] = name_id["z_2n"];
  offset_id[make_pair("He3", offset(2, 0))] = name_id["z_3n"];
  offset_id[make_pair("He3", offset(2, -1))] = name_id["z_4n"];
  offset_id[make_pair("He3", offset(1, 2))] = name_id["p"];
  offset_id[make_pair("He3", offset(0, 1))] = name_id["z_2p"];
  offset_id[make_pair("He3", offset(1, 1))] = name_id["d"];
  offset_id[make_pair("He3", offset(1, 0))] = name_id["t"];
  offset_id[make_pair("He3", offset(0, -1))] = name_id["a"];
  // alpha:
  offset_id[make_pair("a", offset(0, 0))] = name_id["scattering"];
  offset_id[make_pair("a", offset(2, 4))] = name_id["absorption"];
  offset_id[make_pair("a", offset(2, 3))] = name_id["n"];
  offset_id[make_pair("a", offset(2, 2))] = name_id["z_2n"];
  offset_id[make_pair("a", offset(2, 1))] = name_id["z_3n"];
  offset_id[make_pair("a", offset(2, 0))] = name_id["z_4n"];
  offset_id[make_pair("a", offset(1, 3))] = name_id["p"];
  offset_id[make_pair("a", offset(0, 2))] = name_id["z_2p"];
  offset_id[make_pair("a", offset(1, 2))] = name_id["d"];
  offset_id[make_pair("a", offset(1, 1))] = name_id["t"];
  offset_id[make_pair("a", offset(0, 1))] = name_id["He3"];
  // gamma:
  offset_id[make_pair("gamma", offset(0, -1))] = name_id["n"];
  offset_id[make_pair("gamma", offset(0, -2))] = name_id["z_2n"];
  offset_id[make_pair("gamma", offset(0, -3))] = name_id["z_3n"];
  offset_id[make_pair("gamma", offset(0, -4))] = name_id["z_4n"];
  offset_id[make_pair("gamma", offset(-1, -1))] = name_id["p"];
  offset_id[make_pair("gamma", offset(-2, -2))] = name_id["z_2p"];
  offset_id[make_pair("gamma", offset(-1, -2))] = name_id["d"];
  offset_id[make_pair("gamma", offset(-1, -3))] = name_id["t"];
  offset_id[make_pair("gamma", offset(-2, -3))] = name_id["He3"];
  offset_id[make_pair("gamma", offset(-2, -4))] = name_id["a"];
  // decay:
  offset_id[make_pair("decay", offset(0, -1))] = name_id["n"];
  offset_id[make_pair("decay", offset(0, -2))] = name_id["z_2n"];
  offset_id[make_pair("decay", offset(0, -3))] = name_id["z_3n"];
  offset_id[make_pair("decay", offset(0, -4))] = name_id["z_4n"];
  offset_id[make_pair("decay", offset(-1, -1))] = name_id["p"];
  offset_id[make_pair("decay", offset(-2, -2))] = name_id["z_2p"];
  offset_id[make_pair("decay", offset(-1, -2))] = name_id["d"];
  offset_id[make_pair("decay", offset(-1, -3))] = name_id["t"];
  offset_id[make_pair("decay", offset(-2, -3))] = name_id["He3"];
  offset_id[make_pair("decay", offset(-2, -4))] = name_id["a"];
  offset_id[make_pair("decay", offset(1, 0))] = name_id["bminus"];
  offset_id[make_pair("decay", offset(-1, 0))] = name_id["bplus"];
  offset_id[make_pair("decay", offset(1, -1))] = name_id["bminus_n"];
  offset_id[make_pair("decay", offset(-1, -4))] = name_id["bminus_a"];
  offset_id[make_pair("decay", offset(0, 0))] = name_id["it"];
  offset_id[make_pair("decay", offset(-3, -4))] = name_id["bplus_a"];
  offset_id[make_pair("decay", offset(-2, -1))] = name_id["bplus_p"];
  offset_id[make_pair("decay", offset(1, -2))] = name_id["bminus_2n"];
  offset_id[make_pair("decay", offset(1, -3))] = name_id["bminus_3n"];
  offset_id[make_pair("decay", offset(1, -4))] = name_id["bminus_4n"];
  offset_id[make_pair("decay", offset(-3, -2))] = name_id["bplus_2p"];
  offset_id[make_pair("decay", offset(-4, -3))] = name_id["bplus_3p"];
  offset_id[make_pair("decay", offset(2, 0))] = name_id["decay_2bminus"];
  offset_id[make_pair("decay", offset(-2, 0))] = name_id["decay_2bplus"];
  offset_id[make_pair("decay", offset(-6, -14))] = name_id["decay_14c"];
  
  // pre-loaded child offsets
  std::map<std::pair<std::string, int>, unsigned int>::iterator ioffid;
  for (ioffid = offset_id.begin(); ioffid != offset_id.end(); ioffid++) {
    id_offset[make_pair(ioffid->first.first, ioffid->second)] = ioffid->first.second;
  };
  // neutrons:
  id_offset[make_pair("n", name_id["nHe3"])] = offset(-2, -3);
  id_offset[make_pair("n", name_id["nHe3_1"])] = offset(-2, -3, 2);
  id_offset[make_pair("n", name_id["nHe3_2"])] = offset(-2, -3, 2);
  id_offset[make_pair("n", name_id["z_3np"])] = offset(-1, -3);
  id_offset[make_pair("n", name_id["nd"])] = offset(-1, -2);
  id_offset[make_pair("n", name_id["nd_1"])] = offset(-1, -2, 1);
  id_offset[make_pair("n", name_id["nd_2"])] = offset(-1, -2, 2);
  id_offset[make_pair("n", name_id["np"])] = offset(-1, -1);
  id_offset[make_pair("n", name_id["np_1"])] = offset(-1, -1, 1);
  id_offset[make_pair("n", name_id["np_2"])] = offset(-1, -1, 2);
  id_offset[make_pair("n", name_id["n"])] = offset(0, 0);
  id_offset[make_pair("n", name_id["gamma"])] = offset(0, 1);
  // decay:
  id_offset[make_pair("decay", name_id["bminus_p"])] = offset(0, -1);
  id_offset[make_pair("decay", name_id["ec_2p"])] = offset(-3, -2);
  id_offset[make_pair("decay", name_id["ec"])] = offset(-1, 0);
  id_offset[make_pair("decay", name_id["ec_bplus"])] = offset(-1, 0);
  id_offset[make_pair("decay", name_id["ecp"])] = offset(-2, -1);
  id_offset[make_pair("decay", name_id["eca"])] = offset(-3, -4);
  id_offset[make_pair("decay", name_id["decay_2ec"])] = offset(-2, 0);
  return NULL;
};
void * pyne::rxname::_ = pyne::rxname::_fill_maps();


unsigned int pyne::rxname::hash(std::string s) {
  return pyne::rxname::hash(s.c_str());
  };

unsigned int pyne::rxname::hash(const char * s) {
  // Modified from http://cboard.cprogramming.com/tech-board/114650-string-hashing-algorithm.html#post853145
  // starting from h = 32*2^5 > 1000, rather than 0, to reserve space for MT numbers
  int c;
  unsigned int h = 32; 
  while((c = *s++)) {
    h = ((h << 5) + h) ^ c;
  }
  return h;
};


// ************************
// *** name functions *****
// ************************

std::string pyne::rxname::name(char * s) {
  return pyne::rxname::name(std::string(s));
  };

std::string pyne::rxname::name(std::string s) {
  if (0 < names.count(s))
    return s;
  if (0 < altnames.count(s))
    return id_name[altnames[s]];
  // see if id in string form
  int i = 0;
  int I = s.length();
  int found = 0;
  while(0 <= found && i < I) {
    found = pyne::digits.find(s[i]);
    i++;
  }
  if (0<=found)
    return pyne::rxname::name(atoi(s.c_str()));
  // dead...
  throw NotAReaction(s, "???");
};


std::string pyne::rxname::name(int n) {
  return pyne::rxname::name((unsigned int) n);
};

std::string pyne::rxname::name(unsigned int n) {
  if (0 < id_name.count(n))
    return id_name[n];
  if (0 < mt_id.count(n))
    return id_name[mt_id[n]];
  throw NotAReaction(n, "???");
};


std::string pyne::rxname::name(int from_nuc, int to_nuc, std::string z) {
  // This assumes nuclides are in id form
  std::pair<std::string, int> key = std::make_pair(z, to_nuc - from_nuc);
  if (0 == offset_id.count(key))
    throw IndeterminateReactionForm("z=" + z + ", " + pyne::to_str(from_nuc) + \
                                    ", " + pyne::to_str(to_nuc), "???");
  return id_name[offset_id[key]];
};

std::string pyne::rxname::name(std::string from_nuc, int to_nuc, std::string z) {
  return pyne::rxname::name(pyne::nucname::id(from_nuc), 
                            pyne::nucname::id(to_nuc), z);
};

std::string pyne::rxname::name(int from_nuc, std::string to_nuc, std::string z) {
  return pyne::rxname::name(pyne::nucname::id(from_nuc), 
                            pyne::nucname::id(to_nuc), z);
};

std::string pyne::rxname::name(std::string from_nuc, std::string to_nuc, std::string z) {
  return pyne::rxname::name(pyne::nucname::id(from_nuc), 
                            pyne::nucname::id(to_nuc), z);
};



// **********************
// *** id functions *****
// **********************
unsigned int pyne::rxname::id(int x) {
  return name_id[pyne::rxname::name(x)];
};
  
unsigned int pyne::rxname::id(unsigned int x) {
  if (0 < id_name.count(x))
    return x;
  if (0 < mt_id.count(x))
    return mt_id[x];
  return name_id[pyne::rxname::name(x)];
};
  
unsigned int pyne::rxname::id(const char * x) {
  return name_id[pyne::rxname::name(x)];
};
  
unsigned int pyne::rxname::id(std::string x) {
  if (0 < names.count(x))
    return name_id[x];
  if (0 < altnames.count(x))
    return altnames[x];
  return name_id[pyne::rxname::name(x)];  
};
  
unsigned int pyne::rxname::id(int from_nuc, int to_nuc, std::string z) {
  // This assumes nuclides are in id form
  std::pair<std::string, int> key = std::make_pair(z, to_nuc - from_nuc);
  if (0 == offset_id.count(key))
    throw IndeterminateReactionForm("z=" + z + ", " + pyne::to_str(from_nuc) + \
                                    ", " + pyne::to_str(to_nuc), "???");
  return offset_id[key];
};
  
unsigned int pyne::rxname::id(int from_nuc, std::string to_nuc, std::string z) {
  return pyne::rxname::id(pyne::nucname::id(from_nuc), 
                          pyne::nucname::id(to_nuc), z);
};
  
unsigned int pyne::rxname::id(std::string from_nuc, int to_nuc, std::string z) {
  return pyne::rxname::id(pyne::nucname::id(from_nuc), 
                          pyne::nucname::id(to_nuc), z);
};
  
unsigned int pyne::rxname::id(std::string from_nuc, std::string to_nuc, std::string z) {
  return pyne::rxname::id(pyne::nucname::id(from_nuc), 
                          pyne::nucname::id(to_nuc), z);
};


// **********************
// *** MT functions *****
// **********************
unsigned int pyne::rxname::mt(int x) {
  unsigned int rxid = pyne::rxname::id(x);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(unsigned int x) {
  unsigned int rxid = pyne::rxname::id(x);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(char * x) {
  unsigned int rxid = pyne::rxname::id(x);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(std::string x) {
  unsigned int rxid = pyne::rxname::id(x);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(int from_nuc, int to_nuc, std::string z) {
  unsigned int rxid = pyne::rxname::id(from_nuc, to_nuc, z);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(int from_nuc, std::string to_nuc, std::string z) {
  unsigned int rxid = pyne::rxname::id(from_nuc, to_nuc, z);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(std::string from_nuc, int to_nuc, std::string z) {
  unsigned int rxid = pyne::rxname::id(from_nuc, to_nuc, z);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(std::string from_nuc, std::string to_nuc, std::string z) {
  unsigned int rxid = pyne::rxname::id(from_nuc, to_nuc, z);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};


// ***********************
// *** label functions ***
// ***********************
std::string pyne::rxname::label(int x) {
  return labels[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::label(unsigned int x) {
  return labels[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::label(char * x) {
  return labels[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::label(std::string x) {
  return labels[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::label(int from_nuc, int to_nuc, std::string z) {
  return labels[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::label(int from_nuc, std::string to_nuc, std::string z) {
  return labels[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::label(std::string from_nuc, int to_nuc, std::string z) {
  return labels[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::label(std::string from_nuc, std::string to_nuc, std::string z) {
  return labels[pyne::rxname::id(from_nuc, to_nuc, z)];
};


// *********************
// *** doc functions ***
// *********************
std::string pyne::rxname::doc(int x) {
  return docs[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::doc(unsigned int x) {
  return docs[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::doc(char * x) {
  return docs[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::doc(std::string x) {
  return docs[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::doc(int from_nuc, int to_nuc, std::string z) {
  return docs[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::doc(int from_nuc, std::string to_nuc, std::string z) {
  return docs[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::doc(std::string from_nuc, int to_nuc, std::string z) {
  return docs[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::doc(std::string from_nuc, std::string to_nuc, std::string z) {
  return docs[pyne::rxname::id(from_nuc, to_nuc, z)];
};


// ***********************
// *** child functions ***
// ***********************

int pyne::rxname::child(int nuc, unsigned int rx, std::string z) {
  // This assumes nuclides are in id form
  std::pair<std::string, unsigned int> key = std::make_pair(z, rx);
  if (0 == id_offset.count(key))
    throw IndeterminateReactionForm("z=" + z + ", rx=" + pyne::to_str(rx), "???");
  int to_nuc = nuc + id_offset[key];
  if (!pyne::nucname::isnuclide(to_nuc))
    throw pyne::nucname::NotANuclide(nuc, to_nuc);
  return to_nuc;
};

int pyne::rxname::child(int nuc, std::string rx, std::string z) {
  return child(nuc, id(rx), z);
};

int pyne::rxname::child(std::string nuc, unsigned int rx, std::string z) {
  return child(pyne::nucname::id(nuc), rx, z);
};

int pyne::rxname::child(std::string nuc, std::string rx, std::string z) {
  return child(pyne::nucname::id(nuc), id(rx), z);
};


// ************************
// *** parent functions ***
// ************************

int pyne::rxname::parent(int nuc, unsigned int rx, std::string z) {
  // This assumes nuclides are in id form
  std::pair<std::string, unsigned int> key = std::make_pair(z, rx);
  if (0 == id_offset.count(key))
    throw IndeterminateReactionForm("z=" + z + ", rx=" + pyne::to_str(rx), "???");
  int from_nuc = nuc - id_offset[key];
  if (!pyne::nucname::isnuclide(from_nuc))
    throw pyne::nucname::NotANuclide(from_nuc, nuc);
  return from_nuc;
};

int pyne::rxname::parent(int nuc, std::string rx, std::string z) {
  return parent(nuc, id(rx), z);
};

int pyne::rxname::parent(std::string nuc, unsigned int rx, std::string z) {
  return parent(pyne::nucname::id(nuc), rx, z);
};

int pyne::rxname::parent(std::string nuc, std::string rx, std::string z) {
  return parent(pyne::nucname::id(nuc), id(rx), z);
};

//
// end of cpp/rxname.cpp
//


//
// start of cpp/data.cpp
//
// Implements basic nuclear data functions.
#ifndef PYNE_IS_AMALGAMATED
#include "data.h"
#endif


//
// Math Helpers
//

const double pyne::pi = 3.14159265359;
const double pyne::N_A = 6.0221415e+23;
const double pyne::barns_per_cm2 = 1e24;
const double pyne::cm2_per_barn = 1e-24;
const double pyne::sec_per_day = 24.0 * 3600.0;

/********************************/
/*** data_checksums Functions ***/
/********************************/

std::map<std::string, std::string> pyne::get_data_checksums() {
    std::map<std::string, std::string> temp_map;
    // Initialization of dataset hashes
    temp_map["/atomic_mass"]="10edfdc662e35bdfab91beb89285efff";
    temp_map["/material_library"]="8b10864378fbd88538434679acf908cc";
    temp_map["/neutron/eaf_xs"]="29622c636c4a3a46802207b934f9516c";
    temp_map["/neutron/scattering_lengths"]="a24d391cc9dc0fc146392740bb97ead4";
    temp_map["/neutron/simple_xs"]="3d6e086977783dcdf07e5c6b0c2416be";
    
    return temp_map;
};

std::map<std::string, std::string> pyne::data_checksums = 
  pyne::get_data_checksums();

/*****************************/
/*** atomic_mass Functions ***/
/*****************************/
std::map<int, double> pyne::atomic_mass_map = std::map<int, double>();

void pyne::_load_atomic_mass_map() {
  // Loads the important parts of atomic_wight table into atomic_mass_map

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(atomic_mass_struct));
  H5Tinsert(desc, "nuc",   HOFFSET(atomic_mass_struct, nuc),   H5T_NATIVE_INT);
  H5Tinsert(desc, "mass",  HOFFSET(atomic_mass_struct, mass),  H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "error", HOFFSET(atomic_mass_struct, error), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "abund", HOFFSET(atomic_mass_struct, abund), H5T_NATIVE_DOUBLE);

  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

  // Open the data set
  hid_t atomic_mass_set = H5Dopen2(nuc_data_h5, "/atomic_mass", H5P_DEFAULT);
  hid_t atomic_mass_space = H5Dget_space(atomic_mass_set);
  int atomic_mass_length = H5Sget_simple_extent_npoints(atomic_mass_space);

  // Read in the data
  atomic_mass_struct * atomic_mass_array = new atomic_mass_struct[atomic_mass_length];
  H5Dread(atomic_mass_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, atomic_mass_array);

  // close the nuc_data library, before doing anything stupid
  H5Dclose(atomic_mass_set);
  H5Fclose(nuc_data_h5);

  // Ok now that we have the array of stucts, put it in the map
  for(int n = 0; n < atomic_mass_length; n++) {
    atomic_mass_map[atomic_mass_array[n].nuc] = atomic_mass_array[n].mass;
    natural_abund_map[atomic_mass_array[n].nuc] = atomic_mass_array[n].abund;
  }

  delete[] atomic_mass_array;
};


double pyne::atomic_mass(int nuc) {
  // Find the nuclide's mass in AMU
  std::map<int, double>::iterator nuc_iter, nuc_end;

  nuc_iter = atomic_mass_map.find(nuc);
  nuc_end = atomic_mass_map.end();

  // First check if we already have the nuc mass in the map
  if (nuc_iter != nuc_end)
    return (*nuc_iter).second;

  // Next, fill up the map with values from the 
  // nuc_data.h5, if the map is empty.
  if (atomic_mass_map.empty()) {
    // Don't fail if we can't load the library
      _load_atomic_mass_map();
      return atomic_mass(nuc);
  };

  double aw;
  int nucid = nucname::id(nuc);

  // If in an excited state, return the ground
  // state mass...not strictly true, but good guess.
  if (0 < nucid%10000) {
    aw = atomic_mass((nucid/10000)*10000);
    atomic_mass_map[nuc] = aw;
    return aw;
  };

  // Finally, if none of these work, 
  // take a best guess based on the 
  // aaa number.
  aw = (double) ((nucid/10000)%1000);
  atomic_mass_map[nuc] = aw;
  return aw;
};


double pyne::atomic_mass(char * nuc) {
  int nuc_zz = nucname::id(nuc);
  return atomic_mass(nuc_zz);
};


double pyne::atomic_mass(std::string nuc) {
  int nuc_zz = nucname::id(nuc);
  return atomic_mass(nuc_zz);
};


/*******************************/
/*** natural_abund functions ***/
/*******************************/

std::map<int, double> pyne::natural_abund_map = std::map<int, double>();

double pyne::natural_abund(int nuc) {
  // Find the nuclide's natural abundance
  std::map<int, double>::iterator nuc_iter, nuc_end;

  nuc_iter = natural_abund_map.find(nuc);
  nuc_end = natural_abund_map.end();

  // First check if we already have the nuc mass in the map
  if (nuc_iter != nuc_end)
    return (*nuc_iter).second;

  // Next, fill up the map with values from the 
  // nuc_data.h5, if the map is empty.
  if (natural_abund_map.empty()) {
    // Don't fail if we can't load the library
      _load_atomic_mass_map();
      return natural_abund(nuc);
  };

  double na;
  int nucid = nucname::id(nuc);

  // If in an excited state, return the ground
  // state abundance...not strictly true, but good guess.
  if (0 < nucid%10000) {
    na = natural_abund((nucid/10000)*10000);
    atomic_mass_map[nuc] = na;
    return na;
  };

  // Finally, if none of these work, 
  // take a best guess based on the 
  // aaa number.
  na = 0.0;
  natural_abund_map[nuc] = na;
  return na;
};


double pyne::natural_abund(char * nuc) {
  int nuc_zz = nucname::id(nuc);
  return natural_abund(nuc_zz);
};


double pyne::natural_abund(std::string nuc) {
  int nuc_zz = nucname::id(nuc);
  return natural_abund(nuc_zz);
};



/*****************************/
/*** Q_value Functions ***/
/*****************************/
std::map<int, double> pyne::q_val_map = std::map<int, double>();

void pyne::_load_q_val_map() {
  // Loads the important parts of q_value table into q_value_map

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(q_val_struct));
  H5Tinsert(desc, "nuc", HOFFSET(q_val_struct, nuc),  H5T_NATIVE_INT);
  H5Tinsert(desc, "q_val", HOFFSET(q_val_struct, q_val), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "gamma_frac", HOFFSET(q_val_struct, gamma_frac), H5T_NATIVE_DOUBLE);

  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

  // Open the data set
  hid_t q_val_set = H5Dopen2(nuc_data_h5, "/neutron/q_values", H5P_DEFAULT);
  hid_t q_val_space = H5Dget_space(q_val_set);
  int q_val_length = H5Sget_simple_extent_npoints(q_val_space);

  // Read in the data
  q_val_struct * q_val_array = new q_val_struct[q_val_length];
  H5Dread(q_val_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, q_val_array);

  // close the nuc_data library, before doing anything stupid
  H5Dclose(q_val_set);
  H5Fclose(nuc_data_h5);

  // Ok now that we have the array of structs, put it in the map
  for(int n = 0; n < q_val_length; n++){
    q_val_map[q_val_array[n].nuc] = q_val_array[n].q_val;
    gamma_frac_map[q_val_array[n].nuc] = q_val_array[n].gamma_frac;
  }

  delete[] q_val_array;
};


double pyne::q_val(int nuc) {
  // Find the nuclide's q_val in MeV/fission
  std::map<int, double>::iterator nuc_iter, nuc_end;

  nuc_iter = q_val_map.find(nuc);
  nuc_end = q_val_map.end();

  // First check if we already have the nuc q_val in the map
  if (nuc_iter != nuc_end) 
    return (*nuc_iter).second;

  // Next, fill up the map with values from the nuc_data.h5 if the map is empty.
  if (q_val_map.empty()) {
    // Don't fail if we can't load the library
      _load_q_val_map();
      return q_val(nuc);
  };
  
  double qv;
  int nucid = nucname::id(nuc);
  if (nucid != nuc)
    return q_val(nucid);

  // If nuclide is not found, return 0
  qv = 0.0;
  q_val_map[nuc] = qv;
  return qv;
};


double pyne::q_val(char * nuc) {
  int nuc_zz = nucname::id(nuc);
  return q_val(nuc_zz);
};


double pyne::q_val(std::string nuc) {
  int nuc_zz = nucname::id(nuc);
  return q_val(nuc_zz);
};


/*******************************/
/*** gamma_frac functions ***/
/*******************************/

std::map<int, double> pyne::gamma_frac_map = std::map<int, double>();

double pyne::gamma_frac(int nuc) {
  // Find the nuclide's fraction of Q that comes from gammas
  std::map<int, double>::iterator nuc_iter, nuc_end;

  nuc_iter = gamma_frac_map.find(nuc);
  nuc_end = gamma_frac_map.end();

  // First check if we already have the gamma_frac in the map
  if (nuc_iter != nuc_end)
    return (*nuc_iter).second;

  // Next, fill up the map with values from nuc_data.h5 if the map is empty.
  if (gamma_frac_map.empty()) {
    // Don't fail if we can't load the library
      _load_q_val_map();
      return gamma_frac(nuc);
  };

  double gf;
  int nucid = nucname::id(nuc);
  if (nucid != nuc)
    return gamma_frac(nucid);

  // If nuclide is not found, return 0
  gf = 0.0;
  gamma_frac_map[nucid] = gf;
  return gf;
};


double pyne::gamma_frac(char * nuc) {
  int nuc_zz = nucname::id(nuc);
  return gamma_frac(nuc_zz);
};


double pyne::gamma_frac(std::string nuc) {
  int nuc_zz = nucname::id(nuc);
  return gamma_frac(nuc_zz);
};


/***********************************/
/*** scattering length functions ***/
/***********************************/
std::map<int, xd_complex_t> pyne::b_coherent_map = std::map<int, xd_complex_t>();
std::map<int, xd_complex_t> pyne::b_incoherent_map = std::map<int, xd_complex_t>();
std::map<int, double> pyne::b_map = std::map<int, double>();


void pyne::_load_scattering_lengths() {
  // Loads the important parts of atomic_wight table into atomic_mass_map
  herr_t status;

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(scattering_lengths_struct));
  status = H5Tinsert(desc, "nuc", HOFFSET(scattering_lengths_struct, nuc), H5T_NATIVE_INT);
  status = H5Tinsert(desc, "b_coherent", HOFFSET(scattering_lengths_struct, b_coherent), 
                      h5wrap::PYTABLES_COMPLEX128);
  status = H5Tinsert(desc, "b_incoherent", HOFFSET(scattering_lengths_struct, b_incoherent), 
                      h5wrap::PYTABLES_COMPLEX128);
  status = H5Tinsert(desc, "xs_coherent", HOFFSET(scattering_lengths_struct, xs_coherent), 
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "xs_incoherent", HOFFSET(scattering_lengths_struct, xs_incoherent), 
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "xs", HOFFSET(scattering_lengths_struct, xs), H5T_NATIVE_DOUBLE);

  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

  // Open the data set
  hid_t scat_len_set = H5Dopen2(nuc_data_h5, "/neutron/scattering_lengths", H5P_DEFAULT);
  hid_t scat_len_space = H5Dget_space(scat_len_set);
  int scat_len_length = H5Sget_simple_extent_npoints(scat_len_space);

  // Read in the data
  scattering_lengths_struct * scat_len_array = new scattering_lengths_struct[scat_len_length];
  status = H5Dread(scat_len_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, scat_len_array);

  // close the nuc_data library, before doing anything stupid
  status = H5Dclose(scat_len_set);
  status = H5Fclose(nuc_data_h5);

  // Ok now that we have the array of stucts, put it in the maps
  for(int n = 0; n < scat_len_length; n++) {
    b_coherent_map[scat_len_array[n].nuc] = scat_len_array[n].b_coherent;
    b_incoherent_map[scat_len_array[n].nuc] = scat_len_array[n].b_incoherent;
  };

  delete[] scat_len_array;
};



//
// Coherent functions 
//


xd_complex_t pyne::b_coherent(int nuc) {
  // Find the nuclide's bound scattering length in cm
  std::map<int, xd_complex_t>::iterator nuc_iter, nuc_end;

  nuc_iter = b_coherent_map.find(nuc);
  nuc_end = b_coherent_map.end();

  // First check if we already have the nuc in the map
  if (nuc_iter != nuc_end)
    return (*nuc_iter).second;

  // Next, fill up the map with values from the 
  // nuc_data.h5, if the map is empty.
  if (b_coherent_map.empty()) {
    _load_scattering_lengths();
    return b_coherent(nuc);
  };

  xd_complex_t bc;
  int nucid = nucname::id(nuc);
  int znum = nucname::znum(nucid);
  int anum = nucname::anum(nucid);

  // Try to find a nuclide with matching A-number
  nuc_iter = b_coherent_map.begin();
  while (nuc_iter != nuc_end) {
    if (anum == nucname::anum((*nuc_iter).first)) {
      bc = (*nuc_iter).second;
      b_coherent_map[nuc] = bc;
      return bc;
    };
    nuc_iter++;
  };

  // Try to find a nuclide with matching Z-number
  nuc_iter = b_coherent_map.begin();
  while (nuc_iter != nuc_end) {
    if (znum == nucname::znum((*nuc_iter).first)) {
      bc = (*nuc_iter).second;
      b_coherent_map[nuc] = bc;
      return bc;
    };
    nuc_iter++;
  };

  // Finally, if none of these work, 
  // just return zero...
  bc.re = 0.0;
  bc.im = 0.0;
  b_coherent_map[nuc] = bc;
  return bc;
};


xd_complex_t pyne::b_coherent(char * nuc) {
  int nuc_zz = nucname::id(nuc);
  return b_coherent(nuc_zz);
};


xd_complex_t pyne::b_coherent(std::string nuc) {
  int nuc_zz = nucname::id(nuc);
  return b_coherent(nuc_zz);
};



//
// Incoherent functions 
//


xd_complex_t pyne::b_incoherent(int nuc) {
  // Find the nuclide's bound inchoherent scattering length in cm
  std::map<int, xd_complex_t>::iterator nuc_iter, nuc_end;

  nuc_iter = b_incoherent_map.find(nuc);
  nuc_end = b_incoherent_map.end();

  // First check if we already have the nuc in the map
  if (nuc_iter != nuc_end)
    return (*nuc_iter).second;

  // Next, fill up the map with values from the 
  // nuc_data.h5, if the map is empty.
  if (b_incoherent_map.empty()) {
    _load_scattering_lengths();
    return b_incoherent(nuc);
  };

  xd_complex_t bi;
  int nucid = nucname::id(nuc);
  int znum = nucname::znum(nucid);
  int anum = nucname::anum(nucid);

  // Try to find a nuclide with matching A-number
  nuc_iter = b_incoherent_map.begin();
  while (nuc_iter != nuc_end) {
    if (anum == nucname::anum((*nuc_iter).first)) {
      bi = (*nuc_iter).second;
      b_incoherent_map[nuc] = bi;
      return bi;
    };
    nuc_iter++;
  };

  // Try to find a nuclide with matching Z-number
  nuc_iter = b_incoherent_map.begin();
  while (nuc_iter != nuc_end) {
    if (znum == nucname::znum((*nuc_iter).first)) {
      bi = (*nuc_iter).second;
      b_incoherent_map[nuc] = bi;
      return bi;
    };
    nuc_iter++;
  };

  // Finally, if none of these work, 
  // just return zero...
  bi.re = 0.0;
  bi.im = 0.0;
  b_incoherent_map[nuc] = bi;
  return bi;
};


xd_complex_t pyne::b_incoherent(char * nuc) {
  return b_incoherent(nucname::id(nuc));
};


xd_complex_t pyne::b_incoherent(std::string nuc) {
  return b_incoherent(nucname::id(nuc));
};



//
// b functions
//

double pyne::b(int nuc) {
  // Find the nuclide's bound scattering length in cm
  std::map<int, double>::iterator nuc_iter, nuc_end;

  nuc_iter = b_map.find(nuc);
  nuc_end = b_map.end();

  // First check if we already have the nuc in the map
  if (nuc_iter != nuc_end)
    return (*nuc_iter).second;

  // Next, calculate the value from coherent and incoherent lengths
  xd_complex_t bc = b_coherent(nuc);
  xd_complex_t bi = b_incoherent(nuc);

  double b_val = sqrt(bc.re*bc.re + bc.im*bc.im + bi.re*bi.re + bi.im*bi.im);

  return b_val;
};


double pyne::b(char * nuc) {
  int nucid = nucname::id(nuc);
  return b(nucid);
};


double pyne::b(std::string nuc) {
  int nucid = nucname::id(nuc);
  return b(nucid);
};



//
// Fission Product Yield Data 
//
std::map<std::pair<int, int>, double> pyne::wimsdfpy_data = \
  std::map<std::pair<int, int>, double>();

void pyne::_load_wimsdfpy() {
  herr_t status;

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(wimsdfpy_struct));
  status = H5Tinsert(desc, "from_nuc", HOFFSET(wimsdfpy_struct, from_nuc), 
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "to_nuc", HOFFSET(wimsdfpy_struct, to_nuc), 
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "yields", HOFFSET(wimsdfpy_struct, yields),
                     H5T_NATIVE_DOUBLE);

  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY, 
                              H5P_DEFAULT);

  // Open the data set
  hid_t wimsdfpy_set = H5Dopen2(nuc_data_h5, "/neutron/wimsd_fission_products", 
                                H5P_DEFAULT);
  hid_t wimsdfpy_space = H5Dget_space(wimsdfpy_set);
  int wimsdfpy_length = H5Sget_simple_extent_npoints(wimsdfpy_space);

  // Read in the data
  wimsdfpy_struct * wimsdfpy_array = new wimsdfpy_struct[wimsdfpy_length];
  status = H5Dread(wimsdfpy_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, wimsdfpy_array);

  // close the nuc_data library, before doing anything stupid
  status = H5Dclose(wimsdfpy_set);
  status = H5Fclose(nuc_data_h5);

  // Ok now that we have the array of stucts, put it in the maps
  for(int n=0; n < wimsdfpy_length; n++) {
    wimsdfpy_data[std::make_pair(wimsdfpy_array[n].from_nuc, 
      wimsdfpy_array[n].to_nuc)] = wimsdfpy_array[n].yields;
  };

  delete[] wimsdfpy_array;
};


std::map<std::pair<int, int>, pyne::ndsfpysub_struct> pyne::ndsfpy_data = \
  std::map<std::pair<int, int>, pyne::ndsfpysub_struct>();

void pyne::_load_ndsfpy() {
  herr_t status;

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(ndsfpy_struct));
  status = H5Tinsert(desc, "from_nuc", HOFFSET(ndsfpy_struct, from_nuc),
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "to_nuc", HOFFSET(ndsfpy_struct, to_nuc),
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "yield_thermal", HOFFSET(ndsfpy_struct, yield_thermal),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "yield_thermal_err", HOFFSET(ndsfpy_struct, yield_thermal_err),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "yield_fast", HOFFSET(ndsfpy_struct, yield_fast),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "yield_fast_err", HOFFSET(ndsfpy_struct, yield_fast_err),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "yield_14MeV", HOFFSET(ndsfpy_struct, yield_14MeV),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "yield_14MeV_err", HOFFSET(ndsfpy_struct, yield_14MeV_err),
                     H5T_NATIVE_DOUBLE);

  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY,
                              H5P_DEFAULT);

  // Open the data set
  hid_t ndsfpy_set = H5Dopen2(nuc_data_h5, "/neutron/nds_fission_products",
                                H5P_DEFAULT);
  hid_t ndsfpy_space = H5Dget_space(ndsfpy_set);
  int ndsfpy_length = H5Sget_simple_extent_npoints(ndsfpy_space);

  // Read in the data
  ndsfpy_struct * ndsfpy_array = new ndsfpy_struct[ndsfpy_length];
  status = H5Dread(ndsfpy_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, ndsfpy_array);

  // close the nuc_data library, before doing anything stupid
  status = H5Dclose(ndsfpy_set);
  status = H5Fclose(nuc_data_h5);

  ndsfpysub_struct ndsfpysub_temp;

  // Ok now that we have the array of structs, put it in the maps
  for(int n=0; n < ndsfpy_length; n++) {
    ndsfpysub_temp.yield_thermal = ndsfpy_array[n].yield_thermal;
    ndsfpysub_temp.yield_thermal_err = ndsfpy_array[n].yield_thermal_err;
    ndsfpysub_temp.yield_fast = ndsfpy_array[n].yield_fast;
    ndsfpysub_temp.yield_fast_err = ndsfpy_array[n].yield_fast_err;
    ndsfpysub_temp.yield_14MeV = ndsfpy_array[n].yield_14MeV;
    ndsfpysub_temp.yield_14MeV_err = ndsfpy_array[n].yield_14MeV_err;
    ndsfpy_data[std::make_pair(ndsfpy_array[n].from_nuc,
      ndsfpy_array[n].to_nuc)] = ndsfpysub_temp;
  };



  delete[] ndsfpy_array;
};

double pyne::fpyield(std::pair<int, int> from_to, int source, bool get_error) {
  // Note that this may be expanded eventually to include other
  // sources of fission product data.

  // Find the parent/child pair branch ratio as a fraction
  if (source == 0) {
    std::map<std::pair<int, int>, double>::iterator fpy_iter, fpy_end;
    fpy_iter = wimsdfpy_data.find(from_to);
    fpy_end = wimsdfpy_data.end();
    if (fpy_iter != fpy_end)
        //if (get_error == true) return 0;
        return (*fpy_iter).second;
  } else {
    std::map<std::pair<int, int>, ndsfpysub_struct>::iterator fpy_iter, fpy_end;
    fpy_iter = ndsfpy_data.find(from_to);
    fpy_end = ndsfpy_data.end();
    if (fpy_iter != fpy_end) {
        switch (source) {
          case 1:
            if (get_error)
                return (*fpy_iter).second.yield_thermal_err;
            return (*fpy_iter).second.yield_thermal;
            break;
          case 2:
            if (get_error)
                return (*fpy_iter).second.yield_fast_err;
            return (*fpy_iter).second.yield_fast;
            break;
          case 3:
            if (get_error)
                return (*fpy_iter).second.yield_14MeV_err;
            return (*fpy_iter).second.yield_14MeV;
            break;
        }
    }
  }


  // Next, fill up the map with values from the
  // nuc_data.h5, if the map is empty.
  if ((source == 0 ) && (wimsdfpy_data.empty())) {
    _load_wimsdfpy();
    return fpyield(from_to, 0, get_error);
  }else if (ndsfpy_data.empty()) {
    _load_ndsfpy();
    return fpyield(from_to, source, get_error);
  }

  // Finally, if none of these work, 
  // assume the value is stable
  double fpy = 0.0;
  wimsdfpy_data[from_to] = fpy;
  return fpy;
};

double pyne::fpyield(int from_nuc, int to_nuc, int source, bool get_error) {
  return fpyield(std::pair<int, int>(nucname::id(from_nuc), 
                                     nucname::id(to_nuc)), source, get_error);
};

double pyne::fpyield(char * from_nuc, char * to_nuc, int source, bool get_error) {
  return fpyield(std::pair<int, int>(nucname::id(from_nuc), 
                                     nucname::id(to_nuc)), source, get_error);
};

double pyne::fpyield(std::string from_nuc, std::string to_nuc, int source, 
                     bool get_error) {
  return fpyield(std::pair<int, int>(nucname::id(from_nuc), 
                                     nucname::id(to_nuc)), source, get_error);
};


/***********************/
/*** decay functions ***/
/***********************/

//
// Data access tools
// 

bool pyne::swapmapcompare::operator()(const std::pair<int, double>& lhs, 
const std::pair<int, double>& rhs) const {
    return lhs.second<rhs.second || (!(rhs.second<lhs.second) && 
      lhs.first<rhs.first); 
};

template<typename T, typename U> std::vector<T> pyne::data_access(
double energy_min, double energy_max, size_t valoffset, std::map<std::pair<int,
double>, U>  &data) {
  typename std::map<std::pair<int, double>, U, swapmapcompare>::iterator 
    nuc_iter, nuc_end, it;
  std::map<std::pair<int, double>, U, swapmapcompare> dc(data.begin(), 
    data.end());
  std::vector<T> result;
  if (energy_max < energy_min){
    double temp = energy_max;
    energy_max = energy_min;
    energy_min = temp;
  } 
  nuc_iter = dc.lower_bound(std::make_pair(0, energy_min));
  nuc_end = dc.upper_bound(std::make_pair(9999999999, energy_max));
  T *ret;
  // First check if we already have the nuc in the map
  for (it = nuc_iter; it!= nuc_end; ++it){
    ret = (T *)((char *)&(it->second) + valoffset);
    result.push_back(*ret);
  }
  // Next, fill up the map with values from the
  // nuc_data.h5, if the map is empty.
  if (data.empty())
  {
    _load_data<U>();
    return data_access<T, U>(energy_min, energy_max, valoffset, data);
  };
  return result;
};

template<typename T, typename U> std::vector<T> pyne::data_access(int parent, 
double min, double max, size_t valoffset, 
std::map<std::pair<int, double>, U>  &data) {
  typename std::map<std::pair<int, double>, U>::iterator nuc_iter, nuc_end, it;
  std::vector<T> result;
  nuc_iter = data.lower_bound(std::make_pair(parent,min));
  nuc_end = data.upper_bound(std::make_pair(parent,max));
  T *ret;
  // First check if we already have the nuc in the map
  for (it = nuc_iter; it!= nuc_end; ++it){
    ret = (T *)((char *)&(it->second) + valoffset);
    result.push_back(*ret);
  }
  // Next, fill up the map with values from the
  // nuc_data.h5, if the map is empty.
  if (data.empty())
  {
    _load_data<U>();
    return data_access<T, U>(parent, min, max, valoffset, data);
  };
  return result;
};

template<typename T, typename U> T pyne::data_access(std::pair<int, int> 
from_to, size_t valoffset, std::map<std::pair<int, int>, U> &data) {
  typename std::map<std::pair<int, int>, U>::iterator nuc_iter, nuc_end;

  nuc_iter = data.find(from_to);
  nuc_end = data.end();
  T *ret;
  // First check if we already have the nuc in the map
  if (nuc_iter != nuc_end){
    ret = (T *)((char *)&(nuc_iter->second) + valoffset);
    return *ret;
  }
  // Next, fill up the map with values from the
  // nuc_data.h5, if the map is empty.
  if (data.empty())
  {
    _load_data<U>();
    return data_access<T, U>(from_to, valoffset, data);
  };
  // This is okay for now because we only return ints and doubles
  return 0;
}

template<typename T, typename U> std::vector<T> pyne::data_access(int parent, 
size_t valoffset, std::map<std::pair<int, int>, U> &data){
  typename std::map<std::pair<int, int>, U>::iterator nuc_iter, nuc_end, it;
  std::vector<T> result;
  nuc_iter = data.lower_bound(std::make_pair(parent,0));
  nuc_end = data.upper_bound(std::make_pair(parent,9999999999));
  T *ret;
  // First check if we already have the nuc in the map
  for (it = nuc_iter; it!= nuc_end; ++it){
    ret = (T *)((char *)&(it->second) + valoffset);
    result.push_back(*ret);
  }
  // Next, fill up the map with values from the
  // nuc_data.h5, if the map is empty.
  if (data.empty())
  {
    _load_data<U>();
    return data_access<T, U>(parent, valoffset, data);
  };
  return result;
};

template<typename T, typename U> std::vector<T> pyne::data_access(int parent, 
size_t valoffset, std::map<std::pair<int, unsigned int>, U> &data){
  typename std::map<std::pair<int, unsigned int>, U>::iterator nuc_iter,
   nuc_end, it;
  std::vector<T> result;
  nuc_iter = data.lower_bound(std::make_pair(parent,0));
  nuc_end = data.upper_bound(std::make_pair(parent,UINT_MAX));
  T *ret;
  // First check if we already have the nuc in the map
  for (it = nuc_iter; it!= nuc_end; ++it){
    ret = (T *)((char *)&(it->second) + valoffset);
    result.push_back(*ret);
  }
  // Next, fill up the map with values from the
  // nuc_data.h5, if the map is empty.
  if (data.empty())
  {
    _load_data<U>();
    return data_access<T, U>(parent, valoffset, data);
  };
  return result;
};

template<typename U> double pyne::data_access(int nuc, 
size_t valoffset, std::map<int, U> &data){
  typename std::map<int, U>::iterator nuc_iter,
   nuc_end;
  nuc_iter = data.find(nuc);
  nuc_end = data.end();
  // First check if we already have the nuc in the map
  if (nuc_iter != nuc_end){
    return *(double *)((char *)&(nuc_iter->second) + valoffset);
  }
  // Next, fill up the map with values from the
  // nuc_data.h5, if the map is empty.
  if (data.empty())
  {
    _load_data<U>();
    return data_access<U>(nuc, valoffset, data);
  };
  throw pyne::nucname::NotANuclide(nuc, "");
};


//
// Load atomic data
//

std::map<int, pyne::atomic_struct> pyne::atomic_data_map;

template<> void pyne::_load_data<pyne::atomic_struct>() {
  // Loads the atomic table into memory
  herr_t status;

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(atomic_struct));
  status = H5Tinsert(desc, "z", HOFFSET(atomic_struct, z),
                      H5T_NATIVE_INT);
  status = H5Tinsert(desc, "k_shell_fluor", HOFFSET(atomic_struct, k_shell_fluor),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "k_shell_fluor_error", HOFFSET(atomic_struct, k_shell_fluor_error),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "l_shell_fluor", HOFFSET(atomic_struct, l_shell_fluor),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "l_shell_fluor_error", HOFFSET(atomic_struct, l_shell_fluor_error),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "prob", HOFFSET(atomic_struct, prob), 
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "k_shell_be", HOFFSET(atomic_struct, k_shell_be),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "k_shell_be_err", HOFFSET(atomic_struct, k_shell_be_err),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "li_shell_be", HOFFSET(atomic_struct, li_shell_be),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "li_shell_be_err", HOFFSET(atomic_struct, li_shell_be_err),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "mi_shell_be", HOFFSET(atomic_struct, mi_shell_be),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "mi_shell_be_err", HOFFSET(atomic_struct, mi_shell_be_err),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "ni_shell_be", HOFFSET(atomic_struct, ni_shell_be),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "ni_shell_be_err", HOFFSET(atomic_struct, ni_shell_be_err),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "kb_to_ka", HOFFSET(atomic_struct, kb_to_ka),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "kb_to_ka_err", HOFFSET(atomic_struct, kb_to_ka_err),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "ka2_to_ka1", HOFFSET(atomic_struct, ka2_to_ka1),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "ka2_to_ka1_err", HOFFSET(atomic_struct, ka2_to_ka1_err),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "l_auger", HOFFSET(atomic_struct, l_auger), 
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "k_auger", HOFFSET(atomic_struct, k_auger),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "ka1_x_ray_en", HOFFSET(atomic_struct, ka1_x_ray_en),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "ka1_x_ray_en_err", HOFFSET(atomic_struct, ka1_x_ray_en_err),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "ka2_x_ray_en", HOFFSET(atomic_struct, ka2_x_ray_en),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "ka2_x_ray_en_err", HOFFSET(atomic_struct, ka2_x_ray_en_err),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "kb_x_ray_en", HOFFSET(atomic_struct, kb_x_ray_en),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "l_x_ray_en", HOFFSET(atomic_struct, l_x_ray_en),
                      H5T_NATIVE_DOUBLE);
                      
  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY, 
                              H5P_DEFAULT);
  // Open the data set
  hid_t atomic_set = H5Dopen2(nuc_data_h5, "/decay/atomic", H5P_DEFAULT);
  hid_t atomic_space = H5Dget_space(atomic_set);
  int atomic_length = H5Sget_simple_extent_npoints(atomic_space);

  // Read in the data
  atomic_struct * atomic_array = new atomic_struct[atomic_length];
  status = H5Dread(atomic_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                   atomic_array);

  // close the nuc_data library, before doing anything stupid
  status = H5Dclose(atomic_set);
  status = H5Fclose(nuc_data_h5);

  for (int i = 0; i < atomic_length; ++i) {
      atomic_data_map[atomic_array[i].z] = atomic_array[i];
  }
  
  delete[] atomic_array;

}

std::vector<std::pair<double, double> >
  pyne::calculate_xray_data(int z, double k_conv, double l_conv) {
  double xk = 0;
  double xka = 0;
  double xka1 = 0;
  double xka2 = 0;
  double xkb = 0;
  double xl = 0;
  if (!isnan(k_conv)) {
    xk = data_access<atomic_struct> (z, offsetof(atomic_struct, k_shell_fluor),
     atomic_data_map)*k_conv;
    xka = xk / (1.0 + data_access<atomic_struct> (z, offsetof(atomic_struct, 
     kb_to_ka), atomic_data_map));
    xka1 = xka / (1.0 + data_access<atomic_struct> (z, offsetof(atomic_struct, 
     ka2_to_ka1), atomic_data_map));
    xka2 = xka - xka1;
    xkb = xk - xka;
    if (!isnan(l_conv)) {
        xl = (l_conv + k_conv*data_access<atomic_struct> (z, offsetof(atomic_struct, 
     prob), atomic_data_map))*data_access<atomic_struct> (z, offsetof(atomic_struct, 
     l_shell_fluor), atomic_data_map);
    }
  } else if (!isnan(l_conv)) {
    xl = l_conv*data_access<atomic_struct> (z, offsetof(atomic_struct, 
     l_shell_fluor), atomic_data_map);
  }
  std::vector<std::pair<double, double> > result;
  result.push_back(std::make_pair(data_access<atomic_struct> (z, offsetof(atomic_struct, 
     ka1_x_ray_en), atomic_data_map),xka1));
  result.push_back(std::make_pair(data_access<atomic_struct> (z, offsetof(atomic_struct, 
     ka2_x_ray_en), atomic_data_map),xka2));
  result.push_back(std::make_pair(data_access<atomic_struct> (z, offsetof(atomic_struct, 
     kb_x_ray_en), atomic_data_map),xkb));
  result.push_back(std::make_pair(data_access<atomic_struct> (z, offsetof(atomic_struct, 
     l_x_ray_en), atomic_data_map),xl));

  return result;
}


//
// Load level data
//

std::map<std::pair<int,double>, pyne::level_struct> pyne::level_data_lvl_map;
std::map<std::pair<int,unsigned int>, 
  pyne::level_struct> pyne::level_data_rx_map;


template<> void pyne::_load_data<pyne::level_struct>()
{

  // Loads the level table into memory
  herr_t status;

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(level_struct));
  status = H5Tinsert(desc, "nuc_id", HOFFSET(level_struct, nuc_id),
                      H5T_NATIVE_INT);
  status = H5Tinsert(desc, "rx_id", HOFFSET(level_struct, rx_id),
                     H5T_NATIVE_UINT);
  status = H5Tinsert(desc, "half_life", HOFFSET(level_struct, half_life),
                      H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "level", HOFFSET(level_struct, level), 
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "branch_ratio", HOFFSET(level_struct, branch_ratio),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "metastable", HOFFSET(level_struct, metastable),
                      H5T_NATIVE_INT);
  status = H5Tinsert(desc, "special", HOFFSET(level_struct, special),
                      H5T_C_S1);
  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY, 
                              H5P_DEFAULT);
  // Open the data set
  hid_t level_set = H5Dopen2(nuc_data_h5, "/decay/level_list", H5P_DEFAULT);
  hid_t level_space = H5Dget_space(level_set);
  int level_length = H5Sget_simple_extent_npoints(level_space);

  // Read in the data
  level_struct * level_array = new level_struct[level_length];
  status = H5Dread(level_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                   level_array);

  // close the nuc_data library, before doing anything stupid
  status = H5Dclose(level_set);
  status = H5Fclose(nuc_data_h5);

  for (int i = 0; i < level_length; ++i) {
    if (level_array[i].rx_id == 0)
      level_data_lvl_map[std::make_pair(level_array[i].nuc_id,
                                        level_array[i].level)] = level_array[i];
    else
      level_data_rx_map[std::make_pair(level_array[i].nuc_id,
                                       level_array[i].rx_id)] = level_array[i];
  }
  
  delete[] level_array;
}

//
// level id
//
int pyne::id_from_level(int nuc, double level, std::string special) {
  int nostate = (nuc / 10000) * 10000;
  if (level_data_lvl_map.empty()) {
    _load_data<level_struct>();
  }

  std::map<std::pair<int, double>, level_struct>::iterator nuc_lower, nuc_upper;

  nuc_lower = level_data_lvl_map.lower_bound(std::make_pair(nostate, 0.0));
  nuc_upper = level_data_lvl_map.upper_bound(std::make_pair(nostate+9999,
                                             DBL_MAX));
  double min = DBL_MAX;
  //by default return input nuc_id with level stripped
  int ret_id = nuc;
  for (std::map<std::pair<int, double>, level_struct>::iterator it=nuc_lower; 
  it!=nuc_upper;
       ++it) {
    if ((abs(level - it->second.level) < min) && 
    ((char)it->second.special == special.c_str()[0]) &&
    !isnan(it->second.level)) {
      min = abs(level - it->second.level);
      ret_id = it->second.nuc_id;
    }
  }
  if (min > 1.0)
    ret_id = nuc;
  return ret_id;
}

int pyne::id_from_level(int nuc, double level){
    return id_from_level(nuc, level, " ");
}
//
// Metastable id data
//

int pyne::metastable_id(int nuc, int m) {
  int nostate = (nuc / 10000) * 10000;
  if (m==0) return nostate;
  if (level_data_lvl_map.empty()) {
    _load_data<level_struct>();
  }

  std::map<std::pair<int, double>, level_struct>::iterator nuc_lower, nuc_upper;

  nuc_lower = level_data_lvl_map.lower_bound(std::make_pair(nostate, 0.0));
  nuc_upper = level_data_lvl_map.upper_bound(std::make_pair(nostate+9999, 
  DBL_MAX));
  for (std::map<std::pair<int, double>, level_struct>::iterator it=nuc_lower; 
  it!=nuc_upper; ++it) {
    if (it->second.metastable == m)
        return it->second.nuc_id;
  }

  return nuc;
}

int pyne::metastable_id(int nuc) {
  return metastable_id(nuc, 1);
}

//
// Decay children data
//


std::set<int> pyne::decay_children(int nuc) {
  std::vector<unsigned int> part = data_access<unsigned int, level_struct>(nuc,
    offsetof(level_struct, rx_id), level_data_rx_map);
  std::set<int> result;
  for (std::vector<unsigned int>::iterator it=part.begin(); it!=part.end(); 
  ++it) {
    if (*it == 36125)  
      result.insert((nuc /10000) * 10000);
    else 
      result.insert((rxname::child(nuc,*it,"decay") /10000) * 10000);
  }
  return result;
}

std::set<int> pyne::decay_children(char * nuc)
{
  return decay_children(nucname::id(nuc));
};

std::set<int> pyne::decay_children(std::string nuc)
{
  return decay_children(nucname::id(nuc));
};

//
// Excitation state energy data
//

double pyne::state_energy(int nuc)
{
  std::vector<double> result = data_access<double, level_struct>(nuc, 0.0, 
  DBL_MAX, offsetof(level_struct, level), level_data_lvl_map);
  if (result.size() == 1)
    return result[0]/1000.0;
  return 0.0;
}

double pyne::state_energy(char * nuc)
{
  return state_energy(nucname::id(nuc));
};


double pyne::state_energy(std::string nuc)
{
  return state_energy(nucname::id(nuc));
};


//
// Decay constant data
//

double pyne::decay_const(int nuc)
{  
    std::vector<double> result = data_access<double, level_struct>(nuc, 0.0,
      DBL_MAX, offsetof(level_struct, half_life), level_data_lvl_map);
    if (result.size() == 1) {
        return log(2.0)/result[0];
    }
    return 0.0;
}


double pyne::decay_const(char * nuc) {
  int nuc_zz = nucname::id(nuc);
  return decay_const(nuc_zz);
};


double pyne::decay_const(std::string nuc) {
  int nuc_zz = nucname::id(nuc);
  return decay_const(nuc_zz);
};


//
// Half-life data
//


double pyne::half_life(int nuc) {
    std::vector<double> result = data_access<double, level_struct>(nuc, 0.0,  
    DBL_MAX, offsetof(level_struct, half_life), level_data_lvl_map);
    if (result.size() == 1) {
        return result[0];
    }
    return 1.0/0.0;
};


double pyne::half_life(char * nuc) {
  int nuc_zz = nucname::id(nuc);
  return half_life(nuc_zz);
};

double pyne::half_life(std::string nuc) {
  int nuc_zz = nucname::id(nuc);
  return half_life(nuc_zz);
};

//
// Branch ratio data
//


double pyne::branch_ratio(std::pair<int, int> from_to) {
  std::vector<unsigned int> part1 = data_access<unsigned int, level_struct>(
    from_to.first, offsetof(level_struct, rx_id), level_data_rx_map);
  std::vector<double> part2 = data_access<double, level_struct>(from_to.first,
    offsetof(level_struct, branch_ratio), level_data_rx_map);
  double result = 0;
  if ((from_to.first == from_to.second) && 
      (half_life(from_to.first) == std::numeric_limits<double>::infinity()))
    return 1.0;
  for (std::vector<unsigned int>::size_type i=0; i < part1.size(); ++i) {
    if ((part1[i] != 0) && ((rxname::child(from_to.first,part1[i],"decay")
      / 10000) * 10000 == from_to.second))
      result = result + part2[i]*0.01;
    if ((part1[i] == 36125) && 
        (((from_to.first/10000)*10000) == ((from_to.second/10000)*10000)) && 
        (from_to.second % 10000 == 0)) { 
      return 1.0;
    }
  }
  
  return result;
}

double pyne::branch_ratio(int from_nuc, int to_nuc) {
  return branch_ratio(std::pair<int, int>(nucname::id(from_nuc), 
                                          nucname::id(to_nuc)));
};

double pyne::branch_ratio(char * from_nuc, char * to_nuc) {
  return branch_ratio(std::pair<int, int>(nucname::id(from_nuc), 
                                          nucname::id(to_nuc)));
};

double pyne::branch_ratio(std::string from_nuc, std::string to_nuc) {
  return branch_ratio(std::pair<int, int>(nucname::id(from_nuc), 
                                          nucname::id(to_nuc)));
};

std::map<std::pair<int, int>, pyne::decay_struct> pyne::decay_data = \
  std::map<std::pair<int, int>, pyne::decay_struct>();

template<> void pyne::_load_data<pyne::decay_struct>() {

  // Loads the decay table into memory
  herr_t status;

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(decay_struct));
  status = H5Tinsert(desc, "parent", HOFFSET(decay_struct, parent),
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "child", HOFFSET(decay_struct, child),
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "decay", HOFFSET(decay_struct, decay),
                     H5T_NATIVE_UINT);
  status = H5Tinsert(desc, "half_life", HOFFSET(decay_struct, half_life), 
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "half_life_error", HOFFSET(decay_struct, 
                     half_life_error), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "branch_ratio", HOFFSET(decay_struct, branch_ratio),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "photon_branch_ratio", HOFFSET(decay_struct, 
                     photon_branch_ratio), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "photon_branch_ratio_err", HOFFSET(decay_struct,
                     photon_branch_ratio_error), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "beta_branch_ratio", HOFFSET(decay_struct, 
                     beta_branch_ratio), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "beta_branch_ratio_err", HOFFSET(decay_struct,
                     beta_branch_ratio_error), H5T_NATIVE_DOUBLE);
  
  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY, 
                              H5P_DEFAULT);

  // Open the data set
  hid_t decay_set = H5Dopen2(nuc_data_h5, "/decay/decays", H5P_DEFAULT);
  hid_t decay_space = H5Dget_space(decay_set);
  int decay_length = H5Sget_simple_extent_npoints(decay_space);

  // Read in the data
  decay_struct * decay_array = new decay_struct[decay_length];
  status = H5Dread(decay_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                   decay_array);

  // close the nuc_data library, before doing anything stupid
  status = H5Dclose(decay_set);
  status = H5Fclose(nuc_data_h5);

  for (int i = 0; i < decay_length; ++i) {
    decay_data[std::make_pair(decay_array[i].parent, decay_array[i].child)] = \
      decay_array[i];
  }
  delete[] decay_array;
}

std::pair<double, double> pyne::decay_half_life(std::pair<int, int> from_to){
  return std::make_pair(data_access<double, decay_struct>(from_to, offsetof(
   decay_struct, half_life), decay_data), data_access<double, decay_struct>(
   from_to, offsetof(decay_struct, half_life_error), decay_data));
};

std::vector<std::pair<double, double> >pyne::decay_half_lifes(int parent) {
  std::vector<std::pair<double, double> > result;
  std::vector<double> part1 = data_access<double, decay_struct>(parent, 
    offsetof(decay_struct, half_life), decay_data);
  std::vector<double> part2 = data_access<double, decay_struct>(parent,
    offsetof(decay_struct, half_life_error), decay_data);
  for(int i = 0; i < part1.size(); ++i){
    result.push_back(std::make_pair(part1[i],part2[i]));
  }
  return result;
}

double pyne::decay_branch_ratio(std::pair<int, int> from_to) {
  return data_access<double, decay_struct>(from_to, offsetof(decay_struct,
    branch_ratio), decay_data);
};

std::vector<double> pyne::decay_branch_ratios(int parent) {
  return data_access<double, decay_struct>(parent, offsetof(decay_struct, 
    branch_ratio), decay_data);
}

std::pair<double, double> pyne::decay_photon_branch_ratio(std::pair<int,int> 
from_to) {
  return std::make_pair(data_access<double, decay_struct>(from_to, 
    offsetof(decay_struct, photon_branch_ratio), decay_data),
    data_access<double, decay_struct>(from_to, offsetof(decay_struct, 
    photon_branch_ratio_error), decay_data));
};

std::vector<std::pair<double, double> >pyne::decay_photon_branch_ratios(
int parent) {
  std::vector<std::pair<double, double> > result;
  std::vector<double> part1 = data_access<double, decay_struct>(parent, 
    offsetof(decay_struct, photon_branch_ratio), decay_data);
  std::vector<double> part2 = data_access<double, decay_struct>(parent, 
    offsetof(decay_struct, photon_branch_ratio_error), decay_data);
  for(int i = 0; i < part1.size(); ++i){
    result.push_back(std::make_pair(part1[i],part2[i]));
  }
  return result;
}

std::pair<double, double> pyne::decay_beta_branch_ratio(std::pair<int,int> 
from_to) {
  return std::make_pair(data_access<double, decay_struct>(from_to, 
    offsetof(decay_struct, beta_branch_ratio), decay_data),
    data_access<double, decay_struct>(from_to, offsetof(decay_struct, 
    beta_branch_ratio_error), decay_data));
};

std::vector<std::pair<double, double> >pyne::decay_beta_branch_ratios(
int parent) {
  std::vector<std::pair<double, double> > result;
  std::vector<double> part1 = data_access<double, decay_struct>(parent, 
    offsetof(decay_struct, beta_branch_ratio), decay_data);
  std::vector<double> part2 = data_access<double, decay_struct>(parent, 
    offsetof(decay_struct, beta_branch_ratio_error), decay_data);
  for(int i = 0; i < part1.size(); ++i){
    result.push_back(std::make_pair(part1[i],part2[i]));
  }
  return result;
}

std::map<std::pair<int, double>, pyne::gamma_struct> pyne::gamma_data;

template<> void pyne::_load_data<pyne::gamma_struct>() {

  // Loads the gamma table into memory
  herr_t status;

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(gamma_struct));
  status = H5Tinsert(desc, "from_nuc", HOFFSET(gamma_struct, from_nuc), 
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "to_nuc", HOFFSET(gamma_struct, to_nuc), 
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "parent_nuc", HOFFSET(gamma_struct, parent_nuc),
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "energy", HOFFSET(gamma_struct, energy),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "energy_err", HOFFSET(gamma_struct, energy_err),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "photon_intensity", HOFFSET(gamma_struct, 
                     photon_intensity), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "photon_intensity_err", HOFFSET(gamma_struct, 
                     photon_intensity_err), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "conv_intensity", HOFFSET(gamma_struct, 
                     conv_intensity), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "conv_intensity_err", HOFFSET(gamma_struct, 
                     conv_intensity_err), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "total_intensity", HOFFSET(gamma_struct, 
                     total_intensity), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "total_intensity_err", HOFFSET(gamma_struct, 
                     total_intensity_err), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "k_conv_e", HOFFSET(gamma_struct, k_conv_e), 
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "l_conv_e", HOFFSET(gamma_struct, l_conv_e), 
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "m_conv_e", HOFFSET(gamma_struct, m_conv_e), 
                     H5T_NATIVE_DOUBLE);


  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY, 
                              H5P_DEFAULT);

  // Open the data set
  hid_t gamma_set = H5Dopen2(nuc_data_h5, "/decay/gammas", H5P_DEFAULT);
  hid_t gamma_space = H5Dget_space(gamma_set);
  int gamma_length = H5Sget_simple_extent_npoints(gamma_space);

  // Read in the data
  gamma_struct * gamma_array = new gamma_struct[gamma_length];
  status = H5Dread(gamma_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                   gamma_array);

  // close the nuc_data library, before doing anything stupid
  status = H5Dclose(gamma_set);
  status = H5Fclose(nuc_data_h5);

  for (int i = 0; i < gamma_length; ++i) {
    gamma_data[std::make_pair(gamma_array[i].parent_nuc, 
      gamma_array[i].energy)] = gamma_array[i];
  }
  delete[] gamma_array;
}

std::vector<std::pair<double, double> > pyne::gamma_energy(int parent){
  std::vector<std::pair<double, double> > result;
  std::vector<double> part1 = data_access<double, gamma_struct>(parent, 0.0, 
    DBL_MAX, offsetof(gamma_struct, energy), gamma_data);
  std::vector<double> part2 = data_access<double, gamma_struct>(parent, 0.0, 
    DBL_MAX, offsetof(gamma_struct, energy_err), gamma_data);
  for(int i = 0; i < part1.size(); ++i){
    result.push_back(std::make_pair(part1[i],part2[i]));
  }
  return result;
};

std::vector<std::pair<double, double> > pyne::gamma_photon_intensity(
int parent) {
  std::vector<std::pair<double, double> > result;
  std::vector<double> part1 = data_access<double, gamma_struct>(parent, 0.0, 
    DBL_MAX, offsetof(gamma_struct, photon_intensity), gamma_data);
  std::vector<double> part2 = data_access<double, gamma_struct>(parent, 0.0, 
    DBL_MAX, offsetof(gamma_struct, photon_intensity_err), gamma_data);
  for(int i = 0; i < part1.size(); ++i){
    result.push_back(std::make_pair(part1[i],part2[i]));
  }
  return result;
};

std::vector<std::pair<double, double> > pyne::gamma_photon_intensity(
double energy, double error) {
  std::vector<std::pair<double, double> > result;
  std::vector<double> part1 = data_access<double, gamma_struct>(energy+error,
    energy-error, offsetof(gamma_struct, photon_intensity), gamma_data);
  std::vector<double> part2 = data_access<double, gamma_struct>(energy+error,
    energy-error, offsetof(gamma_struct, photon_intensity_err), gamma_data);
  for(int i = 0; i < part1.size(); ++i){
    result.push_back(std::make_pair(part1[i],part2[i]));
  }
  return result;
};

std::vector<std::pair<double, double> > pyne::gamma_conversion_intensity(
int parent) {
  std::vector<std::pair<double, double> > result;
  std::vector<double> part1 = data_access<double, gamma_struct>(parent, 0.0, 
    DBL_MAX, offsetof(gamma_struct, conv_intensity), gamma_data);
  std::vector<double> part2 = data_access<double, gamma_struct>(parent, 0.0, 
    DBL_MAX, offsetof(gamma_struct, conv_intensity_err), gamma_data);
  for(int i = 0; i < part1.size(); ++i){
    result.push_back(std::make_pair(part1[i],part2[i]));
  }
  return result;
};

std::vector<std::pair<double, double> > pyne::gamma_total_intensity(
int parent) {
  std::vector<std::pair<double, double> > result;
  std::vector<double> part1 = data_access<double, gamma_struct>(parent, 0.0, 
    DBL_MAX, offsetof(gamma_struct, total_intensity), gamma_data);
  std::vector<double> part2 = data_access<double, gamma_struct>(parent, 0.0, 
    DBL_MAX, offsetof(gamma_struct, total_intensity_err), gamma_data);
  for(int i = 0; i < part1.size(); ++i){
    result.push_back(std::make_pair(part1[i],part2[i]));
  }
  return result;
};

std::vector<std::pair<int, int> > pyne::gamma_from_to(int parent) {
  std::vector<std::pair<int, int> > result;
  std::vector<int> part1 = data_access<int, gamma_struct>(parent, 0.0, DBL_MAX,
    offsetof(gamma_struct, from_nuc), gamma_data);
  std::vector<int> part2 = data_access<int, gamma_struct>(parent, 0.0, DBL_MAX,
    offsetof(gamma_struct, to_nuc), gamma_data);
  for(int i = 0; i < part1.size(); ++i){
    result.push_back(std::make_pair(part1[i],part2[i]));
  }
  return result;
};

std::vector<std::pair<int, int> > pyne::gamma_from_to(double energy, 
double error) {
  std::vector<std::pair<int, int> > result;
  std::vector<int> part1 = data_access<int, gamma_struct>(energy+error,
    energy-error, offsetof(gamma_struct, from_nuc), gamma_data);
  std::vector<int> part2 = data_access<int, gamma_struct>(energy+error,
    energy-error, offsetof(gamma_struct, to_nuc), gamma_data);
  for(int i = 0; i < part1.size(); ++i){
    result.push_back(std::make_pair(part1[i],part2[i]));
  }
  return result;
};

std::vector<int> pyne::gamma_parent(double energy, double error) {
  return data_access<int, gamma_struct>(energy+error, energy-error,
    offsetof(gamma_struct, parent_nuc), gamma_data);
};


std::vector<std::vector<std::pair<double, double> > > 
  pyne::gamma_xrays(int parent) {
  std::vector<std::vector<std::pair<double, double> > > result;
  std::vector<double> k_list = data_access<double, gamma_struct>(parent, 0.0, DBL_MAX,
    offsetof(gamma_struct, k_conv_e), gamma_data);
  std::vector<double> l_list = data_access<double, gamma_struct>(parent, 0.0, DBL_MAX,
    offsetof(gamma_struct, l_conv_e), gamma_data);
  std::vector<int> nuc_list = data_access<int, gamma_struct>(parent, 0.0, 
    DBL_MAX, offsetof(gamma_struct, from_nuc), gamma_data);
  for(int i = 0; i < k_list.size(); ++i){
    result.push_back(calculate_xray_data(nucname::znum(nuc_list[i]), k_list[i], 
                                         l_list[i]));
  }
  return result;
};


std::map<std::pair<int, double>, pyne::alpha_struct> pyne::alpha_data;

template<> void pyne::_load_data<pyne::alpha_struct>() {

  // Loads the alpha table into memory
  herr_t status;

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(alpha_struct));
  status = H5Tinsert(desc, "from_nuc", HOFFSET(alpha_struct, from_nuc),
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "to_nuc", HOFFSET(alpha_struct, to_nuc),
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "energy", HOFFSET(alpha_struct, energy),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "intensity", HOFFSET(alpha_struct, intensity),
                     H5T_NATIVE_DOUBLE);


  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY,
                              H5P_DEFAULT);

  // Open the data set
  hid_t alpha_set = H5Dopen2(nuc_data_h5, "/decay/alphas", H5P_DEFAULT);
  hid_t alpha_space = H5Dget_space(alpha_set);
  int alpha_length = H5Sget_simple_extent_npoints(alpha_space);

  // Read in the data
  alpha_struct * alpha_array = new alpha_struct[alpha_length];
  status = H5Dread(alpha_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                   alpha_array);

  // close the nuc_data library, before doing anything stupid
  status = H5Dclose(alpha_set);
  status = H5Fclose(nuc_data_h5);

  for (int i = 0; i < alpha_length; ++i) {
    alpha_data[std::make_pair(alpha_array[i].from_nuc, alpha_array[i].energy)]
    = alpha_array[i];
  }
  delete[] alpha_array;
}

std::vector<double > pyne::alpha_energy(int parent){
  return data_access<double, alpha_struct>(parent, 0.0, DBL_MAX, 
                     offsetof(alpha_struct,energy), alpha_data);
};
std::vector<double> pyne::alpha_intensity(int parent){
  return data_access<double, alpha_struct>(parent, 0.0, DBL_MAX, 
                     offsetof(alpha_struct,intensity), alpha_data);
};

std::vector<int> pyne::alpha_parent(double energy, double error) {
  return data_access<int, alpha_struct>(energy+error, energy-error, 
                     offsetof(alpha_struct, from_nuc), alpha_data);
};

std::vector<int> pyne::alpha_child(double energy, double error) {
  return data_access<int, alpha_struct>(energy+error, energy-error, 
                     offsetof(alpha_struct, to_nuc), alpha_data);
};

std::vector<int> pyne::alpha_child(int parent){
  return data_access<int, alpha_struct>(parent, 0.0, DBL_MAX,
                     offsetof(alpha_struct, to_nuc), alpha_data);
};

std::map<std::pair<int, double>, pyne::beta_struct> pyne::beta_data;

template<> void pyne::_load_data<pyne::beta_struct>() {

  // Loads the beta table into memory
  herr_t status;

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(beta_struct));
  status = H5Tinsert(desc, "endpoint_energy", HOFFSET(beta_struct, 
                     endpoint_energy), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "avg_energy", HOFFSET(beta_struct, avg_energy),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "intensity", HOFFSET(beta_struct, intensity),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "from_nuc", HOFFSET(beta_struct, from_nuc),
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "to_nuc", HOFFSET(beta_struct, to_nuc), 
                     H5T_NATIVE_INT);


  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY,
                              H5P_DEFAULT);

  // Open the data set
  hid_t beta_set = H5Dopen2(nuc_data_h5, "/decay/betas", H5P_DEFAULT);
  hid_t beta_space = H5Dget_space(beta_set);
  int beta_length = H5Sget_simple_extent_npoints(beta_space);

  // Read in the data
  beta_struct * beta_array = new beta_struct[beta_length];
  status = H5Dread(beta_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, beta_array);

  // close the nuc_data library, before doing anything stupid
  status = H5Dclose(beta_set);
  status = H5Fclose(nuc_data_h5);

  for (int i = 0; i < beta_length; ++i) {
    beta_data[std::make_pair(beta_array[i].from_nuc, beta_array[i].avg_energy)]
    = beta_array[i];
  }
  delete[] beta_array;
}

std::vector<double > pyne::beta_endpoint_energy(int parent){
  return data_access<double, beta_struct>(parent, 0.0, DBL_MAX,  
                     offsetof(beta_struct, endpoint_energy), beta_data);
};

std::vector<double > pyne::beta_average_energy(int parent){
  return data_access<double, beta_struct>(parent, 0.0, DBL_MAX,  
                     offsetof(beta_struct, avg_energy), beta_data);
};

std::vector<double> pyne::beta_intensity(int parent){
  return data_access<double, beta_struct>(parent, 0.0, DBL_MAX, 
                     offsetof(beta_struct, intensity), beta_data);
};

std::vector<int> pyne::beta_parent(double energy, double error) {
  return data_access<int, beta_struct>(energy+error, energy-error, 
                     offsetof(beta_struct, from_nuc), beta_data);
};

std::vector<int> pyne::beta_child(double energy, double error) {
  return data_access<int, beta_struct>(energy+error, energy-error, 
                     offsetof(beta_struct, to_nuc), beta_data);
};

std::vector<int> pyne::beta_child(int parent){
  return data_access<int, beta_struct>(parent, 0.0, DBL_MAX,  
                     offsetof(beta_struct, to_nuc),beta_data);
};


std::map<std::pair<int, double>, pyne::ecbp_struct> pyne::ecbp_data;

template<> void pyne::_load_data<pyne::ecbp_struct>() {

  // Loads the ecbp table into memory
  herr_t status;

  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(ecbp_struct));
  status = H5Tinsert(desc, "from_nuc", HOFFSET(ecbp_struct, from_nuc),
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "to_nuc", HOFFSET(ecbp_struct, to_nuc),
                     H5T_NATIVE_INT);
  status = H5Tinsert(desc, "endpoint_energy", HOFFSET(ecbp_struct,
                     endpoint_energy),H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "avg_energy", HOFFSET(ecbp_struct, avg_energy),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "beta_plus_intensity", HOFFSET(ecbp_struct, 
                     beta_plus_intensity), H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "ec_intensity", HOFFSET(ecbp_struct, ec_intensity),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "k_conv_e", HOFFSET(ecbp_struct, k_conv_e),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "l_conv_e", HOFFSET(ecbp_struct, l_conv_e),
                     H5T_NATIVE_DOUBLE);
  status = H5Tinsert(desc, "m_conv_e", HOFFSET(ecbp_struct, m_conv_e),
                     H5T_NATIVE_DOUBLE);

  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY,
                              H5P_DEFAULT);

  // Open the data set
  hid_t ecbp_set = H5Dopen2(nuc_data_h5, "/decay/ecbp", H5P_DEFAULT);
  hid_t ecbp_space = H5Dget_space(ecbp_set);
  int ecbp_length = H5Sget_simple_extent_npoints(ecbp_space);

  // Read in the data
  ecbp_struct * ecbp_array = new ecbp_struct[ecbp_length];
  status = H5Dread(ecbp_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, ecbp_array);

  // close the nuc_data library, before doing anything stupid
  status = H5Dclose(ecbp_set);
  status = H5Fclose(nuc_data_h5);

  for (int i = 0; i < ecbp_length; ++i) {
    ecbp_data[std::make_pair(ecbp_array[i].from_nuc, ecbp_array[i].avg_energy)]
    = ecbp_array[i];
  }
  delete[] ecbp_array;
}

std::vector<double > pyne::ecbp_endpoint_energy(int parent){
  return data_access<double, ecbp_struct>(parent, 0.0, DBL_MAX,  
                     offsetof(ecbp_struct,endpoint_energy), ecbp_data);
};

std::vector<double > pyne::ecbp_average_energy(int parent){
  return data_access<double, ecbp_struct>(parent, 0.0, DBL_MAX, 
                     offsetof(ecbp_struct, avg_energy), ecbp_data);
};

std::vector<double> pyne::ec_intensity(int parent){
  return data_access<double, ecbp_struct>(parent, 0.0, DBL_MAX, 
                     offsetof(ecbp_struct, ec_intensity), ecbp_data);
};

std::vector<double> pyne::bp_intensity(int parent){
  return data_access<double, ecbp_struct>(parent, 0.0, DBL_MAX,  
                     offsetof(ecbp_struct, beta_plus_intensity), ecbp_data);
};

std::vector<int> pyne::ecbp_parent(double energy, double error) {
  return data_access<int, ecbp_struct>(energy+error, energy-error,
                     offsetof(ecbp_struct, from_nuc), ecbp_data);
};

std::vector<int> pyne::ecbp_child(double energy, double error) {
  return data_access<int, ecbp_struct>(energy+error, energy-error, 
                     offsetof(ecbp_struct, to_nuc), ecbp_data);
};

std::vector<int> pyne::ecbp_child(int parent){
  return data_access<int, ecbp_struct>(parent, 0.0, DBL_MAX, 
                     offsetof(ecbp_struct, to_nuc), ecbp_data);
};

std::vector<std::vector<std::pair<double, double> > > 
  pyne::ecbp_xrays(int parent) {
  std::vector<std::vector<std::pair<double, double> > > result;
  std::vector<double> k_list = data_access<double, ecbp_struct>(parent, 0.0, DBL_MAX,
    offsetof(ecbp_struct, k_conv_e), ecbp_data);
  std::vector<double> l_list = data_access<double, ecbp_struct>(parent, 0.0, DBL_MAX,
    offsetof(ecbp_struct, l_conv_e), ecbp_data);
  std::vector<int> nuc_list = data_access<int, ecbp_struct>(parent, 0.0, 
    DBL_MAX, offsetof(ecbp_struct, from_nuc), ecbp_data);
  for(int i = 0; i < k_list.size(); ++i){
    result.push_back(calculate_xray_data(nucname::znum(nuc_list[i]), k_list[i], 
                                         l_list[i]));
  }
  return result;
};

//////////////////////////////////////////
//////////// simple xs data //////////////
//////////////////////////////////////////

typedef struct simple_xs_struct {
  int nuc;
  double sigma_t;
  double sigma_s;
  double sigma_e;
  double sigma_i;
  double sigma_a;
  double sigma_gamma;
  double sigma_f;
  double sigma_alpha;
  double sigma_proton;
  double sigma_deut;
  double sigma_trit;
  double sigma_2n;
  double sigma_3n;
  double sigma_4n;
} simple_xs_struct;

std::map<std::string, std::map<int, std::map<int, double> > > pyne::simple_xs_map;

// loads the simple cross section data for the specified energy band from
// the nuc_data.h5 file into memory.
static void _load_simple_xs_map(std::string energy) {
  //Check to see if the file is in HDF5 format.
  if (!pyne::file_exists(pyne::NUC_DATA_PATH))
    throw pyne::FileNotFound(pyne::NUC_DATA_PATH);

  bool ish5 = H5Fis_hdf5(pyne::NUC_DATA_PATH.c_str());
  if (!ish5)
    throw h5wrap::FileNotHDF5(pyne::NUC_DATA_PATH);

  using pyne::rxname::id;
  std::map<unsigned int, size_t> rxns;
  rxns[id("tot")] = offsetof(simple_xs_struct, sigma_t);
  rxns[id("scat")] = offsetof(simple_xs_struct, sigma_s);
  rxns[id("elas")] = offsetof(simple_xs_struct, sigma_e);
  rxns[id("inel")] = offsetof(simple_xs_struct, sigma_i);
  rxns[id("abs")] = offsetof(simple_xs_struct, sigma_a);
  rxns[id("gamma")] = offsetof(simple_xs_struct, sigma_gamma);
  rxns[id("fiss")] = offsetof(simple_xs_struct, sigma_f);
  rxns[id("alpha")] = offsetof(simple_xs_struct, sigma_alpha);
  rxns[id("proton")] = offsetof(simple_xs_struct, sigma_proton);
  rxns[id("deut")] = offsetof(simple_xs_struct, sigma_deut);
  rxns[id("trit")] = offsetof(simple_xs_struct, sigma_trit);
  rxns[id("z_2n")] = offsetof(simple_xs_struct, sigma_2n);
  rxns[id("z_3n")] = offsetof(simple_xs_struct, sigma_3n);
  rxns[id("z_4n")] = offsetof(simple_xs_struct, sigma_4n);

  // Get the HDF5 compound type (table) description
  hid_t desc = H5Tcreate(H5T_COMPOUND, sizeof(simple_xs_struct));
  H5Tinsert(desc, "nuc",   HOFFSET(simple_xs_struct, nuc),   H5T_NATIVE_INT);
  H5Tinsert(desc, "sigma_t",  HOFFSET(simple_xs_struct, sigma_t),  H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_s", HOFFSET(simple_xs_struct, sigma_s), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_e", HOFFSET(simple_xs_struct, sigma_e), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_i", HOFFSET(simple_xs_struct, sigma_i), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_a", HOFFSET(simple_xs_struct, sigma_a), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_gamma", HOFFSET(simple_xs_struct, sigma_gamma), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_f", HOFFSET(simple_xs_struct, sigma_f), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_alpha", HOFFSET(simple_xs_struct, sigma_alpha), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_proton", HOFFSET(simple_xs_struct, sigma_proton), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_deut", HOFFSET(simple_xs_struct, sigma_deut), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_trit", HOFFSET(simple_xs_struct, sigma_trit), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_2n", HOFFSET(simple_xs_struct, sigma_2n), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_3n", HOFFSET(simple_xs_struct, sigma_3n), H5T_NATIVE_DOUBLE);
  H5Tinsert(desc, "sigma_4n", HOFFSET(simple_xs_struct, sigma_4n), H5T_NATIVE_DOUBLE);

  // Open the HDF5 file
  hid_t nuc_data_h5 = H5Fopen(pyne::NUC_DATA_PATH.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

  // build path to prober simple xs table
  std::string path = "/neutron/simple_xs/" + energy;

  // Open the data set
  hid_t simple_xs_set = H5Dopen2(nuc_data_h5, path.c_str(), H5P_DEFAULT);
  hid_t simple_xs_space = H5Dget_space(simple_xs_set);
  int n = H5Sget_simple_extent_npoints(simple_xs_space);

  // Read in the data
  simple_xs_struct* array = new simple_xs_struct[n];
  H5Dread(simple_xs_set, desc, H5S_ALL, H5S_ALL, H5P_DEFAULT, array);

  // close the nuc_data library, before doing anything stupid
  H5Dclose(simple_xs_set);
  H5Fclose(nuc_data_h5);

  // Ok now that we have the array of stucts, put it in the map
  for(int i = 0; i < n; i++) {
    std::map<unsigned int, size_t>::iterator it;
    for (it = rxns.begin(); it != rxns.end(); ++it) {
      double xs = *(double*)((char*)&array[i] + it->second);
      pyne::simple_xs_map[energy][array[i].nuc][it->first] = xs;
    }
  }
  delete[] array;
}

double pyne::simple_xs(int nuc, int rx_id, std::string energy) {
  std::set<std::string> energies;
  energies.insert("thermal");
  energies.insert("thermal_maxwell_ave");
  energies.insert("resonance_integral");
  energies.insert("fourteen_MeV");
  energies.insert("fission_spectrum_ave");

  if (energies.count(energy) == 0) {
    throw InvalidSimpleXS("Energy '" + energy + 
        "' is not a valid simple_xs group");
  } else if (simple_xs_map.count(energy) == 0) {
    _load_simple_xs_map(energy);
  }

  if (simple_xs_map[energy].count(nuc) == 0) {
    throw InvalidSimpleXS(rxname::name(rx_id) + 
        " is not a valid simple_xs nuclide");
  } else if (simple_xs_map[energy][nuc].count(rx_id) == 0) {
    throw InvalidSimpleXS(rxname::name(rx_id) + 
        " is not a valid simple_xs reaction");
  }

  return simple_xs_map[energy][nuc][rx_id];
}

double pyne::simple_xs(int nuc, std::string rx, std::string energy) {
  return pyne::simple_xs(nucname::id(nuc), rxname::id(rx), energy);
}
double pyne::simple_xs(std::string nuc, int rx, std::string energy) {
  return pyne::simple_xs(nucname::id(nuc), rxname::id(rx), energy);
}
double pyne::simple_xs(std::string nuc, std::string rx, std::string energy) {
  return pyne::simple_xs(nucname::id(nuc), rxname::id(rx), energy);
}

//
// end of cpp/data.cpp
//


//
// start of cpp/jsoncpp.cpp
//
/// Json-cpp amalgated source (http://jsoncpp.sourceforge.net/).
/// It is intented to be used with #include <json/json.h>

// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: LICENSE
// //////////////////////////////////////////////////////////////////////

/*
The JsonCpp library's source code, including accompanying documentation, 
tests and demonstration applications, are licensed under the following
conditions...

The author (Baptiste Lepilleur) explicitly disclaims copyright in all 
jurisdictions which recognize such a disclaimer. In such jurisdictions, 
this software is released into the Public Domain.

In jurisdictions which do not recognize Public Domain property (e.g. Germany as of
2010), this software is Copyright (c) 2007-2010 by Baptiste Lepilleur, and is
released under the terms of the MIT License (see below).

In jurisdictions which recognize Public Domain property, the user of this 
software may choose to accept it either as 1) Public Domain, 2) under the 
conditions of the MIT License (see below), or 3) under the terms of dual 
Public Domain/MIT License conditions described here, as they choose.

The MIT License is about as close to Public Domain as a license can get, and is
described in clear, concise terms at:

   http://en.wikipedia.org/wiki/MIT_License
   
The full text of the MIT License follows:

========================================================================
Copyright (c) 2007-2010 Baptiste Lepilleur

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
========================================================================
(END LICENSE TEXT)

The MIT license is compatible with both the GPL and commercial
software, affording one all of the rights of Public Domain with the
minor nuisance of being required to keep the above copyright notice
and license text in the source code. Note also that by accepting the
Public Domain "license" you can re-license your copy using whatever
license you like.

*/

// //////////////////////////////////////////////////////////////////////
// End of content of file: LICENSE
// //////////////////////////////////////////////////////////////////////





#ifdef PYNE_IS_AMALGAMATED
  #if !defined(JSON_IS_AMALGAMATION)
    #define JSON_IS_AMALGAMATION
  #endif
#else
  #include <json/json.h>
#endif


// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_tool.h
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef LIB_JSONCPP_JSON_TOOL_H_INCLUDED
# define LIB_JSONCPP_JSON_TOOL_H_INCLUDED

/* This header provides common string manipulation support, such as UTF-8,
 * portable conversion from/to string...
 *
 * It is an internal header that must not be exposed.
 */

namespace Json {

/// Converts a unicode code-point to UTF-8.
static inline std::string 
codePointToUTF8(unsigned int cp) {
   std::string result;
   
   // based on description from http://en.wikipedia.org/wiki/UTF-8

   if (cp <= 0x7f) {
      result.resize(1);
      result[0] = static_cast<char>(cp);
   } else if (cp <= 0x7FF) {
      result.resize(2);
      result[1] = static_cast<char>(0x80 | (0x3f & cp));
      result[0] = static_cast<char>(0xC0 | (0x1f & (cp >> 6)));
   } else if (cp <= 0xFFFF) {
      result.resize(3);
      result[2] = static_cast<char>(0x80 | (0x3f & cp));
      result[1] = 0x80 | static_cast<char>((0x3f & (cp >> 6)));
      result[0] = 0xE0 | static_cast<char>((0xf & (cp >> 12)));
   } else if (cp <= 0x10FFFF) {
      result.resize(4);
      result[3] = static_cast<char>(0x80 | (0x3f & cp));
      result[2] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
      result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 12)));
      result[0] = static_cast<char>(0xF0 | (0x7 & (cp >> 18)));
   }

   return result;
}


/// Returns true if ch is a control character (in range [0,32[).
static inline bool 
isControlCharacter(char ch) {
   return ch > 0 && ch <= 0x1F;
}


enum { 
   /// Constant that specify the size of the buffer that must be passed to uintToString.
   uintToStringBufferSize = 3*sizeof(LargestUInt)+1 
};

// Defines a char buffer for use with uintToString().
typedef char UIntToStringBuffer[uintToStringBufferSize];


/** Converts an unsigned integer to string.
 * @param value Unsigned interger to convert to string
 * @param current Input/Output string buffer. 
 *        Must have at least uintToStringBufferSize chars free.
 */
static inline void 
uintToString( LargestUInt value, 
              char *&current ) {
   *--current = 0;
   do {
      *--current = char(value % 10) + '0';
      value /= 10;
   }
   while ( value != 0 );
}

} // namespace Json {

#endif // LIB_JSONCPP_JSON_TOOL_H_INCLUDED

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_tool.h
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_reader.cpp
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#if !defined(JSON_IS_AMALGAMATION)
# include <json/reader.h>
# include <json/value.h>
# include "json_tool.h"
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <utility>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>

#if _MSC_VER >= 1400 // VC++ 8.0
#pragma warning( disable : 4996 )   // disable warning about strdup being deprecated.
#endif

namespace Json {

// Implementation of class Features
// ////////////////////////////////

Features::Features()
   : allowComments_( true )
   , strictRoot_( false ) {
}


Features 
Features::all() {
   return Features();
}


Features 
Features::strictMode() {
   Features features;
   features.allowComments_ = false;
   features.strictRoot_ = true;
   return features;
}

// Implementation of class Reader
// ////////////////////////////////


static inline bool 
in( Reader::Char c, Reader::Char c1, Reader::Char c2, Reader::Char c3, Reader::Char c4 ) {
   return c == c1  ||  c == c2  ||  c == c3  ||  c == c4;
}

static inline bool 
in( Reader::Char c, Reader::Char c1, Reader::Char c2, Reader::Char c3, Reader::Char c4, Reader::Char c5 ) {
   return c == c1  ||  c == c2  ||  c == c3  ||  c == c4  ||  c == c5;
}


static bool 
containsNewLine( Reader::Location begin, 
                 Reader::Location end ) {
   for ( ;begin < end; ++begin )
      if ( *begin == '\n'  ||  *begin == '\r' )
         return true;
   return false;
}


// Class Reader
// //////////////////////////////////////////////////////////////////

Reader::Reader()
   : features_( Features::all() ) {
}


Reader::Reader( const Features &features )
   : features_( features ) {
}


bool
Reader::parse( const std::string &document, 
               Value &root,
               bool collectComments ) {
   document_ = document;
   const char *begin = document_.c_str();
   const char *end = begin + document_.length();
   return parse( begin, end, root, collectComments );
}


bool
Reader::parse( std::istream& sin,
               Value &root,
               bool collectComments ) {
   //std::istream_iterator<char> begin(sin);
   //std::istream_iterator<char> end;
   // Those would allow streamed input from a file, if parse() were a
   // template function.

   // Since std::string is reference-counted, this at least does not
   // create an extra copy.
   std::string doc;
   std::getline(sin, doc, (char)EOF);
   return parse( doc, root, collectComments );
}

bool 
Reader::parse( const char *beginDoc, const char *endDoc, 
               Value &root,
               bool collectComments ) {
   if ( !features_.allowComments_ ) {
      collectComments = false;
   }

   begin_ = beginDoc;
   end_ = endDoc;
   collectComments_ = collectComments;
   current_ = begin_;
   lastValueEnd_ = 0;
   lastValue_ = 0;
   commentsBefore_ = "";
   errors_.clear();
   while ( !nodes_.empty() )
      nodes_.pop();
   nodes_.push( &root );
   
   bool successful = readValue();
   Token token;
   skipCommentTokens( token );
   if ( collectComments_  &&  !commentsBefore_.empty() )
      root.setComment( commentsBefore_, commentAfter );
   if ( features_.strictRoot_ ) {
      if ( !root.isArray()  &&  !root.isObject() ) {
         // Set error location to start of doc, ideally should be first token found in doc
         token.type_ = tokenError;
         token.start_ = beginDoc;
         token.end_ = endDoc;
         addError( "A valid JSON document must be either an array or an object value.",
                   token );
         return false;
      }
   }
   return successful;
}


bool
Reader::readValue() {
   Token token;
   skipCommentTokens( token );
   bool successful = true;

   if ( collectComments_  &&  !commentsBefore_.empty() ) {
      currentValue().setComment( commentsBefore_, commentBefore );
      commentsBefore_ = "";
   }


   switch ( token.type_ ) {
   case tokenObjectBegin:
      successful = readObject( token );
      break;
   case tokenArrayBegin:
      successful = readArray( token );
      break;
   case tokenNumber:
      successful = decodeNumber( token );
      break;
   case tokenString:
      successful = decodeString( token );
      break;
   case tokenTrue:
      currentValue() = true;
      break;
   case tokenFalse:
      currentValue() = false;
      break;
   case tokenNull:
      currentValue() = Value();
      break;
   default:
      return addError( "Syntax error: value, object or array expected.", token );
   }

   if ( collectComments_ ) {
      lastValueEnd_ = current_;
      lastValue_ = &currentValue();
   }

   return successful;
}


void 
Reader::skipCommentTokens( Token &token ) {
   if ( features_.allowComments_ ) {
      do {
         readToken( token );
      }
      while ( token.type_ == tokenComment );
   } else {
      readToken( token );
   }
}


bool 
Reader::expectToken( TokenType type, Token &token, const char *message ) {
   readToken( token );
   if ( token.type_ != type )
      return addError( message, token );
   return true;
}


bool 
Reader::readToken( Token &token ) {
   skipSpaces();
   token.start_ = current_;
   Char c = getNextChar();
   bool ok = true;
   switch ( c ) {
   case '{':
      token.type_ = tokenObjectBegin;
      break;
   case '}':
      token.type_ = tokenObjectEnd;
      break;
   case '[':
      token.type_ = tokenArrayBegin;
      break;
   case ']':
      token.type_ = tokenArrayEnd;
      break;
   case '"':
      token.type_ = tokenString;
      ok = readString();
      break;
   case '/':
      token.type_ = tokenComment;
      ok = readComment();
      break;
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
   case '-':
      token.type_ = tokenNumber;
      readNumber();
      break;
   case 't':
      token.type_ = tokenTrue;
      ok = match( "rue", 3 );
      break;
   case 'f':
      token.type_ = tokenFalse;
      ok = match( "alse", 4 );
      break;
   case 'n':
      token.type_ = tokenNull;
      ok = match( "ull", 3 );
      break;
   case ',':
      token.type_ = tokenArraySeparator;
      break;
   case ':':
      token.type_ = tokenMemberSeparator;
      break;
   case 0:
      token.type_ = tokenEndOfStream;
      break;
   default:
      ok = false;
      break;
   }
   if ( !ok )
      token.type_ = tokenError;
   token.end_ = current_;
   return true;
}


void 
Reader::skipSpaces() {
   while ( current_ != end_ ) {
      Char c = *current_;
      if ( c == ' '  ||  c == '\t'  ||  c == '\r'  ||  c == '\n' )
         ++current_;
      else
         break;
   }
}


bool 
Reader::match( Location pattern, 
               int patternLength ) {
   if ( end_ - current_ < patternLength )
      return false;
   int index = patternLength;
   while ( index-- )
      if ( current_[index] != pattern[index] )
         return false;
   current_ += patternLength;
   return true;
}


bool
Reader::readComment() {
   Location commentBegin = current_ - 1;
   Char c = getNextChar();
   bool successful = false;
   if ( c == '*' )
      successful = readCStyleComment();
   else if ( c == '/' )
      successful = readCppStyleComment();
   if ( !successful )
      return false;

   if ( collectComments_ ) {
      CommentPlacement placement = commentBefore;
      if ( lastValueEnd_  &&  !containsNewLine( lastValueEnd_, commentBegin ) )
      {
         if ( c != '*'  ||  !containsNewLine( commentBegin, current_ ) )
            placement = commentAfterOnSameLine;
      }

      addComment( commentBegin, current_, placement );
   }
   return true;
}


void 
Reader::addComment( Location begin, 
                    Location end, 
                    CommentPlacement placement ) {
   assert( collectComments_ );
   if ( placement == commentAfterOnSameLine )
   {
      assert( lastValue_ != 0 );
      lastValue_->setComment( std::string( begin, end ), placement );
   } else {
      if ( !commentsBefore_.empty() )
         commentsBefore_ += "\n";
      commentsBefore_ += std::string( begin, end );
   }
}


bool 
Reader::readCStyleComment() {
   while ( current_ != end_ ) {
      Char c = getNextChar();
      if ( c == '*'  &&  *current_ == '/' )
         break;
   }
   return getNextChar() == '/';
}


bool 
Reader::readCppStyleComment() {
   while ( current_ != end_ ) {
      Char c = getNextChar();
      if (  c == '\r'  ||  c == '\n' )
         break;
   }
   return true;
}


void 
Reader::readNumber() {
   while ( current_ != end_ ) {
      if ( !(*current_ >= '0'  &&  *current_ <= '9')  &&
           !in( *current_, '.', 'e', 'E', '+', '-' ) )
         break;
      ++current_;
   }
}

bool
Reader::readString() {
   Char c = 0;
   while ( current_ != end_ ) {
      c = getNextChar();
      if ( c == '\\' )
         getNextChar();
      else if ( c == '"' )
         break;
   }
   return c == '"';
}


bool 
Reader::readObject( Token &/*tokenStart*/ ) {
   Token tokenName;
   std::string name;
   currentValue() = Value( objectValue );
   while ( readToken( tokenName ) ) {
      bool initialTokenOk = true;
      while ( tokenName.type_ == tokenComment  &&  initialTokenOk )
         initialTokenOk = readToken( tokenName );
      if  ( !initialTokenOk )
         break;
      if ( tokenName.type_ == tokenObjectEnd  &&  name.empty() )  // empty object
         return true;
      if ( tokenName.type_ != tokenString )
         break;
      
      name = "";
      if ( !decodeString( tokenName, name ) )
         return recoverFromError( tokenObjectEnd );

      Token colon;
      if ( !readToken( colon ) ||  colon.type_ != tokenMemberSeparator ) {
         return addErrorAndRecover( "Missing ':' after object member name", 
                                    colon, 
                                    tokenObjectEnd );
      }
      Value &value = currentValue()[ name ];
      nodes_.push( &value );
      bool ok = readValue();
      nodes_.pop();
      if ( !ok ) // error already set
         return recoverFromError( tokenObjectEnd );

      Token comma;
      if ( !readToken( comma )
            ||  ( comma.type_ != tokenObjectEnd  &&  
                  comma.type_ != tokenArraySeparator &&
                  comma.type_ != tokenComment ) ) {
         return addErrorAndRecover( "Missing ',' or '}' in object declaration", 
                                    comma, 
                                    tokenObjectEnd );
      }
      bool finalizeTokenOk = true;
      while ( comma.type_ == tokenComment &&
              finalizeTokenOk )
         finalizeTokenOk = readToken( comma );
      if ( comma.type_ == tokenObjectEnd )
         return true;
   }
   return addErrorAndRecover( "Missing '}' or object member name", 
                              tokenName, 
                              tokenObjectEnd );
}


bool 
Reader::readArray( Token &/*tokenStart*/ ) {
   currentValue() = Value( arrayValue );
   skipSpaces();
   if ( *current_ == ']' ) { // empty array
      Token endArray;
      readToken( endArray );
      return true;
   }
   int index = 0;
   for (;;) {
      Value &value = currentValue()[ index++ ];
      nodes_.push( &value );
      bool ok = readValue();
      nodes_.pop();
      if ( !ok ) // error already set
         return recoverFromError( tokenArrayEnd );

      Token token;
      // Accept Comment after last item in the array.
      ok = readToken( token );
      while ( token.type_ == tokenComment  &&  ok ) {
         ok = readToken( token );
      }
      bool badTokenType = ( token.type_ != tokenArraySeparator  &&
                            token.type_ != tokenArrayEnd );
      if ( !ok  ||  badTokenType ) {
         return addErrorAndRecover( "Missing ',' or ']' in array declaration", 
                                    token, 
                                    tokenArrayEnd );
      }
      if ( token.type_ == tokenArrayEnd )
         break;
   }
   return true;
}


bool 
Reader::decodeNumber( Token &token ) {
   bool isDouble = false;
   for ( Location inspect = token.start_; inspect != token.end_; ++inspect )
   {
      isDouble = isDouble  
                 ||  in( *inspect, '.', 'e', 'E', '+' )  
                 ||  ( *inspect == '-'  &&  inspect != token.start_ );
   }
   if ( isDouble )
      return decodeDouble( token );
   // Attempts to parse the number as an integer. If the number is
   // larger than the maximum supported value of an integer then
   // we decode the number as a double.
   Location current = token.start_;
   bool isNegative = *current == '-';
   if ( isNegative )
      ++current;
   Value::LargestUInt maxIntegerValue = isNegative ? Value::LargestUInt(-Value::minLargestInt) 
                                                   : Value::maxLargestUInt;
   Value::LargestUInt threshold = maxIntegerValue / 10;
   Value::UInt lastDigitThreshold = Value::UInt( maxIntegerValue % 10 );
   assert( lastDigitThreshold >=0  &&  lastDigitThreshold <= 9 );
   Value::LargestUInt value = 0;
   while ( current < token.end_ ) {
      Char c = *current++;
      if ( c < '0'  ||  c > '9' )
         return addError( "'" + std::string( token.start_, token.end_ ) + "' is not a number.", token );
      Value::UInt digit(c - '0');
      if ( value >= threshold ) {
         // If the current digit is not the last one, or if it is
         // greater than the last digit of the maximum integer value,
         // the parse the number as a double.
         if ( current != token.end_  ||  digit > lastDigitThreshold ) {
            return decodeDouble( token );
         }
      }
      value = value * 10 + digit;
   }
   if ( isNegative )
      currentValue() = -Value::LargestInt( value );
   else if ( value <= Value::LargestUInt(Value::maxInt) )
      currentValue() = Value::LargestInt( value );
   else
      currentValue() = value;
   return true;
}


bool 
Reader::decodeDouble( Token &token ) {
   double value = 0;
   const int bufferSize = 32;
   int count;
   int length = int(token.end_ - token.start_);
   if ( length <= bufferSize ) {
      Char buffer[bufferSize+1];
      memcpy( buffer, token.start_, length );
      buffer[length] = 0;
      count = sscanf( buffer, "%lf", &value );
   } else {
      std::string buffer( token.start_, token.end_ );
      count = sscanf( buffer.c_str(), "%lf", &value );
   }

   if ( count != 1 )
      return addError( "'" + std::string( token.start_, token.end_ ) + "' is not a number.", token );
   currentValue() = value;
   return true;
}


bool 
Reader::decodeString( Token &token ) {
   std::string decoded;
   if ( !decodeString( token, decoded ) )
      return false;
   currentValue() = decoded;
   return true;
}


bool 
Reader::decodeString( Token &token, std::string &decoded ) {
   decoded.reserve( token.end_ - token.start_ - 2 );
   Location current = token.start_ + 1; // skip '"'
   Location end = token.end_ - 1;      // do not include '"'
   while ( current != end ) {
      Char c = *current++;
      if ( c == '"' )
         break;
      else if ( c == '\\' ) {
         if ( current == end )
            return addError( "Empty escape sequence in string", token, current );
         Char escape = *current++;
         switch ( escape ) {
         case '"': decoded += '"'; break;
         case '/': decoded += '/'; break;
         case '\\': decoded += '\\'; break;
         case 'b': decoded += '\b'; break;
         case 'f': decoded += '\f'; break;
         case 'n': decoded += '\n'; break;
         case 'r': decoded += '\r'; break;
         case 't': decoded += '\t'; break;
         case 'u': {
               unsigned int unicode;
               if ( !decodeUnicodeCodePoint( token, current, end, unicode ) )
                  return false;
               decoded += codePointToUTF8(unicode);
            }
            break;
         default:
            return addError( "Bad escape sequence in string", token, current );
         }
      }
      else {
         decoded += c;
      }
   }
   return true;
}

bool
Reader::decodeUnicodeCodePoint( Token &token, 
                                     Location &current, 
                                     Location end, 
                                     unsigned int &unicode ) {

   if ( !decodeUnicodeEscapeSequence( token, current, end, unicode ) )
      return false;
   if (unicode >= 0xD800 && unicode <= 0xDBFF) {
      // surrogate pairs
      if (end - current < 6)
         return addError( "additional six characters expected to parse unicode surrogate pair.", token, current );
      unsigned int surrogatePair;
      if (*(current++) == '\\' && *(current++)== 'u') {
         if (decodeUnicodeEscapeSequence( token, current, end, surrogatePair )) {
            unicode = 0x10000 + ((unicode & 0x3FF) << 10) + (surrogatePair & 0x3FF);
         } else
            return false;
      } else
         return addError( "expecting another \\u token to begin the second half of a unicode surrogate pair", token, current );
   }
   return true;
}

bool 
Reader::decodeUnicodeEscapeSequence( Token &token, 
                                     Location &current, 
                                     Location end, 
                                     unsigned int &unicode ) {
   if ( end - current < 4 )
      return addError( "Bad unicode escape sequence in string: four digits expected.", token, current );
   unicode = 0;
   for ( int index =0; index < 4; ++index ) {
      Char c = *current++;
      unicode *= 16;
      if ( c >= '0'  &&  c <= '9' )
         unicode += c - '0';
      else if ( c >= 'a'  &&  c <= 'f' )
         unicode += c - 'a' + 10;
      else if ( c >= 'A'  &&  c <= 'F' )
         unicode += c - 'A' + 10;
      else
         return addError( "Bad unicode escape sequence in string: hexadecimal digit expected.", token, current );
   }
   return true;
}


bool 
Reader::addError( const std::string &message, 
                  Token &token,
                  Location extra ) {
   ErrorInfo info;
   info.token_ = token;
   info.message_ = message;
   info.extra_ = extra;
   errors_.push_back( info );
   return false;
}


bool 
Reader::recoverFromError( TokenType skipUntilToken ) {
   int errorCount = int(errors_.size());
   Token skip;
   for (;;) {
      if ( !readToken(skip) )
         errors_.resize( errorCount ); // discard errors caused by recovery
      if ( skip.type_ == skipUntilToken  ||  skip.type_ == tokenEndOfStream )
         break;
   }
   errors_.resize( errorCount );
   return false;
}


bool 
Reader::addErrorAndRecover( const std::string &message, 
                            Token &token,
                            TokenType skipUntilToken ) {
   addError( message, token );
   return recoverFromError( skipUntilToken );
}


Value &
Reader::currentValue() {
   return *(nodes_.top());
}


Reader::Char 
Reader::getNextChar() {
   if ( current_ == end_ )
      return 0;
   return *current_++;
}


void 
Reader::getLocationLineAndColumn( Location location,
                                  int &line,
                                  int &column ) const {
   Location current = begin_;
   Location lastLineStart = current;
   line = 0;
   while ( current < location  &&  current != end_ ) {
      Char c = *current++;
      if ( c == '\r' ) {
         if ( *current == '\n' )
            ++current;
         lastLineStart = current;
         ++line;
      } else if ( c == '\n' ) {
         lastLineStart = current;
         ++line;
      }
   }
   // column & line start at 1
   column = int(location - lastLineStart) + 1;
   ++line;
}


std::string
Reader::getLocationLineAndColumn( Location location ) const {
   int line, column;
   getLocationLineAndColumn( location, line, column );
   char buffer[18+16+16+1];
   sprintf( buffer, "Line %d, Column %d", line, column );
   return buffer;
}


// Deprecated. Preserved for backward compatibility
std::string 
Reader::getFormatedErrorMessages() const {
    return getFormattedErrorMessages();
}


std::string 
Reader::getFormattedErrorMessages() const {
   std::string formattedMessage;
   for ( Errors::const_iterator itError = errors_.begin();
         itError != errors_.end();
         ++itError ) {
      const ErrorInfo &error = *itError;
      formattedMessage += "* " + getLocationLineAndColumn( error.token_.start_ ) + "\n";
      formattedMessage += "  " + error.message_ + "\n";
      if ( error.extra_ )
         formattedMessage += "See " + getLocationLineAndColumn( error.extra_ ) + " for detail.\n";
   }
   return formattedMessage;
}


std::istream& operator>>( std::istream &sin, Value &root ) {
    Json::Reader reader;
    bool ok = reader.parse(sin, root, true);
    //JSON_ASSERT( ok );
    if (!ok) throw std::runtime_error(reader.getFormattedErrorMessages());
    return sin;
}


} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_reader.cpp
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_batchallocator.h
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSONCPP_BATCHALLOCATOR_H_INCLUDED
# define JSONCPP_BATCHALLOCATOR_H_INCLUDED

# include <stdlib.h>
# include <assert.h>

# ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION

namespace Json {

/* Fast memory allocator.
 *
 * This memory allocator allocates memory for a batch of object (specified by
 * the page size, the number of object in each page).
 *
 * It does not allow the destruction of a single object. All the allocated objects
 * can be destroyed at once. The memory can be either released or reused for future
 * allocation.
 * 
 * The in-place new operator must be used to construct the object using the pointer
 * returned by allocate.
 */
template<typename AllocatedType
        ,const unsigned int objectPerAllocation>
class BatchAllocator {
public:
   typedef AllocatedType Type;

   BatchAllocator( unsigned int objectsPerPage = 255 )
      : freeHead_( 0 )
      , objectsPerPage_( objectsPerPage ) {
//      printf( "Size: %d => %s\n", sizeof(AllocatedType), typeid(AllocatedType).name() );
      assert( sizeof(AllocatedType) * objectPerAllocation >= sizeof(AllocatedType *) ); // We must be able to store a slist in the object free space.
      assert( objectsPerPage >= 16 );
      batches_ = allocateBatch( 0 );   // allocated a dummy page
      currentBatch_ = batches_;
   }

   ~BatchAllocator() {
      for ( BatchInfo *batch = batches_; batch;  ) {
         BatchInfo *nextBatch = batch->next_;
         free( batch );
         batch = nextBatch;
      }
   }

   /// allocate space for an array of objectPerAllocation object.
   /// @warning it is the responsability of the caller to call objects constructors.
   AllocatedType *allocate() {
      if ( freeHead_ ) { // returns node from free list.
         AllocatedType *object = freeHead_;
         freeHead_ = *(AllocatedType **)object;
         return object;
      }
      if ( currentBatch_->used_ == currentBatch_->end_ ) {
         currentBatch_ = currentBatch_->next_;
         while ( currentBatch_  &&  currentBatch_->used_ == currentBatch_->end_ )
            currentBatch_ = currentBatch_->next_;

         if ( !currentBatch_  ) { // no free batch found, allocate a new one
            currentBatch_ = allocateBatch( objectsPerPage_ );
            currentBatch_->next_ = batches_; // insert at the head of the list
            batches_ = currentBatch_;
         }
      }
      AllocatedType *allocated = currentBatch_->used_;
      currentBatch_->used_ += objectPerAllocation;
      return allocated;
   }

   /// Release the object.
   /// @warning it is the responsability of the caller to actually destruct the object.
   void release( AllocatedType *object ) {
      assert( object != 0 );
      *(AllocatedType **)object = freeHead_;
      freeHead_ = object;
   }

private:
   struct BatchInfo {
      BatchInfo *next_;
      AllocatedType *used_;
      AllocatedType *end_;
      AllocatedType buffer_[objectPerAllocation];
   };

   // disabled copy constructor and assignement operator.
   BatchAllocator( const BatchAllocator & );
   void operator =( const BatchAllocator &);

   static BatchInfo *allocateBatch( unsigned int objectsPerPage ) {
      const unsigned int mallocSize = sizeof(BatchInfo) - sizeof(AllocatedType)* objectPerAllocation
                                + sizeof(AllocatedType) * objectPerAllocation * objectsPerPage;
      BatchInfo *batch = static_cast<BatchInfo*>( malloc( mallocSize ) );
      batch->next_ = 0;
      batch->used_ = batch->buffer_;
      batch->end_ = batch->buffer_ + objectsPerPage;
      return batch;
   }

   BatchInfo *batches_;
   BatchInfo *currentBatch_;
   /// Head of a single linked list within the allocated space of freeed object
   AllocatedType *freeHead_;
   unsigned int objectsPerPage_;
};


} // namespace Json

# endif // ifndef JSONCPP_DOC_INCLUDE_IMPLEMENTATION

#endif // JSONCPP_BATCHALLOCATOR_H_INCLUDED


// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_batchallocator.h
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_valueiterator.inl
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

// included by json_value.cpp

namespace Json {

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class ValueIteratorBase
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

ValueIteratorBase::ValueIteratorBase()
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   : current_()
   , isNull_( true ) {
}
#else
   : isArray_( true )
   , isNull_( true ) {
   iterator_.array_ = ValueInternalArray::IteratorState();
}
#endif


#ifndef JSON_VALUE_USE_INTERNAL_MAP
ValueIteratorBase::ValueIteratorBase( const Value::ObjectValues::iterator &current )
   : current_( current )
   , isNull_( false ) {
}
#else
ValueIteratorBase::ValueIteratorBase( const ValueInternalArray::IteratorState &state )
   : isArray_( true ) {
   iterator_.array_ = state;
}


ValueIteratorBase::ValueIteratorBase( const ValueInternalMap::IteratorState &state )
   : isArray_( false ) {
   iterator_.map_ = state;
}
#endif

Value &
ValueIteratorBase::deref() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   return current_->second;
#else
   if ( isArray_ )
      return ValueInternalArray::dereference( iterator_.array_ );
   return ValueInternalMap::value( iterator_.map_ );
#endif
}


void 
ValueIteratorBase::increment() {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   ++current_;
#else
   if ( isArray_ )
      ValueInternalArray::increment( iterator_.array_ );
   ValueInternalMap::increment( iterator_.map_ );
#endif
}


void 
ValueIteratorBase::decrement() {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   --current_;
#else
   if ( isArray_ )
      ValueInternalArray::decrement( iterator_.array_ );
   ValueInternalMap::decrement( iterator_.map_ );
#endif
}


ValueIteratorBase::difference_type 
ValueIteratorBase::computeDistance( const SelfType &other ) const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
# ifdef JSON_USE_CPPTL_SMALLMAP
   return current_ - other.current_;
# else
   // Iterator for null value are initialized using the default
   // constructor, which initialize current_ to the default
   // std::map::iterator. As begin() and end() are two instance 
   // of the default std::map::iterator, they can not be compared.
   // To allow this, we handle this comparison specifically.
   if ( isNull_  &&  other.isNull_ ) {
      return 0;
   }


   // Usage of std::distance is not portable (does not compile with Sun Studio 12 RogueWave STL,
   // which is the one used by default).
   // Using a portable hand-made version for non random iterator instead:
   //   return difference_type( std::distance( current_, other.current_ ) );
   difference_type myDistance = 0;
   for ( Value::ObjectValues::iterator it = current_; it != other.current_; ++it ) {
      ++myDistance;
   }
   return myDistance;
# endif
#else
   if ( isArray_ )
      return ValueInternalArray::distance( iterator_.array_, other.iterator_.array_ );
   return ValueInternalMap::distance( iterator_.map_, other.iterator_.map_ );
#endif
}


bool 
ValueIteratorBase::isEqual( const SelfType &other ) const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   if ( isNull_ ) {
      return other.isNull_;
   }
   return current_ == other.current_;
#else
   if ( isArray_ )
      return ValueInternalArray::equals( iterator_.array_, other.iterator_.array_ );
   return ValueInternalMap::equals( iterator_.map_, other.iterator_.map_ );
#endif
}


void 
ValueIteratorBase::copy( const SelfType &other ) {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   current_ = other.current_;
#else
   if ( isArray_ )
      iterator_.array_ = other.iterator_.array_;
   iterator_.map_ = other.iterator_.map_;
#endif
}


Value 
ValueIteratorBase::key() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   const Value::CZString czstring = (*current_).first;
   if ( czstring.c_str() ) {
      if ( czstring.isStaticString() )
         return Value( StaticString( czstring.c_str() ) );
      return Value( czstring.c_str() );
   }
   return Value( czstring.index() );
#else
   if ( isArray_ )
      return Value( ValueInternalArray::indexOf( iterator_.array_ ) );
   bool isStatic;
   const char *memberName = ValueInternalMap::key( iterator_.map_, isStatic );
   if ( isStatic )
      return Value( StaticString( memberName ) );
   return Value( memberName );
#endif
}


UInt 
ValueIteratorBase::index() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   const Value::CZString czstring = (*current_).first;
   if ( !czstring.c_str() )
      return czstring.index();
   return Value::UInt( -1 );
#else
   if ( isArray_ )
      return Value::UInt( ValueInternalArray::indexOf( iterator_.array_ ) );
   return Value::UInt( -1 );
#endif
}


const char *
ValueIteratorBase::memberName() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   const char *name = (*current_).first.c_str();
   return name ? name : "";
#else
   if ( !isArray_ )
      return ValueInternalMap::key( iterator_.map_ );
   return "";
#endif
}


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class ValueConstIterator
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

ValueConstIterator::ValueConstIterator() {
}


#ifndef JSON_VALUE_USE_INTERNAL_MAP
ValueConstIterator::ValueConstIterator( const Value::ObjectValues::iterator &current )
   : ValueIteratorBase( current ) {
}
#else
ValueConstIterator::ValueConstIterator( const ValueInternalArray::IteratorState &state )
   : ValueIteratorBase( state ) {
}

ValueConstIterator::ValueConstIterator( const ValueInternalMap::IteratorState &state )
   : ValueIteratorBase( state ) {
}
#endif

ValueConstIterator &
ValueConstIterator::operator =( const ValueIteratorBase &other ) {
   copy( other );
   return *this;
}


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class ValueIterator
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

ValueIterator::ValueIterator() {
}


#ifndef JSON_VALUE_USE_INTERNAL_MAP
ValueIterator::ValueIterator( const Value::ObjectValues::iterator &current )
   : ValueIteratorBase( current ) {
}
#else
ValueIterator::ValueIterator( const ValueInternalArray::IteratorState &state )
   : ValueIteratorBase( state ) {
}

ValueIterator::ValueIterator( const ValueInternalMap::IteratorState &state )
   : ValueIteratorBase( state ) {
}
#endif

ValueIterator::ValueIterator( const ValueConstIterator &other )
   : ValueIteratorBase( other ) {
}

ValueIterator::ValueIterator( const ValueIterator &other )
   : ValueIteratorBase( other ) {
}

ValueIterator &
ValueIterator::operator =( const SelfType &other ) {
   copy( other );
   return *this;
}

} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_valueiterator.inl
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_value.cpp
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#if !defined(JSON_IS_AMALGAMATION)
# include <json/value.h>
# include <json/writer.h>
# ifndef JSON_USE_SIMPLE_INTERNAL_ALLOCATOR
#  include "json_batchallocator.h"
# endif // #ifndef JSON_USE_SIMPLE_INTERNAL_ALLOCATOR
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <iostream>
#include <utility>
#include <stdexcept>
#include <cstring>
#include <cassert>
#ifdef JSON_USE_CPPTL
# include <cpptl/conststring.h>
#endif
#include <cstddef>    // size_t

#define JSON_ASSERT_UNREACHABLE assert( false )
#define JSON_ASSERT( condition ) assert( condition );  // @todo <= change this into an exception throw
#define JSON_FAIL_MESSAGE( message ) throw std::runtime_error( message );
#define JSON_ASSERT_MESSAGE( condition, message ) if (!( condition )) JSON_FAIL_MESSAGE( message )

namespace Json {

const Value Value::null;
const Int Value::minInt = Int( ~(UInt(-1)/2) );
const Int Value::maxInt = Int( UInt(-1)/2 );
const UInt Value::maxUInt = UInt(-1);
const Int64 Value::minInt64 = Int64( ~(UInt64(-1)/2) );
const Int64 Value::maxInt64 = Int64( UInt64(-1)/2 );
const UInt64 Value::maxUInt64 = UInt64(-1);
const LargestInt Value::minLargestInt = LargestInt( ~(LargestUInt(-1)/2) );
const LargestInt Value::maxLargestInt = LargestInt( LargestUInt(-1)/2 );
const LargestUInt Value::maxLargestUInt = LargestUInt(-1);


/// Unknown size marker
static const unsigned int unknown = (unsigned)-1;


/** Duplicates the specified string value.
 * @param value Pointer to the string to duplicate. Must be zero-terminated if
 *              length is "unknown".
 * @param length Length of the value. if equals to unknown, then it will be
 *               computed using strlen(value).
 * @return Pointer on the duplicate instance of string.
 */
static inline char *
duplicateStringValue( const char *value, 
                      unsigned int length = unknown ) {
   if ( length == unknown )
      length = (unsigned int)strlen(value);
   char *newString = static_cast<char *>( malloc( length + 1 ) );
   JSON_ASSERT_MESSAGE( newString != 0, "Failed to allocate string value buffer" );
   memcpy( newString, value, length );
   newString[length] = 0;
   return newString;
}


/** Free the string duplicated by duplicateStringValue().
 */
static inline void 
releaseStringValue( char *value ) {
   if ( value )
      free( value );
}

} // namespace Json


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// ValueInternals...
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
#if !defined(JSON_IS_AMALGAMATION)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
#  include "json_internalarray.inl"
#  include "json_internalmap.inl"
# endif // JSON_VALUE_USE_INTERNAL_MAP

# include "json_valueiterator.inl"
#endif // if !defined(JSON_IS_AMALGAMATION)

namespace Json {

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class Value::CommentInfo
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////


Value::CommentInfo::CommentInfo()
   : comment_( 0 ) {
}

Value::CommentInfo::~CommentInfo() {
   if ( comment_ )
      releaseStringValue( comment_ );
}


void 
Value::CommentInfo::setComment( const char *text ) {
   if ( comment_ )
      releaseStringValue( comment_ );
   JSON_ASSERT( text != 0 );
   JSON_ASSERT_MESSAGE( text[0]=='\0' || text[0]=='/', "Comments must start with /");
   // It seems that /**/ style comments are acceptable as well.
   comment_ = duplicateStringValue( text );
}


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class Value::CZString
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
# ifndef JSON_VALUE_USE_INTERNAL_MAP

// Notes: index_ indicates if the string was allocated when
// a string is stored.

Value::CZString::CZString( ArrayIndex index )
   : cstr_( 0 )
   , index_( index ) {
}

Value::CZString::CZString( const char *cstr, DuplicationPolicy allocate )
   : cstr_( allocate == duplicate ? duplicateStringValue(cstr) 
                                  : cstr )
   , index_( allocate ) {
}

Value::CZString::CZString( const CZString &other )
: cstr_( other.index_ != noDuplication &&  other.cstr_ != 0
                ?  duplicateStringValue( other.cstr_ )
                : other.cstr_ )
   , index_( other.cstr_ ? (other.index_ == noDuplication ? noDuplication : duplicate)
                         : other.index_ ) {
}

Value::CZString::~CZString() {
   if ( cstr_  &&  index_ == duplicate )
      releaseStringValue( const_cast<char *>( cstr_ ) );
}

void 
Value::CZString::swap( CZString &other ) {
   std::swap( cstr_, other.cstr_ );
   std::swap( index_, other.index_ );
}

Value::CZString &
Value::CZString::operator =( const CZString &other ) {
   CZString temp( other );
   swap( temp );
   return *this;
}

bool 
Value::CZString::operator<( const CZString &other ) const  {
   if ( cstr_ )
      return strcmp( cstr_, other.cstr_ ) < 0;
   return index_ < other.index_;
}

bool 
Value::CZString::operator==( const CZString &other ) const  {
   if ( cstr_ )
      return strcmp( cstr_, other.cstr_ ) == 0;
   return index_ == other.index_;
}


ArrayIndex 
Value::CZString::index() const {
   return index_;
}


const char *
Value::CZString::c_str() const {
   return cstr_;
}

bool 
Value::CZString::isStaticString() const {
   return index_ == noDuplication;
}

#endif // ifndef JSON_VALUE_USE_INTERNAL_MAP


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class Value::Value
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/*! \internal Default constructor initialization must be equivalent to:
 * memset( this, 0, sizeof(Value) )
 * This optimization is used in ValueInternalMap fast allocator.
 */
Value::Value( ValueType type )
   : type_( type )
   , allocated_( 0 )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   switch ( type ) {
   case nullValue:
      break;
   case intValue:
   case uintValue:
      value_.int_ = 0;
      break;
   case realValue:
      value_.real_ = 0.0;
      break;
   case stringValue:
      value_.string_ = 0;
      break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   case arrayValue:
   case objectValue:
      value_.map_ = new ObjectValues();
      break;
#else
   case arrayValue:
      value_.array_ = arrayAllocator()->newArray();
      break;
   case objectValue:
      value_.map_ = mapAllocator()->newMap();
      break;
#endif
   case booleanValue:
      value_.bool_ = false;
      break;
   default:
      JSON_ASSERT_UNREACHABLE;
   }
}


#if defined(JSON_HAS_INT64)
Value::Value( UInt value )
   : type_( uintValue )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   value_.uint_ = value;
}

Value::Value( Int value )
   : type_( intValue )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   value_.int_ = value;
}

#endif // if defined(JSON_HAS_INT64)


Value::Value( Int64 value )
   : type_( intValue )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   value_.int_ = value;
}


Value::Value( UInt64 value )
   : type_( uintValue )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   value_.uint_ = value;
}

Value::Value( double value )
   : type_( realValue )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   value_.real_ = value;
}

Value::Value( const char *value )
   : type_( stringValue )
   , allocated_( true )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   value_.string_ = duplicateStringValue( value );
}


Value::Value( const char *beginValue, 
              const char *endValue )
   : type_( stringValue )
   , allocated_( true )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   value_.string_ = duplicateStringValue( beginValue, 
                                          (unsigned int)(endValue - beginValue) );
}


Value::Value( const std::string &value )
   : type_( stringValue )
   , allocated_( true )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif 
{
   value_.string_ = duplicateStringValue( value.c_str(), 
                                          (unsigned int)value.length() );

}

Value::Value( const StaticString &value )
   : type_( stringValue )
   , allocated_( false )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   value_.string_ = const_cast<char *>( value.c_str() );
}


# ifdef JSON_USE_CPPTL
Value::Value( const CppTL::ConstString &value )
   : type_( stringValue )
   , allocated_( true )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   value_.string_ = duplicateStringValue( value, value.length() );
}
# endif

Value::Value( bool value )
   : type_( booleanValue )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   value_.bool_ = value;
}


Value::Value( const Value &other )
   : type_( other.type_ )
   , comments_( 0 )
# ifdef JSON_VALUE_USE_INTERNAL_MAP
   , itemIsUsed_( 0 )
#endif
{
   switch ( type_ ) {
   case nullValue:
   case intValue:
   case uintValue:
   case realValue:
   case booleanValue:
      value_ = other.value_;
      break;
   case stringValue:
      if ( other.value_.string_ ) {
         value_.string_ = duplicateStringValue( other.value_.string_ );
         allocated_ = true;
      }
      else
         value_.string_ = 0;
      break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   case arrayValue:
   case objectValue:
      value_.map_ = new ObjectValues( *other.value_.map_ );
      break;
#else
   case arrayValue:
      value_.array_ = arrayAllocator()->newArrayCopy( *other.value_.array_ );
      break;
   case objectValue:
      value_.map_ = mapAllocator()->newMapCopy( *other.value_.map_ );
      break;
#endif
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   if ( other.comments_ ) {
      comments_ = new CommentInfo[numberOfCommentPlacement];
      for ( int comment =0; comment < numberOfCommentPlacement; ++comment ) {
         const CommentInfo &otherComment = other.comments_[comment];
         if ( otherComment.comment_ )
            comments_[comment].setComment( otherComment.comment_ );
      }
   }
}


Value::~Value() {
   switch ( type_ ) {
   case nullValue:
   case intValue:
   case uintValue:
   case realValue:
   case booleanValue:
      break;
   case stringValue:
      if ( allocated_ )
         releaseStringValue( value_.string_ );
      break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   case arrayValue:
   case objectValue:
      delete value_.map_;
      break;
#else
   case arrayValue:
      arrayAllocator()->destructArray( value_.array_ );
      break;
   case objectValue:
      mapAllocator()->destructMap( value_.map_ );
      break;
#endif
   default:
      JSON_ASSERT_UNREACHABLE;
   }

   if ( comments_ )
      delete[] comments_;
}

Value &
Value::operator=( const Value &other ) {
   Value temp( other );
   swap( temp );
   return *this;
}

void 
Value::swap( Value &other ) {
   ValueType temp = type_;
   type_ = other.type_;
   other.type_ = temp;
   std::swap( value_, other.value_ );
   int temp2 = allocated_;
   allocated_ = other.allocated_;
   other.allocated_ = temp2;
}

ValueType 
Value::type() const {
   return type_;
}


int 
Value::compare( const Value &other ) const {
   if ( *this < other )
      return -1;
   if ( *this > other )
      return 1;
   return 0;
}


bool 
Value::operator <( const Value &other ) const {
   int typeDelta = type_ - other.type_;
   if ( typeDelta )
      return typeDelta < 0 ? true : false;
   switch ( type_ ) {
   case nullValue:
      return false;
   case intValue:
      return value_.int_ < other.value_.int_;
   case uintValue:
      return value_.uint_ < other.value_.uint_;
   case realValue:
      return value_.real_ < other.value_.real_;
   case booleanValue:
      return value_.bool_ < other.value_.bool_;
   case stringValue:
      return ( value_.string_ == 0  &&  other.value_.string_ )
             || ( other.value_.string_  
                  &&  value_.string_  
                  && strcmp( value_.string_, other.value_.string_ ) < 0 );
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   case arrayValue:
   case objectValue: {
         int delta = int( value_.map_->size() - other.value_.map_->size() );
         if ( delta )
            return delta < 0;
         return (*value_.map_) < (*other.value_.map_);
      }
#else
   case arrayValue:
      return value_.array_->compare( *(other.value_.array_) ) < 0;
   case objectValue:
      return value_.map_->compare( *(other.value_.map_) ) < 0;
#endif
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return false;  // unreachable
}

bool 
Value::operator <=( const Value &other ) const {
   return !(other < *this);
}

bool 
Value::operator >=( const Value &other ) const {
   return !(*this < other);
}

bool 
Value::operator >( const Value &other ) const {
   return other < *this;
}

bool 
Value::operator ==( const Value &other ) const {
   //if ( type_ != other.type_ )
   // GCC 2.95.3 says:
   // attempt to take address of bit-field structure member `Json::Value::type_'
   // Beats me, but a temp solves the problem.
   int temp = other.type_;
   if ( type_ != temp )
      return false;
   switch ( type_ ) {
   case nullValue:
      return true;
   case intValue:
      return value_.int_ == other.value_.int_;
   case uintValue:
      return value_.uint_ == other.value_.uint_;
   case realValue:
      return value_.real_ == other.value_.real_;
   case booleanValue:
      return value_.bool_ == other.value_.bool_;
   case stringValue:
      return ( value_.string_ == other.value_.string_ )
             || ( other.value_.string_  
                  &&  value_.string_  
                  && strcmp( value_.string_, other.value_.string_ ) == 0 );
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   case arrayValue:
   case objectValue:
      return value_.map_->size() == other.value_.map_->size()
             && (*value_.map_) == (*other.value_.map_);
#else
   case arrayValue:
      return value_.array_->compare( *(other.value_.array_) ) == 0;
   case objectValue:
      return value_.map_->compare( *(other.value_.map_) ) == 0;
#endif
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return false;  // unreachable
}

bool 
Value::operator !=( const Value &other ) const
{
   return !( *this == other );
}

const char *
Value::asCString() const {
   JSON_ASSERT( type_ == stringValue );
   return value_.string_;
}


std::string 
Value::asString() const {
   switch ( type_ ) {
   case nullValue:
      return "";
   case stringValue:
      return value_.string_ ? value_.string_ : "";
   case booleanValue:
      return value_.bool_ ? "true" : "false";
   case intValue:
   case uintValue:
   case realValue:
   case arrayValue:
   case objectValue:
      JSON_FAIL_MESSAGE( "Type is not convertible to string" );
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return ""; // unreachable
}

# ifdef JSON_USE_CPPTL
CppTL::ConstString 
Value::asConstString() const
{
   return CppTL::ConstString( asString().c_str() );
}
# endif


Value::Int 
Value::asInt() const {
   switch ( type_ )
   {
   case nullValue:
      return 0;
   case intValue:
      JSON_ASSERT_MESSAGE( value_.int_ >= minInt  &&  value_.int_ <= maxInt, "unsigned integer out of signed int range" );
      return Int(value_.int_);
   case uintValue:
      JSON_ASSERT_MESSAGE( value_.uint_ <= UInt(maxInt), "unsigned integer out of signed int range" );
      return Int(value_.uint_);
   case realValue:
      JSON_ASSERT_MESSAGE( value_.real_ >= minInt  &&  value_.real_ <= maxInt, "Real out of signed integer range" );
      return Int( value_.real_ );
   case booleanValue:
      return value_.bool_ ? 1 : 0;
   case stringValue:
   case arrayValue:
   case objectValue:
      JSON_FAIL_MESSAGE( "Type is not convertible to int" );
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return 0; // unreachable;
}


Value::UInt 
Value::asUInt() const {
   switch ( type_ ) {
   case nullValue:
      return 0;
   case intValue:
      JSON_ASSERT_MESSAGE( value_.int_ >= 0, "Negative integer can not be converted to unsigned integer" );
      JSON_ASSERT_MESSAGE( value_.int_ <= maxUInt, "signed integer out of UInt range" );
      return UInt(value_.int_);
   case uintValue:
      JSON_ASSERT_MESSAGE( value_.uint_ <= maxUInt, "unsigned integer out of UInt range" );
      return UInt(value_.uint_);
   case realValue:
      JSON_ASSERT_MESSAGE( value_.real_ >= 0  &&  value_.real_ <= maxUInt,  "Real out of unsigned integer range" );
      return UInt( value_.real_ );
   case booleanValue:
      return value_.bool_ ? 1 : 0;
   case stringValue:
   case arrayValue:
   case objectValue:
      JSON_FAIL_MESSAGE( "Type is not convertible to uint" );
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return 0; // unreachable;
}


# if defined(JSON_HAS_INT64)

Value::Int64
Value::asInt64() const {
   switch ( type_ ) {
   case nullValue:
      return 0;
   case intValue:
      return value_.int_;
   case uintValue:
      JSON_ASSERT_MESSAGE( value_.uint_ <= UInt64(maxInt64), "unsigned integer out of Int64 range" );
      return value_.uint_;
   case realValue:
      JSON_ASSERT_MESSAGE( value_.real_ >= minInt64  &&  value_.real_ <= maxInt64, "Real out of Int64 range" );
      return Int( value_.real_ );
   case booleanValue:
      return value_.bool_ ? 1 : 0;
   case stringValue:
   case arrayValue:
   case objectValue:
      JSON_FAIL_MESSAGE( "Type is not convertible to Int64" );
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return 0; // unreachable;
}


Value::UInt64
Value::asUInt64() const {
   switch ( type_ ) {
   case nullValue:
      return 0;
   case intValue:
      JSON_ASSERT_MESSAGE( value_.int_ >= 0, "Negative integer can not be converted to UInt64" );
      return value_.int_;
   case uintValue:
      return value_.uint_;
   case realValue:
      JSON_ASSERT_MESSAGE( value_.real_ >= 0  &&  value_.real_ <= maxUInt64,  "Real out of UInt64 range" );
      return UInt( value_.real_ );
   case booleanValue:
      return value_.bool_ ? 1 : 0;
   case stringValue:
   case arrayValue:
   case objectValue:
      JSON_FAIL_MESSAGE( "Type is not convertible to UInt64" );
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return 0; // unreachable;
}
# endif // if defined(JSON_HAS_INT64)


LargestInt 
Value::asLargestInt() const {
#if defined(JSON_NO_INT64)
    return asInt();
#else
    return asInt64();
#endif
}


LargestUInt 
Value::asLargestUInt() const {
#if defined(JSON_NO_INT64)
    return asUInt();
#else
    return asUInt64();
#endif
}


double 
Value::asDouble() const {
   switch ( type_ ) {
   case nullValue:
      return 0.0;
   case intValue:
      return static_cast<double>( value_.int_ );
   case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
      return static_cast<double>( value_.uint_ );
#else // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
      return static_cast<double>( Int(value_.uint_/2) ) * 2 + Int(value_.uint_ & 1);
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
   case realValue:
      return value_.real_;
   case booleanValue:
      return value_.bool_ ? 1.0 : 0.0;
   case stringValue:
   case arrayValue:
   case objectValue:
      JSON_FAIL_MESSAGE( "Type is not convertible to double" );
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return 0; // unreachable;
}

float
Value::asFloat() const {
   switch ( type_ ) {
   case nullValue:
      return 0.0f;
   case intValue:
      return static_cast<float>( value_.int_ );
   case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
      return static_cast<float>( value_.uint_ );
#else // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
      return static_cast<float>( Int(value_.uint_/2) ) * 2 + Int(value_.uint_ & 1);
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
   case realValue:
      return static_cast<float>( value_.real_ );
   case booleanValue:
      return value_.bool_ ? 1.0f : 0.0f;
   case stringValue:
   case arrayValue:
   case objectValue:
      JSON_FAIL_MESSAGE( "Type is not convertible to float" );
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return 0.0f; // unreachable;
}

bool 
Value::asBool() const {
   switch ( type_ ) {
   case nullValue:
      return false;
   case intValue:
   case uintValue:
      return value_.int_ != 0;
   case realValue:
      return value_.real_ != 0.0;
   case booleanValue:
      return value_.bool_;
   case stringValue:
      return value_.string_  &&  value_.string_[0] != 0;
   case arrayValue:
   case objectValue:
      return value_.map_->size() != 0;
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return false; // unreachable;
}


bool 
Value::isConvertibleTo( ValueType other ) const {
   switch ( type_ ) {
   case nullValue:
      return true;
   case intValue:
      return ( other == nullValue  &&  value_.int_ == 0 )
             || other == intValue
             || ( other == uintValue  && value_.int_ >= 0 )
             || other == realValue
             || other == stringValue
             || other == booleanValue;
   case uintValue:
      return ( other == nullValue  &&  value_.uint_ == 0 )
             || ( other == intValue  && value_.uint_ <= (unsigned)maxInt )
             || other == uintValue
             || other == realValue
             || other == stringValue
             || other == booleanValue;
   case realValue:
      return ( other == nullValue  &&  value_.real_ == 0.0 )
             || ( other == intValue  &&  value_.real_ >= minInt  &&  value_.real_ <= maxInt )
             || ( other == uintValue  &&  value_.real_ >= 0  &&  value_.real_ <= maxUInt )
             || other == realValue
             || other == stringValue
             || other == booleanValue;
   case booleanValue:
      return ( other == nullValue  &&  value_.bool_ == false )
             || other == intValue
             || other == uintValue
             || other == realValue
             || other == stringValue
             || other == booleanValue;
   case stringValue:
      return other == stringValue
             || ( other == nullValue  &&  (!value_.string_  ||  value_.string_[0] == 0) );
   case arrayValue:
      return other == arrayValue
             ||  ( other == nullValue  &&  value_.map_->size() == 0 );
   case objectValue:
      return other == objectValue
             ||  ( other == nullValue  &&  value_.map_->size() == 0 );
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return false; // unreachable;
}


/// Number of values in array or object
ArrayIndex 
Value::size() const {
   switch ( type_ ) {
   case nullValue:
   case intValue:
   case uintValue:
   case realValue:
   case booleanValue:
   case stringValue:
      return 0;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   case arrayValue:  // size of the array is highest index + 1
      if ( !value_.map_->empty() ) {
         ObjectValues::const_iterator itLast = value_.map_->end();
         --itLast;
         return (*itLast).first.index()+1;
      }
      return 0;
   case objectValue:
      return ArrayIndex( value_.map_->size() );
#else
   case arrayValue:
      return Int( value_.array_->size() );
   case objectValue:
      return Int( value_.map_->size() );
#endif
   default:
      JSON_ASSERT_UNREACHABLE;
   }
   return 0; // unreachable;
}


bool 
Value::empty() const {
   if ( isNull() || isArray() || isObject() )
      return size() == 0u;
   else
      return false;
}


bool
Value::operator!() const {
   return isNull();
}


void 
Value::clear() {
   JSON_ASSERT( type_ == nullValue  ||  type_ == arrayValue  || type_ == objectValue );

   switch ( type_ )
   {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   case arrayValue:
   case objectValue:
      value_.map_->clear();
      break;
#else
   case arrayValue:
      value_.array_->clear();
      break;
   case objectValue:
      value_.map_->clear();
      break;
#endif
   default:
      break;
   }
}

void 
Value::resize( ArrayIndex newSize ) {
   JSON_ASSERT( type_ == nullValue  ||  type_ == arrayValue );
   if ( type_ == nullValue )
      *this = Value( arrayValue );
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   ArrayIndex oldSize = size();
   if ( newSize == 0 )
      clear();
   else if ( newSize > oldSize )
      (*this)[ newSize - 1 ];
   else {
      for ( ArrayIndex index = newSize; index < oldSize; ++index )
      {
         value_.map_->erase( index );
      }
      assert( size() == newSize );
   }
#else
   value_.array_->resize( newSize );
#endif
}


Value &
Value::operator[]( ArrayIndex index ) {
   JSON_ASSERT( type_ == nullValue  ||  type_ == arrayValue );
   if ( type_ == nullValue )
      *this = Value( arrayValue );
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   CZString key( index );
   ObjectValues::iterator it = value_.map_->lower_bound( key );
   if ( it != value_.map_->end()  &&  (*it).first == key )
      return (*it).second;

   ObjectValues::value_type defaultValue( key, null );
   it = value_.map_->insert( it, defaultValue );
   return (*it).second;
#else
   return value_.array_->resolveReference( index );
#endif
}


Value &
Value::operator[]( int index ) {
   JSON_ASSERT( index >= 0 );
   return (*this)[ ArrayIndex(index) ];
}


const Value &
Value::operator[]( ArrayIndex index ) const {
   JSON_ASSERT( type_ == nullValue  ||  type_ == arrayValue );
   if ( type_ == nullValue )
      return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   CZString key( index );
   ObjectValues::const_iterator it = value_.map_->find( key );
   if ( it == value_.map_->end() )
      return null;
   return (*it).second;
#else
   Value *value = value_.array_->find( index );
   return value ? *value : null;
#endif
}


const Value &
Value::operator[]( int index ) const {
   JSON_ASSERT( index >= 0 );
   return (*this)[ ArrayIndex(index) ];
}


Value &
Value::operator[]( const char *key ) {
   return resolveReference( key, false );
}


Value &
Value::resolveReference( const char *key, 
                         bool isStatic ) {
   JSON_ASSERT( type_ == nullValue  ||  type_ == objectValue );
   if ( type_ == nullValue )
      *this = Value( objectValue );
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   CZString actualKey( key, isStatic ? CZString::noDuplication 
                                     : CZString::duplicateOnCopy );
   ObjectValues::iterator it = value_.map_->lower_bound( actualKey );
   if ( it != value_.map_->end()  &&  (*it).first == actualKey )
      return (*it).second;

   ObjectValues::value_type defaultValue( actualKey, null );
   it = value_.map_->insert( it, defaultValue );
   Value &value = (*it).second;
   return value;
#else
   return value_.map_->resolveReference( key, isStatic );
#endif
}


Value 
Value::get( ArrayIndex index, 
            const Value &defaultValue ) const {
   const Value *value = &((*this)[index]);
   return value == &null ? defaultValue : *value;
}


bool 
Value::isValidIndex( ArrayIndex index ) const {
   return index < size();
}



const Value &
Value::operator[]( const char *key ) const {
   JSON_ASSERT( type_ == nullValue  ||  type_ == objectValue );
   if ( type_ == nullValue )
      return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   CZString actualKey( key, CZString::noDuplication );
   ObjectValues::const_iterator it = value_.map_->find( actualKey );
   if ( it == value_.map_->end() )
      return null;
   return (*it).second;
#else
   const Value *value = value_.map_->find( key );
   return value ? *value : null;
#endif
}


Value &
Value::operator[]( const std::string &key ) {
   return (*this)[ key.c_str() ];
}


const Value &
Value::operator[]( const std::string &key ) const {
   return (*this)[ key.c_str() ];
}

Value &
Value::operator[]( const StaticString &key ) {
   return resolveReference( key, true );
}


# ifdef JSON_USE_CPPTL
Value &
Value::operator[]( const CppTL::ConstString &key ) {
   return (*this)[ key.c_str() ];
}


const Value &
Value::operator[]( const CppTL::ConstString &key ) const {
   return (*this)[ key.c_str() ];
}
# endif


Value &
Value::append( const Value &value ) {
   return (*this)[size()] = value;
}


Value 
Value::get( const char *key, 
            const Value &defaultValue ) const {
   const Value *value = &((*this)[key]);
   return value == &null ? defaultValue : *value;
}


Value 
Value::get( const std::string &key,
            const Value &defaultValue ) const {
   return get( key.c_str(), defaultValue );
}

Value
Value::removeMember( const char* key ) {
   JSON_ASSERT( type_ == nullValue  ||  type_ == objectValue );
   if ( type_ == nullValue )
      return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   CZString actualKey( key, CZString::noDuplication );
   ObjectValues::iterator it = value_.map_->find( actualKey );
   if ( it == value_.map_->end() )
      return null;
   Value old(it->second);
   value_.map_->erase(it);
   return old;
#else
   Value *value = value_.map_->find( key );
   if (value){
      Value old(*value);
      value_.map_.remove( key );
      return old;
   } else {
      return null;
   }
#endif
}

Value
Value::removeMember( const std::string &key ) {
   return removeMember( key.c_str() );
}

# ifdef JSON_USE_CPPTL
Value 
Value::get( const CppTL::ConstString &key,
            const Value &defaultValue ) const {
   return get( key.c_str(), defaultValue );
}
# endif

bool 
Value::isMember( const char *key ) const {
   const Value *value = &((*this)[key]);
   return value != &null;
}


bool 
Value::isMember( const std::string &key ) const {
   return isMember( key.c_str() );
}


# ifdef JSON_USE_CPPTL
bool 
Value::isMember( const CppTL::ConstString &key ) const {
   return isMember( key.c_str() );
}
#endif

Value::Members 
Value::getMemberNames() const {
   JSON_ASSERT( type_ == nullValue  ||  type_ == objectValue );
   if ( type_ == nullValue )
       return Value::Members();
   Members members;
   members.reserve( value_.map_->size() );
#ifndef JSON_VALUE_USE_INTERNAL_MAP
   ObjectValues::const_iterator it = value_.map_->begin();
   ObjectValues::const_iterator itEnd = value_.map_->end();
   for ( ; it != itEnd; ++it )
      members.push_back( std::string( (*it).first.c_str() ) );
#else
   ValueInternalMap::IteratorState it;
   ValueInternalMap::IteratorState itEnd;
   value_.map_->makeBeginIterator( it );
   value_.map_->makeEndIterator( itEnd );
   for ( ; !ValueInternalMap::equals( it, itEnd ); ValueInternalMap::increment(it) )
      members.push_back( std::string( ValueInternalMap::key( it ) ) );
#endif
   return members;
}
//
//# ifdef JSON_USE_CPPTL
//EnumMemberNames
//Value::enumMemberNames() const
//{
//   if ( type_ == objectValue )
//   {
//      return CppTL::Enum::any(  CppTL::Enum::transform(
//         CppTL::Enum::keys( *(value_.map_), CppTL::Type<const CZString &>() ),
//         MemberNamesTransform() ) );
//   }
//   return EnumMemberNames();
//}
//
//
//EnumValues 
//Value::enumValues() const
//{
//   if ( type_ == objectValue  ||  type_ == arrayValue )
//      return CppTL::Enum::anyValues( *(value_.map_), 
//                                     CppTL::Type<const Value &>() );
//   return EnumValues();
//}
//
//# endif


bool
Value::isNull() const {
   return type_ == nullValue;
}


bool 
Value::isBool() const {
   return type_ == booleanValue;
}


bool 
Value::isInt() const {
   return type_ == intValue;
}


bool 
Value::isUInt() const {
   return type_ == uintValue;
}


bool 
Value::isIntegral() const {
   return type_ == intValue  
          ||  type_ == uintValue  
          ||  type_ == booleanValue;
}


bool 
Value::isDouble() const {
   return type_ == realValue;
}


bool 
Value::isNumeric() const {
   return isIntegral() || isDouble();
}


bool 
Value::isString() const {
   return type_ == stringValue;
}


bool 
Value::isArray() const {
   return type_ == nullValue  ||  type_ == arrayValue;
}


bool 
Value::isObject() const {
   return type_ == nullValue  ||  type_ == objectValue;
}


void 
Value::setComment( const char *comment,
                   CommentPlacement placement ) {
   if ( !comments_ )
      comments_ = new CommentInfo[numberOfCommentPlacement];
   comments_[placement].setComment( comment );
}


void 
Value::setComment( const std::string &comment,
                   CommentPlacement placement ) {
   setComment( comment.c_str(), placement );
}


bool 
Value::hasComment( CommentPlacement placement ) const {
   return comments_ != 0  &&  comments_[placement].comment_ != 0;
}

std::string 
Value::getComment( CommentPlacement placement ) const {
   if ( hasComment(placement) )
      return comments_[placement].comment_;
   return "";
}


std::string 
Value::toStyledString() const {
   StyledWriter writer;
   return writer.write( *this );
}


Value::const_iterator 
Value::begin() const {
   switch ( type_ ) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
   case arrayValue:
      if ( value_.array_ ) {
         ValueInternalArray::IteratorState it;
         value_.array_->makeBeginIterator( it );
         return const_iterator( it );
      }
      break;
   case objectValue:
      if ( value_.map_ ) {
         ValueInternalMap::IteratorState it;
         value_.map_->makeBeginIterator( it );
         return const_iterator( it );
      }
      break;
#else
   case arrayValue:
   case objectValue:
      if ( value_.map_ )
         return const_iterator( value_.map_->begin() );
      break;
#endif
   default:
      break;
   }
   return const_iterator();
}

Value::const_iterator 
Value::end() const {
   switch ( type_ ) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
   case arrayValue:
      if ( value_.array_ ) {
         ValueInternalArray::IteratorState it;
         value_.array_->makeEndIterator( it );
         return const_iterator( it );
      }
      break;
   case objectValue:
      if ( value_.map_ ) {
         ValueInternalMap::IteratorState it;
         value_.map_->makeEndIterator( it );
         return const_iterator( it );
      }
      break;
#else
   case arrayValue:
   case objectValue:
      if ( value_.map_ )
         return const_iterator( value_.map_->end() );
      break;
#endif
   default:
      break;
   }
   return const_iterator();
}


Value::iterator 
Value::begin() {
   switch ( type_ ) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
   case arrayValue:
      if ( value_.array_ ) {
         ValueInternalArray::IteratorState it;
         value_.array_->makeBeginIterator( it );
         return iterator( it );
      }
      break;
   case objectValue:
      if ( value_.map_ ) {
         ValueInternalMap::IteratorState it;
         value_.map_->makeBeginIterator( it );
         return iterator( it );
      }
      break;
#else
   case arrayValue:
   case objectValue:
      if ( value_.map_ )
         return iterator( value_.map_->begin() );
      break;
#endif
   default:
      break;
   }
   return iterator();
}

Value::iterator 
Value::end() {
   switch ( type_ ) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
   case arrayValue:
      if ( value_.array_ ) {
         ValueInternalArray::IteratorState it;
         value_.array_->makeEndIterator( it );
         return iterator( it );
      }
      break;
   case objectValue:
      if ( value_.map_ ) {
         ValueInternalMap::IteratorState it;
         value_.map_->makeEndIterator( it );
         return iterator( it );
      }
      break;
#else
   case arrayValue:
   case objectValue:
      if ( value_.map_ )
         return iterator( value_.map_->end() );
      break;
#endif
   default:
      break;
   }
   return iterator();
}


// class PathArgument
// //////////////////////////////////////////////////////////////////

PathArgument::PathArgument()
   : kind_( kindNone ) {
}


PathArgument::PathArgument( ArrayIndex index )
   : index_( index )
   , kind_( kindIndex ) {
}


PathArgument::PathArgument( const char *key )
   : key_( key )
   , kind_( kindKey ) {
}


PathArgument::PathArgument( const std::string &key )
   : key_( key.c_str() )
   , kind_( kindKey ) {
}

// class Path
// //////////////////////////////////////////////////////////////////

Path::Path( const std::string &path,
            const PathArgument &a1,
            const PathArgument &a2,
            const PathArgument &a3,
            const PathArgument &a4,
            const PathArgument &a5 ) {
   InArgs in;
   in.push_back( &a1 );
   in.push_back( &a2 );
   in.push_back( &a3 );
   in.push_back( &a4 );
   in.push_back( &a5 );
   makePath( path, in );
}


void 
Path::makePath( const std::string &path,
                const InArgs &in ) {
   const char *current = path.c_str();
   const char *end = current + path.length();
   InArgs::const_iterator itInArg = in.begin();
   while ( current != end ) {
      if ( *current == '[' ) {
         ++current;
         if ( *current == '%' )
            addPathInArg( path, in, itInArg, PathArgument::kindIndex );
         else {
            ArrayIndex index = 0;
            for ( ; current != end && *current >= '0'  &&  *current <= '9'; ++current )
               index = index * 10 + ArrayIndex(*current - '0');
            args_.push_back( index );
         }
         if ( current == end  ||  *current++ != ']' )
            invalidPath( path, int(current - path.c_str()) );
      }
      else if ( *current == '%' ) {
         addPathInArg( path, in, itInArg, PathArgument::kindKey );
         ++current;
      }
      else if ( *current == '.' ) {
         ++current;
      } else {
         const char *beginName = current;
         while ( current != end  &&  !strchr( "[.", *current ) )
            ++current;
         args_.push_back( std::string( beginName, current ) );
      }
   }
}


void 
Path::addPathInArg( const std::string &path, 
                    const InArgs &in, 
                    InArgs::const_iterator &itInArg, 
                    PathArgument::Kind kind ) {
   if ( itInArg == in.end() ) {
      // Error: missing argument %d
   } else if ( (*itInArg)->kind_ != kind ) {
      // Error: bad argument type
   } else {
      args_.push_back( **itInArg );
   }
}


void 
Path::invalidPath( const std::string &path, 
                   int location ) {
   // Error: invalid path.
}


const Value &
Path::resolve( const Value &root ) const {
   const Value *node = &root;
   for ( Args::const_iterator it = args_.begin(); it != args_.end(); ++it ) {
      const PathArgument &arg = *it;
      if ( arg.kind_ == PathArgument::kindIndex ) {
         if ( !node->isArray()  ||  node->isValidIndex( arg.index_ ) ) {
            // Error: unable to resolve path (array value expected at position...
         }
         node = &((*node)[arg.index_]);
      } else if ( arg.kind_ == PathArgument::kindKey ) {
         if ( !node->isObject() ) {
            // Error: unable to resolve path (object value expected at position...)
         }
         node = &((*node)[arg.key_]);
         if ( node == &Value::null ) {
            // Error: unable to resolve path (object has no member named '' at position...)
         }
      }
   }
   return *node;
}


Value 
Path::resolve( const Value &root, 
               const Value &defaultValue ) const {
   const Value *node = &root;
   for ( Args::const_iterator it = args_.begin(); it != args_.end(); ++it ) {
      const PathArgument &arg = *it;
      if ( arg.kind_ == PathArgument::kindIndex ) {
         if ( !node->isArray()  ||  node->isValidIndex( arg.index_ ) )
            return defaultValue;
         node = &((*node)[arg.index_]);
      } else if ( arg.kind_ == PathArgument::kindKey ) {
         if ( !node->isObject() )
            return defaultValue;
         node = &((*node)[arg.key_]);
         if ( node == &Value::null )
            return defaultValue;
      }
   }
   return *node;
}


Value &
Path::make( Value &root ) const {
   Value *node = &root;
   for ( Args::const_iterator it = args_.begin(); it != args_.end(); ++it ) {
      const PathArgument &arg = *it;
      if ( arg.kind_ == PathArgument::kindIndex ) {
         if ( !node->isArray() ) {
            // Error: node is not an array at position ...
         }
         node = &((*node)[arg.index_]);
      } else if ( arg.kind_ == PathArgument::kindKey ) {
         if ( !node->isObject() ) {
            // Error: node is not an object at position...
         }
         node = &((*node)[arg.key_]);
      }
   }
   return *node;
}


} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_value.cpp
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_writer.cpp
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#if !defined(JSON_IS_AMALGAMATION)
# include <json/writer.h>
# include "json_tool.h"
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <utility>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#if _MSC_VER >= 1400 // VC++ 8.0
#pragma warning( disable : 4996 )   // disable warning about strdup being deprecated.
#endif

namespace Json {

static bool containsControlCharacter( const char* str ) {
   while ( *str ) {
      if ( isControlCharacter( *(str++) ) )
         return true;
   }
   return false;
}


std::string valueToString( LargestInt value ) {
   UIntToStringBuffer buffer;
   char *current = buffer + sizeof(buffer);
   bool isNegative = value < 0;
   if ( isNegative )
      value = -value;
   uintToString( LargestUInt(value), current );
   if ( isNegative )
      *--current = '-';
   assert( current >= buffer );
   return current;
}


std::string valueToString( LargestUInt value ) {
   UIntToStringBuffer buffer;
   char *current = buffer + sizeof(buffer);
   uintToString( value, current );
   assert( current >= buffer );
   return current;
}

#if defined(JSON_HAS_INT64)

std::string valueToString( Int value )
{
   return valueToString( LargestInt(value) );
}


std::string valueToString( UInt value ) {
   return valueToString( LargestUInt(value) );
}

#endif // # if defined(JSON_HAS_INT64)


std::string valueToString( double value ) {
   char buffer[32];
#if defined(_MSC_VER) && defined(__STDC_SECURE_LIB__) // Use secure version with visual studio 2005 to avoid warning. 
   sprintf_s(buffer, sizeof(buffer), "%#.16g", value); 
#else	
   sprintf(buffer, "%#.16g", value); 
#endif
   char* ch = buffer + strlen(buffer) - 1;
   if (*ch != '0') return buffer; // nothing to truncate, so save time
   while(ch > buffer && *ch == '0'){
     --ch;
   }
   char* last_nonzero = ch;
   while(ch >= buffer){
     switch(*ch){
     case '0':
     case '1':
     case '2':
     case '3':
     case '4':
     case '5':
     case '6':
     case '7':
     case '8':
     case '9':
       --ch;
       continue;
     case '.':
       // Truncate zeroes to save bytes in output, but keep one.
       *(last_nonzero+2) = '\0';
       return buffer;
     default:
       return buffer;
     }
   }
   return buffer;
}


std::string valueToString( bool value ) {
   return value ? "true" : "false";
}

std::string valueToQuotedString( const char *value ) {
   // Not sure how to handle unicode...
   if (strpbrk(value, "\"\\\b\f\n\r\t") == NULL && !containsControlCharacter( value ))
      return std::string("\"") + value + "\"";
   // We have to walk value and escape any special characters.
   // Appending to std::string is not efficient, but this should be rare.
   // (Note: forward slashes are *not* rare, but I am not escaping them.)
   std::string::size_type maxsize = strlen(value)*2 + 3; // allescaped+quotes+NULL
   std::string result;
   result.reserve(maxsize); // to avoid lots of mallocs
   result += "\"";
   for (const char* c=value; *c != 0; ++c) {
      switch(*c) {
         case '\"':
            result += "\\\"";
            break;
         case '\\':
            result += "\\\\";
            break;
         case '\b':
            result += "\\b";
            break;
         case '\f':
            result += "\\f";
            break;
         case '\n':
            result += "\\n";
            break;
         case '\r':
            result += "\\r";
            break;
         case '\t':
            result += "\\t";
            break;
         //case '/':
            // Even though \/ is considered a legal escape in JSON, a bare
            // slash is also legal, so I see no reason to escape it.
            // (I hope I am not misunderstanding something.
            // blep notes: actually escaping \/ may be useful in javascript to avoid </ 
            // sequence.
            // Should add a flag to allow this compatibility mode and prevent this 
            // sequence from occurring.
         default:
            if ( isControlCharacter( *c ) ) {
               std::ostringstream oss;
               oss << "\\u" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << static_cast<int>(*c);
               result += oss.str();
            } else {
               result += *c;
            }
            break;
      }
   }
   result += "\"";
   return result;
}

// Class Writer
// //////////////////////////////////////////////////////////////////
Writer::~Writer() {
}


// Class FastWriter
// //////////////////////////////////////////////////////////////////

FastWriter::FastWriter()
   : yamlCompatiblityEnabled_( false ) {
}


void 
FastWriter::enableYAMLCompatibility() {
   yamlCompatiblityEnabled_ = true;
}


std::string 
FastWriter::write( const Value &root ) {
   document_ = "";
   writeValue( root );
   document_ += "\n";
   return document_;
}


void 
FastWriter::writeValue( const Value &value ) {
   switch ( value.type() ) {
   case nullValue:
      document_ += "null";
      break;
   case intValue:
      document_ += valueToString( value.asLargestInt() );
      break;
   case uintValue:
      document_ += valueToString( value.asLargestUInt() );
      break;
   case realValue:
      document_ += valueToString( value.asDouble() );
      break;
   case stringValue:
      document_ += valueToQuotedString( value.asCString() );
      break;
   case booleanValue:
      document_ += valueToString( value.asBool() );
      break;
   case arrayValue: {
         document_ += "[";
         int size = value.size();
         for ( int index =0; index < size; ++index ) {
            if ( index > 0 )
               document_ += ",";
            writeValue( value[index] );
         }
         document_ += "]";
      }
      break;
   case objectValue: {
         Value::Members members( value.getMemberNames() );
         document_ += "{";
         for ( Value::Members::iterator it = members.begin(); 
               it != members.end(); 
               ++it ) {
            const std::string &name = *it;
            if ( it != members.begin() )
               document_ += ",";
            document_ += valueToQuotedString( name.c_str() );
            document_ += yamlCompatiblityEnabled_ ? ": " 
                                                  : ":";
            writeValue( value[name] );
         }
         document_ += "}";
      }
      break;
   }
}


// Class StyledWriter
// //////////////////////////////////////////////////////////////////

StyledWriter::StyledWriter()
   : rightMargin_( 74 )
   , indentSize_( 3 ) {
}


std::string 
StyledWriter::write( const Value &root ) {
   document_ = "";
   addChildValues_ = false;
   indentString_ = "";
   writeCommentBeforeValue( root );
   writeValue( root );
   writeCommentAfterValueOnSameLine( root );
   document_ += "\n";
   return document_;
}


void 
StyledWriter::writeValue( const Value &value ) {
   switch ( value.type() ) {
   case nullValue:
      pushValue( "null" );
      break;
   case intValue:
      pushValue( valueToString( value.asLargestInt() ) );
      break;
   case uintValue:
      pushValue( valueToString( value.asLargestUInt() ) );
      break;
   case realValue:
      pushValue( valueToString( value.asDouble() ) );
      break;
   case stringValue:
      pushValue( valueToQuotedString( value.asCString() ) );
      break;
   case booleanValue:
      pushValue( valueToString( value.asBool() ) );
      break;
   case arrayValue:
      writeArrayValue( value);
      break;
   case objectValue:
      {
         Value::Members members( value.getMemberNames() );
         if ( members.empty() )
            pushValue( "{}" );
         else {
            writeWithIndent( "{" );
            indent();
            Value::Members::iterator it = members.begin();
            for (;;) {
               const std::string &name = *it;
               const Value &childValue = value[name];
               writeCommentBeforeValue( childValue );
               writeWithIndent( valueToQuotedString( name.c_str() ) );
               document_ += " : ";
               writeValue( childValue );
               if ( ++it == members.end() ) {
                  writeCommentAfterValueOnSameLine( childValue );
                  break;
               }
               document_ += ",";
               writeCommentAfterValueOnSameLine( childValue );
            }
            unindent();
            writeWithIndent( "}" );
         }
      }
      break;
   }
}


void 
StyledWriter::writeArrayValue( const Value &value ) {
   unsigned size = value.size();
   if ( size == 0 )
      pushValue( "[]" );
   else {
      bool isArrayMultiLine = isMultineArray( value );
      if ( isArrayMultiLine ) {
         writeWithIndent( "[" );
         indent();
         bool hasChildValue = !childValues_.empty();
         unsigned index =0;
         for (;;) {
            const Value &childValue = value[index];
            writeCommentBeforeValue( childValue );
            if ( hasChildValue )
               writeWithIndent( childValues_[index] );
            else {
               writeIndent();
               writeValue( childValue );
            }
            if ( ++index == size ) {
               writeCommentAfterValueOnSameLine( childValue );
               break;
            }
            document_ += ",";
            writeCommentAfterValueOnSameLine( childValue );
         }
         unindent();
         writeWithIndent( "]" );
      } else { // output on a single line
         assert( childValues_.size() == size );
         document_ += "[ ";
         for ( unsigned index =0; index < size; ++index ) {
            if ( index > 0 )
               document_ += ", ";
            document_ += childValues_[index];
         }
         document_ += " ]";
      }
   }
}


bool 
StyledWriter::isMultineArray( const Value &value ) {
   int size = value.size();
   bool isMultiLine = size*3 >= rightMargin_ ;
   childValues_.clear();
   for ( int index =0; index < size  &&  !isMultiLine; ++index ) {
      const Value &childValue = value[index];
      isMultiLine = isMultiLine  ||
                     ( (childValue.isArray()  ||  childValue.isObject())  &&  
                        childValue.size() > 0 );
   }
   if ( !isMultiLine ) { // check if line length > max line length
      childValues_.reserve( size );
      addChildValues_ = true;
      int lineLength = 4 + (size-1)*2; // '[ ' + ', '*n + ' ]'
      for ( int index =0; index < size  &&  !isMultiLine; ++index ) {
         writeValue( value[index] );
         lineLength += int( childValues_[index].length() );
         isMultiLine = isMultiLine  &&  hasCommentForValue( value[index] );
      }
      addChildValues_ = false;
      isMultiLine = isMultiLine  ||  lineLength >= rightMargin_;
   }
   return isMultiLine;
}


void 
StyledWriter::pushValue( const std::string &value ) {
   if ( addChildValues_ )
      childValues_.push_back( value );
   else
      document_ += value;
}


void 
StyledWriter::writeIndent() {
   if ( !document_.empty() )
   {
      char last = document_[document_.length()-1];
      if ( last == ' ' )     // already indented
         return;
      if ( last != '\n' )    // Comments may add new-line
         document_ += '\n';
   }
   document_ += indentString_;
}


void 
StyledWriter::writeWithIndent( const std::string &value ) {
   writeIndent();
   document_ += value;
}


void 
StyledWriter::indent() {
   indentString_ += std::string( indentSize_, ' ' );
}


void 
StyledWriter::unindent() {
   assert( int(indentString_.size()) >= indentSize_ );
   indentString_.resize( indentString_.size() - indentSize_ );
}


void 
StyledWriter::writeCommentBeforeValue( const Value &root ) {
   if ( !root.hasComment( commentBefore ) )
      return;
   document_ += normalizeEOL( root.getComment( commentBefore ) );
   document_ += "\n";
}


void 
StyledWriter::writeCommentAfterValueOnSameLine( const Value &root ) {
   if ( root.hasComment( commentAfterOnSameLine ) )
      document_ += " " + normalizeEOL( root.getComment( commentAfterOnSameLine ) );

   if ( root.hasComment( commentAfter ) ) {
      document_ += "\n";
      document_ += normalizeEOL( root.getComment( commentAfter ) );
      document_ += "\n";
   }
}


bool 
StyledWriter::hasCommentForValue( const Value &value ) {
   return value.hasComment( commentBefore )
          ||  value.hasComment( commentAfterOnSameLine )
          ||  value.hasComment( commentAfter );
}


std::string 
StyledWriter::normalizeEOL( const std::string &text ) {
   std::string normalized;
   normalized.reserve( text.length() );
   const char *begin = text.c_str();
   const char *end = begin + text.length();
   const char *current = begin;
   while ( current != end ) {
      char c = *current++;
      if ( c == '\r' ) { // mac or dos EOL
         if ( *current == '\n' ) // convert dos EOL
            ++current;
         normalized += '\n';
      } else // handle unix EOL & other char
         normalized += c;
   }
   return normalized;
}


// Class StyledStreamWriter
// //////////////////////////////////////////////////////////////////

StyledStreamWriter::StyledStreamWriter( std::string indentation )
   : document_(NULL)
   , rightMargin_( 74 )
   , indentation_( indentation ) {
}


void
StyledStreamWriter::write( std::ostream &out, const Value &root ) {
   document_ = &out;
   addChildValues_ = false;
   indentString_ = "";
   writeCommentBeforeValue( root );
   writeValue( root );
   writeCommentAfterValueOnSameLine( root );
   *document_ << "\n";
   document_ = NULL; // Forget the stream, for safety.
}


void 
StyledStreamWriter::writeValue( const Value &value ) {
   switch ( value.type() ) {
   case nullValue:
      pushValue( "null" );
      break;
   case intValue:
      pushValue( valueToString( value.asLargestInt() ) );
      break;
   case uintValue:
      pushValue( valueToString( value.asLargestUInt() ) );
      break;
   case realValue:
      pushValue( valueToString( value.asDouble() ) );
      break;
   case stringValue:
      pushValue( valueToQuotedString( value.asCString() ) );
      break;
   case booleanValue:
      pushValue( valueToString( value.asBool() ) );
      break;
   case arrayValue:
      writeArrayValue( value);
      break;
   case objectValue: {
         Value::Members members( value.getMemberNames() );
         if ( members.empty() )
            pushValue( "{}" );
         else {
            writeWithIndent( "{" );
            indent();
            Value::Members::iterator it = members.begin();
            for (;;) {
               const std::string &name = *it;
               const Value &childValue = value[name];
               writeCommentBeforeValue( childValue );
               writeWithIndent( valueToQuotedString( name.c_str() ) );
               *document_ << " : ";
               writeValue( childValue );
               if ( ++it == members.end() ) {
                  writeCommentAfterValueOnSameLine( childValue );
                  break;
               }
               *document_ << ",";
               writeCommentAfterValueOnSameLine( childValue );
            }
            unindent();
            writeWithIndent( "}" );
         }
      }
      break;
   }
}


void 
StyledStreamWriter::writeArrayValue( const Value &value ) {
   unsigned size = value.size();
   if ( size == 0 )
      pushValue( "[]" );
   else {
      bool isArrayMultiLine = isMultineArray( value );
      if ( isArrayMultiLine ) {
         writeWithIndent( "[" );
         indent();
         bool hasChildValue = !childValues_.empty();
         unsigned index =0;
         for (;;) {
            const Value &childValue = value[index];
            writeCommentBeforeValue( childValue );
            if ( hasChildValue )
               writeWithIndent( childValues_[index] );
            else {
               writeIndent();
               writeValue( childValue );
            }
            if ( ++index == size ) {
               writeCommentAfterValueOnSameLine( childValue );
               break;
            }
            *document_ << ",";
            writeCommentAfterValueOnSameLine( childValue );
         }
         unindent();
         writeWithIndent( "]" );
      }
      else { // output on a single line
         assert( childValues_.size() == size );
         *document_ << "[ ";
         for ( unsigned index =0; index < size; ++index ) {
            if ( index > 0 )
               *document_ << ", ";
            *document_ << childValues_[index];
         }
         *document_ << " ]";
      }
   }
}


bool 
StyledStreamWriter::isMultineArray( const Value &value ) {
   int size = value.size();
   bool isMultiLine = size*3 >= rightMargin_ ;
   childValues_.clear();
   for ( int index =0; index < size  &&  !isMultiLine; ++index ) {
      const Value &childValue = value[index];
      isMultiLine = isMultiLine  ||
                     ( (childValue.isArray()  ||  childValue.isObject())  &&  
                        childValue.size() > 0 );
   }
   if ( !isMultiLine ) { // check if line length > max line length
      childValues_.reserve( size );
      addChildValues_ = true;
      int lineLength = 4 + (size-1)*2; // '[ ' + ', '*n + ' ]'
      for ( int index =0; index < size  &&  !isMultiLine; ++index ) {
         writeValue( value[index] );
         lineLength += int( childValues_[index].length() );
         isMultiLine = isMultiLine  &&  hasCommentForValue( value[index] );
      }
      addChildValues_ = false;
      isMultiLine = isMultiLine  ||  lineLength >= rightMargin_;
   }
   return isMultiLine;
}


void 
StyledStreamWriter::pushValue( const std::string &value ) {
   if ( addChildValues_ )
      childValues_.push_back( value );
   else
      *document_ << value;
}


void 
StyledStreamWriter::writeIndent() {
  /*
    Some comments in this method would have been nice. ;-)

   if ( !document_.empty() )
   {
      char last = document_[document_.length()-1];
      if ( last == ' ' )     // already indented
         return;
      if ( last != '\n' )    // Comments may add new-line
         *document_ << '\n';
   }
  */
   *document_ << '\n' << indentString_;
}


void 
StyledStreamWriter::writeWithIndent( const std::string &value ) {
   writeIndent();
   *document_ << value;
}


void 
StyledStreamWriter::indent() {
   indentString_ += indentation_;
}


void 
StyledStreamWriter::unindent() {
   assert( indentString_.size() >= indentation_.size() );
   indentString_.resize( indentString_.size() - indentation_.size() );
}


void 
StyledStreamWriter::writeCommentBeforeValue( const Value &root ) {
   if ( !root.hasComment( commentBefore ) )
      return;
   *document_ << normalizeEOL( root.getComment( commentBefore ) );
   *document_ << "\n";
}


void 
StyledStreamWriter::writeCommentAfterValueOnSameLine( const Value &root ) {
   if ( root.hasComment( commentAfterOnSameLine ) )
      *document_ << " " + normalizeEOL( root.getComment( commentAfterOnSameLine ) );

   if ( root.hasComment( commentAfter ) ) {
      *document_ << "\n";
      *document_ << normalizeEOL( root.getComment( commentAfter ) );
      *document_ << "\n";
   }
}


bool 
StyledStreamWriter::hasCommentForValue( const Value &value ) {
   return value.hasComment( commentBefore )
          ||  value.hasComment( commentAfterOnSameLine )
          ||  value.hasComment( commentAfter );
}


std::string 
StyledStreamWriter::normalizeEOL( const std::string &text ) {
   std::string normalized;
   normalized.reserve( text.length() );
   const char *begin = text.c_str();
   const char *end = begin + text.length();
   const char *current = begin;
   while ( current != end ) {
      char c = *current++;
      if ( c == '\r' ) { // mac or dos EOL
         if ( *current == '\n' ) // convert dos EOL
            ++current;
         normalized += '\n';
      } else // handle unix EOL & other char
         normalized += c;
   }
   return normalized;
}


std::ostream& operator<<( std::ostream &sout, const Value &root ) {
   Json::StyledStreamWriter writer;
   writer.write(sout, root);
   return sout;
}


} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_writer.cpp
// //////////////////////////////////////////////////////////////////////





//
// end of cpp/jsoncpp.cpp
//


