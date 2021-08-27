#include "parser.hpp"

using namespace std;

vector<Entry> parseKaggleData(int n, const string& fn) {
  auto data = algs::io::readFileToList(fn);
  data.pop_front();
  vector<Entry> inp;
  size_t H = 25, W = 25;
  for (const auto& s : data) {
    if (s.starts_with("id")) continue;  // Header

    Entry e;
    string delimiter = ",";

    size_t i = 0;
    string token = s.substr(i, s.find(delimiter, i) - i);
    e.id = stoi(token);
    i += token.size() + delimiter.size();

    token = s.substr(i, s.find(delimiter, i) - i);
    e.delta = stoi(token);
    i += token.size() + delimiter.size();

    size_t offset = i;
    for (; i < s.size(); i += 2) {
      char token = s[i];
      assert(i + 1 == s.size() || s[i + 1] == ',');
      if (!((i - offset) % H)) {
        e.f.push_back({});
        e.f.back().resize(W);
      }
      e.f.back()[(i - offset) % H] = token == '1' ? '#' : '.';
    }
    inp.push_back(move(e));
    if (!(inp.size() % 500)) std::cout << '.' << flush;
    if (n > 0 && !--n) break;
  }
  return inp;
}
