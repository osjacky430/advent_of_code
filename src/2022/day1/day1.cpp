#include <fmt/format.h>
#include <fstream>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/algorithm/partial_sort.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <string>
#include <vector>

auto to_list_of_calories(auto&& t_inputs) {
  using ranges::views::split, ranges::views::transform, ranges::accumulate;
  auto&& elves_calory_list =
    t_inputs     //
    | split("")  //
    | transform([](auto&& t_calories) {
        return accumulate(t_calories, 0, [](auto&& t_v, auto&& t_in) { return t_v + std::stoi(t_in); });
      });

  return elves_calory_list;
}

void part1() {
  using ranges::getlines, ranges::max;

  std::ifstream elves_inventory_list((INPUT_FILE));

  auto lines               = getlines(elves_inventory_list);
  auto list_of_calories    = to_list_of_calories(lines);
  auto elf_with_max_calory = max(list_of_calories);

  fmt::println("{}", elf_with_max_calory);
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::accumulate, ranges::partial_sort, ranges::views::take;

  std::ifstream elves_inventory_list((INPUT_FILE));

  auto lines = getlines(elves_inventory_list);
  auto rng   = to_list_of_calories(lines) | to_vector;

  partial_sort(rng, begin(rng) + 3, std::greater<>());

  auto sum_of_greatest_3 = accumulate(rng | take(3), 0);
  fmt::println("sum of greatest 3: {}", sum_of_greatest_3);
}

int main(int /*unused*/, char** /*unused*/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}