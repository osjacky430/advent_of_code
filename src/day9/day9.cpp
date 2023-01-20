#include "pairview.hpp"
#include "split_helper.hpp"

#include <array>
#include <cmath>
#include <cstdlib>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/view/zip.hpp>
#include <set>

#include <range/v3/algorithm/minmax.hpp>
#include <range/v3/view/join.hpp>

using Coor = std::pair<int, int>;

template <std::size_t KnotNum>
void print_knot(std::array<std::vector<Coor>, KnotNum> const& t_footprint) {
  using ranges::minmax, ranges::views::join;

  auto const [min_x, max_x] = minmax(t_footprint | join, std::less<int>{}, [](auto&& t_coor) { return t_coor.first; });
  auto const [min_y, max_y] = minmax(t_footprint | join, std::less<int>{}, [](auto&& t_coor) { return t_coor.second; });

  auto const width  = static_cast<std::size_t>(max_x.first - min_x.first + 1);
  auto const length = static_cast<std::size_t>(max_y.second - min_y.second + 1);

  for (std::size_t i = 0; i < t_footprint.front().size(); ++i) {
    std::vector<std::string> map(length, std::string(width, '.'));
    for (std::size_t j = 0; j < KnotNum; ++j) {
      auto const [x, y] = t_footprint[j][i];
      auto const y_idx  = static_cast<std::size_t>(y - min_y.second);
      auto const x_idx  = static_cast<std::size_t>(x - min_x.first);
      map[y_idx][x_idx] = static_cast<char>(j + static_cast<int>('0'));
    }

    for (auto&& row : map) {
      fmt::print("{}\n", row);
    }
    fmt::print("\n");
  }
}

void move(char const t_dir, auto&& t_ropes) {
  using ranges::views::zip, ranges::span, ranges::distance;

  auto& [head, tail] = t_ropes[0];
  switch (t_dir) {
    case 'R':
      ++head;
      break;
    case 'U':
      --tail;
      break;
    case 'L':
      --head;
      break;
    case 'D':
      ++tail;
      break;
  }

  auto&& rng = range_helper::pairview(t_ropes);
  // auto&& rng = zip(span<Coor>(t_ropes.begin(), t_ropes.end() - 1), span<Coor>(t_ropes.begin() + 1, t_ropes.end()));
  for (auto iter = begin(rng); iter != end(rng); ++iter) {
    auto&& [first, second] = *iter;
    auto& [x_head, y_head] = *first;
    auto& [x_tail, y_tail] = *second;

    auto const x_diff = x_head - x_tail;
    auto const y_diff = y_head - y_tail;
    if (std::abs(x_diff) > 1) {
      if (std::abs(y_diff) != 0) {
        y_tail += static_cast<int>(std::copysign(1, y_diff));
      }

      x_tail += static_cast<int>(std::copysign(1, x_diff));
    } else if (std::abs(y_diff) > 1) {
      if (std::abs(x_diff) != 0) {
        x_tail += static_cast<int>(std::copysign(1, x_diff));
      }

      y_tail += static_cast<int>(std::copysign(1, y_diff));
    }
  }
}

void part1() {
  using ranges::getlines;

  std::ifstream in((INPUT_FILE));

  auto rng = getlines(in);

  std::array<Coor, 2> rope;
  std::set<Coor> visited;
  visited.emplace(0, 0);

  for (auto&& instruction : rng) {
    auto const decomposed = split_string(instruction);
    auto const direction  = decomposed[0][0];
    auto const amount     = std::stoi(std::string(decomposed[1]));
    for (int i = 0; i < amount; ++i) {
      move(direction, rope);
      visited.emplace(rope.back());
    }
  }

  fmt::print("position visited: {}\n", visited.size());
}

void part2() {
  using ranges::getlines;

  std::ifstream in((INPUT_FILE));

  auto rng = getlines(in);

  std::array<Coor, 10> rope;
  std::array<std::vector<Coor>, 10> footprint;
  std::set<Coor> visited;
  visited.emplace(0, 0);

  for (auto&& instruction : rng) {
    auto const cmd       = split_string(instruction);
    auto const direction = cmd[0][0];
    auto const amount    = std::stoi(cmd[1]);

    for (int i = 0; i < amount; ++i) {
      move(direction, rope);
      for (std::size_t j = 0; j < rope.size(); ++j) {
        footprint[j].push_back(rope[j]);
      }
      visited.emplace(rope.back());
    }
  }

  // print_knot(footprint);
  fmt::print("position visited: {}\n", visited.size());
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}