
# geo order 1
foreach(_DIM 2 3)
  math(EXPR _T_ORDER '${FEELPP_INSTANTIATION_ORDER_MAX}-1')
  feelpp_max( MAX_ORDER 1 ${_T_ORDER})
  message(STATUS "[feelpp] ELECTRIC ${_DIM}D PNG1 N=1..${MAX_ORDER}")
  foreach( _T_ORDER RANGE 1 ${MAX_ORDER} )
    genLibElectric(
      DIM     ${_DIM}
      P_ORDER ${_T_ORDER}
      GEO_ORDER 1
      )
    set(FEELPP_MODELS_LIBRARIES_ELECTRIC  ${FEELPP_MODELS_LIBRARIES_ELECTRIC} ${ELECTRIC_LIB_NAME}  )
  endforeach()
endforeach()

# isoparametric
foreach(_DIM 2 3)
  math(EXPR _T_ORDER '${FEELPP_INSTANTIATION_ORDER_MAX}-1')
  feelpp_min( MAX_ORDER ${FEELPP_MESH_MAX_ORDER} ${_T_ORDER})
  message(STATUS "[feelpp] ELECTRIC ${_DIM}D PNGN N=2..${MAX_ORDER}")
  foreach( _T_ORDER RANGE 2 ${MAX_ORDER} )
    genLibElectric(
      DIM     ${_DIM}
      P_ORDER ${_T_ORDER}
      GEO_ORDER ${_T_ORDER}
      )
    set(FEELPP_MODELS_LIBRARIES_ELECTRIC  ${FEELPP_MODELS_LIBRARIES_ELECTRIC} ${ELECTRIC_LIB_NAME} )
  endforeach()
endforeach()

add_library(feelpp_toolbox_electric_lib SHARED libelectric.cpp )
add_library(Feelpp::feelpp_toolbox_electric_lib ALIAS feelpp_toolbox_electric_lib )  # to match exported target
set_target_properties(feelpp_toolbox_electric_lib PROPERTIES OUTPUT_NAME "feelpp_toolbox_electric_lib")
target_link_libraries(feelpp_toolbox_electric_lib ${FEELPP_MODELS_LIBRARIES_ELECTRIC})
set_property(TARGET feelpp_toolbox_electric_lib PROPERTY MACOSX_RPATH ON)
install(TARGETS feelpp_toolbox_electric_lib DESTINATION lib/ COMPONENT Libs EXPORT feelpp-toolboxes-targets)

FILE(GLOB files "*.hpp")
INSTALL(FILES ${files} DESTINATION include/feelpp/toolboxes/feel/feelmodels/electric COMPONENT Devel)
