#include <condition_variable>

#include "field.hpp"
#include "genetic_algorithm.hpp"
#include "gui.hpp"
#include "header.hpp"
#include "helpers.hpp"
#include "parser.hpp"

using namespace std;

void runGui(const string& fn, size_t H, size_t W, size_t n) {
  Field f(algs::io::readFileToList(fn), H, W);
  size_t i;
  bool open = true;
  std::thread t([&] {
    GUI g(H, W);
    while (open && (open = g.isOpen())) g.update(f.field(), i);
  });

  for (i = 0; i < n && open; i++) {
    try {
      f.step();
    } catch (const runtime_error& e) {
      cout << e.what() << endl << "Iteration: " << i << endl;
      break;
    }
    // usleep(100000);
  }
  open = false;
  t.join();
}

void runGeneticAlgorithm() {
  XInitThreads();
  auto inp = parseKaggleData(1);
  // auto i = algs::io::readFileToList("data/glider.txt");
  // vector<Entry> inp{{0, 1, Field(i).field()}};
  for (auto& [id, delta, f] : inp) {
    size_t max_n = 20, n = max_n;

    bool open = true;
    (void)open;
    optional<Field> best;
    /* std::thread t(
        [&](const auto f) {
          (void)f;
          while (!best.has_value()) usleep(100);
          GUI g1(f.size(), f[0].size());
          GUI g2(best->H, best->W);
          while (open && g2.isOpen() && g1.isOpen()) {
            g1.update(f);
            g2.update(best->field());
          }
        },
        f); */
    delta = 1;
    GeneticAlgorithm g(Field(move(f)), delta, 10000, 10e-3, 10, 5, 20, 0.1);
    // GeneticAlgorithm g(Field(move(f)), delta, 100, 10e-3, 7, 1, 20, 0.5);
    while (g.step() != 1 && n--) {
      Metrics m = calculateMetrics(f, Field(g.getBest()), delta);
      Field tmp = Field(g.getBest());
      for (size_t k = 0; k < delta; k++) tmp.step();
      best = move(tmp);
      std::cout << "N: " << max_n - n << ", Fitness: " << g.getBestFitness()
                << ", TP: " << m.true_pos << ", TN: " << m.true_neg
                << ", FP: " << m.false_pos << ", FN: " << m.false_neg
                << std::endl;
      /* for (const auto& l : best->field()) {
        ranges::copy(l, ostream_iterator<char>(cout, ""));
        std::cout << std::endl;
      }
      std::cout << std::endl; */

      /* std::cout << "Precision: " << m.precision << ", recall: " << m.recall
                << std::endl; */
    }
    open = false;
    // t.join();
  }
}

int main(void) {
  runGeneticAlgorithm();
  /* constexpr size_t power = 10;
  constexpr size_t H = 1 << power, W = 1 << power; */
  // constexpr size_t W = 50, H = 50;
  // runGui("data/input.txt", H, W, 1500);
  // runGui("data/glider.txt", H, W, 100);

  return 0;
}
