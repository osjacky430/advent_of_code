#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <range/v3/action/sort.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <string>
#include <vector>

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::views::split, ranges::max, ranges::views::transform,
    ranges::accumulate;

  std::ifstream elves_inventory_list((INPUT_FILE));

  auto lines = getlines(elves_inventory_list) | to_vector;

  auto list_of_calories =
    lines        //
    | split("")  //
    | transform([](auto&& t_calories) {
        return accumulate(t_calories, 0, [](auto&& t_v, auto&& t_in) { return t_v + std::stoi(t_in); });
      });

  auto elf_with_max_calory = max(list_of_calories);
  fmt::print("{}\n", elf_with_max_calory);
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::views::split, ranges::max, ranges::views::transform,
    ranges::accumulate, ranges::actions::sort, ranges::views::take;

  std::ifstream elves_inventory_list((INPUT_FILE));  //

  auto lines = getlines(elves_inventory_list) | to_vector;

  auto rng = lines        //
             | split("")  //
             | transform([](auto&& t_calories) {
                 return accumulate(t_calories, 0, [](auto&& t_v, auto&& t_in) { return t_v + std::stoi(t_in); });
               })                          //
             | to_vector                   //
             | sort(std::greater<int>());  // partial_sort will be better!

  auto sum_of_greatest_3 = accumulate(rng | take(3), 0);
  fmt::print("sum of greatest 3: {}\n", sum_of_greatest_3);
}

int main(int /*unused*/, char** /*unused*/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}