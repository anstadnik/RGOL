#pragma once

#include <execution>
#include <mutex>
#include <numeric>
#include <queue>
#include <ranges>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "field.hpp"
#include "gui.hpp"

class GeneticAlgorithm {
 public:
  using VC = std::vector<char>;
  using VVC = std::vector<VC>;
  GeneticAlgorithm(const Field& target, size_t delta, size_t pool_size = 1000,
                   float mutation_rate = 10e-5, float live_multiplier = 10,
                   size_t n_elitist = 5, size_t stagnation_limit = 100,
                   float percent_extermination = 0.3);
  // ~GeneticAlgorithm();
  float step();
  void computeFitness();
  // void computeFitness();
  std::vector<std::pair<size_t, size_t>> selection();
  Field crossover(const Field& a, const Field& b, float a_fitness,
                  float b_fitness);
  std::vector<Field> getElite();
  void removeBest();
  Field mutate(Field&& f);
  void extermination();
  const Field& getBest() const;
  float getBestFitness() const;

 private:
  std::vector<Field> pool;
  /* TODO: try double for fitness <27-08-21, astadnik> */
  std::vector<double> fitness;

  const int H, W;

  const Field target;
  const size_t pool_size;
  const size_t delta;
  const float mutation_rate;
  const size_t n_elitist;
  const float live_multiplier;
  const int max_fitness;
  size_t stagnation_limit;

  size_t best_index = 0;
  size_t n_stagnation = 0;
  float percent_extermination;
  double best_fitness = 0;

  static constexpr size_t rand_mult = 100;
};

typedef struct Metrics_ {
  size_t true_pos = 0;
  size_t true_neg = 0;
  size_t false_pos = 0;
  size_t false_neg = 0;
  float precision = 0;
  float recall = 0;
} Metrics;

Metrics calculateMetrics(const Field::VVC& target, Field&& best, size_t delta);
