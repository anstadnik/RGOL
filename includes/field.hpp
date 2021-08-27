#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <string>

#include "algs/algs.h"

class Field {
 public:
  using FIELD_T = std::vector<std::vector<char>>;

  Field(const std::list<std::string>& fn, size_t H = 0, size_t W = 0);
  static Field get_random(size_t W, size_t H);
  void step();
  operator std::string() const;
  const FIELD_T& field() const;
  const size_t H, W;

 private:
  void countNeighbours();
  void updateStates();

  std::array<FIELD_T, 2> f_;
  size_t cur_field = 0;
  mutable std::mutex m;
};
std::ostream& operator<<(std::ostream& os, const Field& f);
