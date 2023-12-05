#include <algorithm>
#include <cctype>
#include <fmt/format.h>
#include <fstream>
#include <numeric>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/transform.hpp>
#include <ranges>

void part1() {
  using ranges::getlines;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);

  // assume there must be a number in the string
  constexpr auto isdigit_pred   = [](auto const& t_chr) { return std::isdigit(t_chr); };
  constexpr auto extract_number = [=](auto&& t_str) {
    auto const first_num  = static_cast<int>(*std::ranges::find_if(t_str, isdigit_pred) - '0');
    auto const second_num = static_cast<int>(*std::ranges::find_if(t_str | std::views::reverse, isdigit_pred) - '0');

    return first_num * 10 + second_num;
  };

  fmt::println("Sum of the callibration value: {}",
               ranges::accumulate(rng | ranges::views::transform(extract_number), 0));
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
