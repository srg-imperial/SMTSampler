# This function works around a CMake issue with setting include directories of
# imported libraries built with `ExternalProject_Add`.
# https://gitlab.kitware.com/cmake/cmake/issues/15052
function(make_include_dir TARGET)
  get_target_property(DIR ${TARGET} INTERFACE_INCLUDE_DIRECTORIES)
  file(MAKE_DIRECTORY ${DIR})
endfunction()

include(ProcessorCount)
ProcessorCount(NPROC)
if(NPROC EQUAL 0)
  set(NPROC 1)
endif()

set(Z3_PATCH_PATH "${CMAKE_SOURCE_DIR}/z3-patch")
include(ExternalProject)
ExternalProject_Add(Z3Prover
  GIT_REPOSITORY
  "https://github.com/Z3Prover/z3.git"
  GIT_TAG
  bb7ad4e938ec3ade23282142119e77c838b1f7d1
  PREFIX
  Z3Prover/
  UPDATE_COMMAND
  ""
  PATCH_COMMAND
  cmake -E copy ${Z3_PATCH_PATH}/mk_util.py scripts/mk_util.py
  &&
  cmake -E copy ${Z3_PATCH_PATH}/rewriter_def.h src/ast/rewriter/rewriter_def.h
  &&
  cmake -E copy ${Z3_PATCH_PATH}/model.cpp src/model/model.cpp
  &&
  cmake -E copy ${Z3_PATCH_PATH}/permutation_matrix.h src/util/lp/permutation_matrix.h
  BUILD_IN_SOURCE
  TRUE
  CONFIGURE_COMMAND
  CXX=${CMAKE_CXX_COMPILER} CC=${CMAKE_C_COMPILER} python scripts/mk_make.py --prefix=${CMAKE_BINARY_DIR}/Z3Prover/
  BUILD_COMMAND
  cd build && make -j${NPROC}
  INSTALL_COMMAND
  cd build && make install
  BUILD_BYPRODUCTS
  ${CMAKE_BINARY_DIR}/Z3Prover/lib/${CMAKE_SHARED_LIBRARY_PREFIX}z3${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${CMAKE_BINARY_DIR}/Z3Prover/bin/z3${CMAKE_EXECUTABLE_SUFFIX}
)

ExternalProject_Get_Property(Z3Prover INSTALL_DIR)

add_library(z3 SHARED IMPORTED GLOBAL)
add_dependencies(z3 Z3Prover)
set_target_properties(z3 PROPERTIES
  IMPORTED_LOCATION
  ${INSTALL_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}z3${CMAKE_SHARED_LIBRARY_SUFFIX}
  INTERFACE_LINK_DIRECTORIES
  ${INSTALL_DIR}/lib/
  INTERFACE_INCLUDE_DIRECTORIES
  ${INSTALL_DIR}/include/
  IMPORTED_NO_SONAME TRUE
)
make_include_dir(z3)
