#include "SMTSampler/smtsampler.h"
#include "SMTSampler-c/smtsampler.h"

#include <string>
#include <fstream>

using namespace smtsampler;

int smtsampler_run(const char *input, unsigned seed, int max_samples, double max_time,
        int strategy, const char *output) {
  int retval = 0;

  std::string input_file(input);
  std::ofstream results_file(output);
  SMTSampler s(std::move(input_file), std::string(), seed, max_samples, max_time, strategy, results_file);
  try {
    s.run();
  } catch(FinishException e) {

  } catch (SMTSamplerException e) {
    return e.EC.value();
  } catch(...) {
    return -1;
  }

  return 0;
}
