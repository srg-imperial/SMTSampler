cmake_minimum_required(VERSION 3.12)
project(SMTSampler)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

include(AddZ3)

add_executable(smt-sampler
  smtsampler.cpp
)
target_link_libraries(smt-sampler PRIVATE z3)
