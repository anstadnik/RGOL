#pragma once

#include "field.hpp"

typedef struct Entry_ {
  size_t id;
  size_t delta;
  Field::VVC f;
} Entry;

std::vector<Entry> parseKaggleData(int n = -1,
                                   const std::string& fn = "data/test.csv");

