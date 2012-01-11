# Find Trilinos library
#
# loosely based off FindBoost.cmake
#
# Example:
#     find_package( Trilinos COMPONENTS epetra aztecoo )
#     if(Trilinos_FOUND)
#        include_directories(${Trilinos_INCLUDE_DIR})
#        add_executable(foo foo.cc)
#        target_link_libraries(foo ${Trilinos_LIBRARIES})
#     endif()
#     if(Trilinos_epetra_FOUND)
#        include_directories(${Trilinos_INCLUDE_DIR})
#        add_executable(bar bar.cc)
#        target_link_libraries(bar ${Trilinos_epetra_LIBRARY})
#     endif()
#
#### Optional input variables:
#  Trilinos_ROOT                  Hint the directory with lib and include
#
#### RESULTS:
#
#  Trilinos_INCLUDE_DIR           Where to find the Trilinos header files
#  Trilinos_LIBRARIES             Libraries to link against to use Trilinos.
#  Trilinos_FOUND                 Found all of the requested Trilinos packages
#
#  Trilinos_${COMPONENT}_FOUND    Found a specific package library
#  Trilinos_${COMPONENT}_LIBRARY  Library path for a specific package
#  Trilinos_vecLib_FRAMEWORK      Command to link against the vecLib framework
#                                 (needed for mac)

macro(__TRILINOS_PRINT_STATUS)
  message(STATUS "=========== TRILINOS STATUS ===============
   Trilinos_FOUND            ${Trilinos_FOUND}
   Trilinos_ALREADY_CACHED   ${Trilinos_ALREADY_CACHED}
   Trilinos_LIBRARIES        ${Trilinos_LIBRARIES}
   Trilinos_INCLUDE_DIR      ${Trilinos_INCLUDE_DIR}
   Trilinos_epetra_FOUND     ${Trilinos_epetra_FOUND}
   Trilinos_epetra_LIBRARY   ${Trilinos_epetra_LIBRARY}
   Trilinos_vecLib_FRAMEWORK ${Trilinos_vecLib_FRAMEWORK}
============================================
   "
   )
endmacro(__TRILINOS_PRINT_STATUS)


set(Trilinos_FOUND TRUE)

set(Trilinos_ERROR_REASONS)
set(Trilinos_ALREADY_CACHED FALSE)

find_path(Trilinos_INCLUDE_DIR "Teuchos_any.hpp" 
  HINTS
    "${Trilinos_ROOT}"
    ENV
      TRILINOS_ROOT
  PATHS
    /usr
    /usr/local
    /filespace/groups/cnerg/cyclus/trilinos/build
  PATH_SUFFIXES
    include
  )

if(Trilinos_INCLUDE_DIR)
  set(Trilinos_ALREADY_CACHED TRUE)
  mark_as_advanced(Trilinos_INCLUDE_DIR)
else(Trilinos_INCLUDE_DIR)
  # we don't even know the include directory, so we have to find the library
  # for sure
  set(Trilinos_FOUND FALSE)
  list(APPEND Trilinos_ERROR_REASONS
      "Include directory not found." )
endif(Trilinos_INCLUDE_DIR)

#__TRILINOS_PRINT_STATUS()

# verify that in addition to the include directory, we have all the desired
# library components cached
if(Trilinos_ALREADY_CACHED)
  foreach(COMPONENT ${Trilinos_FIND_COMPONENTS})
    if(Trilinos_${COMPONENT}_LIBRARY)
      set(Trilinos_${COMPONENT}_FOUND TRUE)
    else(Trilinos_${COMPONENT}_LIBRARY)
      set(Trilinos_ALREADY_CACHED FALSE)
      set(Trilinos_${COMPONENT}_FOUND FALSE)
    endif(Trilinos_${COMPONENT}_LIBRARY)
  endforeach(COMPONENT)
endif(Trilinos_ALREADY_CACHED)

if(Trilinos_ALREADY_CACHED)
  #message(STATUS "All Trilinos headers and required components "
  #        "(${Trilinos_FIND_COMPONENTS}) were found in the cache.")
else(Trilinos_ALREADY_CACHED)
  ##### Trilinos was not in the cache. We have to search for it ourselves. #####

  ##### Detailed lookup of each component
  foreach(COMPONENT ${Trilinos_FIND_COMPONENTS})
    set(LIBVAR "Trilinos_${COMPONENT}_LIBRARY")
    message(STATUS "Looking for Trilinos component ${COMPONENT}")
  
    find_library(${LIBVAR}
      NAMES ${COMPONENT} 
      HINTS "${Trilinos_INCLUDE_DIR}/.."
            "${Trilinos_ROOT}"
        ENV
          TRILINOS_ROOT
      PATHS
        /usr
        /usr/local
        /filespace/groups/cnerg/cyclus/trilinos/build
      PATH_SUFFIXES
        lib
      )

    set(Trilinos_${COMPONENT}_FOUND TRUE)
    if(${LIBVAR} )
      mark_as_advanced(${LIBVAR})
    else(${LIBVAR} )
      set(Trilinos_${COMPONENT}_FOUND FALSE)
      set(Trilinos_FOUND FALSE)
      list(APPEND Trilinos_ERROR_REASONS
          "Library component ${COMPONENT} not found." )
    endif(${LIBVAR} )
  endforeach(COMPONENT)

  # On mac systems, we likely have to link against the vecLib framework
  if(APPLE AND NOT Trilinos_vecLib_FRAMEWORK)
    include(CMakeFindFrameworks)
    CMAKE_FIND_FRAMEWORKS(vecLib)
    if(vecLib_FRAMEWORKS)
      set(
        Trilinos_vecLib_FRAMEWORK ${vecLib_FRAMEWORKS}
        CACHE PATH "Path to vecLib framework"
      )
      mark_as_advanced(Trilinos_vecLib_FRAMEWORK)
    else(vecLib_FRAMEWORKS)
      set(Trilinos_FOUND FALSE)
      list(APPEND Trilinos_ERROR_REASONS
          "vecLib framework not found." )
    endif(vecLib_FRAMEWORKS)
  endif(APPLE AND NOT Trilinos_vecLib_FRAMEWORK)

endif(Trilinos_ALREADY_CACHED)

# build Trilinos_LIBRARIES
foreach(COMPONENT ${Trilinos_FIND_COMPONENTS})
  set(LIBVAR "Trilinos_${COMPONENT}_LIBRARY")
  list(APPEND Trilinos_LIBRARIES ${${LIBVAR}})
endforeach(COMPONENT)
if(APPLE)
  list(APPEND Trilinos_LIBRARIES "-framework vecLib")
endif(APPLE)

##__TRILINOS_PRINT_STATUS()

## Final result: see if we found it, error if appropriate
# (this is mostly duplicated by FIND_PACKAGE_HANDLE_STANDARD_ARGS )
if(Trilinos_FOUND)
  if(NOT Trilinos_FIND_QUIETLY AND NOT Trilinos_ALREADY_CACHED)
    message(STATUS "Found the following Trilinos libraries:")
    foreach ( COMPONENT  ${Trilinos_FIND_COMPONENTS} )
      message (STATUS "  ${COMPONENT}")
    endforeach(COMPONENT)
  endif(NOT Trilinos_FIND_QUIETLY AND NOT Trilinos_ALREADY_CACHED)

  if(Trilinos_ROOT STREQUAL "Trilinos-NOTFOUND")
    message(STATUS "clearing Trilinos_ROOT")
    # clear trilinos_ROOT if the user didn't end up using it
    unset( Trilinos_ROOT CACHE)
  endif(Trilinos_ROOT STREQUAL "Trilinos-NOTFOUND")
else (Trilinos_FOUND)
  if(Trilinos_FIND_REQUIRED)
    set(THE_ERROR_MESSAGE "Unable to find the requested Trilinos libraries:")
    foreach( REASON ${Trilinos_ERROR_REASONS})
      set(THE_ERROR_MESSAGE "${THE_ERROR_MESSAGE}\n   + ${REASON}") 
    endforeach(REASON)
    message(SEND_ERROR "${THE_ERROR_MESSAGE}\n"
      "** Try setting the Trilinos_ROOT variable and re-configuring. **")
    set(
      Trilinos_ROOT "Trilinos-NOTFOUND"
      CACHE PATH "Path to Trilinos base"
    )
  endif(Trilinos_FIND_REQUIRED)
endif(Trilinos_FOUND)
