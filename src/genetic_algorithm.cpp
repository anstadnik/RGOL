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
  assert(pool_size > n_elitist);
  pool.reserve(pool_size);
  for (size_t i = 0; i < pool_size; i++) {
    pool.push_back(
        Field::get_random(H, W, max(2ul, i / max(1ul, pool_size / 500))));
    /* if (i % 100)
      continue;
    const auto& f = pool.back();
    for (const auto& l : f.field()) {
      ranges::copy(l, ostream_iterator<char>(cout, ""));
      std::cout << std::endl;
    }
    std::cout << std::endl;
    Field f_(f);
    // for (size_t k = 0; k < delta; k++) f_.step();
    Metrics m = calculateMetrics(target.field(), Field(f), delta);
    std::cout << "TP: " << m.true_pos << ", FP: " << m.false_pos
              << ", FN: " << m.false_neg << std::endl;
    GUI::showField(f_.field(), "Pool"); */
  }
  // exit(1);
  fitness.resize(pool_size);
  computeFitness();
}

float GeneticAlgorithm::step() {
  /* {
    // thread t(GUI::showField, ref(target), "Target");
    for (const auto& f : pool) {
      for (const auto& l : f.field()) {
        ranges::copy(l, ostream_iterator<char>(cout, ""));
        std::cout << std::endl;
      }
      std::cout << std::endl;
      Field f_(f);
      // for (size_t k = 0; k < delta; k++) f_.step();
      Metrics m = calculateMetrics(target.field(), Field(f), delta);
      std::cout << "TP: " << m.true_pos << ", FP: " << m.false_pos
                << ", FN: " << m.false_neg << std::endl;
      GUI::showField(f_.field(), "Pool");
    }
  }
  exit(1); */

  auto mating_pool = selection();
  vector<Field> pool_;
  pool_.reserve(pool_size);
  for (const auto& [a, b] : mating_pool)
    pool_.push_back(
        mutate(crossover(pool[a], pool[b], fitness[a], fitness[b])));
  auto&& elitists = getElite();
  pool_.insert(pool_.end(), std::make_move_iterator(elitists.begin()),
               std::make_move_iterator(elitists.end()));
  /* for (size_t i = 0; i < 3; i++)
    assert(dbg(pool_[mating_pool[pool_size - 1 - i].first]) ==
    dbg(pool[mating_pool[pool_size - 1 - i].first])); */
  pool = move(pool_);
  computeFitness();
  /* map<Field::FIELD_T, int> m;
  for (const auto& f : pool) m[f.field()]++;
  map<int, int> m_;
  for (const auto& i : m) m_[i.second]++; */
  /* multiset<int> ns;
  ranges::transform(m, inserter(ns, ns.begin()), [](const pair<Field::FIELD_T,
  int>& p) {return p.second;}); */
  // dbg(m_);

  return *max_element(fitness.begin(), fitness.end());
}

void GeneticAlgorithm::computeFitness() {
  vector<size_t> ns(pool_size);
  iota(ns.begin(), ns.end(), 0);
  for_each(execution::par, ns.begin(), ns.end(), [&](const size_t i) {
    // for (size_t i = 0; i < pool_size; i++) {
    pair<int, int> n_matches{0, 0};  // N of live and death matches
    auto f(pool[i]);

    for (size_t k = 0; k < delta; k++) f.step();
    for (auto l = f.field().begin(), l_ = target.field().begin();
         l_ != target.field().end(); l++, l_++)
      for (auto c = l->begin(), c_ = l_->begin(); c_ != l_->end(); c++, c_++)
        if (*c == *c_) {
          if (*c == '#')
            n_matches.first++;
          else
            n_matches.second++;
        }

    // dbg(n_matches);
    fitness[i] = float(n_matches.first * live_multiplier + n_matches.second) /
                 max_fitness;

    // }
  });
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
  Field::FIELD_T ret(a.field());
  size_t threshold = 1000 - 1000 * b_fitness / (a_fitness + b_fitness);
  for (auto [l, l_] = tuple{ret.begin(), b.field().begin()}; l < ret.end();
       l++, l_++)
    for (auto [c, c_] = tuple{l->begin(), l_->begin()}; c < l->end(); c++, c_++)
      if (randomGen(1000) > threshold) *c = *c_;
  return Field(move(ret));
}

Field GeneticAlgorithm::mutate(Field&& f) {
  for (auto l = f.f_[f.cur_field].begin(); l < f.f_[f.cur_field].end(); l++)
    for (auto c = l->begin(); c < l->end(); c++) {
      if (randomGen(10000) < mutation_rate * 10000) flip(*c);
    }
  return move(f);
}

const Field& GeneticAlgorithm::getBest() const { return pool[best_index]; }

Metrics calculateMetrics(const Field::FIELD_T& target, Field&& best,
                         size_t delta) {
  Metrics m;
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
  m.precision = float(m.true_pos) / (m.true_pos + m.false_pos);
  m.recall = float(m.true_pos) / (m.true_pos + m.false_neg);
  return m;
}
