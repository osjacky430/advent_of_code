#include "string_util.hpp"
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/getlines.hpp>

void part1() {
  using ranges::getlines, ranges::accumulate;

  std::ifstream in((INPUT_FILE));

  auto rng = getlines(in);

  std::vector<int> x_value{1};
  for (auto&& instr : rng) {
    x_value.push_back(x_value.back());
    if (auto decomposed_cmd = split_string(instr); decomposed_cmd[0] != "noop") {
      x_value.push_back(x_value.back() + std::stoi(decomposed_cmd[1]));
    }
  }

  static constexpr std::array CYCLE_NUMS = {20UL, 60UL, 100UL, 140UL, 180UL, 220UL};

  fmt::println("sum of signal strength: {}", accumulate(CYCLE_NUMS, 0, [x_value](int t_sum, std::size_t t_cycle) {
                 return t_sum + x_value[t_cycle - 1] * static_cast<int>(t_cycle);
               }));
}

void update_crt(std::string& t_row, int const t_cycle, int const t_sprite_pos) {
  if ((t_sprite_pos - 1 <= t_cycle) and (t_cycle <= t_sprite_pos + 1)) {
    t_row[static_cast<std::size_t>(t_cycle)] = '#';
  }
}

void part2() {
  using ranges::getlines;

  static constexpr auto HEIGHT = 6;
  static constexpr auto WIDTH  = 40;

  std::ifstream in((INPUT_FILE));

  auto rng = getlines(in);

  std::array<std::string, HEIGHT> crt;
  crt.fill("........................................");

  int sprite_position = 1;

  for (std::size_t cycle = 0; auto&& instr : rng) {
    ++cycle;
    update_crt(crt[(cycle - 1UL) / WIDTH], (static_cast<int>(cycle) - 1) % WIDTH, sprite_position);
    if (auto const decomposed_cmd = split_string(instr); decomposed_cmd[0] == "addx") {
      ++cycle;
      update_crt(crt[(cycle - 1UL) / WIDTH], (static_cast<int>(cycle) - 1) % WIDTH, sprite_position);
      sprite_position += stoi(std::string(decomposed_cmd[1]));
    }
  }

  for (auto&& row : crt) {
    fmt::println("{}", row);
  }
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}