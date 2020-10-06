add_executable(smt-sampler
  smtsampler.cpp
)

target_link_libraries(smt-sampler PRIVATE SMTSampler)

add_subdirectory(SMTSampler-c)
