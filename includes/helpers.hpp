#pragma once

#include <cassert>
#include <random>
#include <vector>

#include "algs/algs.h"
template <int MAX>
bool randomGen() {
  static std::mt19937 gen{std::random_device{}()};
  static int r = gen();
  if (r > MAX)
    r /= MAX;
  else
    r = gen();
  return (r % MAX);
}
