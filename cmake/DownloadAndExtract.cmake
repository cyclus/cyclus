# Macro for downloading a tar file and extracting it if a sample file
# doesn't already exist.

macro(download_and_extract _url _checkfile)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_checkfile}")
    # don't need to do anything
    message(STATUS "${_checkfile} exists, no need to download or extract!")
  else()
    get_filename_component(_base "${_url}" NAME)
    # download the file if we need to
    if(NOT EXIST "${CMAKE_CURRENT_SOURCE_DIR}/${_base}")
      message(STATUS "Downloading ${_url} -> ${_base}")
      file(DOWNLOAD "${_url}" "${CMAKE_CURRENT_SOURCE_DIR}/${_base}"
           SHOW_PROGRESS STATUS _rtn TLS_VERIFY OFF)
      if(NOT "0" IN_LIST _rtn)
        message(FATAL_ERROR ${_rtn})
      endif()
    endif()
    # extract the file
    message(STATUS "Extracting ${_base}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xvf "${_base}"
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  endif()
endmacro()