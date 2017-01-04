# copy_when_diff(src target)
#
# This macro library copies over a file only if the contents of the source
# file from the contents of the target or if the target does not exist.

macro(copy_when_diff src target)
  if(EXISTS "${target}")
    # check if the file is actually different before copying
    file(READ ${src} _SRC_CONTENTS)
    file(READ ${target} _TARGET_CONTENTS)
    if(NOT _SRC_CONTENTS STREQUAL _TARGET_CONTENTS)
        message("!!! ${src} does not equal ${target}")
        #file(GENERATE OUTPUT "${target}" INPUT "${src}")
        configure_file("${src}" "${target}" COPYONLY)
    else()
        message("!!! ${src} does equal ${target}")
    endif(NOT _SRC_CONTENTS STREQUAL _TARGET_CONTENTS)
  else(EXISTS "${target}")
    # just copy the file, since the target doesn't exist.
    message("!!! ${target} dies not exist")
    #file(GENERATE OUTPUT "${target}" INPUT "${src}")
    configure_file("${src}" "${target}" COPYONLY)
  endif(EXISTS "${target}")
endmacro(copy_when_diff)

