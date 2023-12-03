#include <algorithm>
#include <cmath>
#include <concepts>
#include <fmt/format.h>
#include <fstream>
#include <list>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/transform.hpp>
#include <vector>

template <typename T>
  requires(std::integral<T>)
void mix(std::list<T>& t_numbers, std::vector<typename std::list<T>::iterator>& t_nodes) {
  using ranges::for_each;
  for_each(t_nodes, [&, list_end = t_numbers.size() - 1](auto&& t_node) {
    auto const move = *t_node;
    if (move == 0) {
      return;
    }

    // circular list: e.g [-1, 1, 3], end = 2, i.e. -1 (idx = 0) left shift 1 will move to idx = 1 (0 - 1 + 2)
    //                                               1 (idx = 1) right shift 1 will move to idx = 0 (1 + 1 - 2)
    auto const move_steps = static_cast<std::size_t>(std::abs(move)) % list_end;
    auto pos              = t_numbers.erase(t_node);
    if (move > 0) {
      for (std::size_t i = 0; i < move_steps; ++i) {
        if (pos == t_numbers.end()) {
          pos = t_numbers.begin();
        }

        ++pos;
      }
    } else {
      for (std::size_t i = 0; i < move_steps; ++i) {
        if (pos == t_numbers.begin()) {
          pos = t_numbers.end();
        }

        --pos;
      }
    }

    t_node = t_numbers.insert(pos, move);
  });
}

constexpr auto operator""_K(unsigned long long t_v) noexcept { return t_v * 1000; }

void part1() {
  using ranges::getlines, ranges::to, ranges::views::transform, ranges::find, ranges::to_vector;

  std::fstream in((INPUT_FILE));

  auto rng = getlines(in) | transform([](auto&& t_str) { return std::stoi(t_str); }) | to<std::list>;
  std::vector<typename std::list<int>::iterator> nodes;
  nodes.reserve(rng.size());
  for (auto node = rng.begin(); node != rng.end(); ++node) {
    nodes.push_back(node);
  }

  mix(rng, nodes);
  auto const zero    = static_cast<std::size_t>(distance(rng.begin(), find(rng, 0)));
  auto const numbers = rng | to_vector;

  auto const first = numbers[(zero + 1_K) % numbers.size()];
  auto const sec   = numbers[(zero + 2_K) % numbers.size()];
  auto const third = numbers[(zero + 3_K) % numbers.size()];
  fmt::println("1000: {}, 2000: {}, 3000: {}, sum: {}", first, sec, third, first + sec + third);
}

inline constexpr auto DECRYPTION_KEY = 811'589'153;

void part2() {
  using ranges::getlines, ranges::views::transform, ranges::to, ranges::find, ranges::to_vector;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in) | transform([](auto&& t_str) { return std::stol(t_str) * DECRYPTION_KEY; }) | to<std::list>;

  std::vector<typename std::list<long>::iterator> nodes;
  nodes.reserve(rng.size());
  for (auto node = rng.begin(); node != rng.end(); ++node) {
    nodes.push_back(node);
  }

  for (std::size_t i = 0; i < 10; ++i) {
    mix(rng, nodes);
  }

  // the grove coordinates can be found by looking at the 1000th, 2000th, and 3000th numbers after the value ***0***
  // for some reason, I misread this part and keep using the one at zero idx in pre-mix list
  auto const zero    = static_cast<std::size_t>(distance(rng.begin(), find(rng, 0)));
  auto const numbers = rng | to_vector;

  auto const first = numbers[(zero + 1_K) % numbers.size()];
  auto const sec   = numbers[(zero + 2_K) % numbers.size()];
  auto const third = numbers[(zero + 3_K) % numbers.size()];
  fmt::println("1000: {}, 2000: {}, 3000: {}, sum: {}", first, sec, third, first + sec + third);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
