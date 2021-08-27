#pragma once

#include <execution>
#include <numeric>
#include <queue>
#include <ranges>
#include <string>
#include <vector>

#include "field.hpp"

class GeneticAlgorithm {
 public:
  GeneticAlgorithm(const Field& target, size_t delta, size_t pool_size = 1000,
                   float mutation_rate = 10e-5, float live_multiplier = 10,
                   size_t n_elitist = 5);
  // ~GeneticAlgorithm();
  float step();
  void computeFitness();
  std::vector<std::pair<size_t, size_t>> selection();
  Field& crossover(Field& a, const Field& b, float a_fitness, float b_fitness);
  Field& mutate(Field& f);

 private:
  std::vector<Field> pool;
  /* TODO: try double for fitness <27-08-21, astadnik> */
  std::vector<float> fitness;
  const int H, W;
  const Field target;
  const size_t pool_size;
  const size_t delta;
  const float mutation_rate;
  const size_t n_elitist;
  const float live_multiplier;
  const int max_fitness;
};
