macro(cyclus_init_model _path _name)
  #SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY 
  #  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${_path})
  
  # Build the cyclus executable from the CYCLUS_SRC source files
  ADD_LIBRARY( ${_name}       ${_name}.cpp )
  SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} ${_name} )

  
  CONFIGURE_FILE(
    ${CYCLUS_SOURCE_DIR}${_path}/${_name}.rng
    ${CMAKE_CURRENT_BINARY_DIR}/${_name}.rng
    COPYONLY
    )
  
  install(TARGETS ${_name}
    LIBRARY DESTINATION cyclus/lib${_path}
    COMPONENT ${_path}
    )
  
  SET(RNG_INCLUDES ${RNG_INCLUDES}
    "<include href='..${_path}/${_name}.rng'/>"
    PARENT_SCOPE)

  install(FILES "${_name}.rng"
    DESTINATION cyclus/${_path}
    COMPONENT "${_path}.rng"
    )
endmacro()
  

macro(cyclus_init_fac _name)
  SET(MODEL_PATH "/Models/Facility/${_name}")
  cyclus_init_model(${MODEL_PATH} ${_name})

  SET(FacilityTestSource ${FacilityTestSource} 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}Tests.cpp 
    PARENT_SCOPE)

  SET(FAC_REFS ${FAC_REFS}
    "<ref name='${_name}'/>"
    PARENT_SCOPE)
endmacro()

macro(cyclus_init_reg _name)
  SET(MODEL_PATH "/Models/Region/${_name}")
  cyclus_init_model(${MODEL_PATH} ${_name})

  SET(RegionTestSource ${RegionTestSource} 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}Tests.cpp 
    PARENT_SCOPE)

  SET(REG_REFS ${REG_REFS}
    "<ref name='${_name}'/>"
    PARENT_SCOPE)
endmacro()

macro(cyclus_init_inst _name)
  SET(MODEL_PATH "/Models/Inst/${_name}")
  cyclus_init_model(${MODEL_PATH} ${_name})

  SET(InstTestSource ${InstTestSource} 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}Tests.cpp 
    PARENT_SCOPE)

  SET(INST_REFS ${INST_REFS}
    "<ref name='${_name}'/>"
    PARENT_SCOPE)
endmacro()

macro(cyclus_init_mkt _name)
  SET(MODEL_PATH "/Models/Market/${_name}")
  cyclus_init_model(${MODEL_PATH} ${_name})

  SET(MarketTestSource ${MarketTestSource} 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}Tests.cpp 
    PARENT_SCOPE)

  SET(MKT_REFS ${MKT_REFS}
    "<ref name='${_name}'/>"
    PARENT_SCOPE)
endmacro()

macro(cyclus_init_conv  _name)
  SET(MODEL_PATH "/Models/Converter/${_name}")
  cyclus_init_model(${MODEL_PATH} ${_name})

  SET(ConverterTestSource ${ConverterTestSource} 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}Tests.cpp 
    PARENT_SCOPE)

  SET(CONV_REFS ${CONV_REFS}
    "<ref name='${_name}'/>"
    PARENT_SCOPE)
endmacro()


macro(cyclus_init_stub  _name)
  SET(MODEL_PATH "/Models/Stub/${_name}")
  cyclus_init_model(${MODEL_PATH} ${_name})

  SET(StubTestSource ${StubTestSource} 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}Tests.cpp 
    PARENT_SCOPE)

  SET(STUB_REFS ${STUB_REFS}
    "<ref name='${_name}'/>"
    PARENT_SCOPE)
endmacro()

macro(cyclus_init_stubcomm  _name)
  SET(MODEL_PATH "/Models/StubComm/${_name}")
  cyclus_init_model(${MODEL_PATH} ${_name})

  SET(STUBCOMM_REFS ${STUBCOMM_REFS}
    "<ref name='${_name}'/>"
    PARENT_SCOPE)
endmacro()

