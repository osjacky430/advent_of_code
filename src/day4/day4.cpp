#include <charconv>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <iterator>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>
#include <string_view>
#include <vector>

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::views::split, ranges::count_if, ranges::distance,
    ranges::views::transform, ranges::copy;

  std::ifstream assignment_list((INPUT_FILE));

  auto rng    = getlines(assignment_list) | to_vector;
  auto amount = count_if(rng, [](auto&& t_sections) {
    auto section_list = t_sections | split(',');

    std::vector<int> result;
    for (auto&& sections : section_list) {
      copy(sections | split('-') | transform([](auto&& t_sec) {
             int section = 0;

             std::string_view v(&*t_sec.begin(), static_cast<std::size_t>(distance(t_sec)));
             [[maybe_unused]] auto [ptr, ec]{std::from_chars(v.data(), v.data() + v.size(), section)};

             return section;
           }),
           std::back_inserter(result));
    }

    auto const first_start = result[0];
    auto const first_end   = result[1];
    auto const sec_start   = result[2];
    auto const sec_end     = result[3];

    return (first_start <= sec_start and sec_end <= first_end) or (sec_start <= first_start and first_end <= sec_end);
  });

  fmt::print("fully contain amount: {}\n", amount);
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::views::split, ranges::count_if, ranges::distance,
    ranges::views::transform, ranges::copy;

  std::ifstream assignment_list((INPUT_FILE));

  auto rng    = getlines(assignment_list) | to_vector;
  auto amount = count_if(rng, [](auto&& t_sections) {
    auto section_list = t_sections | split(',');

    std::vector<int> result;
    for (auto&& sections : section_list) {
      copy(sections | split('-') | transform([](auto&& t_sec) {
             int section = 0;

             std::string_view v(&*t_sec.begin(), static_cast<std::size_t>(distance(t_sec)));
             [[maybe_unused]] auto [ptr, ec]{std::from_chars(v.data(), v.data() + v.size(), section)};

             return section;
           }),
           std::back_inserter(result));
    }

    auto const first_start = result[0];
    auto const first_end   = result[1];
    auto const sec_start   = result[2];
    auto const sec_end     = result[3];

    return first_start <= sec_end and sec_start <= first_end;
  });

  fmt::print("overlapped amount: {}\n", amount);
}

int main(int /*unused*/, char** /*unused*/) {
  part1();
  part2();
  return EXIT_SUCCESS;
}