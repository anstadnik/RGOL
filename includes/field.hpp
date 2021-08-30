#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <string>

#include "algs/algs.h"
#include "helpers.hpp"


class Field {
 public:
  using FIELD_T = std::vector<std::vector<char>>;

  Field(const std::list<std::string>& fn, size_t H_ = 0, size_t W_ = 0);
  explicit Field(const Field& other);
  Field(Field&& other);
  Field(FIELD_T&& f_);
  Field& operator=(Field&& other);
  bool operator==(const Field&) const;
  static Field get_random(size_t H, size_t W, size_t ratio);
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
  static constexpr int ratio_of_death_to_live_in_random_init = 4;
  friend class GeneticAlgorithm;
};
std::ostream& operator<<(std::ostream& os, const Field& f);
