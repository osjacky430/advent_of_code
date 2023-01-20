#include "split_helper.hpp"

#include <cstdlib>
#include <deque>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <range/v3/algorithm/partial_sort.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/split_when.hpp>
#include <range/v3/view/transform.hpp>
#include <vector>

struct Monkey {
  long long unsigned dividend_ = 3;
  std::deque<long long unsigned> items_;
  std::pair<char, long long unsigned> operation_;
  long long unsigned test_;

  std::array<int, 2> recipient_;

  long long inspected_time_ = 0;

  [[nodiscard]] bool no_items() const { return this->items_.empty(); }

  template <typename WorryLevelManager>
  [[nodiscard]] auto calculate_worry_level(WorryLevelManager&& t_worry_lvl_manager) {
    auto [op, val]     = this->operation_;
    auto& current_item = this->items_.front();
    if (val == 0) {
      val = current_item;
    }

    if (op == '*') {
      current_item *= val;
    } else {
      current_item += val;
    }

    current_item = t_worry_lvl_manager(current_item);
    return current_item;
  }

  template <typename WorryLevelManager>
  [[nodiscard]] bool apply_test(WorryLevelManager&& t_worry_lvl_manager) {
    return this->calculate_worry_level(std::forward<WorryLevelManager>(t_worry_lvl_manager)) % this->test_ == 0;
  }

  template <typename WorryLevelManager>
  [[nodiscard]] std::size_t get_recipient(WorryLevelManager&& t_worry_lvl_manager) {
    return static_cast<std::size_t>(this->apply_test(t_worry_lvl_manager) ? this->recipient_[0] : this->recipient_[1]);
  }

  [[nodiscard]] auto throw_item() {
    auto to_throw = this->items_.front();
    this->items_.pop_front();
    return to_throw;
  }

  void receive_item(auto&& t_worry_lvl) { this->items_.push_back(t_worry_lvl); }

  [[nodiscard]] auto get_test() const noexcept { return this->test_; }
  [[nodiscard]] auto get_inspected_time() const noexcept { return this->inspected_time_; }
};

inline auto const start_with_monkey = [](auto&& t_str, auto&& /**/) {
  return std::pair{t_str->starts_with("Monkey"), t_str};
};

inline auto const to_integer = [](auto&& t_str) {
  return static_cast<long long unsigned>(std::stoi(std::string(t_str)));
};

template <typename WorryLevelManager>
void execute_turn(Monkey& t_current_monkey, std::vector<Monkey>& t_monkeys, WorryLevelManager&& t_worry_lvl_manager) {
  while (not t_current_monkey.no_items()) {
    auto& recipient = t_monkeys[t_current_monkey.get_recipient(std::forward<WorryLevelManager>(t_worry_lvl_manager))];
    recipient.receive_item(t_current_monkey.throw_item());
    ++t_current_monkey.inspected_time_;
  }
}

std::vector<Monkey> get_monkey_list(std::fstream& t_in) {
  using ranges::getlines, ranges::views::split_when, ranges::to_vector, ranges::views::transform, ranges::to,
    ranges::views::drop, ranges::partial_sort;

  auto rng = getlines(t_in) | to_vector;

  std::vector<Monkey> monkeys;
  for (auto&& monkey_list : rng | split_when(start_with_monkey)) {
    auto const& starting_item   = split_string(monkey_list[1]);
    auto const& operation       = split_string(monkey_list[2]);
    auto const& test            = static_cast<long long unsigned>(std::stoi(split_string(monkey_list[3]).back()));
    auto const& true_recipient  = std::stoi(split_string(monkey_list[4]).back());
    auto const& false_recipient = std::stoi(split_string(monkey_list[5]).back());

    auto& monkey      = monkeys.emplace_back();
    monkey.items_     = starting_item | drop(4) | transform(to_integer) | to<std::deque>;
    monkey.operation_ = std::pair{
      operation[6][0],
      operation.back() == "old" ? 0 : static_cast<std::size_t>(std::stoi(operation.back())),
    };
    monkey.test_      = test;
    monkey.recipient_ = {true_recipient, false_recipient};
  }

  return monkeys;
}

void part1() {
  using ranges::partial_sort;

  std::fstream in((INPUT_FILE));

  auto const worry_lvl_management = [](auto const t_worry_lvl) { return t_worry_lvl / 3; };

  auto monkeys = get_monkey_list(in);
  for (std::size_t i = 0; i < 20; ++i) {
    for (auto&& monkey : monkeys) {
      execute_turn(monkey, monkeys, worry_lvl_management);
    }
  }

  partial_sort(begin(monkeys), begin(monkeys) + 2, end(monkeys), std::greater{}, &Monkey::get_inspected_time);
  fmt::print("monkey business: {}\n", monkeys[0].inspected_time_ * monkeys[1].inspected_time_);
}

void part2() {
  using ranges::partial_sort, ranges::accumulate;
  std::fstream in((INPUT_FILE));

  auto monkeys                        = get_monkey_list(in);
  auto const max_meaningful_worry_lvl = accumulate(monkeys, 1ULL, std::multiplies<>{}, &Monkey::get_test);
  auto const worry_lvl_management     = [=](auto const t_worry_lvl) { return t_worry_lvl % max_meaningful_worry_lvl; };
  for (std::size_t i = 0; i < 10000; ++i) {
    for (auto&& monkey : monkeys) {
      execute_turn(monkey, monkeys, worry_lvl_management);
    }
  }

  partial_sort(begin(monkeys), begin(monkeys) + 2, end(monkeys), std::greater{}, &Monkey::get_inspected_time);
  fmt::print("monkey business: {}\n", monkeys[0].inspected_time_ * monkeys[1].inspected_time_);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}