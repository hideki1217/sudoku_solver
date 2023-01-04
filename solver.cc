#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <vector>

template <int N>
class Solver {
  static constexpr int L = N * N;
  const int nan = -1;

  bool check_x(int y) {
    std::array<bool, L + 1> mem;
    std::fill(mem.begin(), mem.end(), false);
    for (int j = 0; j < L; j++) {
      auto v = table(y, j);
      if (isnan(v)) continue;
      if (mem[v]) return false;
      mem[v] = true;
    }
    return true;
  }
  bool check_y(int x) {
    std::array<bool, L + 1> mem;
    std::fill(mem.begin(), mem.end(), false);
    for (int j = 0; j < L; j++) {
      auto v = table(j, x);
      if (isnan(v)) continue;
      if (mem[v]) return false;
      mem[v] = true;
    }
    return true;
  }
  bool check_box(int b) {
    std::array<bool, L + 1> mem;
    std::fill(mem.begin(), mem.end(), false);
    auto x0 = (b % N) * N, y0 = b / N;
    for (int j = 0; j < L; j++) {
      auto v = table(y0 + j / N, x0 + j % N);
      if (isnan(v)) continue;
      if (mem[v]) return false;
      mem[v] = true;
    }
    return true;
  }

 public:
  Solver() {
    _table.resize(L * L);
    std::fill(_table.begin(), _table.end(), nan);

    _mem.resize(L * L);
  }

  void clear_mem() {
    for (auto& m : _mem) {
      std::fill(m.begin(), m.end(), true);
    }
  }

  inline constexpr bool isnan(int x) { return x == nan; }

  bool check_global() {
    for (int x = 0; x < L; x++) {
      if (!check_y(x)) return false;
    }

    for (int y = 0; y < L; y++) {
      if (!check_x(y)) return false;
    }

    for (int b = 0; b < L; b++) {
      if (!check_box(b)) return false;
    }
    return true;
  }

  bool check_local(int x, int y) {
    if (!check_y(x)) return false;
    if (!check_x(y)) return false;
    if (!check_box(x % N + N * (y / N))) return false;
    return true;
  }

  bool is_complete() {
    for (int i = 0; i < L * L; i++) {
      if (isnan(_table[i])) return false;
    }
    return true;
  }

  void update_mem() {
    clear_mem();

    for (int i = 0; i < _table.size(); i++) {
      auto v = _table[i];
      if (isnan(v)) continue;
      // 固定されているところは候補なし
      std::fill(_mem[i].begin(), _mem[i].end(), false);

      auto x = i % L, y = i / L;
      auto i0 = (x / N) * N + L * ((y / N) * N);
      auto x0 = i0 % L, y0 = i0 / L;

      for (int j = 0; j < L; j++) {
        _mem[j * L + x][v] = false;
      }
      for (int j = 0; j < L; j++) {
        _mem[y * L + j][v] = false;
      }
      for (int j = 0; j < L; j++) {
        _mem[(y0 + j / N) * L + (x0 + j % N)][v] = false;
      }
    }
  }

  int search() {
    int count = 0;
    for (int i = 0; i < _table.size(); i++) {
      if (!isnan(_table[i])) continue;

      auto x = i % L, y = i / L;
      auto i0 = (x / N) * N + L * ((y / N) * N);
      auto x0 = i0 % L, y0 = i0 / L;
      for (int n = 1; n <= L; n++) {
        if (!_mem[i][n]) continue;

        // ここを丁寧にやれば解けるor答えが複数or解けない、がわかる.
        {  // 列方向
          bool exist_others = false;
          for (int j = 0; j < L; j++) {
            auto _i = j * L + x;
            if (_i == i) continue;
            exist_others |= _mem[_i][n];
          }
          if (!exist_others) {
            count++;
            _table[i] = n;
            break;
          }
        }
        {  // 行方向
          bool exist_others = false;
          for (int j = 0; j < L; j++) {
            auto _i = y * L + j;
            if (_i == i) continue;
            exist_others |= _mem[_i][n];
          }
          if (!exist_others) {
            count++;
            _table[i] = n;
            break;
          }
        }
        {  // ボックス内
          bool exist_others = false;
          for (int j = 0; j < L; j++) {
            auto _i = (y0 + j / N) * L + (x0 + j % N);
            if (_i == i) continue;
            exist_others |= _mem[_i][n];
          }
          if (!exist_others) {
            count++;
            _table[i] = n;
            break;
          }
        }
      }
    }

    return count;
  }

  int solve() {
    check_global();

    while (!is_complete()) {
      update_mem();

      auto res = search();
      if (res <= 0) return res;
    }
    return 1;
  }

  inline int& table(int y, int x) { return _table[y * L + x]; }
  std::vector<int> _table;
  std::vector<std::array<bool, L + 1>> _mem;
};

int main() {
  Solver<3> model;

  int n;
  std::cin >> n;
  for (int i = 0; i < n; i++) {
    int x, y, v;
    std::cin >> y >> x >> v;
    model.table(y, x) = v;
  }

  printf("%d\n", model.solve());

  for (int i = 0; i < 9; i++) {
    printf("%2d", model.table(i, 0));
    for (int j = 1; j < 9; j++) {
      printf(",%2d", model.table(i, j));
    }
    printf("\n");
  }
}
