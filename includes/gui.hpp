#pragma once

#include <SFML/Graphics.hpp>

#include "field.hpp"

constexpr size_t scale = 2;

class GUI {
 public:
  GUI(size_t H, size_t W);
  ~GUI();
  void update(const Field &f, size_t iteration);
  bool isOpen() const;

 private:
  void checkEvents();
  sf::RenderWindow window;
};
