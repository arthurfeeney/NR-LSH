
#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <functional>

/*
 * Implementations of some simple statistics that are not provided
 * by the standard library (and (without spending too long...) I couldn't
 * find a header-only lib that implemented them.)
 */

// include more complicated functions
#include "topk.hpp"

namespace nr {
namespace stats {

template <typename Cont> double mean(const Cont &c) {
  if (c.size() == 0) {
    // mean of empty container is undefined
    throw std::logic_error("cannot compute mean of empty container");
  }
  auto sum = std::accumulate(c.begin(), c.end(), 0.0);
  return sum / static_cast<double>(c.size());
}

template <typename Cont> double variance(const Cont &c) {
  if (c.size() == 0) {
    throw std::logic_error("cannot compute variance of empty container");
  }
  double m = mean(c);
  double s = std::accumulate(c.begin(), c.end(), 0.0, [m](auto x, auto y) {
    return x + std::pow(y - m, 2);
  });
  return s / static_cast<double>(c.size());
}

template <typename Cont> double stdev(const Cont &c) {
  // stdev is the square root of the variance.
  return std::sqrt(variance(c));
}

template <typename Cont> double median(Cont c) {
  // don't need to sort(?), but c shouldn't be too large anyway.
  // so the time shouldn't be too bad.
  // returns double for when there are an even numer of elements.

  if (c.size() == 0) {
    // median of empty container is undefined
    throw std::logic_error("cannot compute medaian of empty container");
  }

  std::sort(c.begin(), c.end());

  if (c.size() % 2 == 0) {
    // if there's an even number, take average of middle two.
    return static_cast<float>(c[c.size() / 2 - 1] + c[c.size() / 2]) / 2.0;
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

  auto hist_size = *std::max_element(c.begin(), c.end()) + 1;
  std::vector<int64_t> hist(hist_size, 0);
  for (const auto &elem : c) {
    if (elem < 0) {
      throw std::logic_error("stats::histogram cannot take negative values");
    }
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

template <template <typename Sub> typename Cont, typename Sub>
Cont<Sub> nonzero(const Cont<Sub> &c) {
  // returns all nonzero elements of the input container.
  // only works for containers that contain numbers.
  static_assert(std::is_arithmetic<Sub>::value);
  Cont<Sub> nonzero_elements;
  for (auto &elem : c) {
    if (elem != 0) {
      nonzero_elements.push_back(elem);
    }
  }
  return nonzero_elements;
}

template <template<typename Sub> typename Cont, typename Sub>
std::pair<Cont<Sub>, Cont<size_t>> unique(const Cont<Sub> &c) {
  // return unique elements of the input and their indices into the input.
  // returned in order that the elements first appear in.
  // - I.e., unique(2, 2, 1, 3) -> {2, 1, 3}.
  Cont<Sub> unique_values(0);
  Cont<size_t> unique_idx(0);
  size_t idx = 0;
  for(const Sub &elem : c) {
    auto found_iter = std::find(unique_values.begin(), unique_values.end(), elem);

    // if elem is not in unique_values, insert it.
    if(found_iter == unique_values.end()) {
      unique_values.push_back(elem);
      unique_idx.push_back(idx);
    }
    ++idx; // increment index counter.
  }
  return std::make_pair(unique_values, unique_idx);
}

short same_bits(size_t m, size_t n, int64_t bits) {
  // if both have 0's and 1's in the same spot, similarity incremented.
  short count = 0;

  size_t one_bits = m & n; // both ones.
  size_t zero_bits = ~m & ~n; // zero bits. (including those past true range.)

  size_t match_bits = one_bits | zero_bits;

  // iterate through first 'bits' of the matching bits.
  for(int i = 0; i < bits; ++i) {
    count += match_bits & 1;
    match_bits >>= 1;
  }
  return count;
}




} // namespace stats
} // namespace nr
