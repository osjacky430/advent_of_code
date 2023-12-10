#include "string_util.hpp"
#include <array>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/trim.hpp>

using namespace std::string_view_literals;

void part1() {
  using ranges::getlines, ranges::views::transform, ranges::accumulate, ranges::views::enumerate, ranges::views::trim;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);

  static constexpr auto CUBES_COUNT = std::array{12, 13, 14};  // RED, GREEN, BLUE

  constexpr auto get_cubes_grabbed = [](auto const& t_str) {
    auto const game = split_string(t_str, ':');
    auto const sets = split_string(game[1], ';');

    for (auto const& set : sets | transform([](auto const& t_str) { return split_string(t_str, ','); })) {
      for (auto const& ball : set) {
        int quantity           = 0;
        auto trimmed           = ball | trim([](auto&& t_str) { return std::isspace(t_str); });
        auto const [rest, err] = ranges_from_chars(trimmed, quantity);
        if (err != std::errc() or rest.empty()) {
          return 0;
        }

        auto const idx = [=]() {
          switch (rest[1]) {
            case 'r':
              return 0UL;
            case 'g':
              return 1UL;
            case 'b':
              return 2UL;
          }

          std::unreachable();
        }();

        if (quantity > CUBES_COUNT[idx]) {
          return 0;
        }
      }
    }

    int ret_val = 0;
    ranges_from_chars(std::string_view(game[0]).substr(std::size("Game "sv)), ret_val);
    return ret_val;
  };

  fmt::println("id sum: {}", accumulate(rng | transform(get_cubes_grabbed), 0));
}

void part2() {
  using ranges::getlines, ranges::views::transform, ranges::accumulate, ranges::views::enumerate, ranges::views::trim;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);

  constexpr auto get_cubes_power = [](auto const& t_str) {
    auto const game = split_string(t_str, ':');
    auto const sets = split_string(game[1], ';');

    std::array<int, 3> result{0, 0, 0};

    for (auto const& set : sets | transform([](auto const& t_str) { return split_string(t_str, ','); })) {
      for (auto const& ball : set) {
        int quantity         = 0;
        auto trimmed         = ball | trim([](auto&& t_str) { return std::isspace(t_str); });
        auto const [rest, _] = ranges_from_chars(trimmed, quantity);
        auto const idx       = [=]() {
          switch (rest[1]) {
            case 'r':
              return 0UL;
            case 'g':
              return 1UL;
            case 'b':
              return 2UL;
          }

          std::unreachable();
        }();

        result[idx] = std::max(result[idx], quantity);
      }
    }

    return accumulate(result, 1, std::multiplies<>{});
  };

  fmt::println("power sum: {}", accumulate(rng | transform(get_cubes_power), 0));
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
