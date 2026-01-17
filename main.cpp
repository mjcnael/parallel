#include <chrono>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <thread>
#include <vector>

static unsigned GLOBAL_NUM_THREADS = std::thread::hardware_concurrency();

unsigned get_num_threads() { return GLOBAL_NUM_THREADS; }

unsigned set_num_threads(unsigned T) {
  GLOBAL_NUM_THREADS = (T == 0 ? 1 : T);
  return GLOBAL_NUM_THREADS;
}

auto generate(std::size_t n) {
  std::vector<unsigned> v(n);
  for (std::size_t i = 0; i < n; ++i)
    v[i] = i;
  return v;
}

unsigned sum_seq(const unsigned *, std::size_t);
unsigned sum_fs(const unsigned *, std::size_t);
unsigned sum_rr(const unsigned *, std::size_t);
unsigned sum_rr_fs(const unsigned *, std::size_t);
unsigned sum_reduce_cpp(const unsigned *, std::size_t);

struct exp_result {
  double time, speedup, efficiency;
  unsigned T, result;
};

std::vector<exp_result> conduct_experiment(unsigned (*sum)(const unsigned *,
                                                           size_t),
                                           size_t problem_size) {
  unsigned n_threads = get_num_threads();
  auto v = generate(problem_size);
  set_num_threads(1);
  std::vector<exp_result> results(n_threads);
  auto t1 = std::chrono::steady_clock::now();
  auto result = sum(v.data(), v.size());
  auto t2 = std::chrono::steady_clock::now();

  double base_duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

  results[0] = {
      .time = base_duration,
      .speedup = 1.0,
      .efficiency = 1.0,
      .T = 1,
      .result = result,
  };

  for (unsigned threads = 2; threads <= n_threads; ++threads) {
    set_num_threads(threads);
    v = generate(problem_size);

    t1 = std::chrono::steady_clock::now();
    result = sum(v.data(), v.size());
    t2 = std::chrono::steady_clock::now();

    double duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    double speedup = base_duration / duration;
    double efficiency = speedup / threads;

    results[threads - 1] = {
        .time = duration,
        .speedup = speedup,
        .efficiency = efficiency,
        .T = threads,
        .result = result,
    };
  }

  return results;
}

void save_csv(const std::string &filename,
              const std::vector<exp_result> &results) {
  std::ofstream file(filename);
  file << "T,time_ms,speedup,efficiency,result\n";
  for (const auto &r : results) {
    file << r.T << "," << r.time << "," << r.speedup << "," << r.efficiency
         << "," << r.result << "\n";
  }
}

int main(int argc, char *argv[]) {
  constexpr size_t N = 1u << 26;

  {
    auto input = generate(N);
    auto t1 = std::chrono::steady_clock::now();
    auto s = sum_seq(input.data(), input.size());
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "sum_seq: 0x" << std::hex << s << std::dec << " time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << " μs\n";
  }

  {
    auto input = generate(N);
    auto t1 = std::chrono::steady_clock::now();
    auto s = sum_rr(input.data(), input.size());
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "sum_rr: 0x" << std::hex << s << std::dec << " time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << " μs\n";
  }

  {
    auto input = generate(N);
    auto t1 = std::chrono::steady_clock::now();
    auto s = sum_rr_fs(input.data(), input.size());
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "sum_rr_fs: 0x" << std::hex << s << std::dec << " time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << " μs\n";
  }

  {
    auto input = generate(N);
    auto t1 = std::chrono::steady_clock::now();
    auto s = sum_fs(input.data(), input.size());
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "sum_fs: 0x" << std::hex << s << std::dec << " time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << " μs\n";
  }

  {
    auto input = generate(N);
    auto t1 = std::chrono::steady_clock::now();
    auto s = sum_reduce_cpp(input.data(), input.size());
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "sum_reduce_cpp: 0x" << std::hex << s << std::dec << " time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << " μs\n";
  }

  auto results = conduct_experiment(sum_fs, N);
  save_csv("results.csv", results);

  return 0;
}
