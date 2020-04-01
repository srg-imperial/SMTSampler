add_library(SMTSampler SHARED
  smtsampler.cpp)

target_link_libraries(SMTSampler PUBLIC z3)
