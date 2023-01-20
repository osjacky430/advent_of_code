#include "split_helper.hpp"
#include <array>
#include <cstddef>
#include <ctre.hpp>
#include <fmt/format.h>
#include <fstream>
#include <limits>
#include <map>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/transform.hpp>
#include <string>
#include <string_view>
#include <vector>

using DistMap       = std::vector<std::vector<int>>;
using StateBestFlow = std::map<std::size_t, std::size_t>;

struct Valve {
  std::string name_;
  std::vector<std::size_t> neighbor_{};
  int flow_rate_ = 0;

  bool opened_ = false;

  Valve(std::string t_name, int const t_flow) : name_{std::move(t_name)}, flow_rate_(t_flow) {}

  int get_flow_rate() const noexcept { return this->flow_rate_; }
};

void add_to_map(std::vector<Valve>& t_map, std::string const& t_str, std::map<std::string, std::size_t>& t_str_to_idx) {
  auto valve = ctre::match<"Valve ([A-Z]{2})[a-z ]+=([0-9]+);[a-z ]+(.*)">(t_str);

  auto const name         = valve.get<1>().to_string();
  auto const flow_rate    = std::stoi(valve.get<2>().to_string());
  auto const neighbor_str = split_string(valve.get<3>().to_string(), ' ');

  if (not t_str_to_idx.contains(name)) {
    t_str_to_idx.insert({name, t_str_to_idx.size()});
  }

  auto& new_valve = t_map.emplace_back(name, flow_rate);
  for (auto neighbor : neighbor_str) {
    auto n = neighbor.substr(0, 2);
    if (not t_str_to_idx.contains(n)) {
      t_str_to_idx.insert({n, t_str_to_idx.size()});
    }

    new_valve.neighbor_.push_back(t_str_to_idx.at(n));
  }
}

auto generate_shortest_path_matrix(std::vector<Valve> const& t_map,
                                   std::map<std::string, std::size_t> const& t_idx_map) {
  using ranges::for_each;
  // (max / 4) since we will calculate addition of element in dist_mat (not 3 because modulus != 0)
  DistMap dist_mat(t_map.size(), std::vector(t_map.size(), std::numeric_limits<int>::max() / 4));

  for_each(t_map, [&](auto&& t_valve) {
    for (auto&& neighbor : t_valve.neighbor_) {
      dist_mat[t_idx_map.at(t_valve.name_)][neighbor] = 1;
    }
  });

  for (std::size_t j = 0; j < t_map.size(); ++j) {
    for (std::size_t i = 0; i < t_map.size(); ++i) {
      auto const idx = t_idx_map.at(t_map[i].name_);
      for (std::size_t k = 0; k < t_map.size(); ++k) {
        if (dist_mat[idx][j] + dist_mat[j][k] < dist_mat[idx][k]) {
          dist_mat[idx][k] = dist_mat[idx][j] + dist_mat[j][k];
        }
      }
    }
  }

  for (std::size_t i = 0; i < t_map.size(); ++i) {
    dist_mat[i][i] = 0;
  }

  return dist_mat;
}

void traveling_salesman_problem(Valve const& t_start, std::map<std::string, std::size_t> const& t_idx_map,
                                std::vector<Valve> const& t_valves, DistMap const& t_graph, int const t_time_left,
                                std::size_t const t_valve_state, std::size_t const t_flow,
                                StateBestFlow& t_valve_state_best_output) {
  using ranges::views::filter;
  t_valve_state_best_output[t_valve_state] = std::max(t_valve_state_best_output[t_valve_state], t_flow);

  for (auto&& valve : t_valves | filter([](auto&& t_f) { return t_f > 0; }, &Valve::get_flow_rate)) {
    auto const time_left = t_time_left - t_graph[t_idx_map.at(t_start.name_)][t_idx_map.at(valve.name_)] - 1;
    auto const pos       = 1UL << t_idx_map.at(valve.name_);
    if ((t_valve_state & pos) or  // this path is meaningless (opened already), proceed to next one
        time_left <= 0) {         // this path is impossible (timeout), proceed to next one
      continue;
    }

    traveling_salesman_problem(valve, t_idx_map, t_valves, t_graph, time_left, t_valve_state | pos,
                               t_flow + static_cast<std::size_t>(time_left * valve.get_flow_rate()),
                               t_valve_state_best_output);
  }
}

void part1() {
  using ranges::getlines, ranges::max, ranges::views::transform;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);

  std::vector<Valve> map;
  std::map<std::string, std::size_t> idx_map;

  for (auto&& str : rng) {
    add_to_map(map, str, idx_map);
  }

  StateBestFlow answer;
  auto const dist_map = generate_shortest_path_matrix(map, idx_map);
  traveling_salesman_problem(map[idx_map.at("AA")], idx_map, map, dist_map, 30, 0, 0, answer);

  fmt::print("best result: {}\n", max(answer | transform([](auto&& t_pair) { return t_pair.second; })));
}

void part2() {
  using ranges::getlines;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
