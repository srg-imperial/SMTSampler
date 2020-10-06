#ifdef __cplusplus
extern "C" {
#endif

extern int smtsampler_run(const char *input, unsigned seed, int max_samples,
               double max_time, int strategy, const char *output);

#ifdef __cplusplus
}
#endif
