// This file is composed of the following original files:

//   license.txt
//   cpp/pyne.cpp
//   cpp/nucname.cpp
//   cpp/rxname.cpp

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

void pyne::pyne_start()
{
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
std::string pyne::to_str (int t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

std::string pyne::to_str(unsigned int t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

std::string pyne::to_str (double t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

std::string pyne::to_str (bool t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}


int pyne::to_int (std::string s)
{
  return atoi( s.c_str() );
}

double pyne::to_dbl (std::string s)
{
  return strtod( s.c_str(), NULL );
}

double pyne::endftod (char * s) {
  // Converts string from ENDF to float64.
  int pos, mant, exp;
  double v, dbl_exp;

  mant = exp = 0;
  if (s[2] == '.') {
    // Convert an ENDF float
    if (s[9] == '+' or s[9] == '-')
    {
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

std::string pyne::to_upper(std::string s)
{
  // change each element of the string to upper case.
  for(unsigned int i = 0; i < s.length(); i++)
    s[i] = toupper(s[i]);
  return s;
}

std::string pyne::to_lower(std::string s)
{
  // change each element of the string to lower case
  for(unsigned int i = 0; i < s.length(); i++)
    s[i] = tolower(s[i]);
  return s;
}


std::string pyne::capitalize(std::string s)
{
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


std::string pyne::get_flag(char line[], int max_l)
{
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



std::string pyne::remove_substring(std::string s, std::string substr)
{
  // Removes a substring from the string s
  int n_found = s.find(substr);
  while ( 0 <= n_found )
  {
    s.erase( n_found , substr.length() );
    n_found = s.find(substr);
  }
  return s;
}


std::string pyne::remove_characters(std::string s, std::string chars)
{
  // Removes all characters in the string chars from the string s
  for (int i = 0; i < chars.length(); i++ )
  {
    s = remove_substring(s, chars.substr(i, 1) );
  }
  return s;
}


std::string pyne::replace_all_substrings(std::string s, std::string substr, std::string repstr)
{
  // Replaces all instance of substr in s with the string repstr
  int n_found = s.find(substr);
  while ( 0 <= n_found )
  {
    s.replace( n_found , substr.length(), repstr );
    n_found = s.find(substr);
  }
  return s;
};



std::string pyne::last_char(std::string s)
{
    // Returns the last character in a string.
    return s.substr(s.length()-1, 1);
}


std::string pyne::slice_from_end(std::string s, int n, int l)
{
  // Returns the slice of a string using negative indices.
  return s.substr(s.length()+n, l);
}


bool pyne::ternary_ge(int a, int b, int c)
{
  // Returns true id a <= b <= c and flase otherwise.
  return (a <= b && b <= c);
}


bool pyne::contains_substring(std::string s, std::string substr)
{
  // Returns a boolean based on if the sub is in s.
  int n = s.find(substr);
  return ( 0 <= n && n < s.length() );
}


std::string pyne::natural_naming(std::string name)
{
  // Calculates a version on the string name that is a valid
  // variable name, ie it uses only word characters.
  std::string nat_name (name);

  // Replace Whitespace characters with underscores
  nat_name = pyne::replace_all_substrings(nat_name, " ",  "_");
  nat_name = pyne::replace_all_substrings(nat_name, "\t", "_");
  nat_name = pyne::replace_all_substrings(nat_name, "\n", "_");

  // Remove non-word characters
  int n = 0;
  while ( n < nat_name.length() )
  {
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

double pyne::slope(double x2, double y2, double x1, double y1)
{
  // Finds the slope of a line.
  return (y2 - y1) / (x2 - x1);
};


double pyne::solve_line(double x, double x2, double y2, double x1, double y1)
{
  return (slope(x2,y2,x1,y1) * (x - x2)) + y2;
};


double pyne::tanh(double x)
{
  return std::tanh(x);
};

double pyne::coth(double x)
{
  return 1.0 / std::tanh(x);
};




// File Helpers

bool pyne::file_exists(std::string strfilename) 
{
  // Thank you intarwebz for this function!
  // Sepcifically: http://www.techbytes.ca/techbyte103.html
  struct stat stFileInfo; 
  bool blnReturn; 
  int intStat; 

  // Attempt to get the file attributes 
  intStat = stat(strfilename.c_str(), &stFileInfo); 

  if(intStat == 0) 
  { 
    // We were able to get the file attributes 
    // so the file obviously exists. 
    blnReturn = true; 
  } 
  else 
  { 
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
// start of cpp/nucname.cpp
//
// Converts between naming conventions for nuclides.
// zzaaam is for numerals only (923350).
// name is for letters  as well (U-235).
// MCNP is for numerals without the meta-stable flag (92235), as used in MCNP.

#ifndef PYNE_IS_AMALGAMATED
#include "nucname.h"
#endif

/*** Constructs the LL to zz Dictionary ***/
pyne::nucname::name_zz_t pyne::nucname::get_name_zz()
{
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

bool pyne::nucname::isnuclide(std::string nuc)
{
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

bool pyne::nucname::isnuclide(char * nuc)
{
  return isnuclide(std::string(nuc));
};

bool pyne::nucname::isnuclide(int nuc)
{
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
int pyne::nucname::id(int nuc)
{
  if (nuc < 0)
    throw NotANuclide(nuc, "");

  int newnuc;
  int zzz = nuc / 10000000;     // ZZZ ?
  int aaassss = nuc % 10000000; // AAA-SSSS ?
  int aaa = aaassss / 10000;    // AAA ?
  // Nuclide must already be in id form
  if (0 < zzz && zzz <= aaa && aaa <= zzz * 7)
  {
    // Normal nuclide
    return nuc;
  }
  else if (aaassss == 0 && 0 < zz_name.count(zzz))
  {
    // Natural elemental nuclide:  ie for Urnaium = 920000000
    return nuc;
  }
  else if (nuc < 1000 && 0 < zz_name.count(nuc))
    //  Gave Z-number
    return nuc * 10000000;

  // Not in id form, try  ZZZAAAM form.
  zzz = nuc / 10000;     // ZZZ ?
  aaassss = nuc % 10000; // AAA-SSSS ?
  aaa = aaassss / 10;    // AAA ?
  if (zzz <= aaa && aaa <= zzz * 7)
  {
    // ZZZAAAM nuclide
    return (zzz*10000000) + (aaa*10000) + (nuc%10);
  }
  else if (aaa <= zzz && zzz <= aaa * 7 && 0 < zz_name.count(aaa))
  {
    // Cinder-form (aaazzzm), ie 2350920
    return (aaa*10000000) + (zzz*10000) + (nuc%10);
  }
  //else if (aaassss == 0 && 0 == zz_name.count(nuc/1000) && 0 < zz_name.count(zzz))
  else if (aaassss == 0 && 0 < zz_name.count(zzz))
  {
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
  if (zzz <= aaa)
  {
    if (aaa - 400 < 0)
    {
      if (nuc == 95242)
        return nuc * 10000 + 1;  // special case MCNP Am-242m
      else
        return nuc * 10000;  // Nuclide in normal MCNP form
    }
    else
    {
      // Nuclide in MCNP metastable form
      if (nuc == 95642)
        return (95642 - 400)*10000;  // special case MCNP Am-242
      nuc = ((nuc - 400) * 10000) + 1;
      while (3.0 < (float ((nuc/10000)%1000) / float (nuc/10000000)))
        nuc -= 999999;
      return nuc;
    }
  }
  else if (aaa == 0 && 0 < zz_name.count(zzz))
  {
    // MCNP form natural nuclide
    return zzz * 10000000;
  }
  else if (zzz > 1000)
  {
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

int pyne::nucname::id(char * nuc)
{
  std::string newnuc (nuc);
  return id(newnuc);
};

int pyne::nucname::id(std::string nuc)
{
  if (nuc.empty())
    throw NotANuclide(nuc, "<empty>");
  int newnuc;
  std::string elem_name;

  // Get the string into a regular form
  std::string nucstr = pyne::to_upper(nuc);
  nucstr = pyne::remove_substring(nucstr, "-");
  int nuclen = nucstr.length();

  if (pyne::contains_substring(pyne::digits, nucstr.substr(0, 1)))
  {
    if (pyne::contains_substring(pyne::digits, nucstr.substr(nuclen-1, nuclen)))
    {
      // Nuclide must actually be an integer that 
      // just happens to be living in string form.
      newnuc = pyne::to_int(nucstr);
      newnuc = id(newnuc);
    }
    else
    {
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
  }
  else if (pyne::contains_substring(pyne::alphabet, nucstr.substr(0, 1)))
  {
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
  }
  else
  {
    // Clearly not a nuclide
    throw NotANuclide(nuc, nucstr);
  }
  return newnuc;  
};



/**********************/
/*** name functions ***/
/**********************/
std::string pyne::nucname::name(int nuc)
{
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



std::string pyne::nucname::name(char * nuc)
{
  std::string newnuc (nuc);
  return name(newnuc);
}


std::string pyne::nucname::name(std::string nuc)
{
  return name(id(nuc));
}


/**********************/
/*** znum functions ***/
/**********************/
int pyne::nucname::znum(int nuc){
  return id(nuc) / 10000000;
};

int pyne::nucname::znum(char * nuc){
  return id(nuc) / 10000000;
};

int pyne::nucname::znum(std::string nuc){
  return id(nuc) / 10000000;
};

/**********************/
/*** anum functions ***/
/**********************/
int pyne::nucname::anum(int nuc){
  return (id(nuc) / 10000) % 1000;
};

int pyne::nucname::anum(char * nuc){
  return (id(nuc) / 10000) % 1000;
};

int pyne::nucname::anum(std::string nuc){
  return (id(nuc) / 10000) % 1000;
};

/**********************/
/*** snum functions ***/
/**********************/
int pyne::nucname::snum(int nuc){
  return id(nuc) % 10000;
};

int pyne::nucname::snum(char * nuc){
  return id(nuc) % 10000;
};

int pyne::nucname::snum(std::string nuc){
  return id(nuc) % 10000;
};

/************************/
/*** zzaaam functions ***/
/************************/
int pyne::nucname::zzaaam(int nuc)
{
  int nucid = id(nuc);
  int zzzaaa = nucid / 10000;
  int ssss = nucid % 10000;
  if (10 <= ssss)
    ssss = 9;
  return zzzaaa*10 + ssss;
};


int pyne::nucname::zzaaam(char * nuc)
{
  std::string newnuc (nuc);
  return zzaaam(newnuc);
};


int pyne::nucname::zzaaam(std::string nuc)
{
  return zzaaam(id(nuc));
};


int pyne::nucname::zzaaam_to_id(int nuc)
{
  return (nuc/10)*10000 + (nuc%10);
};


int pyne::nucname::zzaaam_to_id(char * nuc)
{
  return zzaaam_to_id(std::string(nuc));
};


int pyne::nucname::zzaaam_to_id(std::string nuc)
{
  return zzaaam_to_id(pyne::to_int(nuc));
};


/**********************/
/*** mcnp functions ***/
/**********************/
int pyne::nucname::mcnp(int nuc)
{
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



int pyne::nucname::mcnp(char * nuc)
{
  std::string newnuc (nuc);
  return mcnp(newnuc);
};



int pyne::nucname::mcnp(std::string nuc)
{
  return mcnp(id(nuc));
};

//
// MCNP -> id
//
int pyne::nucname::mcnp_to_id(int nuc)
{
  int zzz = nuc / 1000;
  int aaa = nuc % 1000; 
  if (zzz <= aaa)
  {
    if (aaa - 400 < 0)
    {
      if (nuc == 95242)
        return nuc * 10000 + 1;  // special case MCNP Am-242m
      else
        return nuc * 10000;  // Nuclide in normal MCNP form
    }
    else
    {
      // Nuclide in MCNP metastable form
      if (nuc == 95642)
        return (95642 - 400)*10000;  // special case MCNP Am-242
      nuc = ((nuc - 400) * 10000) + 1;
      while (3.0 < (float ((nuc/10000)%1000) / float (nuc/10000000)))
        nuc -= 999999;
      return nuc;
    }
  }
  else if (aaa == 0)
    // MCNP form natural nuclide
    return zzz * 10000000;
  throw IndeterminateNuclideForm(nuc, "");
};


int pyne::nucname::mcnp_to_id(char * nuc)
{
  return mcnp_to_id(std::string(nuc));
};


int pyne::nucname::mcnp_to_id(std::string nuc)
{
  return mcnp_to_id(pyne::to_int(nuc));
};


/*************************/
/*** serpent functions ***/
/*************************/
std::string pyne::nucname::serpent(int nuc)
{
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


std::string pyne::nucname::serpent(char * nuc)
{
  std::string newnuc (nuc);
  return serpent(newnuc);
};


std::string pyne::nucname::serpent(std::string nuc)
{
  return serpent(id(nuc));
};

//
// Serpent -> id
//
//int pyne::nucname::serpent_to_id(int nuc)
//{
// Should be ZAID
//};


int pyne::nucname::serpent_to_id(char * nuc)
{
  return serpent_to_id(std::string(nuc));
};


int pyne::nucname::serpent_to_id(std::string nuc)
{
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
std::string pyne::nucname::nist(int nuc)
{
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


std::string pyne::nucname::nist(char * nuc)
{
  std::string newnuc (nuc);
  return nist(newnuc);
};


std::string pyne::nucname::nist(std::string nuc)
{
  return nist(id(nuc));
};


//
// NIST -> id
//
//int pyne::nucname::nist_to_id(int nuc)
//{
// NON-EXISTANT
//};

int pyne::nucname::nist_to_id(char * nuc)
{
  return nist_to_id(std::string(nuc));
};

int pyne::nucname::nist_to_id(std::string nuc)
{
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
int pyne::nucname::cinder(int nuc)
{
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



int pyne::nucname::cinder(char * nuc)
{
  std::string newnuc (nuc);
  return cinder(newnuc);
};



int pyne::nucname::cinder(std::string nuc)
{
  return cinder(id(nuc));
};

//
// Cinder -> Id
//
int pyne::nucname::cinder_to_id(int nuc)
{
  int ssss = nuc % 10;
  int aaazzz = nuc / 10;
  int zzz = aaazzz % 1000;
  int aaa = aaazzz / 1000;
  return (zzz * 10000000) + (aaa * 10000) + ssss;
};


int pyne::nucname::cinder_to_id(char * nuc)
{
  return cinder_to_id(std::string(nuc));
};


int pyne::nucname::cinder_to_id(std::string nuc)
{
  return cinder_to_id(pyne::to_int(nuc));
};




/**********************/
/*** ALARA functions ***/
/**********************/
std::string pyne::nucname::alara(int nuc)
{
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


std::string pyne::nucname::alara(char * nuc)
{
  std::string newnuc (nuc);
  return alara(newnuc);
}


std::string pyne::nucname::alara(std::string nuc)
{
  return alara(id(nuc));
}


//
// Cinder -> Id
//
//int pyne::nucname::alara_to_id(int nuc)
//{
// Not Possible
//};


int pyne::nucname::alara_to_id(char * nuc)
{
  return alara_to_id(std::string(nuc));
};


int pyne::nucname::alara_to_id(std::string nuc)
{
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
int pyne::nucname::sza(int nuc)
{
  int nucid = id(nuc);
  int zzzaaa = nucid / 10000;
  int sss = nucid % 10000;
  return sss * 1000000 + zzzaaa;
}


int pyne::nucname::sza(char * nuc)
{
  std::string newnuc (nuc);
  return sza(newnuc);
}


int pyne::nucname::sza(std::string nuc)
{
  return sza(id(nuc));
}


int pyne::nucname::sza_to_id(int nuc)
{
  int sss = nuc / 1000000;
  int zzzaaa = nuc % 1000000;
  return zzzaaa * 10000 + sss;
}


int pyne::nucname::sza_to_id(char * nuc)
{
  std::string newnuc (nuc);
  return sza_to_id(newnuc);
}


int pyne::nucname::sza_to_id(std::string nuc)
{
  return sza_to_id(pyne::to_int(nuc));
}


//
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
  "disappeareance",
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
  "erel_disappeareance",
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

void * pyne::rxname::_fill_maps()
{
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
    "(z,disap) Neutron disappeareance",
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
    "Energy Release from (z,disap) Neutron disappeareance",
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
    "Desciptive Data",
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
    "(n,disap) Neutron disappeareance",
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
    "Energy Release from (n,disap) Neutron disappeareance",
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
    "Desciptive Data",
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
  };

  // fill the maps
  for (int i = 0; i < NUM_RX_NAMES; i++)
  {
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
  return NULL;
};
void * pyne::rxname::_ = pyne::rxname::_fill_maps();



unsigned int pyne::rxname::hash(std::string s){return pyne::rxname::hash(s.c_str());};
unsigned int pyne::rxname::hash(const char * s)
{
  // Modified from http://cboard.cprogramming.com/tech-board/114650-string-hashing-algorithm.html#post853145
  // starting from h = 32*2^5 > 1000, rather than 0, to reserve space for MT numbers
  int c;
  unsigned int h = 32; 
  while((c = *s++))
  {
    h = ((h << 5) + h) ^ c;
  }
  return h;
};


// ************************
// *** name functions *****
// ************************

std::string pyne::rxname::name(char * s){return pyne::rxname::name(std::string(s));};
std::string pyne::rxname::name(std::string s)
{
  if (0 < names.count(s))
    return s;
  if (0 < altnames.count(s))
    return id_name[altnames[s]];
  // see if id in string form
  int i = 0;
  int I = s.length();
  int found = 0;
  while(0 <= found && i < I)
  {
    found = pyne::digits.find(s[i]);
    i++;
  }
  if (0<=found)
    return pyne::rxname::name(atoi(s.c_str()));
  // dead...
  throw NotAReaction(s, "???");
};


std::string pyne::rxname::name(int n){return pyne::rxname::name((unsigned int) n);};
std::string pyne::rxname::name(unsigned int n)
{
  if (0 < id_name.count(n))
    return id_name[n];
  if (0 < mt_id.count(n))
    return id_name[mt_id[n]];
  throw NotAReaction(n, "???");
};


std::string pyne::rxname::name(int from_nuc, int to_nuc, std::string z)
{
  // This assumes nuclides are in id form
  std::pair<std::string, int> key = std::make_pair(z, to_nuc - from_nuc);
  if (0 == offset_id.count(key))
    throw IndeterminateReactionForm("z=" + z + ", " + pyne::to_str(from_nuc) + \
                                    ", " + pyne::to_str(to_nuc), "???");
  return id_name[offset_id[key]];
};

std::string pyne::rxname::name(std::string from_nuc, int to_nuc, std::string z)
{
  return pyne::rxname::name(pyne::nucname::id(from_nuc), 
                            pyne::nucname::id(to_nuc), z);
};

std::string pyne::rxname::name(int from_nuc, std::string to_nuc, std::string z)
{
  return pyne::rxname::name(pyne::nucname::id(from_nuc), 
                            pyne::nucname::id(to_nuc), z);
};

std::string pyne::rxname::name(std::string from_nuc, std::string to_nuc, std::string z)
{
  return pyne::rxname::name(pyne::nucname::id(from_nuc), 
                            pyne::nucname::id(to_nuc), z);
};



// **********************
// *** id functions *****
// **********************
unsigned int pyne::rxname::id(int x)
{
  return name_id[pyne::rxname::name(x)];
};
  
unsigned int pyne::rxname::id(unsigned int x)
{
  if (0 < id_name.count(x))
    return x;
  if (0 < mt_id.count(x))
    return mt_id[x];
  return name_id[pyne::rxname::name(x)];
};
  
unsigned int pyne::rxname::id(char * x)
{
  return name_id[pyne::rxname::name(x)];
};
  
unsigned int pyne::rxname::id(std::string x)
{
  if (0 < names.count(x))
    return name_id[x];
  if (0 < altnames.count(x))
    return altnames[x];
  return name_id[pyne::rxname::name(x)];  
};
  
unsigned int pyne::rxname::id(int from_nuc, int to_nuc, std::string z)
{
  // This assumes nuclides are in id form
  std::pair<std::string, int> key = std::make_pair(z, to_nuc - from_nuc);
  if (0 == offset_id.count(key))
    throw IndeterminateReactionForm("z=" + z + ", " + pyne::to_str(from_nuc) + \
                                    ", " + pyne::to_str(to_nuc), "???");
  return offset_id[key];
};
  
unsigned int pyne::rxname::id(int from_nuc, std::string to_nuc, std::string z)
{
  return pyne::rxname::id(pyne::nucname::id(from_nuc), 
                          pyne::nucname::id(to_nuc), z);
};
  
unsigned int pyne::rxname::id(std::string from_nuc, int to_nuc, std::string z)
{
  return pyne::rxname::id(pyne::nucname::id(from_nuc), 
                          pyne::nucname::id(to_nuc), z);
};
  
unsigned int pyne::rxname::id(std::string from_nuc, std::string to_nuc, std::string z)
{
  return pyne::rxname::id(pyne::nucname::id(from_nuc), 
                          pyne::nucname::id(to_nuc), z);
};


// **********************
// *** MT functions *****
// **********************
unsigned int pyne::rxname::mt(int x)
{
  unsigned int rxid = pyne::rxname::id(x);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(unsigned int x)
{
  unsigned int rxid = pyne::rxname::id(x);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(char * x)
{
  unsigned int rxid = pyne::rxname::id(x);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(std::string x)
{
  unsigned int rxid = pyne::rxname::id(x);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(int from_nuc, int to_nuc, std::string z)
{
  unsigned int rxid = pyne::rxname::id(from_nuc, to_nuc, z);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(int from_nuc, std::string to_nuc, std::string z)
{
  unsigned int rxid = pyne::rxname::id(from_nuc, to_nuc, z);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(std::string from_nuc, int to_nuc, std::string z)
{
  unsigned int rxid = pyne::rxname::id(from_nuc, to_nuc, z);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};
  
unsigned int pyne::rxname::mt(std::string from_nuc, std::string to_nuc, std::string z)
{
  unsigned int rxid = pyne::rxname::id(from_nuc, to_nuc, z);
  if (0 == id_mt.count(rxid))
    throw NotAReaction();
  return id_mt[rxid];
};


// ***********************
// *** label functions ***
// ***********************
std::string pyne::rxname::label(int x)
{
  return labels[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::label(unsigned int x)
{
  return labels[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::label(char * x)
{
  return labels[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::label(std::string x)
{
  return labels[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::label(int from_nuc, int to_nuc, std::string z)
{
  return labels[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::label(int from_nuc, std::string to_nuc, std::string z)
{
  return labels[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::label(std::string from_nuc, int to_nuc, std::string z)
{
  return labels[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::label(std::string from_nuc, std::string to_nuc, std::string z)
{
  return labels[pyne::rxname::id(from_nuc, to_nuc, z)];
};


// *********************
// *** doc functions ***
// *********************
std::string pyne::rxname::doc(int x)
{
  return docs[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::doc(unsigned int x)
{
  return docs[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::doc(char * x)
{
  return docs[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::doc(std::string x)
{
  return docs[pyne::rxname::id(x)];
};
  
std::string pyne::rxname::doc(int from_nuc, int to_nuc, std::string z)
{
  return docs[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::doc(int from_nuc, std::string to_nuc, std::string z)
{
  return docs[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::doc(std::string from_nuc, int to_nuc, std::string z)
{
  return docs[pyne::rxname::id(from_nuc, to_nuc, z)];
};
  
std::string pyne::rxname::doc(std::string from_nuc, std::string to_nuc, std::string z)
{
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


