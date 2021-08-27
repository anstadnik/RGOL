#pragma once

#include "field.hpp"

typedef struct {
  size_t id;
  int delta;
  Field::FIELD_T f;
} Entry;

std::vector<Entry> parseKaggleData(int n = -1,
                                   const std::string& fn = "data/test.csv");

