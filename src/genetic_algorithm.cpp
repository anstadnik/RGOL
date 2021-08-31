#include "genetic_algorithm.hpp"

using namespace std;

namespace {
void flip(char& c) { c = c == '.' ? '#' : '.'; }
}  // namespace

GeneticAlgorithm::GeneticAlgorithm(const Field& target, size_t delta,
                                   size_t pool_size, float mutation_rate,
                                   float live_multiplier, size_t n_elitist,
                                   size_t stagnation_limit,
                                   float percent_extermination)
    : H(target.H),
      W(target.W),
      target(target),
      pool_size(pool_size),
      delta(delta),
      mutation_rate(mutation_rate),
      n_elitist(n_elitist),
      live_multiplier(live_multiplier),
      max_fitness([&] {
        int n_living = accumulate(
            target.field().begin(), target.field().end(), 0,
            [](int sum, const VC& f) { return sum + ranges::count(f, '#'); });
        return n_living * live_multiplier + H * W - n_living;
      }()),
      stagnation_limit(stagnation_limit),
      percent_extermination(percent_extermination) {
  assert(pool_size > n_elitist);
  pool.reserve(pool_size);
  for (size_t i = 0; i < pool_size; i++)
    /* pool.push_back(Field::get_random(
        H, W,
        max(2ul, ((i + pool_size) / 3) / max(1ul, 2 * pool_size / 3 / 625))));
     */
    pool.push_back(Field::get_random(H, W, 2));
  fitness.resize(pool_size);
  computeFitness();
}

float GeneticAlgorithm::step() {
  // removeBest();
  auto mating_pool = selection();
  vector<Field> pool_;
  pool_.reserve(pool_size);
  for (const auto& [a, b] : mating_pool)
    pool_.push_back(
        mutate(crossover(pool[a], pool[b], fitness[a], fitness[b])));
  auto&& elitists = getElite();
  pool_.insert(pool_.end(), std::make_move_iterator(elitists.begin()),
               std::make_move_iterator(elitists.end()));
  pool = move(pool_);
  float best_fitness_ = best_fitness;
  computeFitness();

  assert(!n_stagnation || best_fitness_ >= best_fitness);
  n_stagnation =
      abs(best_fitness_ - best_fitness) < 10e-4 ? n_stagnation + 1 : 0;
  if (n_stagnation > stagnation_limit) {
    std::cout << "EXTERMINATION!" << std::endl;
    n_stagnation = 0;
    extermination();
  }
  return best_fitness;
}

void GeneticAlgorithm::removeBest() {
  // Hm. I remove best ones
  Field best(this->getBest());
  for (size_t i = 0; i < pool_size; i++)
    if (pool[i] == best)
      pool[i] =
          Field::get_random(H, W, max(2ul, i / max(1ul, pool_size / 500)));
}

void GeneticAlgorithm::computeFitness() {
  vector<size_t> is(pool_size);
  iota(is.begin(), is.end(), 0);
  for_each(execution::par, is.begin(), is.end(), [&](const size_t i) {
    // for (size_t i = 0; i < pool_size; i++) {
    int n_matches_live = 0, n_matches_dead = 0;
    auto f(pool[i]);
    for (size_t k = 0; k < delta; k++) f.step();

    for (auto l = f.field().begin(), l_ = target.field().begin();
         l_ != target.field().end(); l++, l_++)
      for (auto c = l->begin(), c_ = l_->begin(); c_ != l_->end(); c++, c_++)
        if (*c == *c_) *c == '#' ? n_matches_live++ : n_matches_dead++;

    fitness[i] = pow(
        double(n_matches_live * live_multiplier + n_matches_dead) / max_fitness,
        2);
    // }
  });
  auto best = ranges::max_element(fitness);
  best_index = distance(fitness.begin(), best);
  best_fitness = *best;
}

vector<Field> GeneticAlgorithm::getElite() {
  vector<Field> elite;
  priority_queue<std::pair<float, size_t>> q;
  for (size_t i = 0; i < pool_size; i++) q.push({fitness[i], i});
  best_index = q.top().second;
  for (size_t i = 0; i < n_elitist; i++)
    elite.push_back(pool[q.top().second]), q.pop();
  return elite;
}

vector<pair<size_t, size_t>> GeneticAlgorithm::selection() {
  vector<pair<size_t, size_t>> mating_pool;
  mating_pool.reserve(pool_size - n_elitist);

  vector<size_t> random_bucket;
  random_bucket.reserve(pool_size * 100);
  float fitness_sum = accumulate(fitness.begin(), fitness.end(), 0.f);
  float norm_factor = pool_size * 100 / fitness_sum;
  for (size_t i = 0; i < pool_size; i++)
    for (size_t j = 0; j < fitness[i] * norm_factor; j++)
      random_bucket.push_back(i);

  /* TODO: Consider selecting simply by fitness <30-08-21, astadnik> */
  vector<size_t> orgy_pool;
  orgy_pool.reserve((pool_size - n_elitist) * 2);
  ranges::sample(random_bucket, back_inserter(orgy_pool), pool_size * 2,
                 std::mt19937{std::random_device{}()});
  ranges::shuffle(orgy_pool, std::mt19937{std::random_device{}()});
  for (size_t i = 0; i < pool_size - n_elitist; i++)
    mating_pool.push_back({orgy_pool[i * 2], orgy_pool[i * 2 + 1]});

  assert(mating_pool.size() == pool_size - n_elitist);
  return mating_pool;
}

Field GeneticAlgorithm::crossover(const Field& a, const Field& b,
                                  float a_fitness, float b_fitness) {
  VVC ret(a.field());
  size_t threshold =
      rand_mult - rand_mult * b_fitness / (a_fitness + b_fitness);
  for (auto [l, l_] = tuple{ret.begin(), b.field().begin()}; l < ret.end();
       l++, l_++)
    for (auto [c, c_] = tuple{l->begin(), l_->begin()}; c < l->end(); c++, c_++)
      if (randomGen<rand_mult>() > threshold) *c = *c_;
  return Field(move(ret));
}

Field GeneticAlgorithm::mutate(Field&& f) {
  for (auto l = f.f_[f.cur_field].begin(); l < f.f_[f.cur_field].end(); l++)
    for (auto c = l->begin(); c < l->end(); c++) {
      if (randomGen<rand_mult>() < mutation_rate * rand_mult) flip(*c);
    }
  return move(f);
}

void GeneticAlgorithm::extermination() {
  for (size_t i = 0; i < pool_size; i++)
    if (randomGen<rand_mult>() < percent_extermination * rand_mult)
      pool[i] =
          Field::get_random(H, W, max(2ul, i / max(1ul, pool_size / 500)));
}

const Field& GeneticAlgorithm::getBest() const { return pool[best_index]; }

float GeneticAlgorithm::getBestFitness() const { return best_fitness; }

Metrics calculateMetrics(const GeneticAlgorithm::VVC& target, Field&& best,
                         size_t delta) {
  Metrics m;
  // dbg(best);
  for (size_t k = 0; k < delta; k++) best.step();
  for (size_t i = 0; i < best.H; i++) {
    for (size_t j = 0; j < best.W; j++) {
      if (const auto c = best.field()[i][j], c_ = target[i][j]; c != c_) {
        if (c == '#')
          m.false_pos++;
        else
          m.false_neg++;
      } else if (c == '#')
        m.true_pos++;
    }
  }
  m.true_neg = best.H * best.W - (m.true_pos + m.false_neg + m.false_pos);
  m.precision = float(m.true_pos) / (m.true_pos + m.false_pos);
  m.recall = float(m.true_pos) / (m.true_pos + m.false_neg);
  /* dbg(best);
  dbg(Field(VVC(target)));
  dbg(m.true_pos, m.true_neg, m.false_pos, m.false_neg, m.precision, m.recall);
*/
  // exit(1);
  return m;
}
