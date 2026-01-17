#include <cstddef>

unsigned sum_seq(const unsigned *v, std::size_t n) {
  unsigned sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    sum += v[i];
  }
  return sum;
}
