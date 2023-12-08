#include <algorithm>
#include <array>
#include <cctype>
#include <fmt/format.h>
#include <fstream>
#include <numeric>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/cache1.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/indices.hpp>
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

using namespace std::string_view_literals;

struct WordNumber {
  static constexpr auto NUMBER_NAME = std::array{
    "one"sv, "two"sv, "three"sv, "four"sv, "five"sv, "six"sv, "seven"sv, "eight"sv, "nine"sv,
  };

  constexpr WordNumber() = default;
  constexpr explicit WordNumber(std::size_t const t_num) : number_(t_num) {}

  std::size_t number_    = 0;
  std::size_t match_idx_ = 0;

  bool check_character(char t_chr) noexcept {
    auto const number_name = WordNumber::NUMBER_NAME[this->number_ - 1];
    if (number_name[this->match_idx_] == t_chr) {
      ++this->match_idx_;
    } else if (number_name[0] == t_chr) {
      // e.g.: ttwo, the first "if" will fail second time check_character is called, but since the second t matches the
      //       first character of "two", we need to start from 1
      this->match_idx_ = 1;
    } else {
      this->match_idx_ = 0;
      return false;
    }

    if (this->match_idx_ == number_name.size()) {
      this->match_idx_ = 0;
      return true;
    }

    return false;
  }
};

void part2() {
  using ranges::getlines;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);

  constexpr auto create_word_numbers = []() {
    std::array<WordNumber, 9> ret_val;
    ranges::transform(ranges::views::closed_indices(1UL, 9UL), ret_val.begin(),
                      [](auto t_v) { return WordNumber(t_v); });

    return ret_val;
  };

  auto extract_number = [=](auto&& t_str) {
    auto words = create_word_numbers();

    int first_num = -1;
    int last_num  = -1;
    for (auto const chr : t_str) {
      if (std::isdigit(chr) != 0) {
        if (first_num == -1) {
          first_num = static_cast<int>(chr - '0');
        }

        last_num = static_cast<int>(chr - '0');
      }

      for (auto& word : words) {
        if (word.check_character(chr)) {
          if (first_num == -1) {
            first_num = static_cast<int>(word.number_);
          }

          last_num = static_cast<int>(word.number_);
        }
      }
    }

    return first_num * 10 + last_num;
  };

  fmt::println("Sum of the callibration value: {}",
               ranges::accumulate(rng | ranges::views::transform(extract_number), 0));
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
