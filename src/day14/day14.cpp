#include "pairview.hpp"
#include "split_helper.hpp"

#include <iostream>

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <range/v3/algorithm/minmax.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/split_when.hpp>
#include <range/v3/view/transform.hpp>
#include <string>

using Coor = std::pair<int, int>;
using namespace std::string_view_literals;

// this looks like binary tree traverse thingy to me -> recursive is the way
bool drop_sand_part1(std::vector<std::string>& t_map, Coor const& t_start, std::size_t& t_sand_count) {
  auto const [sand_start_x, sand_start_y] = t_start;

  auto sand_x_idx = static_cast<std::size_t>(sand_start_x);
  auto sand_y_idx = static_cast<std::size_t>(sand_start_y);
  if (sand_start_x == 0 or sand_x_idx == t_map[sand_y_idx].size() - 1) {
    return false;
  }

  while (t_map[sand_y_idx][sand_x_idx] == '.') {
    if (++sand_y_idx >= t_map.size()) {
      return false;
    }
  }

  if (t_map[sand_y_idx][sand_x_idx - 1U] == '.') {
    return drop_sand_part1(t_map, Coor{sand_start_x - 1, static_cast<int>(sand_y_idx)}, t_sand_count);
  }

  if (t_map[sand_y_idx][sand_x_idx + 1U] == '.') {
    return drop_sand_part1(t_map, Coor{sand_start_x + 1, static_cast<int>(sand_y_idx)}, t_sand_count);
  }

  t_map[sand_y_idx - 1U][sand_x_idx] = 'o';
  ++t_sand_count;
  return true;
}

auto scan_to_map(std::vector<std::vector<Coor>>& t_scans) {
  using ranges::views::join, ranges::minmax;

  auto const [min_x, max_x] = minmax(t_scans | join, std::less<int>{}, [](auto&& t_coor) { return t_coor.first; });
  auto const [min_y, max_y] = minmax(t_scans | join, std::less<int>{}, [](auto&& t_coor) { return t_coor.second; });

  auto const width  = static_cast<std::size_t>(max_x.first - min_x.first + 1);
  auto const length = static_cast<std::size_t>(max_y.second + 1);

  std::vector<std::string> map(length, std::string(width, '.'));

  for (auto&& scan : t_scans) {
    for (auto&& [first, second] : range_helper::pairview(scan)) {  // need const version of pairview
      auto const [x_start, x_end] = minmax(first->first, second->first);
      for (int x_curr = x_start; x_curr <= x_end; ++x_curr) {
        auto const y_idx  = static_cast<std::size_t>(first->second);
        auto const x_idx  = static_cast<std::size_t>(x_curr - min_x.first);
        map[y_idx][x_idx] = '#';
      }

      auto const [y_start, y_end] = minmax(first->second, second->second);
      for (int y_curr = y_start; y_curr <= y_end; ++y_curr) {
        auto const y_idx  = static_cast<std::size_t>(y_curr);
        auto const x_idx  = static_cast<std::size_t>(first->first - min_x.first);
        map[y_idx][x_idx] = '#';
      }
    }
  }

  return std::pair{map, 500 - min_x.first};
}

auto to_coor_vec(std::string const& t_str) {
  using ranges::to_vector, ranges::views::transform, ranges::views::filter;

  auto const str_to_vec = [](auto&& t_num_str) {
    auto number_split = split_string(std::string(t_num_str), ',');
    return Coor{std::stoi(std::string(number_split[0])), std::stoi(std::string(number_split[1]))};
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

  std::size_t result = 0;
  while (drop_sand_part1(map, Coor{sand_x_idx, 0}, result)) {
  }

  map[0][static_cast<std::size_t>(sand_x_idx)] = '+';

  fmt::print("Units of sand: {}\n", result);
}

bool drop_sand_part2(std::vector<std::string>& t_map, Coor& t_start, Coor const& t_curr_drop_pt,
                     std::size_t& t_sand_count) {
  auto const [sand_drop_x, sand_drop_y] = t_curr_drop_pt;

  auto sand_x_idx = static_cast<std::size_t>(sand_drop_x);
  auto sand_y_idx = static_cast<std::size_t>(sand_drop_y);

  while (t_map[sand_y_idx][sand_x_idx] == '.') {
    ++sand_y_idx;
  }

  if (sand_y_idx == 0) {
    return false;
  }

  if (sand_x_idx == 0U) {
    ++t_start.first;
    ++sand_x_idx;
    for (auto& row : t_map) {
      row = '.' + row;
    }

    t_map.back().front() = '#';
  } else if (sand_x_idx + 1U == t_map[sand_y_idx].size()) {
    for (auto& row : t_map) {
      row += '.';
    }

    t_map.back().back() = '#';
  }

  if (t_map[sand_y_idx][sand_x_idx - 1U] == '.') {
    return drop_sand_part2(t_map, t_start, Coor{static_cast<int>(sand_x_idx) - 1, static_cast<int>(sand_y_idx)},
                           t_sand_count);
  }

  if (t_map[sand_y_idx][sand_x_idx + 1U] == '.') {
    return drop_sand_part2(t_map, t_start, Coor{static_cast<int>(sand_x_idx) + 1, static_cast<int>(sand_y_idx)},
                           t_sand_count);
  }

  t_map[sand_y_idx - 1U][sand_x_idx] = 'o';
  ++t_sand_count;
  return true;
}

void part2() {
  using ranges::getlines, ranges::views::transform, ranges::to_vector;

  std::fstream in((INPUT_FILE));
  auto rng               = getlines(in) | transform(to_coor_vec) | to_vector;
  auto [map, sand_x_idx] = scan_to_map(rng);

  // add floor
  map.push_back(std::string(map.front().size(), '.'));
  map.push_back(std::string(map.front().size(), '#'));

  auto start         = Coor{static_cast<std::size_t>(sand_x_idx), 0};
  std::size_t result = 0;

  while (drop_sand_part2(map, start, start, result)) {
  }

  map[0][static_cast<std::size_t>(start.first)] = '+';
  fmt::print("units of sand: {}\n", result);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}