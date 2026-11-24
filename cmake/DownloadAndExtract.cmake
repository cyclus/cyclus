# Macros for downloading a tar file and extracting it if a sample file
# doesn't already exist.

# Download _url to _dest. Any further arguments are passed on to
# file(DOWNLOAD), e.g. TLS_VERIFY OFF.
#
# raw.githubusercontent.com, which the data files are downloaded from,
# intermittently answers requests from shared CI runners with HTTP errors
# (e.g. when they are rate limited), so a failed download is retried, doubling
# the delay after each attempt, before the configure step fails.
function(cyclus_download _url _dest)
  set(_attempts 5)
  set(_delay 5)
  foreach(_attempt RANGE 1 ${_attempts})
    file(DOWNLOAD "${_url}" "${_dest}" SHOW_PROGRESS STATUS _status LOG _log
         ${ARGN})
    list(GET _status 0 _code)
    if(_code EQUAL 0)
      return()
    endif()
    # A failed download can leave a partial file behind, which would then be
    # taken for the downloaded file.
    file(REMOVE "${_dest}")
    # STATUS only says "HTTP response code said error", so report the status
    # line of the last HTTP response, e.g. "HTTP/2 429", from the log.
    set(_response "no HTTP response")
    string(REGEX MATCHALL "HTTP/[0-9.]+ [0-9]+" _responses "${_log}")
    if(_responses)
      list(GET _responses -1 _response)
    endif()
    list(GET _status 1 _reason)
    string(CONCAT _failure "Downloading ${_url} failed (attempt ${_attempt} of "
                           "${_attempts}): ${_reason} (${_response})")
    if(_attempt LESS _attempts)
      message(WARNING "${_failure}, retrying in ${_delay} seconds")
      execute_process(COMMAND ${CMAKE_COMMAND} -E sleep ${_delay})
      math(EXPR _delay "${_delay} * 2")
    else()
      message(FATAL_ERROR "${_failure}")
    endif()
  endforeach()
endfunction()

macro(download_and_extract _url _checkfile)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_checkfile}")
    # don't need to do anything
    message(STATUS "${_checkfile} exists, no need to download or extract!")
  else()
    get_filename_component(_base "${_url}" NAME)
    # download the file if we need to
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_base}")
      message(STATUS "Downloading ${_url} -> ${_base}")
      cyclus_download("${_url}" "${CMAKE_CURRENT_SOURCE_DIR}/${_base}"
                      TLS_VERIFY OFF)
    endif()
    # extract the file
    message(STATUS "Extracting ${_base}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xvf "${_base}"
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  endif()
endmacro()


macro(download_platform _base_url _base_name _ext_src _ext_plat)
  # first download the src file
  set(_url "${_base_url}/${_base_name}.tar.gz")
  set(_checkfile "${_base_name}${_ext_src}")
  download_and_extract("${_url}" "${_checkfile}")
  # now download the platform specific file
  set(_url "${_base_url}/${_base_name}-${CYCLUS_PLATFORM}.tar.gz")
  set(_checkfile "${_base_name}-${CYCLUS_PLATFORM}${_ext_plat}")
  download_and_extract("${_url}" "${_checkfile}")
endmacro()
