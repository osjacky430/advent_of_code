#include <array>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <queue>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/indices.hpp>
#include <set>
#include <vector>

using Coor        = std::pair<int, int>;
using CoorVisited = std::pair<Coor, int>;

auto get_neighbor(Coor const& t_coor) {
  return std::array{Coor{t_coor.first + 1, t_coor.second}, Coor{t_coor.first - 1, t_coor.second},
                    Coor{t_coor.first, t_coor.second + 1}, Coor{t_coor.first, t_coor.second - 1}};
}

auto default_search_predicate(char const t_neighbor_height, char const t_current_height) {
  return static_cast<int>(t_neighbor_height - t_current_height) <= 1;
}

auto bfs(std::vector<std::string> const& t_map, std::vector<Coor> const& t_start, auto&& t_end_predicate,
         bool (*t_search_predicate)(char, char) = default_search_predicate) {
  // Use std::set<Coor>, where Coor contains pair of int and steps, instead of std::map<Coor, std::size_t>. we don't
  // need to do check if steps to neighbor coordinate is less than previous visitation because in bfs, coor visited
  // previously definitely has less steps than the same one visted later

  std::set<Coor> searched;
  std::queue<CoorVisited> search_list;
  for (auto&& start : t_start) {
    searched.insert(search_list.emplace(start, 0).first);
  }

  while (t_end_predicate(search_list.front().first)) {
    auto const coor_to_visit   = search_list.front();
    auto const [current, step] = coor_to_visit;

    search_list.pop();
    auto height = t_map[static_cast<std::size_t>(current.first)][static_cast<std::size_t>(current.second)];

    for (auto&& neighbor : get_neighbor(current)) {
      auto const [neighbor_x, neighbor_y] = neighbor;

      auto const out_bound = neighbor_x < 0 or neighbor_x >= static_cast<int>(t_map.size()) or  //
                             neighbor_y < 0 or
                             neighbor_y >= static_cast<int>(t_map[static_cast<std::size_t>(neighbor_x)].size());
      auto const visited = searched.contains(neighbor);
      if (out_bound or visited) {
        continue;
      }

      auto const x = static_cast<std::size_t>(neighbor_x);
      auto const y = static_cast<std::size_t>(neighbor_y);
      if (t_search_predicate(t_map[x][y], height)) {
        auto&& coor = search_list.emplace(std::pair{neighbor_x, neighbor_y}, step + 1);
        searched.insert(coor.first);
      }
    }
  }

  return search_list.front().second;
}

auto parse_map(std::fstream& t_in, bool (*t_start_predicate)(char&), bool (*t_end_predicate)(char&)) {
  using ranges::getlines, ranges::to_vector, ranges::views::cartesian_product, ranges::views::indices;

  auto map = getlines(t_in) | to_vector;
  std::vector<Coor> start;
  Coor end;
  for (auto [i, j] : cartesian_product(indices(map.size()), indices(map[0].size()))) {
    if (t_start_predicate(map[i][j])) {
      start.emplace_back(static_cast<int>(i), static_cast<int>(j));
    } else if (t_end_predicate(map[i][j])) {
      end = std::pair{static_cast<int>(i), static_cast<int>(j)};
    }
  }

  return std::tuple{map, start, end};
}

void part1() {
  std::fstream in((INPUT_FILE));

  // just for fun, only do this in self learning project
  constexpr auto start_predicate = [](char& t_c) { return t_c == 'S' ? (t_c = 'a', true) : false; };
  constexpr auto end_predicate   = [](char& t_c) { return t_c == 'E' ? (t_c = 'z', true) : false; };

  auto const [map, start, end]{parse_map(in, start_predicate, end_predicate)};

  fmt::println("min cost: {}", bfs(map, start, [end](Coor const& t_to_search) { return end != t_to_search; }));
}

void part2() {
  // 2 ways to do it:
  //
  //  1. find 'forward', i.e. find minimum steps for all 'a'
  //  2. find 'backward', i.e., BFS starting from 'E' and search for first encountered 'a'
  std::fstream in((INPUT_FILE));

  // just for fun, only do this in self learning project
  constexpr auto start_predicate = [](char& t_c) { return t_c == 'S' ? (t_c = 'a', true) : t_c == 'a'; };
  constexpr auto end_predicate   = [](char& t_c) { return t_c == 'E' ? (t_c = 'z', true) : false; };

  auto const [map, start, end]{parse_map(in, start_predicate, end_predicate)};

  fmt::println("min cost: {}", bfs(map, start, [end](Coor const& t_to_search) { return end != t_to_search; }));
}

void part2_backward_find() {
  std::fstream in((INPUT_FILE));

  // just for fun, only do this in self learning project
  constexpr auto start_predicate = [](char& t_c) { return t_c == 'E' ? (t_c = 'z', true) : false; };

  // end is not meaningful as we are finding first appeared 'a', we don't need to know how many 'a' there are
  constexpr auto end_predicate = [](char& t_c) { return t_c == 'S' ? (t_c = 'a', false) : false; };

  auto const [map, start, _]{parse_map(in, start_predicate, end_predicate)};

  auto const continue_search_predicate = [map](Coor const& t_coor) {
    auto const [x, y] = t_coor;
    return map[static_cast<std::size_t>(x)][static_cast<std::size_t>(y)] != 'a';
  };

  constexpr auto valid_neighbor_predicate = [](char const t_neighbor_height, char const t_current_height) {
    return t_current_height - t_neighbor_height <= 1;
  };

  fmt::println("backward find algo, min cost: {}",
               bfs(map, start, continue_search_predicate, valid_neighbor_predicate));
}

int main(int /**/, char** /**/) {
  part1();
  part2();
  part2_backward_find();

  return EXIT_SUCCESS;
}