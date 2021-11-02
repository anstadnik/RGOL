#pragma once

#include <SFML/Graphics.hpp>

#include "field.hpp"

constexpr size_t scale = 2;

class GUI {
 public:
  GUI(size_t H, size_t W);
  ~GUI();
  static void showField(const Field::VVC& f, const std::string& text = "");
  void update(const Field::VVC&f, size_t iteration = 0, const std::string& text = "");
  bool isOpen() const;

 private:
  void checkEvents();
  sf::RenderWindow window;
};
