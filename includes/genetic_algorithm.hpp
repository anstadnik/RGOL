#pragma once

#include <execution>
#include <numeric>
#include <queue>
#include <ranges>
#include <string>
#include <vector>
#include <thread>
#include <set>
#include <mutex>

#include "field.hpp"
#include "gui.hpp"


class GeneticAlgorithm {
 public:
  GeneticAlgorithm(const Field& target, size_t delta, size_t pool_size = 1000,
                   float mutation_rate = 10e-5, float live_multiplier = 10,
                   size_t n_elitist = 5);
  // ~GeneticAlgorithm();
  float step();
  void computeFitness();
  std::vector<std::pair<size_t, size_t>> selection();
  Field crossover(const Field& a, const Field& b, float a_fitness,
                  float b_fitness);
  std::vector<Field> getElite();
  Field mutate(Field&& f);
  const Field& getBest() const;

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

  size_t best_index;
};

typedef struct Metrics_ {
  size_t true_pos = 0;
  size_t false_pos = 0;
  size_t false_neg = 0;
  float precision = 0;
  float recall = 0;
} Metrics;

Metrics calculateMetrics(const Field::FIELD_T& target, Field&& best,
                         size_t delta);
