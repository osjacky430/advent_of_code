#include <fmt/format.h>
#include <fstream>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/sliding.hpp>
#include <unordered_set>

void detect_packet(std::string_view sv, std::size_t t_number_of_unique_chr) {
  using ranges::views::sliding, ranges::find_if, ranges::begin, ranges::end, ranges::distance;
  auto rng = sv | sliding(t_number_of_unique_chr);

  // this can be done more efficiently, just i'm lazy now
  auto start = find_if(rng, [=](auto t_group) {
    return std::unordered_set<char>(begin(t_group), end(t_group)).size() == t_number_of_unique_chr;
  });

  fmt::print("packet start: {}\n", static_cast<std::size_t>(distance(rng.begin(), start)) + t_number_of_unique_chr);
}

void part1() {
  using ranges::getlines;

  std::ifstream assignment_list((INPUT_FILE));

  auto line = getlines(assignment_list).cached();
  detect_packet(line, 4);
}

void part2() {
  using ranges::getlines, ranges::views::sliding, ranges::find_if;

  std::ifstream assignment_list((INPUT_FILE));

  auto line = getlines(assignment_list).cached();
  detect_packet(line, 14);
}

int main(int /*unused*/, char** /*unused*/) {
  part1();
  part2();
  return EXIT_SUCCESS;
}