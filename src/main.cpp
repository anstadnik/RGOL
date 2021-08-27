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
    while (open && (open = g.isOpen())) g.update(f, i);
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
  auto inp = parseKaggleData(1);
  for (auto& [id, delta, f] : inp) {
    size_t max_n = 1000;
    GeneticAlgorithm g(Field(move(f)), delta, 100000);
    while (dbg(g.step()) != 1 && max_n--)
      ;
    /* while (g.step() != 1 && max_n--)
      ; */
  }
}

int main(void) {
  runGeneticAlgorithm();
  /* constexpr size_t power = 10;
  constexpr size_t H = 1 << power, W = 1 << power; */
  // constexpr size_t W = 50, H = 50;
  // runGui("input.txt", H, W, 1500);
  // runGui("glider.txt", H, W, 100);

  return 0;
}
