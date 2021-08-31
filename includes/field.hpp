#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <string>

#include "algs/algs.h"
#include "helpers.hpp"

class Field {
 public:
  using VC = std::vector<char>;
  using VVC = std::vector<VC>;

  Field(const std::list<std::string>& fn, size_t H_ = 0, size_t W_ = 0);
  explicit Field(const Field& other);
  Field(Field&& other);
  Field(VVC&& f_);
  Field& operator=(Field&& other);
  bool operator==(const Field&) const;
  static Field get_random(size_t H, size_t W, size_t ratio);
  void step();
  operator std::string() const;
  const VVC& field() const;
  const size_t H, W;

 private:
  void countNeighbours();
  void updateStates();

  std::array<VVC, 2> f_;
  std::atomic_size_t cur_field = 0;
  static constexpr int ratio = 3;
  friend class GeneticAlgorithm;
};
std::ostream& operator<<(std::ostream& os, const Field& f);
