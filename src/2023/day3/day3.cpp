#include "string_util.hpp"
#include <fmt/format.h>
#include <fstream>
#include <map>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/indices.hpp>

struct Schemtaic {
  std::vector<std::string> schematic_;

  [[nodiscard]] int size() const noexcept { return static_cast<int>(this->schematic_.size()); }

  [[nodiscard]] std::string_view operator[](int t_y) const noexcept {
    return this->schematic_[static_cast<std::size_t>(t_y)];
  }

  [[nodiscard]] std::optional<char> operator[](int t_x, int t_y) const noexcept {
    int const size          = static_cast<int>(this->schematic_.size());
    bool const out_of_bound = t_x < 0 or t_x >= size or t_y < 0 or t_y >= size;
    if (out_of_bound) {
      return std::nullopt;
    }

    return this->schematic_[static_cast<std::size_t>(t_y)][static_cast<std::size_t>(t_x)];
  }
};

std::pair<int, int> parse_part_num(Schemtaic const& t_schematic, int const t_curr_x, int const t_curr_y) {
  using ranges::views::closed_indices, ranges::views::cartesian_product, ranges::find_if;

  int ret_val             = 0;
  auto const search_begin = t_schematic[t_curr_y].substr(static_cast<std::size_t>(t_curr_x));
  auto const [rest, _]    = ranges_from_chars(search_begin, ret_val);
  auto const digits       = search_begin.size() - rest.size();

  auto const cross = cartesian_product(closed_indices(t_curr_x - 1, t_curr_x + static_cast<int>(digits)),
                                       closed_indices(t_curr_y - 1, t_curr_y + 1));
  auto const res   = find_if(cross, [&](auto const t_coor) {
    auto const [x, y] = t_coor;
    auto const v      = t_schematic[x, y];
    return v and *v != '.' and std::isdigit(*v) == 0;
  });

  if (res != cross.end()) {
    return std::pair{ret_val, static_cast<int>(digits)};
  }

  return std::pair{0, 0};
}

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::views::cartesian_product, ranges::views::indices,
    ranges::views::transform, ranges::accumulate;

  std::fstream in((INPUT_FILE));
  auto const rng = Schemtaic{.schematic_ = getlines(in) | to_vector};

  int const sum = accumulate(cartesian_product(indices(rng.size()), indices(rng.size())) |
                               transform([&, to_skip = 0](auto const t_coor) mutable {
                                 auto const [y, x] = t_coor;
                                 if (std::isdigit(*rng[x, y]) == 0 or --to_skip > 0) {
                                   return 0;
                                 }

                                 auto const [num, size] = parse_part_num(rng, x, y);

                                 to_skip = size;
                                 return num;
                               }),
                             0);

  fmt::println("Total sum: {}", sum);
}

int parse_gear(std::map<std::pair<int, int>, std::vector<int>>& t_map, Schemtaic const& t_schematic, int const t_curr_x,
               int const t_curr_y) {
  using ranges::views::closed_indices, ranges::views::cartesian_product, ranges::for_each;

  int ret_val             = 0;
  auto const search_begin = t_schematic[t_curr_y].substr(static_cast<std::size_t>(t_curr_x));
  auto const [rest, _]    = ranges_from_chars(search_begin, ret_val);
  auto const digits       = search_begin.size() - rest.size();

  auto const cross = cartesian_product(closed_indices(t_curr_x - 1, t_curr_x + static_cast<int>(digits)),
                                       closed_indices(t_curr_y - 1, t_curr_y + 1));
  for_each(cross, [&](auto const t_coor) {
    auto const [x, y] = t_coor;
    if (auto const v = t_schematic[x, y]; v and *v == '*') {
      t_map[std::make_pair(x, y)].push_back(ret_val);
    }
  });

  return static_cast<int>(digits);
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::views::cartesian_product, ranges::views::indices,
    ranges::views::transform, ranges::accumulate, ranges::for_each, ranges::views::filter;

  std::fstream in((INPUT_FILE));
  auto const rng = Schemtaic{.schematic_ = getlines(in) | to_vector};

  std::map<std::pair<int, int>, std::vector<int>> map;
  for_each(cartesian_product(indices(rng.size()), indices(rng.size())), [&, to_skip = 0](auto const t_coor) mutable {
    auto const [y, x] = t_coor;
    if (std::isdigit(*rng[x, y]) == 0 or --to_skip > 0) {
      return;
    }

    // map store coordinate of '*', and the number around it
    to_skip = parse_gear(map, rng, x, y);
  });

  fmt::println("Total sum: {}",
               accumulate(map | filter([](auto&& t_kv) { return t_kv.second.size() == 2; }), 0, ranges::plus{},
                          [](auto&& t_kv) { return t_kv.second[0] * t_kv.second[1]; }));
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
