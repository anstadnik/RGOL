#pragma once
/*
#include <string>
#include <vector>

#include "field.hpp"

class GeneticAlgorithm {
 public:
  GeneticAlgorithm(const std::string& target, size_t pool_size = 1000,
                   float mutation_rate = 0.1);
  // ~GeneticAlgorithm();
  bool step();
  std::vector<float> computeFitness();
  std::vector<std::pair<size_t, size_t>> selection(
      const std::vector<float>& fitness);
  Field crossover(const Field& a, const Field& B, float a_fitness,
                  float b_fitness);
  void mutate(Field& f);

 private:
  std::vector<Field> pool;
  const size_t H, W;
  const Field target;
  const size_t pool_size;
  const float mutation_rate;
}; */
