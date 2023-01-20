#include <algorithm>
#include <ctre.hpp>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <map>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/transform.hpp>
#include <utility>

using Coor = std::pair<int, int>;

inline constexpr auto ROW_TO_CHECK = 2000000;

auto manhattan(Coor const& t_first, Coor const& t_second) {
  return std::abs(t_first.first - t_second.first) + std::abs(t_first.second - t_second.second);
}

class Sensor {
  Coor position_;
  Coor closest_beacon_;

  int manhattan_dist_ = manhattan(position_, closest_beacon_);

  int no_beacon_x_max_cache_ = position_.first + manhattan_dist_;
  int no_beacon_x_min_cache_ = position_.first - manhattan_dist_;

  int no_beacon_y_max_ = position_.second + manhattan_dist_;
  int no_beacon_y_min_ = position_.second - manhattan_dist_;

 public:
  Sensor(Coor const& t_pos, Coor const& t_beacon) : position_{t_pos}, closest_beacon_{t_beacon} {}

  [[nodiscard]] bool can_pose_constraint(int const t_row) const noexcept {
    return this->no_beacon_y_min_ <= t_row and t_row <= this->no_beacon_y_max_;
  }

  [[nodiscard]] Coor get_no_beacon_x_range(int const t_row) const noexcept {
    auto const y_diff = std::abs(this->position_.second - t_row);
    if (t_row == this->closest_beacon_.second) {
      if (this->closest_beacon_.first == this->no_beacon_x_min_cache_ + y_diff) {
        return {this->no_beacon_x_min_cache_ + y_diff + 1, this->no_beacon_x_max_cache_ - y_diff};
      }

      if (this->closest_beacon_.first == this->no_beacon_x_max_cache_ - y_diff) {
        return {this->no_beacon_x_min_cache_ + y_diff, this->no_beacon_x_max_cache_ - y_diff - 1};
      }
    }

    return {this->no_beacon_x_min_cache_ + y_diff, this->no_beacon_x_max_cache_ - y_diff};
  }

  Coor get_closest_beacon() const noexcept { return this->closest_beacon_; }

  [[nodiscard]] auto get_edge() const noexcept {
    auto const amount = static_cast<std::size_t>(this->manhattan_dist_ * 4);
    std::vector<Coor> ret_val;
    ret_val.reserve(amount);
    ret_val.emplace_back(this->position_.first, this->no_beacon_y_min_ - 1);
    ret_val.emplace_back(this->position_.first, this->no_beacon_y_max_ + 1);
    for (int i = 1; i <= this->manhattan_dist_; ++i) {
      ret_val.emplace_back(this->position_.first - i, this->no_beacon_y_min_ - 1 + i);
      ret_val.emplace_back(this->position_.first + i, this->no_beacon_y_min_ - 1 + i);
    }

    for (int i = 1; i <= this->manhattan_dist_ - 1; ++i) {
      ret_val.emplace_back(this->position_.first - i, this->no_beacon_y_max_ + 1 - i);
      ret_val.emplace_back(this->position_.first + i, this->no_beacon_y_max_ + 1 - i);
    }

    return ret_val;
  }

  [[nodiscard]] bool is_in_range(Coor const& t_coor) const noexcept {
    return this->manhattan_dist_ >= manhattan(t_coor, this->position_);
  }

  [[nodiscard]] bool operator==(Sensor const&) const = default;
};

auto str_to_sensor(std::string const& t_str) {
  auto coors = ctre::match<"[a-zA-Z ]+x=(-?[0-9]+), y=(-?[0-9]+):[a-zA-Z ]+x=(-?[0-9]+), y=(-?[0-9]+)">(t_str);
  return Sensor{{std::stoi(coors.get<1>().to_string()), std::stoi(coors.get<2>().to_string())},
                {std::stoi(coors.get<3>().to_string()), std::stoi(coors.get<4>().to_string())}};
}

auto get_merged_ranges(std::vector<Sensor> const& t_sensors, int const t_row) {
  using ranges::views::transform, ranges::to_vector, ranges::views::filter, ranges::sort;

  auto x_ranges = t_sensors                                                                            //
                  | filter([=](auto&& t_sensor) { return t_sensor.can_pose_constraint(t_row); })       //
                  | transform([=](auto&& t_sensor) { return t_sensor.get_no_beacon_x_range(t_row); })  //
                  | to_vector;
  sort(x_ranges);

  std::vector<Coor> reduced_x_ranges{x_ranges.front()};
  for (std::size_t i = 1; i < x_ranges.size(); ++i) {
    auto const [range_x_min, range_x_max] = x_ranges[i];
    auto& [curr_x_min, curr_x_max]        = reduced_x_ranges.back();
    if (range_x_min > curr_x_max) {
      reduced_x_ranges.push_back(x_ranges[i]);
    } else {
      curr_x_max = std::max(range_x_max, curr_x_max);
    }
  }

  return reduced_x_ranges;
}

void part1() {
  using ranges::getlines, ranges::views::transform, ranges::to_vector, ranges::accumulate;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in) | transform(str_to_sensor) | to_vector;

  auto const reduced_x_ranges = get_merged_ranges(rng, ROW_TO_CHECK);
  int result                  = accumulate(reduced_x_ranges, 0, std::plus<>{}, [&](auto&& t_rng) {
    auto&& [min_x, max_x] = t_rng;
    return max_x - min_x + 1;
  });

  fmt::print("number of positions where a beacon cannot be present: {}\n", result);
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::views::transform, ranges::all_of, ranges::views::filter;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in) | transform(str_to_sensor) | to_vector;

  double result = [&]() {
    for (auto&& sensor : rng) {
      auto const edges = sensor.get_edge();
      for (auto&& edge : edges) {
        if (all_of(rng | filter([&](auto&& t_sensor) { return t_sensor != sensor; }),
                   [&](auto&& t_sensor) { return not t_sensor.is_in_range(edge); })) {
          fmt::print("{}, {}\n", edge.first, edge.second);
          return static_cast<double>(edge.first) * 4000000 + static_cast<double>(edge.second);
        }
      }
    }

    return 0.0;  // not reachable
  }();

  fmt::print("distress freq: {}\n", result);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
