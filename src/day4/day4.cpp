#include <charconv>
#include <fmt/format.h>
#include <fstream>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/view/getlines.hpp>
#include <ranges>
#include <vector>

auto const to_ids = [](auto&& t_sections) {
  using std::views::split, std::views::transform, std::views::common;

  std::vector<int> ret_val;
  ret_val.reserve(4);  // hard code 4 because every assignment always contain 4 integers

  for (auto&& sections : t_sections | split(',')) {
    auto tf_view = sections      //
                   | split('-')  //
                   | transform([](auto&& t_sec) {
                       int section = 0;
                       std::from_chars(&*t_sec.begin(), &*t_sec.end(), section);

                       return section;
                     })  //
                   | common;
    ret_val.insert(ret_val.end(), tf_view.begin(), tf_view.end());
  }

  return ret_val;
};

void part1() {
  using ranges::getlines, ranges::count_if;

  std::ifstream assignment_list((INPUT_FILE));

  auto amount = count_if(
    getlines(assignment_list),
    [](auto&& result) {
      auto const first_start = result[0];
      auto const first_end   = result[1];
      auto const sec_start   = result[2];
      auto const sec_end     = result[3];

      return (first_start <= sec_start and sec_end <= first_end) or (sec_start <= first_start and first_end <= sec_end);
    },
    to_ids);

  fmt::print("fully contain amount: {}\n", amount);
}

void part2() {
  using ranges::getlines, ranges::count_if;

  std::ifstream assignment_list((INPUT_FILE));

  auto amount = count_if(
    getlines(assignment_list),
    [](auto&& result) {
      auto const first_start = result[0];
      auto const first_end   = result[1];
      auto const sec_start   = result[2];
      auto const sec_end     = result[3];

      return first_start <= sec_end and sec_start <= first_end;
    },
    to_ids);

  fmt::print("overlapped amount: {}\n", amount);
}

int main(int /*unused*/, char** /*unused*/) {
  part1();
  part2();
  return EXIT_SUCCESS;
}