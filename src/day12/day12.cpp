#include <array>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
// #include <map>
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

auto bfs(std::vector<std::string> const& t_map, std::vector<Coor> const& t_start, Coor const& t_end) {
  // Use std::set<Coor>, where Coor contains pair of int and steps, instead of std::map<Coor, std::size_t>. we don't
  // need to do check if steps to neighbor coordinate is less than previous visitation because in bfs, coor visited
  // previously definitely has less steps than the same one visted later
  // std::map<Coor, std::size_t> searched;

  std::set<Coor> searched;
  std::queue<CoorVisited> search_list;
  for (auto&& start : t_start) {
    searched.insert(search_list.emplace(start, 0).first);
  }

  while (search_list.front().first != t_end) {
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
      if (static_cast<int>(t_map[x][y]) - static_cast<int>(height) <= 1) {
        searched.insert(search_list.emplace(std::pair{neighbor_x, neighbor_y}, step + 1).first);
      }
    }
  }

  return search_list.front().second;
}

auto parse_map(std::fstream& t_in, auto&& t_start_predicate) {
  using ranges::getlines, ranges::to_vector, ranges::views::cartesian_product, ranges::views::indices;

  auto map = getlines(t_in) | to_vector;
  std::vector<Coor> start;
  Coor end;
  for (auto [i, j] : cartesian_product(indices(map.size()), indices(map[0].size()))) {
    if (t_start_predicate(map[i][j])) {
      start.emplace_back(static_cast<int>(i), static_cast<int>(j));
    } else if (map[i][j] == 'E') {
      end       = std::pair{static_cast<int>(i), static_cast<int>(j)};
      map[i][j] = 'z';
    }
  }

  return std::tuple{map, start, end};
}

void part1() {
  std::fstream in((INPUT_FILE));

  auto const [map, start, end]{parse_map(in, [](auto& t_c) {
    if (t_c == 'S') {
      t_c = 'a';
      return true;
    }

    return false;
  })};

  fmt::print("min cost: {}\n", bfs(map, start, end));
}

void part2() {
  // 2 ways to do it:
  //
  //  1. find 'forward', i.e. find minimum steps for all 'a'
  //  2. find 'backward', i.e., BFS starting from 'E' and search for first encountered 'a'
  using ranges::getlines;

  std::fstream in((INPUT_FILE));

  auto const [map, start, end]{parse_map(in, [](auto& t_c) {
    if (t_c == 'S') {
      t_c = 'a';
    }

    return t_c == 'a';
  })};

  fmt::print("min cost: {}\n", bfs(map, start, end));
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}