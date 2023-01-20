#include <array>
#include <fmt/format.h>
#include <fstream>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/getlines.hpp>
#include <vector>

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::accumulate;

  std::ifstream elves_inventory_list((INPUT_FILE));

  static constexpr std::array<std::array<int, 3>, 3> rps_table = {
    //         A  B  C
    std::array{4, 1, 7},  // X
    std::array{8, 5, 2},  // Y
    std::array{3, 9, 6},  // Z
  };

  auto lines = getlines(elves_inventory_list) | to_vector;
  auto score = accumulate(lines, 0, [](auto&& t_v, auto&& t_in) {
    std::size_t const elf_choice = static_cast<std::size_t>(t_in[0]) - static_cast<std::size_t>('A');
    std::size_t const my_choice  = static_cast<std::size_t>(t_in[2]) - static_cast<std::size_t>('X');
    return t_v + rps_table.at(my_choice).at(elf_choice);
  });

  fmt::print("self deduct: {}\n", score);
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::accumulate;

  std::ifstream elves_inventory_list((INPUT_FILE));

  static constexpr std::array<std::array<int, 3>, 3> strat_table = {
    //         A  B  C
    std::array{3, 1, 2},  // X
    std::array{4, 5, 6},  // Y
    std::array{8, 9, 7},  // Z
  };

  auto lines = getlines(elves_inventory_list) | to_vector;
  auto score = accumulate(lines, 0, [](auto&& t_v, auto&& t_in) {
    std::size_t const elf_choice = static_cast<std::size_t>(t_in[0]) - static_cast<std::size_t>('A');
    std::size_t const elf_strat  = static_cast<std::size_t>(t_in[2]) - static_cast<std::size_t>('X');
    return t_v + strat_table.at(elf_strat).at(elf_choice);
  });

  fmt::print("elf strategy: {}\n", score);
}

int main(int /*unused*/, char** /*unused*/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}