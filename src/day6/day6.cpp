#include <bit>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/sliding.hpp>

void detect_packet(std::string_view sv, std::size_t t_number_of_unique_chr) {
  using ranges::views::sliding, ranges::find_if, ranges::distance, ranges::accumulate;
  auto rng = sv | sliding(t_number_of_unique_chr);

  auto start = find_if(rng, [=](auto t_group) {
    static constexpr auto char_to_bit = [](auto&& t_chr) { return 1UL << static_cast<std::size_t>(t_chr - 'a'); };
    auto const flag                   = accumulate(t_group, 0UL, std::bit_or<>{}, char_to_bit);
    return static_cast<std::size_t>(std::popcount(flag)) == t_number_of_unique_chr;
  });

  fmt::print("packet start: {}\n", static_cast<std::size_t>(distance(rng.begin(), start)) + t_number_of_unique_chr);
}

void part1() {
  using ranges::getlines;

  std::ifstream assignment_list((INPUT_FILE));

  auto const& line = getlines(assignment_list).cached();
  detect_packet(line, 4);
}

void part2() {
  using ranges::getlines;

  std::ifstream assignment_list((INPUT_FILE));

  auto const& line = getlines(assignment_list).cached();
  detect_packet(line, 14);
}

int main(int /*unused*/, char** /*unused*/) {
  part1();
  part2();
  return EXIT_SUCCESS;
}