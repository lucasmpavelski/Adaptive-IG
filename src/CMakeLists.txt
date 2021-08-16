# Generic test that uses conan libs
add_executable(main main.cpp)

set(FSP_SOURCES
  FSPProblemFactory.cpp
  MHParamsSpecsFactory.cpp
  global.cpp
)

add_library(flowshop_solver_lib STATIC ${FSP_SOURCES})
target_link_libraries(
  flowshop_solver_lib
  PRIVATE project_options
          project_warnings
          CONAN_PKG::Eigen3
          ${PARADISEO_LIBRARIES}
          pthread
)
target_include_directories(
  flowshop_solver_lib
  PUBLIC ${CMAKE_SOURCE_DIR}/src 
          ${CONAN_INCLUDE_DIRS_EIGEN3}
          ${PARADISEO_INCLUDE_DIR})
set_property(TARGET flowshop_solver_lib PROPERTY POSITION_INDEPENDENT_CODE ON)


message("${PARADISEO_INCLUDE_DIR}")

target_link_libraries(
  main
  PRIVATE project_options
          project_warnings
          flowshop_solver_lib
)

