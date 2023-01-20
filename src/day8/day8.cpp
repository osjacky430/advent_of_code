#include <cstddef>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/indices.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/transform.hpp>
#include <string>
#include <vector>

// void part1_sol2() {
//   std::fstream map((INPUT_FILE));

//   auto map_str = getlines(map) | to_vector;
//
//   we basically look at the map at four side, and put all visible tree in a set
// }

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::views::cartesian_product, ranges::views::indices, ranges::count_if,
    ranges::any_of, ranges::max, ranges::views::single, ranges::views::transform;

  std::fstream map((INPUT_FILE));

  auto map_str      = getlines(map) | to_vector;
  auto const x_size = map_str.back().size();
  auto const y_size = map_str.size();

  // this is not the best way, we are basically brute forcing
  auto const inner_visible_tree_count = count_if(
    cartesian_product(indices(std::size_t(1), y_size - 1), indices(std::size_t(1), x_size - 1)), [&](auto&& t_idx) {
      auto const [y, x]         = t_idx;
      auto const current_height = map_str[y][x];

      auto to_height = transform([&](auto&& t_v) {
        auto const [y_c, x_c] = t_v;
        return map_str[y_c][x_c];
      });

      // this is fucking idiot LMAO, rewrite it later
      bool const left  = max(cartesian_product(single(y), indices(x)) | to_height) < current_height;
      bool const right = max(cartesian_product(single(y), indices(x + 1, x_size)) | to_height) < current_height;
      bool const up    = max(cartesian_product(indices(y), single(x)) | to_height) < current_height;
      bool const down  = max(cartesian_product(indices(y + 1, y_size), single(x)) | to_height) < current_height;

      return down or up or left or right;
    });

  auto visible_tree = static_cast<std::size_t>(inner_visible_tree_count) + (x_size + y_size) * 2 - 4;
  fmt::print("visible tree count: {}\n", visible_tree);
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::views::transform, ranges::max, ranges::find_if,
    ranges::views::reverse, ranges::begin, ranges::views::indices, ranges::views::cartesian_product,
    ranges::views::single;

  std::fstream map((INPUT_FILE));

  auto map_str      = getlines(map) | to_vector;
  auto const x_size = map_str.back().size();
  auto const y_size = map_str.size();

  auto const to_height = [&](auto&& t_v) {
    auto const [y_c, x_c] = t_v;
    return map_str[y_c][x_c];
  };
  auto to_score = transform([&](auto&& t_idx) {
    auto const [y, x] = t_idx;
    auto const height = map_str[y][x];

    auto left_rng  = cartesian_product(single(y), indices(x)) | reverse;
    auto right_rng = cartesian_product(single(y), indices(x + 1, x_size));
    auto down_rng  = cartesian_product(indices(y + 1, y_size), single(x));
    auto up_rng    = cartesian_product(indices(y), single(x)) | reverse;

    auto const height_predicate = [=](auto&& t_v) { return t_v >= height; };
    auto const get_score        = [=](auto&& t_rng) {
      if (auto iter = find_if(t_rng, height_predicate, to_height); iter != t_rng.end()) {
        return static_cast<std::size_t>(iter - begin(t_rng) + 1);
      }

      return static_cast<std::size_t>(t_rng.size());
    };

    std::size_t const left_score  = get_score(left_rng);
    std::size_t const right_score = get_score(right_rng);
    std::size_t const down_score  = get_score(down_rng);
    std::size_t const up_score    = get_score(up_rng);

    return down_score * up_score * right_score * left_score;
  });

  auto index = cartesian_product(indices(std::size_t(1), y_size - 1), indices(std::size_t(1), x_size - 1));
  auto score = max(index | to_score);

  fmt::print("highest scenic tree: {}\n", score);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}