#include <z3++.h>

#include <fstream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace smtsampler {

enum { STRAT_SMTBIT, STRAT_SMTBV, STRAT_SAT };

typedef struct {
  char const *a[3] = {NULL, NULL, NULL};
} triple;

class SMTSampler {
public:
  SMTSampler(std::string input, int max_samples, double max_time, int strategy);
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
  z3::model gen_model(std::string candidate, std::vector<z3::func_decl> ind);
  bool output(z3::model m, int nmut);
  bool output(std::string sample, int nmut);
  void finish();
  z3::check_result solve();
  std::string model_string(z3::model m, std::vector<z3::func_decl> ind);
  double duration(struct timespec * a, struct timespec * b);
  z3::expr literal(int v);

private:
  std::string input_file;

  struct timespec start_time;
  double solver_time = 0.0;
  double check_time = 0.0;
  double cov_time = 0.0;
  double convert_time = 0.0;
  int max_samples;
  double max_time;

  z3::context c;
  int strategy;
  bool convert = false;
  bool const flip_internal = false;
  bool random_soft_bit = false;
  z3::apply_result *res0;
  z3::goal *converted_goal;
  z3::params params;
  z3::optimize opt;
  z3::solver solver;
  z3::model model;
  z3::expr smt_formula;
  std::vector<z3::func_decl> variables;
  std::vector<z3::func_decl> ind;
  std::vector<z3::expr> internal;
  std::vector<z3::expr> constraints;
  std::vector<std::vector<z3::expr>> soft_constraints;
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

  std::ofstream results_file;

  std::unordered_set<Z3_ast> sub;
  std::unordered_set<Z3_ast> sup;
  std::unordered_set<std::string> var_names = {"bv", "true", "false"};
  int num_arrays = 0, num_bv = 0, num_bools = 0, num_bits = 0, num_uf = 0;
  int maxdepth = 0;
};

} // namespace smtsampler
