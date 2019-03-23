
#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <type_traits>

/*
 * Implementations of some simple statistics that are not provided
 * by the standard library (and (without spending too long...) I couldn't
 * find a header-only lib that implemented them...)
 */

namespace nr {
namespace stats {

template <typename Cont> double mean(const Cont &c) {
  auto sum = std::accumulate(c.begin(), c.end(), 0.0);
  return sum / static_cast<double>(c.size());
}

template <typename Cont> double variance(const Cont &c) {
  double m = mean(c);
  double s = std::accumulate(c.begin(), c.end(), 0.0, [m](auto x, auto y) {
    return x + std::pow(y - m, 2);
  });
  return s / static_cast<double>(c.size());
}

template <typename Cont> double stdev(const Cont &c) {
  // stdev is the square root of the variance.
  return std::sqrt<double>(variance(c));
}

template <typename Cont> typename Cont::value_type median(Cont c) {
  // don't need to sort(?), but c shouldn't be too large anyway.
  // so the time shouldn't be too bad.

  if (c.size() == 0) {
    // median of empty container is undefined
    throw std::logic_error("cannot compute medaian of empty container");
  }

  std::sort(c.begin(), c.end());

  if (c.size() % 2 == 0) {
    // if there's an even number, take average of middle two.
    return (c[c.size() / 2 - 1] + c[c.size() / 2]) / 2;
  } else {
    return c[c.size() / 2];
  }
}

template <typename Cont> std::vector<int64_t> histogram(const Cont &c) {
  // computes the frequency of all elements of c.
  // elements of c are guaranteed to be between 0 and max(c)
  // potentially poor space complexity, but I do not think it is a concern?
  // assumes Cont::value_type is integral.
  static_assert(std::is_integral<typename Cont::value_type>::value);

  auto hist_size = *std::max_element(c.begin(), c.end());
  std::vector<int64_t> hist(hist_size, 0);
  for (const auto &elem : c) {
    ++hist[elem];
  }
  return hist;
}

template <template <typename Sub> typename Cont, typename Sub>
Sub mode(const Cont<Sub> &c) {
  // finds the most frequently occuring element of c.
  // It first finds the frequency of all elements.
  // assumes Cont::value_type is integral.
  static_assert(std::is_integral<Sub>::value);

  std::vector<int64_t> hist = histogram(c);
  auto iter_of_most_freq = std::max_element(hist.begin(), hist.end());
  size_t most_freq = std::distance(hist.begin(), iter_of_most_freq);
  return most_freq;
}

} // namespace stats
} // namespace nr
