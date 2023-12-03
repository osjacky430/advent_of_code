#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/reverse.hpp>
#include <utility>
#include <vector>

static inline constexpr auto CHAMBER_WIDTH = 7;
static inline constexpr auto DROP_POINT_X  = 2;  // 0 based index
static inline constexpr auto DROP_POINT_Y  = 3;

struct Coor {
  long long x = 0;
  long long y = 0;

  [[nodiscard]] Coor left() const noexcept { return Coor{x - 1, y}; }
  [[nodiscard]] Coor right() const noexcept { return Coor{x + 1, y}; }
  [[nodiscard]] Coor down() const noexcept { return Coor{x, y - 1}; }

  void move_left() noexcept { --x; }
  void move_right() noexcept { ++x; }
  void move_down() noexcept { --y; }
};

struct State {
  std::size_t rock_idx_;
  std::ptrdiff_t jet_idx_;
  std::array<int, CHAMBER_WIDTH> surface_;
  long long rock_fell_;
  long long height_;

  bool operator==(State const& t_rhs) const noexcept {
    return this->rock_idx_ == t_rhs.rock_idx_ and this->jet_idx_ == t_rhs.jet_idx_ and this->surface_ == t_rhs.surface_;
  }
};

using RockFallHistory = std::vector<State>;

struct Rock {
  std::vector<Coor> positions_;  // starting from bottom left
};

struct Chamber {
  // initial condition, 4 because the "longest" rock has the height of 4, we can shrink this to include only fallable
  // surface, e.g.
  //
  //      [.......]
  //      [.#...##]
  //      [###..##]
  //      [.#####.] <- this layer is not fallable, we can remove this in the content
  //
  // however, if we want to see the history, then we need to keep those
  std::vector<std::uint8_t> content_ = std::vector<std::uint8_t>(4, 0);

  [[nodiscard]] auto get_surface() const noexcept {
    std::array<int, CHAMBER_WIDTH> surface{-1, -1, -1, -1, -1, -1, -1};
    for (std::size_t j = 0; j < this->content_.size(); ++j) {
      for (std::size_t i = 0; i < CHAMBER_WIDTH; ++i) {
        if (surface[i] != -1) {
          continue;
        }

        if ((this->content_[this->content_.size() - 1 - j] & (1UL << i)) != 0) {
          surface[i] = static_cast<int>(j);
        }
      }
    }
    return surface;
  }

  bool check_collision(Rock const& t_rock, Coor const t_coor) {
    auto [x, y] = t_coor;
    if (x > CHAMBER_WIDTH - 1 or x < 0 or y < 0) {
      return true;
    }

    for (auto&& [rock_x, rock_y] : t_rock.positions_) {
      auto const offset_x = static_cast<std::size_t>(rock_x + x);
      auto const offset_y = static_cast<std::size_t>(rock_y + y);
      if (offset_x > CHAMBER_WIDTH - 1) {
        return true;
      }

      if (offset_y >= this->content_.size()) {
        break;
      }

      if ((this->content_[offset_y] & (1UL << offset_x)) != 0) {
        return true;
      }
    }

    return false;
  }

  void mark_rock(Coor const t_coor, Rock const& t_rock) {
    auto const [x, y] = t_coor;
    for (auto&& [rock_x, rock_y] : t_rock.positions_) {
      auto const offset_x = static_cast<std::size_t>(rock_x + x);
      auto const offset_y = static_cast<std::size_t>(rock_y + y);

      while (offset_y + 3 >= this->content_.size()) {
        this->content_.emplace_back();
      }

      this->content_[offset_y] |= static_cast<std::uint8_t>(1U << offset_x);
    }
  }

  void print_chamber(std::filesystem::path const& t_path) const noexcept {
    std::fstream debug_txt(t_path.string());

    using ranges::views::reverse;
    for (auto&& row : this->content_ | reverse) {
      debug_txt << '|';
      for (std::size_t i = 0; i < CHAMBER_WIDTH; ++i) {
        debug_txt << ((row & (1UL << i)) != 0 ? '#' : '.');
      }
      debug_txt << "|\n";
    }

    debug_txt << "+-------+\n\n";
  }
};

std::array const rocks_order{
  // this can be constexpr, but need to replace vector
  Rock{{{0, 0}, {1, 0}, {2, 0}, {3, 0}}},         Rock{{{1, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 2}}},
  Rock{{{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}}}, Rock{{{0, 0}, {0, 1}, {0, 2}, {0, 3}}},
  Rock{{{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
};

auto rock_falling_sim(std::string const& t_jet_input, long long const t_rock_fell) {
  using ranges::views::cycle, ranges::begin, ranges::distance, ranges::find;

  Chamber chamber;
  RockFallHistory history;

  long long unit_tall = 0;

  auto jet_input_rng = cycle(t_jet_input);
  auto dir           = begin(jet_input_rng);
  for (long long rock_num = 0; rock_num != t_rock_fell; ++rock_num) {
    auto const idx     = static_cast<std::size_t>(rock_num) % rocks_order.size();
    auto const jet_idx = distance(begin(jet_input_rng), dir) % static_cast<int>(t_jet_input.size());
    auto const& rock   = rocks_order[idx];

    auto rock_coor = Coor{DROP_POINT_X, unit_tall + DROP_POINT_Y};

    auto state = State{
      .rock_idx_  = idx,
      .jet_idx_   = jet_idx,
      .surface_   = chamber.get_surface(),
      .rock_fell_ = rock_num,
      .height_    = unit_tall,
    };

    if (auto iter = find(history, state); iter != history.end()) {
      auto const& pattern       = *iter;
      auto const height_cycle   = unit_tall - pattern.height_;
      auto const rock_num_cycle = rock_num - pattern.rock_fell_;

      auto const cycle_num               = (t_rock_fell - pattern.rock_fell_) / rock_num_cycle;
      auto const rock_left_after_pattern = (t_rock_fell - pattern.rock_fell_) % rock_num_cycle;

      auto const after_pattern_idx    = static_cast<std::size_t>(iter - history.begin() + rock_left_after_pattern);
      auto const height_after_pattern = history[after_pattern_idx].height_;
      return height_cycle * cycle_num + height_after_pattern;
    }

    history.push_back(state);

    for (;; ++dir) {
      if (*dir == '>' and not chamber.check_collision(rock, rock_coor.right())) {
        rock_coor.move_right();
      } else if (*dir == '<' and not chamber.check_collision(rock, rock_coor.left())) {
        rock_coor.move_left();
      }

      if (chamber.check_collision(rock, rock_coor.down())) {
        unit_tall = std::max(unit_tall, rock_coor.y + rock.positions_.back().y + 1);
        chamber.mark_rock(rock_coor, rock);

        ++dir;
        break;
      }

      rock_coor.move_down();
    }
  }

  return unit_tall;
}

void part1() {
  using ranges::getlines, ranges::begin;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);

  fmt::println("units tall: {}", rock_falling_sim(*begin(rng), 2022));
}

void part2() {
  using ranges::getlines, ranges::begin;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);

  fmt::println("units tall: {}", rock_falling_sim(*begin(rng), 1'000'000'000'000));
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
