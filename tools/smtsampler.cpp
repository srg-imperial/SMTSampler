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
  for (int i = 1; i < argc; ++i) {
    std::cerr << argv[i] << "\n";
    if (std::strcmp(argv[i], "-n") == 0)
      arg_samples = true;
    else if (std::strcmp(argv[i], "-t") == 0)
      arg_time = true;
    else if (std::strcmp(argv[i], "--smtbit") == 0)
      strategy = STRAT_SMTBIT;
    else if (std::strcmp(argv[i], "--smtbv") == 0)
      strategy = STRAT_SMTBV;
    else if (std::strcmp(argv[i], "--sat") == 0)
      strategy = STRAT_SAT;
    else if (std::strcmp(argv[i], "--seed") == 0)
      seeded = true;
    else if (arg_samples) {
      arg_samples = false;
      max_samples = std::atoi(argv[i]);
    } else if (arg_time) {
      arg_time = false;
      max_time = std::atof(argv[i]);
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
  std::ofstream results_file(input_file + ".samples");
  SMTSampler s(std::move(input_file), seed, max_samples, max_time, strategy, results_file);
  s.run();
  results_file.close();
  return 0;
}
