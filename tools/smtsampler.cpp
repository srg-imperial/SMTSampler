#include "SMTSampler/smtsampler.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace smtsampler;

int main(int argc, char *argv[]) {
  int max_samples = 1000000;
  double max_time = 3600.0;
  int strategy = STRAT_SMTBIT;
  unsigned seed = 0;
  if (argc < 2) {
    std::cout << "Argument required: input file\n";
    return 0;
  }
  bool arg_samples = false;
  bool arg_time = false;
  bool seeded = false;
  bool arg_results = false;
  bool array_map = false;
  std::string results_path;
  std::string array_map_path;
  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "-a") == 0)
      array_map = true;
    else if (std::strcmp(argv[i], "-n") == 0)
      arg_samples = true;
    else if (std::strcmp(argv[i], "-t") == 0)
      arg_time = true;
    else if (std::strcmp(argv[i], "-o") == 0)
      arg_results = true;
    else if (std::strcmp(argv[i], "--smtbit") == 0)
      strategy = STRAT_SMTBIT;
    else if (std::strcmp(argv[i], "--smtbv") == 0)
      strategy = STRAT_SMTBV;
    else if (std::strcmp(argv[i], "--sat") == 0)
      strategy = STRAT_SAT;
    else if (std::strcmp(argv[i], "--seed") == 0)
      seeded = true;
    else if (array_map) {
      array_map = false;
      array_map_path = argv[i];
    } else if (arg_samples) {
      arg_samples = false;
      max_samples = std::atoi(argv[i]);
    } else if (arg_time) {
      arg_time = false;
      max_time = std::atof(argv[i]);
    } else if(arg_results) {
      arg_results = false;
      results_path = argv[i];
    } else if (seeded) {
      seeded = false;
      unsigned long seed_value = std::strtoul(argv[i], nullptr, 10);
      seed = seed_value;
      if (seed == 0 || seed != seed_value)
        throw std::out_of_range(
            (seed == 0) ? "Cannot use 0 as a seed value"
                        : "Provided seed does not for within an unsigned int");
    }
  }

  std::string input_file(argv[argc - 1]);
  if (results_path.empty())
    results_path = input_file + ".samples";
  std::ofstream results_file(results_path);
  SMTSampler s(std::move(input_file), std::move(array_map_path), seed, max_samples, max_time, strategy, results_file);
  try {
  s.run();
  } catch(FinishException e) {}
  results_file.flush();
  return 0;
}
