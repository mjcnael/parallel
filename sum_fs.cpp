#include <cstddef>
#include <thread>
#include <vector>

#define CACHE_SIZE 64

struct partial_t {
  alignas(CACHE_SIZE) unsigned v;
};

unsigned get_num_threads();

unsigned sum_fs(const unsigned *v, std::size_t n) {
  unsigned p = get_num_threads();

  auto partial_sums = std::make_unique<partial_t[]>(p);
  std::vector<std::thread> workers(p - 1);

  auto worker_proc = [&](unsigned t) {
    unsigned local_sum = 0;
    for (std::size_t i = t; i < n; i += p)
      local_sum += v[i];
    partial_sums[t].v = local_sum;
  };

  for (unsigned t = 0; t < p - 1; ++t)
    workers[t] = std::thread(worker_proc, t + 1);

  worker_proc(0);

  for (unsigned i = 0; i < p - 1; ++i)
    workers[i].join();

  unsigned result = 0;
  for (std::size_t t = 0; t < p; t++)
    result += partial_sums[t].v;

  return result;
}
