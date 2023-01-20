#include "split_helper.hpp"
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/getlines.hpp>

void parse_assembly() {}

void part1() {
  using ranges::getlines, ranges::accumulate;

  std::ifstream in((INPUT_FILE));

  auto rng = getlines(in);

  std::vector<int> x_value{1};
  for (auto&& instr : rng) {
    auto decomposed_cmd = split_string(instr);
    if (decomposed_cmd[0] == "noop") {
      x_value.push_back(x_value.back());
    } else {
      x_value.push_back(x_value.back());
      x_value.push_back(x_value.back() + std::stoi(decomposed_cmd[1]));
    }
  }

  int sum_result = 0;
  for (auto&& i : {20UL, 60UL, 100UL, 140UL, 180UL, 220UL}) {
    sum_result += x_value[i - 1] * static_cast<int>(i);
  }

  fmt::print("sum of signal strength: {}\n", sum_result);
}

void update_crt(std::string& t_row, int const t_cycle, int const t_sprite_pos) {
  if ((t_sprite_pos - 1 <= t_cycle) and (t_cycle <= t_sprite_pos + 1)) {
    t_row[static_cast<std::size_t>(t_cycle)] = '#';
  }
}

void part2() {
  using ranges::getlines, ranges::accumulate;

  std::ifstream in((INPUT_FILE));

  auto rng = getlines(in);

  std::array<std::string, 6> crt;
  crt.fill("........................................");

  int sprite_position = 1;

  for (std::size_t cycle = 0; auto&& instr : rng) {
    auto const decomposed_cmd = split_string(instr);

    ++cycle;
    update_crt(crt[(cycle - 1UL) / 40UL], (static_cast<int>(cycle) - 1) % 40, sprite_position);
    if (decomposed_cmd[0] == "addx") {
      ++cycle;
      update_crt(crt[(cycle - 1UL) / 40UL], (static_cast<int>(cycle) - 1) % 40, sprite_position);
      sprite_position += stoi(std::string(decomposed_cmd[1]));
    }
  }

  for (auto&& row : crt) {
    fmt::print("{}\n", row);
  }
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}