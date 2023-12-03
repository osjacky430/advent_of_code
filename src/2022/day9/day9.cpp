#include "pairview.hpp"
#include "string_util.hpp"

#include <array>
#include <cmath>
#include <cstdlib>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <range/v3/action/action.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/algorithm/minmax.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/take_last.hpp>
#include <set>
#include <tuple>
#include <type_traits>
#include <utility>

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
      fmt::println("{}", row);
    }
    fmt::println("");
  }
}

namespace {

template <typename T>
concept structure_bindable_pair = requires(T t_v) {
  std::get<0>(t_v);
  std::get<1>(t_v);

  requires std::tuple_size_v<T> == 2;
};

struct RopeMoverFn {
  constexpr auto operator()(char const t_dir) const {
    return ranges::make_action_closure(ranges::bind_back(RopeMoverFn{}, t_dir));
  }

  template <typename Rng>
    requires(structure_bindable_pair<typename std::remove_const_t<std::remove_reference_t<Rng>>::value_type>)
  Rng operator()(Rng&& t_rng, char const t_dir) const {
    auto& [head_x, head_y] = *std::begin(t_rng);
    switch (t_dir) {
      case 'R':
        ++head_x;
        break;
      case 'U':
        --head_y;
        break;
      case 'L':
        --head_x;
        break;
      case 'D':
        ++head_y;
        break;
      default:
        std::unreachable();
    }

    for (auto&& [first, second] : ranges::views::pairview(t_rng)) {
      auto& [x_head, y_head] = first;
      auto& [x_tail, y_tail] = second;

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

    return std::forward<Rng>(t_rng);
  }
};

inline constexpr RopeMoverFn move_rope{};

}  // namespace

void move(char const t_dir, auto&& t_ropes) {
  using ranges::for_each;

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
    default:
      std::unreachable();
  }

  for (auto&& [first, second] : ranges::views::pairview(t_ropes)) {
    auto& [x_head, y_head] = first;
    auto& [x_tail, y_tail] = second;

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
    auto const cmd       = split_string(instruction);
    auto const direction = cmd[0][0];
    auto const amount    = std::stoi(cmd[1]);

    for (int i = 0; i < amount; ++i) {
      ranges::actions::insert(visited, (rope |= ::move_rope(direction)) | ranges::views::take_last(1));
    }
  }

  fmt::println("position visited: {}", visited.size());
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
      ranges::actions::insert(visited, (rope |= ::move_rope(direction)) | ranges::views::take_last(1));
      // for (std::size_t j = 0; j < rope.size(); ++j) {
      //   footprint[j].push_back(rope[j]);
      // }
    }
  }

  fmt::println("position visited: {}", visited.size());
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}