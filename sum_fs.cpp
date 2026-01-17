#include <condition_variable>
#include <cstddef>
#include <memory>
#include <thread>
#include <vector>

#define CACHE_SIZE 64

static std::pair<size_t, size_t> get_subvector(size_t n, unsigned t,
                                               unsigned T) {
  size_t chunk = n / T;

  size_t begin = t * chunk;
  size_t end = (t == T - 1) ? n : begin + chunk;

  return {begin, end};
}

class barrier {
public:
  explicit barrier(unsigned pool_size) : pool_size(pool_size) {}

  void arrive_and_wait() {
    std::unique_lock<std::mutex> lock(m);
    unsigned gen = generation;

    if (++counter == pool_size) {
      counter = 0;
      ++generation;
      cv.notify_all();
    } else {
      cv.wait(lock, [&] { return gen != generation; });
    }
  }

private:
  unsigned generation = 0;
  unsigned counter = 0;
  const unsigned pool_size;
  std::mutex m;
  std::condition_variable cv;
};

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

unsigned sum_reduce_cpp(const unsigned *v, size_t n) {
  unsigned T = get_num_threads();
  std::vector<std::thread> workers(T - 1);
  barrier bar{T};
  auto partial_sums = std::make_unique<partial_t[]>(T);
  auto worker_proc = [v, n, T, &bar, &partial_sums](unsigned t) {
    // Map
    auto [b, e] = get_subvector(n, t, T);
    unsigned r = 0;
    for (size_t i = b; i < e; ++i)
      r += v[i];
    partial_sums[t].v = r;

    // Reduce
    for (size_t step = 1, rem = 2; step < T; step = rem, rem *= 2) {
      bar.arrive_and_wait();
      if (t % rem == 0 && t + step < T) {
        partial_sums[t].v += partial_sums[t + step].v;
      }
    }
  };

  for (unsigned t = 0; t < T - 1; ++t)
    workers[t] = std::thread(worker_proc, t + 1);

  worker_proc(0);

  for (unsigned i = 0; i < T - 1; ++i)
    workers[i].join();

  unsigned result = 0;
  for (std::size_t t = 0; t < T; t++)
    result += partial_sums[t].v;

  return result;
}
