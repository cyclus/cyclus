// This file is composed of the following original files:

//   license.txt
//   cpp/pyne.h
//   cpp/extra_types.h
//   cpp/h5wrap.h
//   cpp/nucname.h
//   cpp/rxname.h

// PyNE amalgated header http://pyne.io/
#ifndef PYNE_52BMSKGZ3FHG3NQI566D4I2ZLY
#define PYNE_52BMSKGZ3FHG3NQI566D4I2ZLY

#define PYNE_IS_AMALGAMATED

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
// start of cpp/pyne.h
//
/// \file pyne.h
/// \author Anthony Scopatz (scopatz\@gmail.com)
///
/// \brief This is the base PyNE library.
///
/// It contains a lot of utility functions and constants that are globaly useful
/// through out the rest of the PyNE infrastructure.
///

// Header for general library file.

#ifndef PYNE_KMMHYNANYFF5BFMEYIP7TUNLHA
#define PYNE_KMMHYNANYFF5BFMEYIP7TUNLHA

//standard libraries
#include <string>
#include <string.h>
#include <sstream>
#include <cctype>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <exception>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <vector>
#include <algorithm>

/*** Macros ***/
/// Determines the length of an array using sizeof().
#define length_array(a) ( sizeof ( a ) / sizeof ( *a ) )

#if defined __APPLE__ || defined __WIN_GNUC__
#if (__GNUC__ >= 4)
  #include <cmath>
  #define isnan(x) std::isnan(x)
#else
  #include <math.h>
  #define isnan(x) __isnand((double)x)
#endif
#endif

#ifdef __WIN_MSVC__
    #define isnan(x) ((x) != (x))
#endif

/// The 'pyne' namespace all PyNE functionality is included in.
namespace pyne {

  void pyne_start (); ///< Initializes PyNE based on environment.

  /// Path to the directory containing the PyNE data.
  extern std::string PYNE_DATA;
  extern std::string NUC_DATA_PATH; ///< Path to the nuc_data.h5 file.

  // String Transformations
  /// string of digit characters
  static std::string digits = "0123456789";
  /// uppercase alphabetical characters
  static std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  /// string of all valid word characters for variable names in programing languages.
  static std::string words = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";

  /// \name String Conversion Functions
  /// \{
  /// Converts the variables of various types to their C++ string representation.
  std::string to_str(int t);
  std::string to_str(unsigned int t);
  std::string to_str(double t);
  std::string to_str(bool t);
  /// \}

  int to_int(std::string s);  ///< Converts a string of digits to an int using atoi().

  double to_dbl(std::string s);  ///< Converts a valid string to a float using atof().

  double endftod(char * s); ///< Converts a string from ENDF format to a float.

  /// Returns an all upper case copy of the string.
  std::string to_upper(std::string s);

  /// Returns an all lower case copy of the string.
  std::string to_lower(std::string s);

  /// Returns a capitalized copy of the string.
  std::string capitalize(std::string s);

  /// Finds and returns the first white-space delimited token of a line.
  /// \param line a character array to take the first token from.
  /// \param max_l an upper bound to the length of the token.  Must be 11 or less.
  /// \returns a the flag as a string
  std::string get_flag(char line[], int max_l);

  /// Creates a copy of \a s with all instances of \a substr taken out.
  std::string remove_substring(std::string s, std::string substr);

  /// Removes all characters in the string \a chars from \a s.
  std::string remove_characters(std::string s, std::string chars);

  /// Replaces all instance of \a substr in \a s with \a repstr.
  std::string replace_all_substrings(std::string s, std::string substr,
                                                    std::string repstr);

  /// Returns the last character in a string.
  std::string last_char(std::string s);

  /// Returns the slice of a string \a s using the negative index \a n and the
  /// length of the slice \a l.
  std::string slice_from_end(std::string s, int n=-1, int l=1);

  /// Returns true if \a a <= \a b <= \a c and flase otherwise.
  bool ternary_ge(int a, int b, int c);

  /// Returns true if \a substr is in \a s.
  bool contains_substring(std::string s, std::string substr);

  /// Calculates a version of the string \a name that is also a valid variable name. 
  /// That is to say that the return value uses only word characters.
  std::string natural_naming(std::string name);

  /// Finds the slope of a line from the points (\a x1, \a y1) and (\a x2, \a y2).
  double slope (double x2, double y2, double x1, double y1);

  /// Solves the equation for the line y = mx + b, given \a x and the points that 
  /// form the line: (\a x1, \a y1) and (\a x2, \a y2).
  double solve_line (double x, double x2, double y2, double x1, double y1);

  double tanh(double x);  ///< The hyperbolic tangent function.
  double coth(double x);  ///< The hyperbolic cotangent function.


  // File Helpers
  /// Returns true if the file can be found.
  bool file_exists(std::string strfilename);

  /// Custom exception to be thrown in the event that a required file is not able to
  /// be found.
  class FileNotFound : public std::exception
  {
  public:

    /// default constructor
    FileNotFound () {};

    /// default destructor
    ~FileNotFound () throw () {};

    /// constructor with the filename \a fname.
    FileNotFound(std::string fname)
    {
      filename = fname;
    };

    /// Creates a helpful error message.
    virtual const char* what() const throw()
    {
      std::string FNFstr ("File not found: ");
      if (!filename.empty())
        FNFstr += filename;

      return (const char *) FNFstr.c_str();
    };

  private:
    std::string filename; ///< unfindable filename.
  };


// End PyNE namespace
};

#endif  // PYNE_KMMHYNANYFF5BFMEYIP7TUNLHA
//
// end of cpp/pyne.h
//


//
// start of cpp/extra_types.h
//
/// \file extra_types.h
/// \author Anthony Scopatz (scopatz\@gmail.com)
///
/// Provides some extra types that may be generally useful

#if !defined(_XDRESS_EXTRA_TYPES_)
#define _XDRESS_EXTRA_TYPES_

#if defined(__cplusplus)
namespace extra_types
{
  /// complex type struct, matching PyTables definition
  typedef struct {
    double re;  ///< real part
    double im;  ///< imaginary part
  } complex_t;

  /// Chivalrously handles C++ memory issues that Cython does
  /// not yet have a syntax for.  This is a template class,
  /// rather than three template functions, because Cython does
  /// not yet support template function wrapping.
  template <class T>
  class MemoryKnight
  {
    public:
      MemoryKnight(){};   ///< Default constructor
      ~MemoryKnight(){};  ///< Default Destructor

      /// Creates a new instance of type T on the heap using
      /// its default constructor.
      /// \return T *
      T * defnew(){return new T();};

      /// Creates a new instance of type T, using T's default 
      /// constructor, at a given location.
      /// \param void * ptr, location to create T instance
      /// \return value of ptr recast as T *
      T * renew(void * ptr){return new (ptr) T();};

      /// Deallocates a location in memory using delete. 
      /// \param T * ptr, location to remove
      void deall(T * ptr){delete ptr;};
  };

// End namespace extra_types
};

#elif defined(__STDC__)

/// complex type struct, matching PyTables definition
typedef struct {
  double re;  ///< real part
  double im;  ///< imaginary part
} complex_t;

#endif

#endif

//
// end of cpp/extra_types.h
//


//
// start of cpp/h5wrap.h
//
/// \file h5wrap.h
/// \author Anthony Scopatz (scopatz\@gmail.com)
///
/// \brief Provides some HDF5 helper functionality in its own namespace

#ifndef PYNE_MRNAFG5GNZDNPCRPX3UCBZ5MFE
#define PYNE_MRNAFG5GNZDNPCRPX3UCBZ5MFE

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <exception>

#include "hdf5.h"

#ifndef PYNE_IS_AMALGAMATED
#include "extra_types.h"
#endif


namespace h5wrap
{
  /// Custom exception for HDF5 indexing errors.
  class HDF5BoundsError: public std::exception
  {
    /// returns error message.
    virtual const char* what() const throw()
    {
      return "Index of point is out of bounds.  Cannot handle in HDF5 file.";
    };
  };


  /// Custom exception for when an existing file is not in a valid HDF5 format.
  class FileNotHDF5: public std::exception
  {
  public:

    /// default constructor
    FileNotHDF5(){};

    /// default destructor
    ~FileNotHDF5() throw () {};

    /// constructor with the filename
    FileNotHDF5(std::string fname)
    {
      filename = fname;
    };

    /// helpful error message that includes the filename
    virtual const char* what() const throw()
    {
      std::string FNH5str ("Not a valid HDF5 file: ");
      if (!filename.empty())
        FNH5str += filename;

      return (const char *) FNH5str.c_str();
    };

  private:
    std::string filename; ///< the file which is not in HDF5 format.
  };


  /// Custom exception for when a group cannot be found in an HDF5 file.
  class GroupNotFound: public std::exception
  {
  public:

    /// default constructor
    GroupNotFound(){};

    /// default destructor
    ~GroupNotFound() throw () {};

    /// constructor with the filename and the groupname
    GroupNotFound(std::string fname, std::string gname)
    {
      filename = fname;
    };

    /// helpful error message that includes the filename and the groupname
    virtual const char* what() const throw()
    {
      std::string msg ("the group ");
      msg += groupname;
      msg += " not found in the file ";
      msg += filename;
      return (const char *) msg.c_str();
    };

  private:
    std::string filename;   ///< the HDF5 file
    std::string groupname;  ///< the group in the hierarchy
  };

  /// Custom exception for when a path is not found in an HDF5 file
  class PathNotFound: public std::exception
  {
  public:

    /// default constructor
    PathNotFound(){};

    /// default destructor
    ~PathNotFound() throw () {};

    /// constructor with the filename and the pathname
    PathNotFound(std::string fname, std::string pname)
    {
      filename = fname;
      path = pname;
    };

    /// helpful error message that includes the filename and the pathname
    virtual const char* what() const throw()
    {
      std::string msg ("the path ");
      msg += path;
      msg += " was not found in the HDF5 file ";
      msg += filename;
      return (const char *) msg.c_str();
    };

  private:
    std::string filename; ///< the HDF5 file
    std::string path;     ///< the path in the file
  };



  // Read-in Functions

  /// Retrieves the \a nth index out of the dataset \a dset (which has an HDF5 
  /// datatype \a dtype).  The value is returned as the C/C++ type given by \a T.
  template <typename T>
  T get_array_index(hid_t dset, int n, hid_t dtype=H5T_NATIVE_DOUBLE)
  {
    hsize_t count  [1] = {1};
    hsize_t offset [1] = {n};

    hid_t dspace = H5Dget_space(dset);
    hsize_t npoints = H5Sget_simple_extent_npoints(dspace);

    //Handle negative indices
    if (n < 0)
        offset[0] = offset[0] + npoints;

    //If still out of range we have a problem
    if (npoints <= offset[0])
        throw HDF5BoundsError();

    H5Sselect_hyperslab(dspace, H5S_SELECT_SET, offset, NULL, count, NULL);

    //Set memmory hyperspace
    hsize_t dimsm[1] = {1};
    hid_t memspace = H5Screate_simple(1, dimsm, NULL);

    hsize_t count_out  [1] = {1};
    hsize_t offset_out [1] = {0};

    H5Sselect_hyperslab(memspace, H5S_SELECT_SET, offset_out, NULL, 
                                 count_out, NULL);

    T data_out [1];
    H5Dread(dset, dtype, memspace, dspace, H5P_DEFAULT, data_out);

    return data_out[0];
  };


  // Conversion functions

  /// Reads in data from an HDF5 file as a C++ set.  \a T should roughly match 
  /// \a dtype.
  /// \param h5file HDF5 file id for an open file.
  /// \param data_path path to the data in the open file.
  /// \param dtype HDF5 data type for the data set at \a data_path.
  /// \return an in memory set of type \a T.
  template <typename T>
  std::set<T> h5_array_to_cpp_set(hid_t h5file, std::string data_path, hid_t dtype=H5T_NATIVE_DOUBLE)
  {
    std::set<T> cpp_set = std::set<T>();
    hsize_t arr_len[1];
    hid_t dset = H5Dopen2(h5file, data_path.c_str(), H5P_DEFAULT);

    // Initilize to dataspace, to find the indices we are looping over
    hid_t arr_space = H5Dget_space(dset);
    int arr_dim = H5Sget_simple_extent_dims(arr_space, arr_len, NULL);

    // Read in data from file to memory
    T * mem_arr = new T [arr_len[0]];
    H5Dread(dset, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, mem_arr);

    // Load new values into the set
    cpp_set.insert(&mem_arr[0], &mem_arr[arr_len[0]]);

    H5Dclose(dset);

    delete[] mem_arr;
    return cpp_set;
  };


  /// Reads in data from an HDF5 file as a 1 dimiensional vector.  \a T should roughly 
  /// match \a dtype.
  /// \param h5file HDF5 file id for an open file.
  /// \param data_path path to the data in the open file.
  /// \param dtype HDF5 data type for the data set at \a data_path.
  /// \return an in memory 1D vector of type \a T.
  template <typename T>
  std::vector<T> h5_array_to_cpp_vector_1d(hid_t h5file, std::string data_path, 
                                           hid_t dtype=H5T_NATIVE_DOUBLE)
  {
    std::vector<T> cpp_vec;
    hsize_t arr_dims [1];
    hid_t dset = H5Dopen2(h5file, data_path.c_str(), H5P_DEFAULT);

    // Initilize to dataspace, to find the indices we are looping over
    hid_t arr_space = H5Dget_space(dset);
    int arr_ndim = H5Sget_simple_extent_dims(arr_space, arr_dims, NULL);

    // Read in data from file to memory
    T mem_arr [arr_dims[0]];
    H5Dread(dset, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, mem_arr);

    // Load new values into the vector
    cpp_vec.assign(mem_arr, mem_arr+arr_dims[0]);

    H5Dclose(dset);
    return cpp_vec;
  };


  /// Reads in data from an HDF5 file as a 2 dimiensional vector.  \a T should roughly 
  /// match \a dtype.
  /// \param h5file HDF5 file id for an open file.
  /// \param data_path path to the data in the open file.
  /// \param dtype HDF5 data type for the data set at \a data_path.
  /// \return an in memory 2D vector of type \a T.
  template <typename T>
  std::vector< std::vector<T> > h5_array_to_cpp_vector_2d(hid_t h5file, std::string data_path, 
                                                          hid_t dtype=H5T_NATIVE_DOUBLE)
  {
    hsize_t arr_dims [2];
    hid_t dset = H5Dopen2(h5file, data_path.c_str(), H5P_DEFAULT);

    // Initilize to dataspace, to find the indices we are looping over
    hid_t arr_space = H5Dget_space(dset);
    int arr_ndim = H5Sget_simple_extent_dims(arr_space, arr_dims, NULL);

    // Read in data from file to memory
    // Have to read in as 1D array to get HDF5 and new keyword
    // to play nice with each other
    T mem_arr [arr_dims[0] * arr_dims[1]];
    H5Dread(dset, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, mem_arr);

    // Load new values into the vector of vectors, using some indexing tricks
    std::vector< std::vector<T> > cpp_vec (arr_dims[0], std::vector<T>(arr_dims[1]));
    for(int i = 0; i < arr_dims[0]; i++)
    {
        cpp_vec[i].assign(mem_arr+(i*arr_dims[1]), mem_arr+((i+1)*arr_dims[1]));
    };

    H5Dclose(dset);
    return cpp_vec;
  };


  /// Reads in data from an HDF5 file as a 3 dimiensional vector.  \a T should roughly 
  /// match \a dtype.
  /// \param h5file HDF5 file id for an open file.
  /// \param data_path path to the data in the open file.
  /// \param dtype HDF5 data type for the data set at \a data_path.
  /// \return an in memory 3D vector of type \a T.
  template <typename T>
  std::vector< std::vector< std::vector<T> > > h5_array_to_cpp_vector_3d(hid_t h5file, 
                                                  std::string data_path, 
                                                  hid_t dtype=H5T_NATIVE_DOUBLE)
  {
    hsize_t arr_dims [3];
    hid_t dset = H5Dopen2(h5file, data_path.c_str(), H5P_DEFAULT);

    // Initilize to dataspace, to find the indices we are looping over
    hid_t arr_space = H5Dget_space(dset);
    int arr_ndim = H5Sget_simple_extent_dims(arr_space, arr_dims, NULL);

    // Read in data from file to memory
    // Have to read in as 1D array to get HDF5 and new keyword
    // to play nice with each other
    T mem_arr [arr_dims[0] * arr_dims[1] * arr_dims[2]];
    H5Dread(dset, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, mem_arr);

    // Load new values into the vector of vectors of vectors, using some indexing tricks
    std::vector< std::vector< std::vector<T> > > cpp_vec (arr_dims[0], std::vector< std::vector<T> >(arr_dims[1], std::vector<T>(arr_dims[2])));
    for(int i = 0; i < arr_dims[0]; i++)
    {
        for(int j = 0; j < arr_dims[1]; j++)
        {
            cpp_vec[i][j].assign(mem_arr+((i*arr_dims[1]*arr_dims[2]) + (j*arr_dims[2])), mem_arr+((i*arr_dims[1]*arr_dims[2]) + ((j+1)*arr_dims[2])));
        };
    };

    H5Dclose(dset);
    return cpp_vec;
  };



  // Classes
  /// A class representing a high-level table contruct whose columns all have the same
  /// type \a T in C/C++ (and the analogous type in HDF5).
  template <typename T>
  class HomogenousTypeTable
  {
  public:

    /// default constructor
    HomogenousTypeTable(){};

    /// default destructor
    ~HomogenousTypeTable(){};

    /// Constructor to load in data upon initialization.  \a T should roughly 
    /// match \a dtype.
    /// \param h5file HDF5 file id for an open file.
    /// \param data_path path to the data in the open file.
    /// \param dtype HDF5 data type for the data set at \a data_path.
    HomogenousTypeTable(hid_t h5file, std::string data_path, hid_t dtype=H5T_NATIVE_DOUBLE)
    {
      hid_t h5_set = H5Dopen2(h5file, data_path.c_str(), H5P_DEFAULT);
      hid_t h5_space = H5Dget_space(h5_set);
      hid_t h5_type = H5Dget_type(h5_set);

      // set path
      path = data_path;

      // set shape
      shape[0] = H5Sget_simple_extent_npoints(h5_space);
      shape[1] = H5Tget_nmembers(h5_type);

      // set cols
      std::string * cols_buf = new std::string [shape[1]];
      for(int n = 0; n < shape[1]; n++)
        cols_buf[n] = H5Tget_member_name(h5_type, n);
      cols.assign(cols_buf, cols_buf+shape[1]);

      // set data
      hid_t col_type;
      T * col_buf = new T [shape[0]];

      data.clear();
      for(int n = 0; n < shape[1]; n++)
      {
        // Make a compound data type of just this column
        col_type = H5Tcreate(H5T_COMPOUND, sizeof(T));
        H5Tinsert(col_type, cols[n].c_str(), 0, dtype);

        // Read in this column
        H5Dread(h5_set, col_type, H5S_ALL, H5S_ALL, H5P_DEFAULT, col_buf);

        // save this column as a vector in out data map
        data[cols[n]] = std::vector<T>(col_buf, col_buf+shape[0]);
      };
      delete[] col_buf;
    };

    // Metadata attributes 
    std::string path; ///< path in file to the data
    int shape [2];    ///< table shape, rows x columns.
    std::vector<std::string> cols;  ///< column names
    /// mapping from column names to column data
    std::map<std::string, std::vector<T> > data;  

    //
    // operator overloads
    //
    /// index into the table by column name (string) 
    std::vector<T> operator[] (std::string col_name)
    {
      return data[col_name];
    };

    /// index into the table by row
    std::map<std::string, T> operator[] (int m)
    {
      std::map<std::string, T> row = std::map<std::string, T>();

      for(int n = 0; n < shape[1]; n++)
        row[cols[n]] = data[cols[n]][m];

      return row;
    };
  };


  /// Create an HDF5 data type for complex 128 bit data, which happens to match the
  /// complex data type that is used by PyTables ^_~.
  inline hid_t _get_PYTABLES_COMPLEX128()
  {
    hid_t ct = H5Tcreate(H5T_COMPOUND, sizeof(extra_types::complex_t));
    H5Tinsert(ct, "r", HOFFSET(extra_types::complex_t, re), H5T_NATIVE_DOUBLE);
    H5Tinsert(ct, "i", HOFFSET(extra_types::complex_t, im), H5T_NATIVE_DOUBLE);
    return ct;
  };

  /// The HDF5 id for a complex data type compatible with PyTables generated data.
  static hid_t PYTABLES_COMPLEX128 = _get_PYTABLES_COMPLEX128();


  /// Determines if a path exists in an hdf5 file.
  /// \param h5file HDF5 file id for an open file.
  /// \param path path to the data in the open file.
  /// \return true or false
  inline bool path_exists(hid_t h5file, std::string path)
  {
    bool rtn = false;
    hid_t ds = H5Dopen2(h5file, path.c_str(), H5P_DEFAULT);
    if (0 <= ds)
    {
      rtn = true;
      H5Dclose(ds);
    }
    else 
    {
      hid_t grp = H5Gopen2(h5file, path.c_str(), H5P_DEFAULT);
      if (0 <= grp)
      {
        rtn = true;
        H5Gclose(grp);
      }
    }
    return rtn;
  };


// End namespace h5wrap
};



#endif

//
// end of cpp/h5wrap.h
//


//
// start of cpp/nucname.h
//
/// \file nucname.h
/// \author Anthony Scopatz (scopatz\@gmail.com)
///
/// \brief Converts between naming conventions for nuclides.

#ifndef PYNE_D35WIXV5DZAA5LLOWBY2BL2DPA
#define PYNE_D35WIXV5DZAA5LLOWBY2BL2DPA
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <exception>
#include <stdlib.h>
#include <stdio.h>

#ifndef PYNE_IS_AMALGAMATED
#include "pyne.h"
#endif

namespace pyne
{
namespace nucname 
{
  typedef std::string name_t; ///< name type
  typedef int zz_t;           ///< Z number type

  typedef std::map<name_t, zz_t> name_zz_t; ///< name and Z num map type
  typedef name_zz_t::iterator name_zz_iter; ///< name and Z num iter type
  name_zz_t get_name_zz();  ///< Creates standard name to Z number mapping.
  extern name_zz_t name_zz; ///< name to Z num map

  typedef std::map<zz_t, name_t> zzname_t;  ///< Z num to name map type
  typedef zzname_t::iterator zzname_iter;   ///< Z num to name iter type
  zzname_t get_zz_name();   ///< Creates standard Z number to name mapping.
  extern zzname_t zz_name;  ///< Z num to name map

  /******************************************/
  /*** Define useful elemental group sets ***/
  /******************************************/

  /// name grouping type (for testing containment)
  typedef std::set<name_t> name_group;  
  typedef name_group::iterator name_group_iter; ///< name grouping iter type

  /// Z number grouping type (for testing containment)
  typedef std::set<zz_t> zz_group;
  typedef zz_group::iterator zz_group_iter; ///< Z number grouping iter

  /// Converts a name group to a Z number group.
  /// \param eg a grouping of nuclides by name
  /// \return a Z numbered group
  zz_group name_to_zz_group (name_group eg);

  extern name_t LAN_array[15];  ///< array of lanthanide names
  extern name_group LAN;        ///< lanthanide name group
  extern zz_group lan;          ///< lanthanide Z number group

  extern name_t ACT_array[15];  ///< array of actinide names
  extern name_group ACT;        ///< actinide name group
  extern zz_group act;          ///< actinide Z number group

  extern name_t TRU_array[22];  ///< array of transuranic names
  extern name_group TRU;        ///< transuranic name group
  extern zz_group tru;          ///< transuranic Z number group

  extern name_t MA_array[10];   ///< array of minor actinide names
  extern name_group MA;         ///< minor actinide name group
  extern zz_group ma;           ///< minor actinide Z number group

  extern name_t FP_array[88];   ///< array of fission product names
  extern name_group FP;         ///< fission product name group
  extern zz_group fp;           ///< fission product Z number group


  /******************/
  /*** Exceptions ***/
  /******************/

  /// Custom expection for declaring that a value does not follow a recognizable 
  /// nuclide naming convention.
  class NotANuclide : public std::exception
  {
  public:
    /// default constructor
    NotANuclide () {}; 

    /// default destructor
    ~NotANuclide () throw () {};

    /// Constructor given previous and current state of nulide name
    /// \param wasptr Previous state, typically user input.
    /// \param nowptr Current state, as far as PyNE could get.
    NotANuclide(std::string wasptr, std::string nowptr)
    {
       nucwas = wasptr;
       nucnow = nowptr;
    };

    /// Constructor given previous and current state of nulide name
    /// \param wasptr Previous state, typically user input.
    /// \param nowptr Current state, as far as PyNE could get.
    NotANuclide(std::string wasptr, int nowptr)
    {
      nucwas = wasptr;
      nucnow = pyne::to_str(nowptr);
    };

    /// Constructor given previous and current state of nulide name
    /// \param wasptr Previous state, typically user input.
    /// \param nowptr Current state, as far as PyNE could get.
    NotANuclide(int wasptr, std::string nowptr)
    {
      nucwas = pyne::to_str(wasptr);
      nucnow = nowptr;
    };

    /// Constructor given previous and current state of nulide name
    /// \param wasptr Previous state, typically user input.
    /// \param nowptr Current state, as far as PyNE could get.
    NotANuclide(int wasptr, int nowptr)
    {
      nucwas = pyne::to_str(wasptr);
      nucnow = pyne::to_str(nowptr);
    };

    /// Generates an informational message for the exception
    /// \return The error string
    virtual const char* what() const throw()
    {
      std::string NaNEstr ("Not a Nuclide! ");
      if (!nucwas.empty())
        NaNEstr += nucwas;

      if (!nucnow.empty())
      {
        NaNEstr += " --> "; 
        NaNEstr += nucnow;
      }
      return (const char *) NaNEstr.c_str();
    };

  private:
    std::string nucwas; ///< previous nuclide state
    std::string nucnow; ///< current nuclide state  
  };

  /// Custom expection for declaring that a value represents one or more nuclides 
  /// in one or more namig conventions
  class IndeterminateNuclideForm : public std::exception
  {
  public:
    /// default constructor
    IndeterminateNuclideForm () {};

    /// default destuctor
    ~IndeterminateNuclideForm () throw () {};

    /// Constructor given previous and current state of nulide name
    /// \param wasptr Previous state, typically user input.
    /// \param nowptr Current state, as far as PyNE could get.
    IndeterminateNuclideForm(std::string wasptr, std::string nowptr)
    {
       nucwas = wasptr;
       nucnow = nowptr;
    };

    /// Constructor given previous and current state of nulide name
    /// \param wasptr Previous state, typically user input.
    /// \param nowptr Current state, as far as PyNE could get.
    IndeterminateNuclideForm(std::string wasptr, int nowptr)
    {
      nucwas = wasptr;
      nucnow = pyne::to_str(nowptr);
    };

    /// Constructor given previous and current state of nulide name
    /// \param wasptr Previous state, typically user input.
    /// \param nowptr Current state, as far as PyNE could get.
    IndeterminateNuclideForm(int wasptr, std::string nowptr)
    {
      nucwas = pyne::to_str(wasptr);
      nucnow = nowptr;
    };

    /// Constructor given previous and current state of nulide name
    /// \param wasptr Previous state, typically user input.
    /// \param nowptr Current state, as far as PyNE could get.
    IndeterminateNuclideForm(int wasptr, int nowptr)
    {
      nucwas = pyne::to_str(wasptr);
      nucnow = pyne::to_str(nowptr);
    };

    /// Generates an informational message for the exception
    /// \return The error string
    virtual const char* what() const throw()
    {
      std::string INFEstr ("Indeterminate nuclide form: ");
      if (!nucwas.empty())
        INFEstr += nucwas;

      if (!nucnow.empty())
      {
        INFEstr += " --> "; 
        INFEstr += nucnow;
      }
      return (const char *) INFEstr.c_str();
    }

  private:
    std::string nucwas; ///< previous nuclide state
    std::string nucnow; ///< current nuclide state
  };

  /// \name isnuclide functions
  /// \{
  /// These functions test if an input \a nuc is a valid nuclide.
  /// \param nuc a possible nuclide
  /// \return a bool
  bool isnuclide(std::string nuc);
  bool isnuclide(char * nuc);
  bool isnuclide(int nuc);
  /// \}

  /// \name Identifier Form Functions
  /// \{
  /// The 'id' nuclide naming convention is the canonical form for representing
  /// nuclides in PyNE. This is termed a ZAS, or ZZZAAASSSS, representation because 
  /// It stores 3 Z-number digits, 3 A-number digits, followed by 4 S-number digits
  /// which the nucleus excitation state. 
  /// \param nuc a nuclide
  /// \return nucid 32-bit integer identifier
  int id(int nuc);
  int id(char * nuc);
  int id(std::string nuc);
  /// \}

  /// \name Name Form Functions
  /// \{
  /// The 'name' nuclide naming convention is the more common, human readable 
  /// notation. The chemical symbol (one or two characters long) is first, followed 
  /// by the nucleon number. Lastly if the nuclide is metastable, the letter M is 
  /// concatenated to the end. For example, ‘H-1’ and ‘Am242M’ are both valid. 
  /// Note that nucname will always return name form with the dash removed and all 
  /// letters uppercase.
  /// \param nuc a nuclide
  /// \return a string nuclide identifier.
  std::string name(int nuc);
  std::string name(char * nuc);
  std::string name(std::string nuc);
  /// \}

  /// \name Z-Number Functions
  /// \{
  /// The Z-number, or charge number, represents the number of protons in a
  /// nuclide.  This function returns that number.
  /// \param nuc a nuclide
  /// \return an integer Z-number.
  int znum(int nuc);
  int znum(char * nuc);
  int znum(std::string nuc);
  /// \}

  /// \name A-Number Functions
  /// \{
  /// The A-number, or nucleon number, represents the number of protons and 
  /// neutrons in a nuclide.  This function returns that number.
  /// \param nuc a nuclide
  /// \return an integer A-number.
  int anum(int nuc);
  int anum(char * nuc);
  int anum(std::string nuc);
  /// \}

  /// \name S-Number Functions
  /// \{
  /// The S-number, or excitation state number, represents the excitation
  /// level of a nuclide.  Normally, this is zero.  This function returns 
  /// that number.
  /// \param nuc a nuclide
  /// \return an integer A-number.
  int snum(int nuc);
  int snum(char * nuc);
  int snum(std::string nuc);
  /// \}

  /// \name ZZAAAM Form Functions
  /// \{
  /// The ZZAAAM nuclide naming convention is the former canonical form for 
  /// nuclides in PyNE. This places the charge of the nucleus out front, then has 
  /// three digits for the atomic mass number, and ends with a metastable flag 
  /// (0 = ground, 1 = first excited state, 2 = second excited state, etc). 
  /// Uranium-235 here would be expressed as ‘922350’.
  /// \param nuc a nuclide
  /// \return an integer nuclide identifier.
  int zzaaam(int nuc);
  int zzaaam(char * nuc);
  int zzaaam(std::string nuc);
  /// \}

  /// \name ZZAAAM Form to Identifier Form Functions
  /// \{
  /// This converts from the ZZAAAM nuclide naming convention 
  /// to the id canonical form  for nuclides in PyNE. 
  /// \param nuc a nuclide in ZZAAAM form.
  /// \return an integer id nuclide identifier.
  int zzaaam_to_id(int nuc);
  int zzaaam_to_id(char * nuc);
  int zzaaam_to_id(std::string nuc);
  /// \}

  /// \name MCNP Form Functions
  /// \{
  /// This is the naming convention used by the MCNP suite of codes.
  /// The MCNP format for entering nuclides is unfortunately non-standard. 
  /// In most ways it is similar to zzaaam form, except that it lacks the metastable 
  /// flag. For information on how metastable isotopes are named, please consult the 
  /// MCNP documentation for more information.
  /// \param nuc a nuclide
  /// \return a string nuclide identifier.
  int mcnp(int nuc);
  int mcnp(char * nuc);
  int mcnp(std::string nuc);
  /// \}

  /// \name MCNP Form to Identifier Form Functions
  /// \{
  /// This converts from the MCNP nuclide naming convention 
  /// to the id canonical form  for nuclides in PyNE. 
  /// \param nuc a nuclide in MCNP form.
  /// \return an integer id nuclide identifier.
  int mcnp_to_id(int nuc);
  int mcnp_to_id(char * nuc);
  int mcnp_to_id(std::string nuc);
  /// \}

  /// \name Serpent Form Functions
  /// \{
  /// This is the string-based naming convention used by the Serpent suite of codes.
  /// The serpent naming convention is similar to name form. However, only the first 
  /// letter in the chemical symbol is uppercase, the dash is always present, and the 
  /// the meta-stable flag is lowercase. For instance, ‘Am-242m’ is the valid serpent 
  /// notation for this nuclide.
  /// \param nuc a nuclide
  /// \return a string nuclide identifier.
  std::string serpent(int nuc);
  std::string serpent(char * nuc);
  std::string serpent(std::string nuc);
  /// \}

  /// \name Serpent Form to Identifier Form Functions
  /// \{
  /// This converts from the Serpent nuclide naming convention 
  /// to the id canonical form  for nuclides in PyNE. 
  /// \param nuc a nuclide in Serpent form.
  /// \return an integer id nuclide identifier.
  //int serpent_to_id(int nuc);  Should be ZAID
  int serpent_to_id(char * nuc);
  int serpent_to_id(std::string nuc);
  /// \}

  /// \name NIST Form Functions
  /// \{
  /// This is the string-based naming convention used by NIST.
  /// The NIST naming convention is also similar to the Serpent form. However, this 
  /// convention contains no metastable information. Moreover, the A-number comes 
  /// before the element symbol. For example, ‘242Am’ is the valid NIST notation.
  /// \param nuc a nuclide
  /// \return a string nuclide identifier.
  std::string nist(int nuc);
  std::string nist(char * nuc);
  std::string nist(std::string nuc);
  /// \}

  /// \name NIST Form to Identifier Form Functions
  /// \{
  /// This converts from the NIST nuclide naming convention 
  /// to the id canonical form  for nuclides in PyNE. 
  /// \param nuc a nuclide in NIST form.
  /// \return an integer id nuclide identifier.
  //int serpent_to_id(int nuc);  NON-EXISTANT
  int nist_to_id(char * nuc);
  int nist_to_id(std::string nuc);
  /// \}

  /// \name CINDER Form Functions
  /// \{
  /// This is the naming convention used by the CINDER burnup library.
  /// The CINDER format is similar to zzaaam form except that the placement of the 
  /// Z- and A-numbers are swapped. Therefore, this format is effectively aaazzzm. 
  /// For example, ‘2420951’ is the valid cinder notation for ‘AM242M’.
  /// \param nuc a nuclide
  /// \return a string nuclide identifier.
  int cinder(int nuc);
  int cinder(char * nuc);
  int cinder(std::string nuc);
  /// \}

  /// \name Cinder Form to Identifier Form Functions
  /// \{
  /// This converts from the Cinder nuclide naming convention 
  /// to the id canonical form  for nuclides in PyNE. 
  /// \param nuc a nuclide in Cinder form.
  /// \return an integer id nuclide identifier.
  int cinder_to_id(int nuc);
  int cinder_to_id(char * nuc);
  int cinder_to_id(std::string nuc);
  /// \}

  /// \name ALARA Form Functions
  /// \{
  /// This is the format used in the ALARA activation code elements library.
  /// For elements, the form is "ll" where ll is the atomic symbol. For isotopes
  /// the form is "ll:AAA". No metastable isotope flag is used.
  /// \param nuc a nuclide
  /// \return a string nuclide identifier.
  std::string alara(int nuc);
  std::string alara(char * nuc);
  std::string alara(std::string nuc);
  /// \}

  /// \name ALARA Form to Identifier Form Functions
  /// \{
  /// This converts from the ALARA nuclide naming convention 
  /// to the id canonical form  for nuclides in PyNE. 
  /// \param nuc a nuclide in ALARA form.
  /// \return an integer id nuclide identifier.
  //int alara_to_id(int nuc); NOT POSSIBLE
  int alara_to_id(char * nuc);
  int alara_to_id(std::string nuc);
  /// \}

  /// \name SZA Form Functions
  /// \{
  /// This is the new format for ACE data tables in the form SSSZZZAAA.
  /// The first three digits represent the excited state (000 = ground,
  /// 001 = first excited state, 002 = second excited state, etc).
  /// The second three digits are the atomic number and the last three
  /// digits are the atomic mass. Prepending zeros can be omitted, making
  /// the SZA form equal to the MCNP form for non-excited nuclides.
  /// \param nuc a nuclide
  /// \return a string nuclide identifier.
  int sza(int nuc);
  int sza(char * nuc);
  int sza(std::string nuc);
  /// \}

  /// \name SZA Form to Identifier Form Functions
  /// \{
  /// This converts from the SZA nuclide naming convention 
  /// to the id canonical form  for nuclides in PyNE. 
  /// \param nuc a nuclide in SZA form.
  /// \return an integer id nuclide identifier.
  int sza_to_id(int nuc);
  int sza_to_id(char * nuc);
  int sza_to_id(std::string nuc);
  /// \}

};
};

#endif  // PYNE_D35WIXV5DZAA5LLOWBY2BL2DPA
//
// end of cpp/nucname.h
//


//
// start of cpp/rxname.h
//
/// \file rxname.h
/// \author Anthony Scopatz (scopatz\@gmail.com)
///
/// \brief Converts between naming conventions for reaction channels.

#ifndef PYNE_7DOEB2PKSBEFFIA3Q2NARI3KFY
#define PYNE_7DOEB2PKSBEFFIA3Q2NARI3KFY
#include <utility>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <exception>
#include <stdlib.h>
#include <stdio.h>

#ifndef PYNE_IS_AMALGAMATED
#include "pyne.h"
#include "nucname.h"
#endif

/// Number of reactions supported by default.
#define NUM_RX_NAMES 549

namespace pyne
{
namespace rxname 
{
  extern std::string _names[NUM_RX_NAMES];  ///< Raw array of reaction names
  /// Set of reaction names, must be valid variable names.
  extern std::set<std::string> names;       
  /// Mapping from reaction ids to reaction names. 
  extern std::map<unsigned int, std::string> id_name;
  /// Mapping from reaction names to reaction ids. 
  extern std::map<std::string, unsigned int> name_id;
  /// Mapping between alternative names for reactions and the reaction id.
  extern std::map<std::string, unsigned int> altnames;
  /// Mapping from reaction ids to MT numbers.
  extern std::map<unsigned int, unsigned int> id_mt;
  /// Mapping from MT numbers to reaction names.
  extern std::map<unsigned int, unsigned int> mt_id;
  /// Mapping from reaction ids to labels (short descriptions).
  extern std::map<unsigned int, std::string> labels;
  /// Mapping from reaction ids to documentation strings (long descriptions).
  extern std::map<unsigned int, std::string> docs;
  /// Mapping from particle type and offset pairs to reaction ids.  
  /// Particle flags are 'n', 'p', 'd', 't', 'He3', 'a', 'gamma', and 'decay'.
  extern std::map<std::pair<std::string, int>, unsigned int> offset_id;
  /// Mapping from particle type and reaction ids to offsets.
  /// Particle flags are 'n', 'p', 'd', 't', 'He3', 'a', 'gamma', and 'decay'.
  extern std::map<std::pair<std::string, unsigned int>, int> id_offset;

  /// A helper function to set the contents of the variables in this library.
  void * _fill_maps();  
  extern void * _;  ///< A dummy variable used when calling #_fill_maps().

  /// A helper function to compute nuclide id offsets from z-, a-, and s- deltas
  inline int offset(int dz, int da, int ds=0) {return dz*10000000 + da*10000 + ds;};

  /// \name Hash Functions
  /// \{
  /// Custom hash function for reaction name to reaction ids.
  /// This functions will not return a value less than 1000, effectively reserving
  /// space for the MT numbers.
  unsigned int hash(std::string s);
  unsigned int hash(const char * s);
  /// \}

  /// \name Name Functions
  /// \{
  /// Returns the canonical name of a reaction channel.
  /// \param n Integer input of possible reaction, nominally an id or MT number.
  /// \param s String input of possible reaction, often a reaction or alternate name.
  /// \param from_nuc Initial target nuclide prior to reaction.  When \a from_nuc is
  ///                 an integer it must be in id form.
  /// \param to_nuc Target nuclide after reaction occurs.  When \a to_nuc is
  ///               an integer it must be in id form.
  /// \param z Flag for incident particle type.
  ///          Particle flags are 'n', 'p', 'd', 't', 'He3', 'a', 'gamma', and 'decay'.
  std::string name(int n);
  std::string name(unsigned int n);
  std::string name(char * s);
  std::string name(std::string s);
  std::string name(int from_nuc, int to_nuc, std::string z="n");
  std::string name(int from_nuc, std::string to_nuc, std::string z="n");
  std::string name(std::string from_nuc, int to_nuc, std::string z="n");
  std::string name(std::string from_nuc, std::string to_nuc, std::string z="n");
  /// \}

  /// \name ID Functions
  /// \{
  /// Returns the recation id of a reaction channel.  This id has been precomputed  
  /// from the hash of the name.
  /// \param x Input reaction specification, may be a reaction name, alternate name,
  ///          an id, or an MT number.
  /// \param from_nuc Initial target nuclide prior to reaction.  When \a from_nuc is
  ///                 an integer it must be in id form.
  /// \param to_nuc Target nuclide after reaction occurs.  When \a to_nuc is
  ///               an integer it must be in id form.
  /// \param z Flag for incident particle type.
  ///          Particle flags are 'n', 'p', 'd', 't', 'He3', 'a', 'gamma', and 'decay'.
  unsigned int id(int x);
  unsigned int id(unsigned int x);
  unsigned int id(char * x);
  unsigned int id(std::string x);
  unsigned int id(int from_nuc, int to_nuc, std::string z="n");
  unsigned int id(int from_nuc, std::string to_nuc, std::string z="n");
  unsigned int id(std::string from_nuc, int to_nuc, std::string z="n");
  unsigned int id(std::string from_nuc, std::string to_nuc, std::string z="n");
  /// \}

  /// \name MT Number Functions
  /// \{
  /// Returns the MT number of a reaction channel, if available.
  /// \param x Input reaction specification, may be a reaction name, alternate name,
  ///          an id, or an MT number.
  /// \param from_nuc Initial target nuclide prior to reaction.  When \a from_nuc is
  ///                 an integer it must be in id form.
  /// \param to_nuc Target nuclide after reaction occurs.  When \a to_nuc is
  ///               an integer it must be in id form.
  /// \param z Flag for incident particle type.
  ///          Particle flags are 'n', 'p', 'd', 't', 'He3', 'a', 'gamma', and 'decay'.
  unsigned int mt(int x);
  unsigned int mt(unsigned int x);
  unsigned int mt(char * x);
  unsigned int mt(std::string x);
  unsigned int mt(int from_nuc, int to_nuc, std::string z="n");
  unsigned int mt(int from_nuc, std::string to_nuc, std::string z="n");
  unsigned int mt(std::string from_nuc, int to_nuc, std::string z="n");
  unsigned int mt(std::string from_nuc, std::string to_nuc, std::string z="n");
  /// \}

  //// \name Label Functions
  /// \{
  /// Returns a short description of a reaction channel.
  /// \param x Input reaction specification, may be a reaction name, alternate name,
  ///          an id, or an MT number.
  /// \param from_nuc Initial target nuclide prior to reaction.  When \a from_nuc is
  ///                 an integer it must be in id form.
  /// \param to_nuc Target nuclide after reaction occurs.  When \a to_nuc is
  ///               an integer it must be in id form.
  /// \param z Flag for incident particle type.
  ///          Particle flags are 'n', 'p', 'd', 't', 'He3', 'a', 'gamma', and 'decay'.
  std::string label(int x);
  std::string label(unsigned int x);
  std::string label(char * x);
  std::string label(std::string x);
  std::string label(int from_nuc, int to_nuc, std::string z="n");
  std::string label(int from_nuc, std::string to_nuc, std::string z="n");
  std::string label(std::string from_nuc, int to_nuc, std::string z="n");
  std::string label(std::string from_nuc, std::string to_nuc, std::string z="n");
  /// \}

  /// \name Documentation Functions
  /// \{
  /// Returns a short description of a reaction channel.
  /// \param x Input reaction specification, may be a reaction name, alternate name,
  ///          an id, or an MT number.
  /// \param from_nuc Initial target nuclide prior to reaction.  When \a from_nuc is
  ///                 an integer it must be in id form.
  /// \param to_nuc Target nuclide after reaction occurs.  When \a to_nuc is
  ///               an integer it must be in id form.
  /// \param z Flag for incident particle type.
  ///          Particle flags are 'n', 'p', 'd', 't', 'He3', 'a', 'gamma', and 'decay'.
  std::string doc(int x);
  std::string doc(unsigned int x);
  std::string doc(char * x);
  std::string doc(std::string x);
  std::string doc(int from_nuc, int to_nuc, std::string z="n");
  std::string doc(int from_nuc, std::string to_nuc, std::string z="n");
  std::string doc(std::string from_nuc, int to_nuc, std::string z="n");
  std::string doc(std::string from_nuc, std::string to_nuc, std::string z="n");
  /// \}

  /// \name Child Functions
  /// \{
  /// Returns the child nuclide comming from a parent for a reaction channel.
  /// \param nuc Nuclide after reaction occurs.  When \a nuc is
  ///               an integer it must be in id form.
  /// \param rx Input reaction specification, may be a reaction name, alternate name,
  ///           an id, or an MT number.
  /// \param z Flag for incident particle type.
  ///          Particle flags are 'n', 'p', 'd', 't', 'He3', 'a', 'gamma', and 'decay'.
  int parent(int nuc, unsigned int rx, std::string z="n");
  int parent(int nuc, std::string rx, std::string z="n");
  int parent(std::string nuc, unsigned int rx, std::string z="n");
  int parent(std::string nuc, std::string rx, std::string z="n");
  /// \}

  /// \name Parent Functions
  /// \{
  /// Returns the parent nuclide comming for a child and a given reaction channel.
  /// \param nuc Initial target nuclide prior to reaction.  When \a nuc is
  ///            an integer it must be in id form.
  /// \param rx Input reaction specification, may be a reaction name, alternate name,
  ///           an id, or an MT number.
  /// \param z Flag for incident particle type.
  ///          Particle flags are 'n', 'p', 'd', 't', 'He3', 'a', 'gamma', and 'decay'.
  int child(int nuc, unsigned int rx, std::string z="n");
  int child(int nuc, std::string rx, std::string z="n");
  int child(std::string nuc, unsigned int rx, std::string z="n");
  int child(std::string nuc, std::string rx, std::string z="n");
  /// \}

  /// Custom exception for declaring a value not to be a valid reaction.  
  class NotAReaction : public std::exception
  {
  public:

    /// default constructor
    NotAReaction () {};

    /// default destructor
    ~NotAReaction () throw () {};

    /// Constructor using original reaction (\a wasptr) and the eventual state
    /// that PyNE calculated (\a nowptr).
    NotAReaction(std::string wasptr, std::string nowptr)
    {
       rxwas = wasptr;
       rxnow = nowptr;
    };

    /// Constructor using original reaction (\a wasptr) and the eventual state
    /// that PyNE calculated (\a nowptr).
    NotAReaction(std::string wasptr, int nowptr)
    {
      rxwas = wasptr;
      rxnow = pyne::to_str(nowptr);
    };

    /// Constructor using original reaction (\a wasptr) and the eventual state
    /// that PyNE calculated (\a nowptr).
    NotAReaction(int wasptr, std::string nowptr)
    {
      rxwas = pyne::to_str(wasptr);
      rxnow = nowptr;
    };

    /// Constructor using original reaction (\a wasptr) and the eventual state
    /// that PyNE calculated (\a nowptr).
    NotAReaction(int wasptr, int nowptr)
    {
      rxwas = pyne::to_str(wasptr);
      rxnow = pyne::to_str(nowptr);
    };

    /// Constructor using original reaction (\a wasptr) and the eventual state
    /// that PyNE calculated (\a nowptr).
    NotAReaction(std::string wasptr, unsigned int nowptr)
    {
      rxwas = wasptr;
      rxnow = pyne::to_str(nowptr);
    };

    /// Constructor using original reaction (\a wasptr) and the eventual state
    /// that PyNE calculated (\a nowptr).
    NotAReaction(unsigned int wasptr, std::string nowptr)
    {
      rxwas = pyne::to_str(wasptr);
      rxnow = nowptr;
    };

    /// Constructor using original reaction (\a wasptr) and the eventual state
    /// that PyNE calculated (\a nowptr).
    NotAReaction(unsigned int wasptr, unsigned int nowptr)
    {
      rxwas = pyne::to_str(wasptr);
      rxnow = pyne::to_str(nowptr);
    };

    /// Returns a helpful error message containing prior and current reaction state.
    virtual const char* what() const throw()
    {
      std::string narxstr ("Not a reaction! ");
      if (!rxwas.empty())
        narxstr += rxwas;

      if (!rxnow.empty())
      {
        narxstr += " --> "; 
        narxstr += rxnow;
      }
      return (const char *) narxstr.c_str();
    };

  private:
    std::string rxwas;  ///< previous reaction state
    std::string rxnow;  ///< current reaction state
  };



  /// Custom exception for declaring a value not to be of ambiquous reaction form.
  class IndeterminateReactionForm : public std::exception
  {
  public:

    /// default constructor
    IndeterminateReactionForm () {};

    /// default destructor
    ~IndeterminateReactionForm () throw () {};

    /// Constructor using original reaction (\a wasptr) and the eventual state
    /// that PyNE calculated (\a nowptr).
    IndeterminateReactionForm(std::string wasptr, std::string nowptr)
    {
       rxwas = wasptr;
       rxnow = nowptr;
    };

    /// Constructor using original reaction (\a wasptr) and the eventual state
    /// that PyNE calculated (\a nowptr).
    IndeterminateReactionForm(std::string wasptr, int nowptr)
    {
      rxwas = wasptr;
      rxnow = pyne::to_str(nowptr);
    };

    /// Constructor using original reaction (\a wasptr) and the eventual state
    /// that PyNE calculated (\a nowptr).
    IndeterminateReactionForm(int wasptr, std::string nowptr)
    {
      rxwas = pyne::to_str(wasptr);
      rxnow = nowptr;
    };

    /// Constructor using original reaction (\a wasptr) and the eventual state
    /// that PyNE calculated (\a nowptr).
    IndeterminateReactionForm(int wasptr, int nowptr)
    {
      rxwas = pyne::to_str(wasptr);
      rxnow = pyne::to_str(nowptr);
    };

    /// Returns a helpful error message containing prior and current reaction state.
    virtual const char* what() const throw()
    {
      std::string INFEstr ("Indeterminate reaction form: ");
      if (!rxwas.empty())
        INFEstr += rxwas;

      if (!rxnow.empty())
      {
        INFEstr += " --> "; 
        INFEstr += rxnow;
      }
      return (const char *) INFEstr.c_str();
    }

  private:
    std::string rxwas;  ///< previous reaction state
    std::string rxnow;  ///< current reaction state
  };
};
};

#endif  // PYNE_7DOEB2PKSBEFFIA3Q2NARI3KFY
//
// end of cpp/rxname.h
//


#endif  // PYNE_52BMSKGZ3FHG3NQI566D4I2ZLY
