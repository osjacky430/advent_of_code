#include <array>
#include <ctre.hpp>
#include <deque>
#include <fmt/format.h>
#include <fstream>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop_last.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/view/take_last.hpp>
#include <range/v3/view/transform.hpp>
#include <stack>
#include <vector>

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::views::split, ranges::views::transform, ranges::views::drop_last,
    ranges::views::reverse, ranges::views::stride, ranges::views::enumerate, ranges::views::filter;

  std::ifstream assignment_list((INPUT_FILE));

  auto lines = getlines(assignment_list) | to_vector;
  auto rng   = lines | split("");

  auto initial_cond = *begin(rng) | to_vector;

  auto const stack_count = static_cast<std::size_t>(initial_cond.back().back() - '0');
  std::vector<std::stack<char>> stacks(stack_count);

  // cartesian product?
  for (auto&& str : initial_cond | drop_last(1) | reverse) {
    std::string_view sv(str.begin() + 1, str.end());

    for (auto [idx, chr] : enumerate(sv | stride(4)) | filter([](auto t_v) { return std::get<1>(t_v) != ' '; })) {
      stacks[idx].push(chr);
    }
  }

  auto commands = *next(begin(rng));
  for (auto cmd : commands) {
    auto moves = ctre::match<"move ([0-9]+) from ([0-9]+) to ([0-9]+)">(cmd);

    auto const quantity = std::stoi(moves.get<1>().to_string());
    auto const from     = std::stoi(moves.get<2>().to_string());
    auto const to       = std::stoi(moves.get<3>().to_string());

    auto& from_stack = stacks[static_cast<std::size_t>(from - 1)];
    auto& to_stack   = stacks[static_cast<std::size_t>(to - 1)];

    for (int i = 0; i < quantity; ++i) {
      to_stack.push(from_stack.top());
      from_stack.pop();
    }
  }

  auto const top_crates = stacks | transform([](auto&& t_v) { return t_v.top(); }) | ranges::to<std::string>;
  fmt::print("top crates with CrateMover 9000: {}\n", top_crates);
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::views::split, ranges::views::transform, ranges::views::drop_last,
    ranges::views::reverse, ranges::views::stride, ranges::views::enumerate, ranges::views::filter;

  std::ifstream assignment_list((INPUT_FILE));

  auto lines = getlines(assignment_list) | to_vector;
  auto rng   = lines | split("");

  auto initial_cond = *begin(rng) | to_vector;

  auto const stack_count = static_cast<std::size_t>(initial_cond.back().back() - '0');
  std::vector<std::deque<char>> stacks(stack_count);

  for (auto&& str : initial_cond | drop_last(1) | reverse) {
    std::string_view sv(str.begin() + 1, str.end());

    for (auto [idx, chr] : enumerate(sv | stride(4)) | filter([](auto t_v) { return std::get<1>(t_v) != ' '; })) {
      stacks[idx].push_back(chr);
    }
  }

  auto commands = *next(begin(rng));
  for (auto cmd : commands) {
    auto moves = ctre::match<"move ([0-9]+) from ([0-9]+) to ([0-9]+)">(cmd);

    auto const quantity = std::stoi(moves.get<1>().to_string());
    auto const from     = std::stoi(moves.get<2>().to_string());
    auto const to       = std::stoi(moves.get<3>().to_string());

    auto& from_stack = stacks[static_cast<std::size_t>(from - 1)];
    auto& to_stack   = stacks[static_cast<std::size_t>(to - 1)];

    auto const begin_idx  = from_stack.size() - static_cast<std::size_t>(quantity);
    auto from_stack_begin = next(from_stack.begin(), static_cast<std::ptrdiff_t>(begin_idx));
    auto from_stack_end   = from_stack.end();
    to_stack.insert(to_stack.end(), from_stack_begin, from_stack_end);
    from_stack.erase(from_stack_begin, from_stack_end);
  }

  auto const top_crates = stacks | transform([](auto&& t_v) { return t_v.back(); }) | ranges::to<std::string>;
  fmt::print("top crates with CrateMover 9001: {}\n", top_crates);
}

int main(int /*unused*/, char** /*unused*/) {
  part1();
  part2();
  return EXIT_SUCCESS;
}