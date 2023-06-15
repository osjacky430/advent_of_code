#include <array>
#include <cctype>
#include <fmt/format.h>
#include <fstream>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_first_of.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/set_algorithm.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/transform.hpp>
#include <string_view>
#include <vector>

inline constexpr int a = static_cast<int>('a');
inline constexpr int A = static_cast<int>('A');

inline constexpr auto ALPHABET_COUNT = 26;

inline constexpr auto get_priority = [](auto&& t_chr) {
  if (std::islower(t_chr)) {
    return static_cast<int>(t_chr) - a + 1;
  }

  return static_cast<int>(t_chr) - A + ALPHABET_COUNT + 1;
};

inline constexpr auto to_idx = [](char const t_chr) {
  if (std::islower(t_chr) != 0) {
    return static_cast<std::size_t>(t_chr) - a + ALPHABET_COUNT;
  }

  return static_cast<std::size_t>(t_chr) - A;
};

void part1_sybrand() {
  using ranges::getlines, ranges::to_vector, ranges::accumulate, ranges::find_first_of, ranges::views::transform;

  std::ifstream rucksack_item_list((INPUT_FILE));

  static constexpr auto to_score = [](std::string const& t_str) {
    auto const midpoint    = t_str.size() / 2;
    auto const first_half  = t_str.substr(0, midpoint);
    auto const second_half = t_str.substr(midpoint, midpoint);
    auto const common      = *find_first_of(first_half, second_half);

    return get_priority(common);
  };

  auto rng    = getlines(rucksack_item_list) | to_vector;
  auto scores = accumulate(rng | transform(to_score), 0);

  fmt::print("sum priority: {}\n", scores);
}

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::accumulate, ranges::views::transform;

  std::ifstream rucksack_item_list((INPUT_FILE));

  auto rng = getlines(rucksack_item_list);

  // basically sybrand's way of doing it, but using string_view instead of string
  static constexpr auto to_score = [](std::string const& t_str) {
    using ranges::find_first_of;
    auto const half_point  = t_str.begin() + static_cast<long int>(t_str.size() / 2);
    auto const first_half  = std::string_view(t_str.begin(), half_point);
    auto const second_half = std::string_view(half_point, t_str.end());
    auto const common      = *find_first_of(first_half, second_half);

    return get_priority(common);
  };

  auto const sum = accumulate(rng | transform(to_score), 0);
  fmt::print("sum priority: {}\n", sum);

  // Original way of finding priority for current rucksack is unreadable IMO. Even though I figure it out pretty fast,
  // this requires way more cognitive load than sybrand's way. I wrote it this way just to avoid raw for loop (and
  // because it's me who write the code, of course I know that I would avoid raw for loop at any cost LMFAO)
  //
  // auto sum = accumulate(rng, 0, [](auto&& t_value, auto&& t_input) {
  //   auto half_point = t_input.begin() + static_cast<long int>(t_input.size() / 2);
  //   std::unordered_set<char> item_hash(t_input.begin(), half_point);
  //   auto const sum_item_priority = accumulate(half_point, t_input.end(), 0, [&](auto&& t_v, auto&& t_in) {
  //     return t_v + [&](auto&& t_chr) {
  //       if (item_hash.contains(t_chr)) {
  //         item_hash.erase(t_chr);
  //         return get_priority(t_chr);
  //       }
  //       return 0;
  //     }(t_in);
  //   });
  //   return t_value + sum_item_priority;
  // });
}

// time complexity: O(nm), where n is the length of the inventory, m the number of rucksack
// space complexity: O(1)
void part2() {
  using ranges::getlines, ranges::to_vector, ranges::accumulate, ranges::views::chunk, ranges::for_each, ranges::find,
    ranges::transform;

  std::ifstream rucksack_item_list((INPUT_FILE));

  auto const rng = getlines(rucksack_item_list) | to_vector;
  auto const sum = accumulate(rng | chunk(3), 0, [&](auto&& t_v, auto&& t_in) {
    std::vector<bool> marker(static_cast<std::size_t>(ALPHABET_COUNT) * 2, true);

    for_each(t_in, [&](auto&& t_str) {
      std::vector<bool> filter(static_cast<std::size_t>(ALPHABET_COUNT) * 2, false);

      for (auto const chr : t_str) {
        auto const idx = to_idx(chr);
        filter[idx]    = marker[idx];
      }

      marker = filter;
    });

    auto const badge_pos = static_cast<std::size_t>(find(marker, true) - marker.begin());
    auto const badge     = static_cast<char>(badge_pos >= 26 ? a + badge_pos - ALPHABET_COUNT : A + badge_pos);
    return t_v + get_priority(badge);
  });

  fmt::print("badge sum: {}\n", sum);
}

// time complexity O(m*n*logn) due to introsort
// space complexity O(1), despite using std::vector (dynamic memory allocation)
void part2_sybrand() {
  using ranges::getlines, ranges::to_vector, ranges::accumulate, ranges::views::chunk, ranges::find_first_of,
    ranges::sort, ranges::set_intersection, ranges::for_each, ranges::views::transform;

  std::ifstream rucksack_item_list((INPUT_FILE));

  auto rng   = getlines(rucksack_item_list) | to_vector;
  auto score = accumulate(rng | chunk(3), 0, [](auto&& t_v, auto&& t_in) {
    for_each(t_in, sort);

    std::vector<char> result;
    set_intersection(t_in[0], t_in[1], back_inserter(result));
    auto const badge = *find_first_of(t_in[2], result);
    return t_v + get_priority(badge);
  });

  fmt::print("badge sum: {}\n", score);
}

int main(int /*unused*/, char** /*unused*/) {
  part1();
  part1_sybrand();
  part2();
  part2_sybrand();

  return EXIT_SUCCESS;
}