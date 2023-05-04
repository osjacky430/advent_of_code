#include <array>
#include <cmath>
#include <fmt/format.h>
#include <fstream>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>
#include <utility>

inline constexpr std::array NUMBER_FORMAT{'=', '-', '0', '1', '2'};
inline constexpr auto POWER = 5;

auto carry(int const t_value) {
  // conversion rule: 5n - 2 <= value <= 5n + 2
  //
  // e.g 31 = 5 * 6 + 1, n = 6 -> 5_dec = 10_snafu, 6_dec = 11_snafu 10_snafu * 11_snafu + 1 = 111
  //     2022 = 5 * 404 + 2  => 2022_dec = 10_snafu * 1=11-_snafu + 2       = 1=11-2_snafu
  //      404 = 5 * 81 - 1   => 404_dec  = 10_snafu * 1=11_snafu + -_snafu  = 1=11-_snafu
  //       81 = 5 * 16 + 1   => 81_dec   = 10_snafu * 1=1_snafu + 1         = 1=11_snafu
  //       16 = 5 * 3 + 1    => 16_dec   = 10_snafu * 1=_sanfu + 1          = 1=1_snafu
  //
  //     2022 = 25 * 80 + 5 * 4 + 2   => 2022_dec = 100_snafu * 1=10_snafu + 10_snafu * 1-_snafu + 1 = 1=11-2_snafu
  //       80 = 25 * 3  + 5 * 1       =>   80_dec = 100_snafu * 1=_snafu + 10_snafu = 1=10_snafu

  // arithmetic rule:
  //
  // e.g.     102_sanfu
  //      +   212_snafu
  //         10-1_snafu
  //         ----------
  //            5
  //           0
  //          3
  //         1

  //        1-=_snafu (18)
  //       10=-_snafu (114)
  //     +  11=_snafu (28)
  //        1==_snafu (13)
  //       -----------
  //           -7      -> -2     -> -2
  //         -4        ->   -5   ->    0
  //        3          ->     3  ->      2
  //       1           ->      1 ->        1
  //      ------------
  //       120=_snafu (173 = 114 + 18 + 28 + 13)
  //
  //
  int current_value = t_value % POWER;
  int next_digit    = t_value / POWER;

  if (current_value > 2) {
    current_value -= POWER;
    ++next_digit;
  } else if (current_value < -2) {
    current_value += POWER;
    --next_digit;
  }

  return std::pair{next_digit, current_value};
}

auto to_snafu(int const t_value) { return NUMBER_FORMAT.at(static_cast<std::size_t>(t_value + 2)); }

void part1() {
  using ranges::getlines, ranges::views::transform, ranges::reverse, ranges::to_vector, ranges::max,
    ranges::views::filter, ranges::to;

  std::fstream in((INPUT_FILE));
  auto const rng = getlines(in) | transform([](auto&& t_str) {
                     reverse(t_str);
                     return t_str;
                   }) |
                   to_vector;

  auto const max_digit = max(rng, std::less<>{}, [](auto&& t_str) { return t_str.size(); }).size();

  std::vector<int> res(max_digit, 0);
  for (std::size_t i = 0; i < max_digit; ++i) {
    auto& current_digit = res[i];
    for (auto&& str : rng | filter([=](auto&& t_str) { return t_str.size() > i; })) {
      switch (str[i]) {
        case '1':
          current_digit += 1;
          break;
        case '2':
          current_digit += 2;
          break;
        case '0':
          break;
        case '-':
          current_digit -= 1;
          break;
        case '=':
          current_digit -= 2;
          break;
        default:
          throw std::runtime_error("invalid input");
      }
    }
  }

  for (std::size_t i = 0; i + 1 < res.size(); ++i) {
    auto&& [next, curr] = carry(res[i]);
    if (i + 1 == res.size() and next != 0) {  // is last digit
      res.push_back(next);
    } else {
      res[i + 1] += next;
    }

    res[i] = curr;
  }

  reverse(res);
  fmt::print("{}\n", res | transform(to_snafu) | to<std::string>);
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
