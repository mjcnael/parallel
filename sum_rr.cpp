#include <cstddef>
#include <cstdlib>
#include <omp.h>

#define CACHE_SIZE 64

unsigned sum_rr(const unsigned *v, std::size_t n) {
  unsigned *partial_sums =
      (unsigned *)malloc(omp_get_num_procs() * sizeof(unsigned));

#pragma omp parallel
  {
    unsigned t = omp_get_thread_num();
    unsigned T = omp_get_num_threads();

    partial_sums[t] = 0;

    for (unsigned i = t; i < n; i += T)
      partial_sums[t] += v[i];
  }

  unsigned s = 0;
  for (std::size_t t = 0; t < omp_get_num_threads(); t++)
    s += partial_sums[t];

  return s;
}

struct partial_t {
  alignas(CACHE_SIZE) unsigned v;
};

unsigned sum_rr_fs(const unsigned *v, std::size_t n);

unsigned sum_rr_fs(const unsigned *v, std::size_t n) {
  partial_t *partial_sums;

#pragma omp parallel
  {
    std::size_t T = omp_get_num_threads();
    std::size_t t = omp_get_thread_num();

#pragma omp single
    {
      partial_sums = (partial_t *)calloc(T, sizeof(partial_t));
    }

    unsigned nn = 0;
    for (std::size_t i = t; i < n; i += T)
      nn += v[i];

    partial_sums[t * CACHE_SIZE / sizeof(unsigned)].v = nn;
  }

  std::size_t T = omp_get_num_threads();
  unsigned s = 0;
  for (std::size_t t = 0; t < T; t++)
    s += partial_sums[t * CACHE_SIZE / sizeof(unsigned)].v;

  return s;
}
