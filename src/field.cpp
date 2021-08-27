#include "field.hpp"

using namespace std;

Field::Field(const std::list<std::string>& init, size_t H, size_t W)
    : H(H ? H : init.size()), W(W ? W : init.front().size()) {
  auto& f = f_[cur_field];

  f_ = {FIELD_T(H, vector<char>(W, '.')), FIELD_T(H, vector<char>(W, '.'))};
  size_t h = init.size(), w = init.begin()->size();
  size_t h_offs = H / 2 - h / 2, w_offs = W / 2 - w / 2;
  auto l = init.begin();
  for (size_t i = 0; i < h; i++, ++l) {
    auto c = l->begin();
    for (size_t j = 0; j < w; j++, ++c) {
      f[h_offs + i][w_offs + j] = *c;
    }
  }
}

Field::Field(const Field& other)
    : H(other.H), W(other.W), f_(other.f_), cur_field(other.cur_field) {}

Field::Field(FIELD_T&& f_) : H(f_.size()), W(f_[0].size()), f_({f_, f_}){}

bool Field::operator==(const Field& other) const {
  return f_ == other.f_ && cur_field == other.cur_field;
}

Field Field::get_random(size_t H, size_t W) {
  Field::FIELD_T f_;
  f_.reserve(H);
  for (size_t i = 0; i < H; i++) {
    f_.push_back(vector<char>(W));
    // ranges::generate(f.back(), []() {return randomGen<ratio_of_death_to_live_in_random_init>() ? '.' : '#';});
    ranges::generate(f_.back(), []() {return '.';});
  }
  auto f = Field(move(f_));
  for (size_t i = 0; i < 5; i++) 
    f.step();
  return f;
}

void Field::countNeighbours() {
  auto& f = f_[cur_field];
  auto& next_f = f_[!cur_field];

  for (auto& l : next_f) fill(l.begin(), l.end(), 0);

  for (size_t i = 0; i < H; i++) {
    auto l = f[i].begin();
    for (size_t j = 0; j < W; j++, l++) {
      if (*l == '#') {
        auto cur_line = next_f.begin() + i;
        auto f = [&](auto&& l_, bool center = false) {
          if (j) l_[j - 1]++;
          if (!center) l_[j]++;
          if (j < W - 2) l_[j + 1]++;
        };
        // auto previous_char = previous_line->begin() + j;
        if (i) f(cur_line[-1]);
        f(cur_line[0], true);
        if (i < H - 2) f(cur_line[1]);
      }
    }
  }
}

void Field::updateStates() {
  auto& f = f_[cur_field];
  auto& next_f = f_[!cur_field];
  const auto fend = f.end();
  for (auto l = f.begin(), l_ = next_f.begin(); l < fend; l++, l_++) {
    const auto& lend = l->end();
    for (auto c = l->begin(), c_ = l_->begin(); c < lend; c++, c_++) {
      if ((*c == '#' && (*c_ == 2 || *c_ == 3)) || (*c == '.' && *c_ == 3))
        *c_ = '#';
      else
        *c_ = '.';
    }
  }
}

void Field::step() {
  countNeighbours();
  updateStates();
  {
    lock_guard<mutex> lk(m);
    cur_field = !cur_field;
  }
}

Field::operator string() const {
  auto& f = f_[cur_field];
  string s;
  s.reserve(H * (W + 1));
  size_t i = 0;
  for (const auto& l : f) {
    s.insert(i, l.data(), W);
    i += W;
    s.insert(i++, "\n");
  }
  assert(i == s.size());
  return s;
}

const Field::FIELD_T& Field::field() const {
  lock_guard<mutex> lk(m);
  return f_[cur_field];
}

ostream& operator<<(ostream& os, const Field& f) { return os << string(f); }
