#include "string_util.hpp"
#include <fmt/format.h>
#include <fstream>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/getlines.hpp>
#include <set>

auto get_matching_number(std::string_view t_str) {
  using ranges::views::transform, ranges::to, ranges::count_if, ranges::views::split, ranges::distance,
    ranges::views::filter;

  auto const delim           = t_str.find('|');
  auto const start           = t_str.find(':');
  auto const winning_str     = t_str.substr(start + 1, delim - 1 - start);
  auto const numbers_i_have  = t_str.substr(delim + 1);
  auto const splitted        = split_string(winning_str);
  auto const winning_numbers = splitted | filter([](auto const t_str) { return not t_str.empty(); }) |
                               transform([](auto const t_str) {
                                 int ret_val = 0;
                                 ranges_from_chars(t_str, ret_val);
                                 return ret_val;
                               }) |
                               to<std::set>;

  auto matching_num = count_if(numbers_i_have | split(' '), [&](auto const t_view) {
    int ret_val = 0;
    std::string_view view{&*t_view.begin(), static_cast<std::size_t>(distance(t_view))};
    ranges_from_chars(view, ret_val);
    return winning_numbers.contains(ret_val);
  });

  return matching_num;
}

void part1() {
  using ranges::getlines, ranges::accumulate;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);

  auto const res = accumulate(rng, 0, ranges::plus{}, [](std::string_view t_str) {
    auto const matching_num = get_matching_number(t_str);
    return matching_num == 0 ? 0 : 1UL << static_cast<std::size_t>(matching_num - 1);
  });

  fmt::println("sum: {}", res);
}

void part2() {
  using ranges::getlines, ranges::views::transform, ranges::to_vector, ranges::accumulate, ranges::views::enumerate;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);

  auto const winning_card_num = rng | transform(get_matching_number) | to_vector;
  std::vector<int> number_for_each_card(winning_card_num.size(), 1);
  for (auto const [idx, val] : enumerate(winning_card_num)) {
    for (int i = 0; i < val; ++i) {
      number_for_each_card[idx + static_cast<std::size_t>(i) + 1] += number_for_each_card[idx];
    }
  }

  fmt::println("total scratchcards: {}", accumulate(number_for_each_card, 0));
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
