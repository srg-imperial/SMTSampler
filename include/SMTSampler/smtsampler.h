#include <system_error>
#include <type_traits>
#include <z3++.h>

#include <exception>
#include <fstream>
#include <map>
#include <stdexcept>
#include <system_error>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <z3_api.h>

namespace smtsampler {

enum class SMTSamplerErrc {
  InvalidZ3Sort = 1,
  InvalidInputFormula,
  InvalidHexValue,
  UnsatFormula = 10,
  UnableToSolve,
  SolutionCheckFailure,
  Finish = 20
};

std::error_code make_error_code(SMTSamplerErrc);

struct SMTSamplerErrorCategory : std::error_category {
  char const *name() const noexcept override;
  std::string message(int ErrorValue) const override;
};

extern struct SMTSamplerErrorCategory const TheSMTSamplerErrorCategory;

std::error_category const &SMTSamplerErrorCategory() {
  return TheSMTSamplerErrorCategory;
}

} // namespace smtsampler

namespace std {

template <>
struct is_error_code_enum<smtsampler::SMTSamplerErrc> : true_type {};

} // namespace std

namespace smtsampler {

enum { STRAT_SMTBIT, STRAT_SMTBV, STRAT_SAT };

typedef struct {
  char const *a[3] = {NULL, NULL, NULL};
} triple;

struct SMTSamplerException : std::runtime_error {
  explicit SMTSamplerException(std::error_code TheEC)
      : std::runtime_error(TheEC.message()), EC(TheEC){};
  virtual ~SMTSamplerException() = default;

  std::error_code EC;
};

struct InvalidZ3SortException : SMTSamplerException {
  InvalidZ3SortException(z3::sort const TheSort)
      : SMTSamplerException(make_error_code(SMTSamplerErrc::InvalidZ3Sort)) {
    WhatString = EC.message();
    WhatString += ": ";
    WhatString += TheSort.to_string();
  }

  virtual char const *what() const noexcept override {
    return WhatString.c_str();
  }

private:
  std::string WhatString;
};

struct InvalidInputFormulaException : SMTSamplerException {
  InvalidInputFormulaException()
      : SMTSamplerException(
            make_error_code(SMTSamplerErrc::InvalidInputFormula)) {}
};

struct InvalidHexValueException : SMTSamplerException {
  explicit InvalidHexValueException(char const c)
      : SMTSamplerException(make_error_code(SMTSamplerErrc::InvalidHexValue)) {
    WhatString = EC.message();
    WhatString += ": ";
    WhatString += c;
  }

  virtual char const *what() const noexcept override {
    return WhatString.c_str();
  }

private:
  std::string WhatString;
};

struct UnsatFormulaException : SMTSamplerException {
  UnsatFormulaException()
      : SMTSamplerException(make_error_code(SMTSamplerErrc::UnsatFormula)) {}
};

struct UnableToSolveException : SMTSamplerException {
  UnableToSolveException()
      : SMTSamplerException(make_error_code(SMTSamplerErrc::UnableToSolve)) {}
};

struct SolutionCheckFailureException : SMTSamplerException {
  explicit SolutionCheckFailureException(int const TheMutationIndex)
      : SMTSamplerException(
            make_error_code(SMTSamplerErrc::SolutionCheckFailure)) {
    WhatString = EC.message();
    WhatString += ": ";
    WhatString += std::to_string(TheMutationIndex);
  }

  virtual char const *what() const noexcept override {
    return WhatString.c_str();
  }

private:
  std::string WhatString;
};

struct FinishException : SMTSamplerException {
  FinishException()
      : SMTSamplerException(make_error_code(SMTSamplerErrc::Finish)) {}
};

class SMTSampler {
public:
  SMTSampler(std::string input, std::string array_map, unsigned seed,
             int max_samples, double max_time, int strategy,
             unsigned soft_arr_idx, std::ostream &output);
  void run();

private:
  void assert_soft(z3::expr const &e);
  void print_stats();
  void visit(z3::expr e, int depth);
  void parse_smt();
  z3::expr evaluate(z3::model m, z3::expr e, bool b, int n);
  std::vector<z3::func_decl> get_variables(z3::model m, bool is_ind);
  void parse_cnf();
  z3::expr value(char const *n, z3::sort s);
  void sample(z3::model m);
  void add_constraints(z3::expr exp, z3::expr val, int count);
  char const *parse_function(std::string const &m_string, size_t &pos,
                             int arity,
                             std::unordered_map<std::string, triple> &values,
                             int index);
  unsigned char hex(char c);
  std::string combine(char const *val_a, char const *val_b, char const *val_c,
                      z3::sort s);
  void combine_function(std::string const &str_a, std::string const &str_b,
                        std::string const &str_c, size_t &pos_a, size_t &pos_b,
                        size_t &pos_c, int arity, z3::sort s,
                        std::string &candidate);
  bool is_ind(int count);
  z3::model gen_model(std::string const &candidate,
                      std::vector<z3::func_decl> ind);
  bool output(z3::model m, int nmut);
  bool output(std::string sample, int nmut);
  void finish();
  z3::check_result solve();
  std::string model_string(z3::model const &m,
                           std::vector<z3::func_decl> const &ind);
  std::string
  output_sample_string(std::string const &TheSample,
                       std::vector<z3::func_decl> const &TheVariables);
  double duration(struct timespec *a, struct timespec *b);
  z3::expr literal(int v);

private:
  std::string input_file;
  std::string array_map_file;
  unsigned input_seed;
  unsigned final_seed;
  bool is_seeded;

  struct timespec start_time;
  double solver_time = 0.0;
  double check_time = 0.0;
  double cov_time = 0.0;
  double convert_time = 0.0;
  int max_samples;
  double max_time;

  int strategy;
  z3::context c;
  bool convert = true;
  bool const flip_internal = false;
  bool random_soft_bit = true;
  unsigned random_soft_arr_idx;
  z3::apply_result *res0;
  z3::goal *converted_goal;
  z3::optimize opt;
  z3::solver solver;
  z3::params params;
  z3::model model;
  z3::expr smt_formula;
  std::vector<z3::func_decl> variables;
  std::vector<z3::func_decl> ind;
  std::vector<z3::expr> internal;
  std::vector<z3::expr> constraints;
  std::vector<std::vector<z3::expr>> soft_constraints;
  std::unordered_map<std::string, std::pair<size_t, bool>> array_map;
  std::vector<std::pair<int, int>> cons_to_ind;
  std::unordered_map<int, std::unordered_set<int>> unsat_ind;
  std::unordered_set<int> unsat_internal;
  std::unordered_set<std::string> all_mutations;
  int epochs = 0;
  int flips = 0;
  int samples = 0;
  int valid_samples = 0;
  int solver_calls = 0;
  int unsat_ind_count = 0;
  int all_ind_count = 0;

  std::ostream &results_stream;

  std::unordered_set<Z3_ast> sub;
  std::unordered_set<Z3_ast> sup;
  std::unordered_set<std::string> var_names = {"bv", "true", "false"};
  int num_arrays = 0, num_bv = 0, num_bools = 0, num_bits = 0, num_uf = 0;
  int maxdepth = 0;
};

} // namespace smtsampler
