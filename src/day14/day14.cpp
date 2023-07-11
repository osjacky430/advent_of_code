#include "pairview.hpp"
#include "string_util.hpp"
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <optional>
#include <range/v3/algorithm/minmax.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>
#include <string>

using Coor = std::pair<int, int>;
using namespace std::string_view_literals;

static constexpr auto CAVE_POURING_POINT_X = 500;

// this looks like binary tree traverse thingy to me -> recursive is the way
bool drop_sand(std::vector<std::string>& t_map, Coor const& t_start, std::size_t& t_sand_count, auto&& t_move_y,
               auto&& t_move_left, auto&& t_move_right) {
  auto const [sand_start_x, sand_start_y] = t_start;

  auto sand_x_idx = static_cast<std::size_t>(sand_start_x);
  auto sand_y_idx = static_cast<std::size_t>(sand_start_y);

  auto const sand_y_pos = t_move_y(t_map, sand_x_idx, sand_y_idx);
  if (not sand_y_pos) {
    return false;
  }

  sand_y_idx = *sand_y_pos;
  if (t_move_left(t_map, sand_x_idx, sand_y_idx)) {
    return drop_sand(t_map, Coor{static_cast<int>(sand_x_idx) - 1, static_cast<int>(sand_y_idx)}, t_sand_count,
                     t_move_y, t_move_left, t_move_right);
  }

  if (t_move_right(t_map, sand_x_idx, sand_y_idx)) {
    return drop_sand(t_map, Coor{static_cast<int>(sand_x_idx) + 1, static_cast<int>(sand_y_idx)}, t_sand_count,
                     t_move_y, t_move_left, t_move_right);
  }

  t_map[sand_y_idx - 1U][sand_x_idx] = 'o';
  ++t_sand_count;
  return true;
}

auto scan_to_map(std::vector<std::vector<Coor>> const& t_scans) {
  using ranges::views::join, ranges::minmax;

  auto const [min_x, max_x] = minmax(t_scans | join, std::less<int>{}, [](auto&& t_coor) { return t_coor.first; });
  auto const [min_y, max_y] = minmax(t_scans | join, std::less<int>{}, [](auto&& t_coor) { return t_coor.second; });

  auto const width  = static_cast<std::size_t>(max_x.first) - static_cast<std::size_t>(min_x.first) + 1;
  auto const length = static_cast<std::size_t>(max_y.second) + 1;

  std::vector<std::string> map(length, std::string(width, '.'));

  for (auto&& scan : t_scans) {
    for (auto&& [first, second] : ranges::views::pairview(scan)) {  // need const version of pairview
      auto const [x_start, x_end] = minmax(first.first, second.first);
      for (int x_curr = x_start; x_curr <= x_end; ++x_curr) {
        auto const y_idx  = static_cast<std::size_t>(first.second);
        auto const x_idx  = static_cast<std::size_t>(x_curr - min_x.first);
        map[y_idx][x_idx] = '#';
      }

      auto const [y_start, y_end] = minmax(first.second, second.second);
      for (int y_curr = y_start; y_curr <= y_end; ++y_curr) {
        auto const y_idx  = static_cast<std::size_t>(y_curr);
        auto const x_idx  = static_cast<std::size_t>(first.first - min_x.first);
        map[y_idx][x_idx] = '#';
      }
    }
  }

  return std::pair{map, CAVE_POURING_POINT_X - min_x.first};
}

auto to_coor_vec(std::string const& t_str) {
  using ranges::to_vector, ranges::views::transform, ranges::views::filter;

  auto const str_to_vec = [](auto&& t_num_str) {
    auto number_split = split_string(t_num_str, ',');
    return Coor{std::stoi(number_split[0]), std::stoi(number_split[1])};
  };

  auto const splitted = split_string(t_str);                                   //
  auto ret_val        = splitted                                               //
                 | filter([](auto&& t_filter) { return t_filter != "->"sv; })  //
                 | transform(str_to_vec)                                       //
                 | to_vector;
  return ret_val;
};

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::minmax, ranges::views::transform;

  std::fstream in((INPUT_FILE));
  auto rng               = getlines(in) | transform(to_coor_vec) | to_vector;
  auto [map, sand_x_idx] = scan_to_map(rng);

  constexpr auto move_y = [](std::vector<std::string> const& t_map, std::size_t t_x,
                             std::size_t t_y) -> std::optional<std::size_t> {
    if (t_x == 0 or t_x == t_map[t_y].size() - 1) {
      return std::nullopt;
    }

    while (t_map[t_y][t_x] == '.') {
      if (++t_y >= t_map.size()) {
        return std::nullopt;
      }
    }

    return t_y;
  };

  constexpr auto move_left = [](std::vector<std::string> const& t_map, std::size_t t_x, std::size_t t_y) {
    return t_map[t_y][t_x - 1U] == '.';
  };

  constexpr auto move_right = [](std::vector<std::string> const& t_map, std::size_t t_x, std::size_t t_y) {
    return t_map[t_y][t_x + 1] == '.';
  };

  std::size_t result = 0;
  while (drop_sand(map, Coor{sand_x_idx, 0}, result, move_y, move_left, move_right)) {
  }

  map[0][static_cast<std::size_t>(sand_x_idx)] = '+';
  fmt::println("Units of sand: {}", result);
}

void part2() {
  using ranges::getlines, ranges::views::transform, ranges::to_vector;

  std::fstream in((INPUT_FILE));
  auto rng               = getlines(in) | transform(to_coor_vec) | to_vector;
  auto [map, sand_x_idx] = scan_to_map(rng);

  auto start = Coor{sand_x_idx, 0};

  constexpr auto move_y = [](std::vector<std::string> const& t_map, std::size_t t_x, std::size_t t_y) {
    while (t_map[t_y][t_x] == '.') {
      ++t_y;
    }

    return t_y != 0 ? std::optional{t_y} : std::nullopt;
  };

  auto move_left = [&start](std::vector<std::string>& t_map, std::size_t& t_x, std::size_t t_y) {
    if (t_x == 0) {
      ++start.first;
      ++t_x;

      for (auto& row : t_map) {
        row.insert(0, "."sv);
      }

      t_map.back().front() = '#';
    }

    return t_map[t_y][t_x - 1] == '.';
  };

  constexpr auto move_right = [](std::vector<std::string>& t_map, std::size_t t_x, std::size_t t_y) {
    if (t_x + 1 == t_map[t_y].size()) {
      for (auto& row : t_map) {
        row += '.';
      }

      t_map.back().back() = '#';
    }

    return t_map[t_y][t_x + 1] == '.';
  };

  // add floor
  map.emplace_back(map.front().size(), '.');
  map.emplace_back(map.front().size(), '#');

  std::size_t result = 0;

  while (drop_sand(map, start, result, move_y, move_left, move_right)) {
  }

  map[0][static_cast<std::size_t>(start.first)] = '+';
  fmt::println("units of sand: {}", result);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}