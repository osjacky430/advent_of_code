#include <cmath>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <limits>
#include <memory>
#include <range/v3/action/sort.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>
#include <tuple>
#include <utility>

using Coor = std::pair<std::size_t, std::size_t>;

// wanna try something I usually don't do
struct BlizzardBase {
  [[nodiscard]] BlizzardBase(const BlizzardBase&) = default;
  BlizzardBase(BlizzardBase&&)                    = delete;
  BlizzardBase& operator=(const BlizzardBase&)    = default;
  BlizzardBase& operator=(BlizzardBase&&)         = delete;

  [[nodiscard]] virtual char get_sign() const noexcept        = 0;
  virtual void move()                                         = 0;
  [[nodiscard]] virtual Coor get_current_pos() const noexcept = 0;

  virtual ~BlizzardBase() = default;
};

template <typename Rng>
struct Blizzard final : BlizzardBase {
  using Cycle     = ranges::cycled_view<Rng>;
  using CycleIter = decltype(begin((Cycle())));

  [[nodiscard]] char get_sign() const noexcept override { return this->sign_; }
  void move() override { ++this->moving_coor_; }

  [[nodiscard]] Coor get_current_pos() const noexcept override {
    auto&& [x, y] = *this->moving_coor_;
    return Coor{x, y};
  }

  char sign_;
  Cycle rng_;
  CycleIter moving_coor_;

  Blizzard(char const t_sign, Rng const& t_moving)
    : sign_{t_sign}, rng_(t_moving | ranges::views::cycle), moving_coor_(begin(rng_)) {}
};

struct MapState {
  Coor my_pos_;
  std::size_t cycle_idx_ = 0;
  std::size_t time_      = 0;

  bool operator==(MapState const&) const = default;
};

auto parse_map(std::vector<std::shared_ptr<BlizzardBase>>& blizzards, std::vector<std::string> const& t_map,
               std::vector<std::size_t> const& t_row, std::vector<std::size_t> const& t_col) {
  using ranges::views::zip, ranges::views::repeat, ranges::views::cycle, ranges::views::drop, ranges::views::reverse;

  Coor init_pos;
  Coor exit_pos;

  blizzards.reserve(t_map.size() * t_map.front().size());

  for (std::size_t i = 0; i < t_map.size(); ++i) {
    auto const& row = t_map[i];
    for (std::size_t j = 0; j < row.size(); ++j) {
      if (char const v = row[j]; v == 'E') [[unlikely]] {
        init_pos = Coor(j, i);
      } else if (v != '.' and v != '#') [[likely]] {
        switch (v) {
          case '>':
            using TRight = decltype(Blizzard{v, zip(cycle(t_row) | drop(j - 1), repeat(i))});
            blizzards.emplace_back(std::make_shared<TRight>(v, zip(cycle(t_row) | drop(j - 1), repeat(i))));
            break;
          case '<':
            using TLeft = decltype(Blizzard{v, zip(cycle(t_row | reverse) | drop(j - 1), repeat(i))});
            blizzards.emplace_back(
              std::make_shared<TLeft>(v, zip(cycle(t_row | reverse) | drop(t_row.size() - j), repeat(i))));
            break;
          case '^':
            using TUp = decltype(Blizzard{v, zip(repeat(j), cycle(t_col | reverse) | drop(i - 1))});
            blizzards.emplace_back(
              std::make_shared<TUp>(v, zip(repeat(j), cycle(t_col | reverse) | drop(t_col.size() - i))));
            break;
          case 'v':
            using TDown = decltype(Blizzard{v, zip(repeat(j), cycle(t_col) | drop(i - 1))});
            blizzards.emplace_back(std::make_shared<TDown>(v, zip(repeat(j), cycle(t_col) | drop(i - 1))));
            break;
          default:
            break;
        }
      } else if (v == '.' and i + 1 == t_map.size()) {
        exit_pos = Coor(j, i);
      }
    }
  }

  return std::pair(init_pos, exit_pos);
}

inline constexpr std::array<std::pair<bool (*)(Coor, std::vector<std::string> const&), Coor (*)(Coor)>, 5> MOVE{
  // right
  std::pair{[](Coor t_current, std::vector<std::string> const& t_map) {
              auto [x, y] = t_current;
              return x + 1 < t_map[y].size() and t_map[y][x + 1] == '.';
            },
            [](Coor t_coor) { return ++t_coor.first, t_coor; }},
  // left
  std::pair{[](Coor t_current, std::vector<std::string> const& t_map) {
              auto [x, y] = t_current;
              return x >= 1 and t_map[y][x - 1] == '.';
            },
            [](Coor t_coor) { return --t_coor.first, t_coor; }},
  // down
  std::pair{[](Coor t_current, std::vector<std::string> const& t_map) {
              auto [x, y] = t_current;
              return y + 1 < t_map.size() and t_map[y + 1][x] == '.';
            },
            [](Coor t_coor) { return ++t_coor.second, t_coor; }},
  // up
  std::pair{[](Coor t_current, std::vector<std::string> const& t_map) {
              auto [x, y] = t_current;
              return y >= 1 and t_map[y - 1][x] == '.';
            },
            [](Coor t_coor) { return --t_coor.second, t_coor; }},
  // wait
  std::pair{[](Coor t_current, std::vector<std::string> const& t_map) {
              auto [x, y] = t_current;
              return t_map[y][x] == '.';
            },
            [](Coor t_coor) { return t_coor; }},
};

void print_blizzard(std::vector<std::string> t_map, std::vector<Coor> const& t_blizzards_pos,
                    std::vector<char> const& t_blizzards_dir, std::size_t const t_current_time,
                    Coor const& t_current_pos = {1, 0}) {
  fmt::print("-- {} minutes ----\n", t_current_time);

  for (std::size_t i = 1; i < t_map.size() - 1; ++i) {
    for (std::size_t j = 1; j < t_map[i].size() - 1; ++j) {
      t_map[i][j] = '.';
    }
  }

  for (std::size_t i = 0; i < t_blizzards_pos.size(); ++i) {
    auto&& [x, y] = t_blizzards_pos[i];
    auto&& dir    = t_blizzards_dir[i];

    if (t_map[y][x] != '.') {
      if (t_map[y][x] == '>' or t_map[y][x] == '^' or t_map[y][x] == '<' or t_map[y][x] == 'v') {
        t_map[y][x] = '2';
      } else {
        ++t_map[y][x];
      }
    } else {
      t_map[y][x] = dir;
    }
  }

  auto&& [c_x, c_y] = t_current_pos;
  t_map[c_y][c_x]   = 'E';

  for (auto&& row : t_map) {
    fmt::print("{}\n", row);
  }
}

auto get_blizzards_dir(std::vector<std::shared_ptr<BlizzardBase>> const& t_blizzards) {
  using ranges::to_vector, ranges::views::indirect, ranges::views::transform;
  return t_blizzards | indirect | transform(&BlizzardBase::get_sign) | to_vector;
}

auto get_blizzards_pos(std::vector<std::shared_ptr<BlizzardBase>> const& t_blizzards) {
  using ranges::to_vector, ranges::views::indirect, ranges::views::transform;
  return t_blizzards | indirect | transform(&BlizzardBase::get_current_pos) | to_vector;
}

auto move_blizzards(std::vector<std::shared_ptr<BlizzardBase>> const& t_blizzards) {
  using ranges::for_each, ranges::views::indirect;
  for_each(t_blizzards | indirect, &BlizzardBase::move);
}

class MoveSimulator {
  std::vector<std::string> map_;

  std::vector<std::shared_ptr<BlizzardBase>> blizzards_;

  std::size_t cycle_       = 0;
  std::size_t current_min_ = std::numeric_limits<std::size_t>::max();

  Coor map_size_{map_.front().size(), map_.size()};
  std::vector<std::vector<Coor>> blizzard_pos_;
  std::vector<MapState> history_;

  void update_map(std::vector<Coor> const& t_blizzards_pos, Coor const t_exit_pos) {
    using ranges::subrange;

    this->map_[t_exit_pos.second][t_exit_pos.first] = '.';
    for (auto&& row : subrange{this->map_.begin() + 1, this->map_.end() - 1}) {
      for (auto&& elem : subrange{row.begin() + 1, row.end() - 1}) {
        elem = '.';
      }
    }

    for (auto&& [x, y] : t_blizzards_pos) {
      this->map_[y][x] = 'x';
    }
  }

 public:
  MoveSimulator(std::vector<std::string> t_map, std::vector<std::shared_ptr<BlizzardBase>> t_blizzards)
    : map_{std::move(t_map)}, blizzards_{std::move(t_blizzards)}, blizzard_pos_{get_blizzards_pos(blizzards_)} {}

  void reset() noexcept {
    this->current_min_ = std::numeric_limits<std::size_t>::max();
    this->history_.clear();
  }

  [[nodiscard]] auto get_result() const noexcept { return this->current_min_; }

  void simulate(Coor const t_current_pos, Coor const t_exit_pos, std::size_t const t_current_time = 1) {
    using ranges::views::indirect, ranges::views::transform, ranges::views::filter, ranges::to_vector, ranges::find_if,
      ranges::actions::sort;

    if (this->blizzard_pos_.size() <= t_current_time and this->cycle_ == 0) {
      // move blizzard first, then decide where we can go
      move_blizzards(this->blizzards_);

      if (auto pos = get_blizzards_pos(this->blizzards_);
          this->blizzard_pos_.empty() or this->blizzard_pos_.front() != pos) {
        this->blizzard_pos_.emplace_back(std::move(pos));
      } else {
        this->cycle_ = t_current_time;
        fmt::print("found cycle: {}\n", this->cycle_);
      }
    }

    auto const blizzard_idx = this->cycle_ == 0 ? t_current_time : t_current_time % this->cycle_;
    if (auto current_state = MapState{t_current_pos, blizzard_idx, t_current_time};
        t_current_time >= this->current_min_ or find_if(this->history_, [&](auto&& t_state) {
                                                  return t_state.my_pos_ == current_state.my_pos_ and
                                                         t_state.cycle_idx_ == current_state.cycle_idx_ and
                                                         t_state.time_ <= current_state.time_;
                                                }) != this->history_.end()) {
      return;
    }

    this->history_.emplace_back(t_current_pos, blizzard_idx, t_current_time);
    this->update_map(this->blizzard_pos_[blizzard_idx], t_exit_pos);
    auto const candidates =
      MOVE                                                                                //
      | filter([&](auto&& t_mover) { return t_mover.first(t_current_pos, this->map_); })  //
      | transform([&](auto&& t_mover) { return t_mover.second(t_current_pos); })          //
      | to_vector                                                                         //
      | sort(std::less<>{}, [&](auto&& t_coor) {
          return std::max(t_coor.first, t_exit_pos.first) - std::min(t_coor.first, t_exit_pos.first) +
                 std::max(t_coor.second, t_exit_pos.second) - std::min(t_coor.second, t_exit_pos.second);
        });

    for (auto&& pos : candidates) {
      if (pos == t_exit_pos) {
        this->current_min_ = t_current_time;
        return;
      }

      this->simulate(pos, t_exit_pos, t_current_time + 1);
    }
  }
};

void debug_blizzard(std::vector<std::string> const& t_map,
                    std::vector<std::shared_ptr<BlizzardBase>> const& t_blizzards, std::size_t const t_turns) {
  using ranges::for_each, ranges::views::indirect;

  for (std::size_t k = 0; k < t_turns; ++k) {
    move_blizzards(t_blizzards);
    print_blizzard(t_map, get_blizzards_pos(t_blizzards), get_blizzards_dir(t_blizzards), k + 1);
  }
}

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::views::iota;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in) | to_vector;

  std::vector<std::shared_ptr<BlizzardBase>> blizzards;
  auto const row_vector       = iota(1UL, rng.front().size() - 1) | to_vector;
  auto const col_vector       = iota(1UL, rng.size() - 1) | to_vector;
  auto&& [init_pos, exit_pos] = parse_map(blizzards, rng, row_vector, col_vector);

  MoveSimulator simulator{rng, blizzards};
  simulator.simulate(init_pos, exit_pos);
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::views::iota;

  std::fstream in((INPUT_FILE));
  auto const rng = getlines(in) | to_vector;

  std::vector<std::shared_ptr<BlizzardBase>> blizzards;
  auto const row_vector       = iota(1UL, rng.front().size() - 1) | to_vector;
  auto const col_vector       = iota(1UL, rng.size() - 1) | to_vector;
  auto&& [init_pos, exit_pos] = parse_map(blizzards, rng, row_vector, col_vector);

  MoveSimulator simulator{rng, blizzards};
  simulator.simulate(init_pos, exit_pos);
  auto total = simulator.get_result();
  fmt::print("init -> exit: {}\n", total);
  simulator.reset();

  simulator.simulate(exit_pos, init_pos, total + 1);
  fmt::print("exit -> init: {}\n", simulator.get_result());
  total = simulator.get_result();
  simulator.reset();

  simulator.simulate(init_pos, exit_pos, total + 1);
  fmt::print("init -> exit: {}\n", simulator.get_result());
}

int main(int /**/, char** /**/) {
  // part1();
  part2();

  return EXIT_SUCCESS;
}
