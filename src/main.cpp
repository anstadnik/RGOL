#include <condition_variable>

#include "field.hpp"
#include "gui.hpp"
#include "header.hpp"

using namespace std;

int main(void) {
  constexpr size_t power = 10;
  constexpr size_t H = 1 << power, W = 1 << power;
  // constexpr size_t W = 50, H = 50;

  Field f(algs::io::readFileToList("input.txt"), H, W);
  // Field f(algs::io::readFileToList("glider.txt"), H, W);

  size_t i;
  bool open = true;
  std::thread t([&] {
    GUI g(H, W);
    while (open && (open = g.isOpen())) g.update(f, i);
  });

  for (i = 0; i < 100 && open; i++) {
    // for (size_t i = 0; i < 10000000000; i++) {
    try {
      f.step();
    } catch (const runtime_error& e) {
      cout << e.what() << endl << "Iteration: " << i << endl;
      break;
    }

    // TUI 1
    // if (i && !(i % 100)) cout << endl;
    // cout << ".";

    // TUI 2
    // cout << f << std::endl;

    // usleep(100000);
  }
  open = false;
  t.join();

  return 0;
}
