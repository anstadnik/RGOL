#pragma once

#include <cassert>
#include <random>
#include <ranges>
#include <vector>
#include <unordered_map>

#include "algs/algs.h"
#include "field.hpp"

inline ulong randomGen(size_t max_v) {
  static std::mt19937 gen{std::random_device{}()};
  static std::unordered_map<size_t, size_t> m;
  if (!m.contains(max_v))
    m[max_v] = gen();
  size_t& r = m[max_v];
  if (r > max_v)
    r /= max_v;
  else
    r = gen();
  return r % max_v;
}

template <int MAX_V>
ulong randomGen() {
  static std::mt19937 gen{std::random_device{}()};
  static size_t r = gen();
  if (r > MAX_V)
    r /= MAX_V;
  else
    r = gen();
  return r % MAX_V;
}
