#include "pairview.hpp"
#include "string_util.hpp"
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <map>
#include <queue>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/algorithm/minmax.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/transform.hpp>
#include <set>
#include <string>
#include <vector>

using Plane = std::pair<int, int>;

struct Cube {
  int x;
  int y;
  int z;

  [[nodiscard]] Plane x_plane() const noexcept { return Plane{this->y, this->z}; }
  [[nodiscard]] Plane y_plane() const noexcept { return Plane{this->x, this->z}; }
  [[nodiscard]] Plane z_plane() const noexcept { return Plane{this->x, this->y}; }

  [[nodiscard]] int x_coor() const noexcept { return this->x; }
  [[nodiscard]] int y_coor() const noexcept { return this->y; }
  [[nodiscard]] int z_coor() const noexcept { return this->z; }

  auto operator<=>(Cube const&) const = default;
};

auto get_surface_area(std::vector<Cube> const& t_droplets, auto&& t_proj) {
  using ranges::views::transform, ranges::sort, ranges::count_if;
  std::map<Plane, std::vector<int>> projections;
  for (auto&& [plane, height] : t_droplets | transform(t_proj)) {
    projections[plane].push_back(height);
  }

  auto ret_val = projections.size();  // outer surface
  for (auto&& [_, heights] : projections) {
    sort(heights);

    auto const concave = count_if(heights | ranges::views::pairviewer, [](auto&& t_rng) {
      auto const [first, second] = t_rng;
      return second - first != 1;
    });

    ret_val += static_cast<std::size_t>(concave);
  }

  return ret_val * 2;
}

auto const to_cube = [](auto&& t_str) {
  auto const num_str = split_string(t_str, ',');
  return Cube{.x = std::stoi(num_str[0]), .y = std::stoi(num_str[1]), .z = std::stoi(num_str[2])};
};

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::views::transform;

  std::fstream in((INPUT_FILE));
  auto const droplets = getlines(in) | transform(to_cube) | to_vector;

  auto x_dir = get_surface_area(droplets, [](auto&& t_d) { return std::pair{t_d.x_plane(), t_d.x}; });
  auto y_dir = get_surface_area(droplets, [](auto&& t_d) { return std::pair{t_d.y_plane(), t_d.y}; });
  auto z_dir = get_surface_area(droplets, [](auto&& t_d) { return std::pair{t_d.z_plane(), t_d.z}; });

  fmt::println("surface area: {}", x_dir + y_dir + z_dir);
}

auto const get_neighbor = [](Cube const& t_queried) {
  return std::array{
    Cube{t_queried.x + 1, t_queried.y, t_queried.z}, Cube{t_queried.x - 1, t_queried.y, t_queried.z},
    Cube{t_queried.x, t_queried.y + 1, t_queried.z}, Cube{t_queried.x, t_queried.y - 1, t_queried.z},
    Cube{t_queried.x, t_queried.y, t_queried.z + 1}, Cube{t_queried.x, t_queried.y, t_queried.z - 1},
  };
};

std::set<Cube> flood_fill(std::set<Cube> const& t_droplets) {
  using ranges::minmax;
  auto const [min_x, max_x] = minmax(t_droplets, std::less<>{}, &Cube::x_coor);
  auto const [min_y, max_y] = minmax(t_droplets, std::less<>{}, &Cube::y_coor);
  auto const [min_z, max_z] = minmax(t_droplets, std::less<>{}, &Cube::z_coor);

  Cube const bottom_left_corner{min_x.x - 1, min_y.y - 1, min_z.z - 1};
  Cube const upper_right_corner{max_x.x + 1, max_y.y + 1, max_z.z + 1};

  std::set<Cube> water;

  std::queue<Cube> bfs;
  bfs.emplace(min_x.x, min_y.y, min_z.z);

  auto const is_in_bound = [&](Cube const& t_queried) {
    return bottom_left_corner.x <= t_queried.x and t_queried.x <= upper_right_corner.x and
           bottom_left_corner.y <= t_queried.y and t_queried.y <= upper_right_corner.y and
           bottom_left_corner.z <= t_queried.z and t_queried.z <= upper_right_corner.z;
  };

  while (not bfs.empty()) {
    auto const curr = bfs.front();
    bfs.pop();

    if (water.contains(curr)) {
      continue;
    }

    water.insert(curr);

    for (auto&& neighbor : get_neighbor(curr)) {
      if (not t_droplets.contains(neighbor) and not water.contains(neighbor) and is_in_bound(neighbor)) {
        bfs.push(neighbor);
      }
    }
  }

  return water;
}

void part2() {
  using ranges::getlines, ranges::views::transform, ranges::to;

  std::fstream in((INPUT_FILE));
  auto const droplets = getlines(in) | transform(to_cube) | to<std::set>;

  auto const water = flood_fill(droplets);

  int area = 0;
  for (auto const& lava : droplets) {
    for (auto&& neighbor : get_neighbor(lava)) {
      area += static_cast<int>(not droplets.contains(neighbor) and water.contains(neighbor));
    }
  }

  fmt::println("surface area: {}", area);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
