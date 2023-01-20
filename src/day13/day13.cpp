#include "split_helper.hpp"
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <iterator>
#include <range/v3/algorithm/lower_bound.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

struct Node {
  using Signal = std::variant<int, Node>;
  std::vector<Signal> signals_;
};

enum class Result { Right = 1, Wrong = -1, Continue = 0 };

Result operator!(Result const& t_v) { return Result{-static_cast<std::underlying_type_t<Result>>(t_v)}; }

struct NodeVisitor {
  [[nodiscard]] constexpr Result operator()(int const t_lhs, int const t_rhs) const {
    if (t_lhs < t_rhs) {
      return Result::Right;
    }

    if (t_lhs == t_rhs) {
      return Result::Continue;
    }

    return Result::Wrong;
  }

  [[nodiscard]] Result operator()(int const t_lhs, Node const& t_rhs) const {
    if (t_rhs.signals_.empty()) {
      return Result::Wrong;
    }

    auto const res = std::visit([&](auto&& t_node) { return this->operator()(t_lhs, t_node); }, t_rhs.signals_.front());
    if (res == Result::Continue) {
      return Result::Right;
    }

    return res;
  }

  [[nodiscard]] Result operator()(Node const& t_lhs, int const& t_rhs) const {
    return not this->operator()(t_rhs, t_lhs);
  }

  [[nodiscard]] Result operator()(Node const& t_lhs, Node const& t_rhs) const {
    using ranges::views::zip;

    for (auto&& [lhs, rhs] : zip(t_lhs.signals_, t_rhs.signals_)) {
      if (auto const res = std::visit(*this, lhs, rhs); res != Result::Continue) {
        return res;
      }
    }

    if (t_rhs.signals_.size() > t_lhs.signals_.size()) {
      return Result::Right;
    }

    if (t_rhs.signals_.size() < t_lhs.signals_.size()) {
      return Result::Wrong;
    }

    return Result::Continue;
  }
};

// parse_int returns int and move t_int_pos to last matching character
int parse_int(auto& t_int_pos, std::size_t const t_size) {
  int ret_val = 0;
  auto [ptr, ec]{std::from_chars(t_int_pos, t_int_pos + t_size, ret_val)};

  std::advance(t_int_pos, ptr - t_int_pos - 1);
  return ret_val;
}

Node parse_node(std::string_view const t_str) {
  using namespace std::string_view_literals;

  if (t_str == "[]"sv) {
    return Node{};
  }

  Node lhs{};
  std::size_t bracket_count = 1;
  std::string_view::iterator bracket_begin;
  for (auto iter = t_str.begin() + 1; iter != t_str.end() - 1; ++iter) {
    if (std::isdigit(*iter)) {
      if (bracket_count == 1) {
        lhs.signals_.emplace_back(parse_int(iter, t_str.size()));
      }
    } else if (*iter == '[') {
      if (bracket_count == 1) {
        bracket_begin = iter;
      }
      ++bracket_count;
    } else if (*iter == ']') {
      if (--bracket_count == 1) {
        lhs.signals_.emplace_back(parse_node(std::string_view{bracket_begin, iter + 1}));
      }
    }
  }

  return lhs;
}

std::vector<std::pair<Node, Node>> signal_parser(std::fstream& t_in) {
  using ranges::getlines, ranges::to_vector, ranges::views::split, ranges::views::transform, ranges::begin,
    ranges::next;

  auto const rng = getlines(t_in) | to_vector;
  auto ret_val   = rng | split("") | transform([](auto&& t_signal_pair) {
                   return std::pair{parse_node(*begin(t_signal_pair)), parse_node(*next(begin(t_signal_pair)))};
                 }) |
                 to_vector;

  return ret_val;
}

void part1() {
  std::fstream in((INPUT_FILE));

  NodeVisitor visitor{};
  auto const signals = signal_parser(in);

  std::size_t result = 0;
  for (std::size_t i = 0; i < signals.size(); ++i) {
    if (auto&& [lhs, rhs] = signals[i]; visitor(lhs, rhs) == Result::Right) {
      result += (i + 1);
    }
  }

  fmt::print("sum of right order signals: {}\n", result);
}

void part2() {
  using namespace std::string_view_literals;
  using ranges::getlines, ranges::to_vector, ranges::views::filter, ranges::sort, ranges::views::transform;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in)                                              //
             | filter([](auto&& t_str) { return not t_str.empty(); })  //
             | transform(parse_node)                                   //
             | to_vector;

  auto const predicate = [visitor = NodeVisitor{}](auto&& t_lhs, auto&& t_rhs) {
    return visitor(t_lhs, t_rhs) == Result::Right;
  };

  sort(rng, predicate);

  // divider_packet_2 need to consider the presence of divider_packet_1 when calculating index (using lower_bound)
  // or using upper_bound to count backward
  auto const divider_packet_1 = ranges::lower_bound(rng, parse_node("[[2]]"sv), predicate) - rng.begin() + 1;
  auto const divider_packet_2 = ranges::lower_bound(rng, parse_node("[[6]]"sv), predicate) - rng.begin() + 2;
  fmt::print("packets position: {}, {}, decoder key: {}\n", divider_packet_1, divider_packet_2,
             divider_packet_1 * divider_packet_2);
}

int main(int /**/, char** /**/) {
  part1();
  part2();
  return EXIT_SUCCESS;
}