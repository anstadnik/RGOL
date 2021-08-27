#include "genetic_algorithm.hpp"

using namespace std;

namespace {
void flip(char& c) { c = c == '.' ? '#' : '.'; }
}  // namespace

GeneticAlgorithm::GeneticAlgorithm(const Field& target, size_t delta,
                                   size_t pool_size, float mutation_rate,
                                   float live_multiplier, size_t n_elitist)
    : H(target.H),
      W(target.W),
      target(target),
      pool_size(pool_size),
      delta(delta),
      mutation_rate(mutation_rate),
      n_elitist(n_elitist),
      live_multiplier(live_multiplier),
      max_fitness([&] {
        int n_living = accumulate(target.field().begin(), target.field().end(),
                                  0, [](int sum, const vector<char>& f) {
                                    return sum + ranges::count(f, '#');
                                  });
        return n_living * live_multiplier + H * W - n_living;
      }()) {
  pool.reserve(pool_size);
  for (size_t i = 0; i < pool_size; i++)
    pool.push_back(Field::get_random(H, W));
  fitness.resize(pool_size);
  computeFitness();
}
// ~GeneticAlgorithm();

float GeneticAlgorithm::step() {
  auto mating_pool = selection();
  vector<Field> pool_ = pool;
  pool_.reserve(pool_size);
  for (const auto& [a, b] : mating_pool)
    mutate(crossover(pool_[a], pool[b], fitness[a], fitness[b]));
  pool = move(pool_);
  computeFitness();
  return *max_element(fitness.begin(), fitness.end());
}

void GeneticAlgorithm::computeFitness() {
  vector<size_t> ns(pool_size);
  iota(ns.begin(), ns.end(), 0);
  for_each(execution::par, ns.begin(), ns.end(), [&](const size_t i) {
    // for (size_t i = 0; i < pool_size; i++) {
    pair<int, int> n_matches{0, 0};  // N of live and death matches
    auto f = pool[i];
    for (size_t k = 0; k < delta; k++) f.step();
    auto l = f.field().begin(), l_ = target.field().begin();
    for (; l_ != target.field().end(); l++, l_++) {
      auto c = l->begin(), c_ = l_->begin();
      for (; c_ != l_->end(); c++, c_++)
        if (*c == *c_) {
          if (*c == '#')
            n_matches.first++;
          else
            n_matches.second++;
        }
    }
    fitness[i] = static_cast<float>(n_matches.first * live_multiplier +
                                    n_matches.second) /
                 max_fitness;
    // }
  });
}

vector<pair<size_t, size_t>> GeneticAlgorithm::selection() {
  vector<pair<size_t, size_t>> mating_pool;
  mating_pool.reserve(pool_size);

  vector<size_t> random_bucket;
  random_bucket.reserve(pool_size * 100);
  float fitness_sum = accumulate(fitness.begin(), fitness.end(), 0.f);
  float norm_factor = pool_size * 100 / fitness_sum;
  for (size_t i = 0; i < pool_size; i++) {
    for (size_t j = 0; j < fitness[i] * norm_factor; j++)
      random_bucket.push_back(i);
  }

  vector<size_t> orgy_pool;
  orgy_pool.reserve((pool_size - n_elitist) * 2);
  ranges::sample(random_bucket, back_inserter(orgy_pool), pool_size * 2,
                 std::mt19937{std::random_device{}()});
  ranges::shuffle(orgy_pool, std::mt19937{std::random_device{}()});
  for (size_t i = 0; i < (pool_size - n_elitist); i++)
    mating_pool.push_back({orgy_pool[i * 2], orgy_pool[i * 2 + 1]});
  priority_queue<std::pair<float, size_t>> q;
  for (size_t i = 0; i < pool_size; i++) q.push({fitness[i], i});
  for (size_t i = 0; i < n_elitist; i++)
    mating_pool.push_back({q.top().second, q.top().second}), q.pop();
  assert(mating_pool.size() == pool_size);
  return mating_pool;
}

Field& GeneticAlgorithm::crossover(Field& a, const Field& b, float a_fitness,
                                   float b_fitness) {
  int threshold = 1000 - 1000 * b_fitness / (a_fitness + b_fitness);
  for (auto [l, l_] = tuple{a.f_[a.cur_field].begin(), b.field().begin()};
       l < a.f_[a.cur_field].end(); l++, l_++)
    for (auto [c, c_] = tuple{l->begin(), l_->begin()}; c < l->end(); c++, c_++)
      if (randomGen<1000>() > threshold) *c = *c_;
  return a;
}

Field& GeneticAlgorithm::mutate(Field& f) {
  for (auto l = f.f_[f.cur_field].begin(); l < f.f_[f.cur_field].end(); l++)
    for (auto c = l->begin(); c < l->end(); c++) {
      if (randomGen<2>() < mutation_rate * 100) flip(*c);
    }
  return f;
}
