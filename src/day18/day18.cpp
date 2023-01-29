#include "split_helper.hpp"
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <map>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/sliding.hpp>
#include <range/v3/view/transform.hpp>
#include <string>
#include <vector>

struct Plane {
  int first;
  int second;

  auto operator<=>(Plane const&) const = default;
};

struct Cube {
  int x;
  int y;
  int z;

  Plane x_plane() const noexcept { return Plane{this->y, this->z}; }
  Plane y_plane() const noexcept { return Plane{this->x, this->z}; }
  Plane z_plane() const noexcept { return Plane{this->x, this->y}; }
};

auto get_surface_area(std::vector<Cube> const& t_droplets, auto&& t_proj) {
  using ranges::views::transform, ranges::sort, ranges::count_if, ranges::views::sliding;
  std::map<Plane, std::vector<int>> projections;
  for (auto&& droplet : t_droplets) {
    auto&& [plane, height] = std::invoke(t_proj, droplet);
    projections[plane].push_back(height);
  }

  auto ret_val = projections.size();  // outer surface
  for (auto&& [plane, heights] : projections) {
    sort(heights);

    auto const concave = count_if(heights | sliding(2), [](auto&& t_rng) {
      auto const first  = *begin(t_rng);
      auto const second = *next(begin(t_rng));
      return second - first != 1;
    });

    ret_val += static_cast<std::size_t>(concave);
  }

  return ret_val * 2;
}

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::views::transform;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in) | transform([](auto&& t_str) {
               auto const num_str = split_string(t_str, ',');
               return Cube{.x = std::stoi(num_str[0]), .y = std::stoi(num_str[1]), .z = std::stoi(num_str[2])};
             }) |
             to_vector;

  auto x_dir = get_surface_area(rng, [](auto&& t_d) { return std::pair{t_d.x_plane(), t_d.x}; });
  auto y_dir = get_surface_area(rng, [](auto&& t_d) { return std::pair{t_d.y_plane(), t_d.y}; });
  auto z_dir = get_surface_area(rng, [](auto&& t_d) { return std::pair{t_d.z_plane(), t_d.z}; });

  fmt::print("surface area: {}\n", x_dir + y_dir + z_dir);
}

void part2() {
  using ranges::getlines;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
