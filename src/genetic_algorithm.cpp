#include "genetic_algorithm.hpp"
/*
GeneticAlgorithm::GeneticAlgorithm(const std::string& target, size_t pool_size,
                                   float mutation_rate)
    : H(algs::io::readFileToList(target).size()),
      W(algs::io::readFileToList(target).begin()->size()),
      target(target, H, W),
      pool_size(pool_size),
      mutation_rate(mutation_rate) {
  pool.reserve(pool_size);
  for (size_t i = 0; i < pool_size; i++) {
    pool.push_back(Field::get_random(H, W));
  }
}
// ~GeneticAlgorithm();
bool GeneticAlgorithm::step() {}
std::vector<float> GeneticAlgorithm::computeFitness() {}
std::vector<std::pair<size_t, size_t>> GeneticAlgorithm::selection(
    const std::vector<float>& fitness) {}
Field GeneticAlgorithm::crossover(const Field& a, const Field& B,
                                  float a_fitness, float b_fitness) {}
void GeneticAlgorithm::mutate(Field& f) {} */
