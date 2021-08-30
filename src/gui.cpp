#include "gui.hpp"

using namespace std;

GUI::GUI(size_t H, size_t W)
    : window(sf::VideoMode(H * scale, W * scale), "Game of life") {
  window.setFramerateLimit(60);
}

void GUI::checkEvents() {
  sf::Event event;

  while (window.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::Closed:
        window.close();
        break;

      case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::Escape ||
            event.key.code == sf::Keyboard::Q)
          window.close();
        break;

      default:
        break;
    }
  }
}

void GUI::showField(const Field::FIELD_T& f, const string& text) {
  GUI g(f.size(), f[0].size());
  g.update(f, 0, text);
  while (g.window.isOpen()) g.checkEvents();
}

void GUI::update(const Field::FIELD_T &f, size_t iteration, const string& text) {
  const size_t H = f.size(), W = f[0].size();
  vector<sf::Uint8> pixels(W * 4 * scale);
  sf::Texture texture;
  texture.create(W * scale, H * scale);

  sf::Sprite sprite(texture);  // needed to draw the texture on screen

  for (size_t i = 0; i < H; i++) {
    const auto& f_ = f[i];
    for (size_t j = 0; j < W; j++) {
      sf::Uint8 v = f_[j] == '.' ? 0 : 255;

      size_t offset = j * scale * 4;
      for (size_t k = 0; k < scale; k++) {
        pixels[offset + k * 4] = v;
        pixels[offset + k * 4 + 1] = v;
        pixels[offset + k * 4 + 2] = v;
        pixels[offset + k * 4 + 3] = 255;
      }
    }
    for (size_t k = 0; k < scale; k++)
      texture.update(pixels.data(), W * scale, 1, 0, i * scale + k);
  }

  window.draw(sprite);

  if (iteration) {
    sf::Font font;
    if (!font.loadFromFile("/home/astadnik/work/madame-web/projects/"
                           "object_detection/resources/"
                           "fonts/JetBrainsMono-Regular.ttf"))
      throw runtime_error("No font");
    sf::Text text("Iteration: " + to_string(iteration), font, 5 * scale);
    window.draw(text);
  }
  if (text.size()) {
    sf::Font font;
    if (!font.loadFromFile("/home/astadnik/work/madame-web/projects/"
                           "object_detection/resources/"
                           "fonts/JetBrainsMono-Regular.ttf"))
      throw runtime_error("No font");
    window.draw(sf::Text(text, font, 5 * scale));
  }
  window.display();
  checkEvents();
}

bool GUI::isOpen() const { return window.isOpen(); }

GUI::~GUI() { window.close(); }
