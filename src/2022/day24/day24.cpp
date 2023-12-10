#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <limits>
#include <range/v3/action/sort.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view.hpp>
#include <utility>

using ranges::views::zip, ranges::views::repeat, ranges::views::cycle, ranges::views::drop, ranges::views::reverse;

using Coor = std::pair<std::size_t, std::size_t>;

template <char Sign>
struct GetMoveType {
  static auto operator()(std::vector<std::size_t> const& t_iter, Coor const t_init) noexcept {
    auto const [j, i] = t_init;

    if constexpr (Sign == '>') {
      return zip(cycle(t_iter) | drop(j - 1), repeat(i));
    }

    if constexpr (Sign == '<') {
      return zip(cycle(t_iter | reverse) | drop(t_iter.size() - j), repeat(i));
    }

    if constexpr (Sign == '^') {
      return zip(repeat(j), cycle(t_iter | reverse) | drop(t_iter.size() - i));
    }

    if constexpr (Sign == 'v') {
      return zip(repeat(j), cycle(t_iter) | drop(i - 1));
    }
  }

  using type = decltype(GetMoveType<Sign>::operator()({}, {}));
};

using Right = typename GetMoveType<'>'>::type;
using Left  = typename GetMoveType<'<'>::type;
using Up    = typename GetMoveType<'^'>::type;
using Down  = typename GetMoveType<'v'>::type;

template <char Sign, typename Rng = GetMoveType<Sign>::type>
class Blizzard {
 public:
  using Cycle     = ranges::cycled_view<Rng>;
  using CycleIter = ranges::iterator_t<Cycle>;

 private:
  [[nodiscard]] auto get_distance() const noexcept { return ranges::distance(begin(this->rng_), this->moving_coor_); }

  explicit Blizzard(Rng const& t_moving) : rng_(t_moving | ranges::views::cycle), moving_coor_(begin(rng_)) {}

 public:
  [[nodiscard]] char get_sign() const noexcept { return Sign; }
  void move() { ++this->moving_coor_; }

  [[nodiscard]] Coor get_current_pos() const noexcept {
    auto&& [x, y] = *this->moving_coor_;
    return Coor{x, y};
  }

  Cycle rng_;
  CycleIter moving_coor_;

  ~Blizzard()                          = default;
  Blizzard& operator=(const Blizzard&) = delete;
  Blizzard& operator=(Blizzard&&)      = delete;

  // we only need to copy view, not iterator, iterator will be invalidate once the view is copied to other location
  // (we can't move t_other.rng_ here, moved object is in unspecified state)
  Blizzard(Blizzard&& t_other) noexcept : rng_{t_other.rng_}, moving_coor_{begin(rng_) + t_other.get_distance()} {}
  Blizzard(Blizzard const& t_other) = delete;

  Blizzard(std::vector<std::size_t> const& t_iter, Coor const t_init)
    : Blizzard(GetMoveType<Sign>::operator()(t_iter, t_init)) {}
};

using Blizzards = std::tuple<std::vector<Blizzard<'>'>>, std::vector<Blizzard<'<'>>, std::vector<Blizzard<'^'>>,
                             std::vector<Blizzard<'v'>>>;

struct MapState {
  Coor my_pos_;
  std::size_t cycle_idx_ = 0;
  std::size_t time_      = 0;

  bool operator==(MapState const&) const = default;
};

auto parse_map(Blizzards& t_blizzards, std::vector<std::string> const& t_map, std::vector<std::size_t> const& t_row,
               std::vector<std::size_t> const& t_col) {
  Coor init_pos;
  Coor exit_pos;

  for (std::size_t i = 0; i < t_map.size(); ++i) {
    auto const& row = t_map[i];
    for (std::size_t j = 0; j < row.size(); ++j) {
      auto const current_coor = Coor(j, i);
      if (char const v = row[j]; v == 'E') [[unlikely]] {
        init_pos = current_coor;
      } else if (v != '.' and v != '#') [[likely]] {
        switch (v) {
          case '>':
            std::get<0>(t_blizzards).emplace_back(t_row, current_coor);
            break;
          case '<':
            std::get<1>(t_blizzards).emplace_back(t_row, current_coor);
            break;
          case '^':
            std::get<2>(t_blizzards).emplace_back(t_col, current_coor);
            break;
          case 'v':
            std::get<3>(t_blizzards).emplace_back(t_col, current_coor);
            break;
          default:
            std::unreachable();
        }
      } else if (v == '.' and i + 1 == t_map.size()) {
        exit_pos = current_coor;
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
  fmt::println("-- {} minutes ----", t_current_time);

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
    fmt::println("{}", row);
  }
}

auto get_blizzards_dir(Blizzards const& t_blizzards) {
  auto blizzard_dir_helper = [&]<std::size_t... I>(std::index_sequence<I...>) {
    using ranges::views::concat, ranges::views::cache1, ranges::to_vector, ranges::views::transform;

    return concat((std::get<I>(t_blizzards)                                                //
                   | transform(&std::tuple_element_t<I, Blizzards>::value_type::get_sign)  //
                   | cache1)                                                               //
                  ...) |
           to_vector;
  };

  return blizzard_dir_helper(std::make_index_sequence<std::tuple_size_v<Blizzards>>{});
}

auto get_blizzards_pos(Blizzards const& t_blizzards) {
  auto blizzard_pos_helper = [&]<std::size_t... I>(std::index_sequence<I...>) {
    using ranges::views::concat, ranges::to_vector, ranges::views::transform, ranges::views::cache1;

    return concat((std::get<I>(t_blizzards)                                                       //
                   | transform(&std::tuple_element_t<I, Blizzards>::value_type::get_current_pos)  //
                   | cache1)                                                                      //
                  ...) |
           to_vector;
  };

  return blizzard_pos_helper(std::make_index_sequence<std::tuple_size_v<Blizzards>>{});
}

auto move_blizzards(Blizzards& t_blizzards) {
  auto move_helper = [&]<std::size_t... I>(std::index_sequence<I...>) {
    (std::invoke(ranges::for_each, std::get<I>(t_blizzards), &std::tuple_element_t<I, Blizzards>::value_type::move),
     ...);
  };

  move_helper(std::make_index_sequence<std::tuple_size_v<Blizzards>>{});
}

class MoveSimulator {
  std::vector<std::string> map_;

  std::reference_wrapper<Blizzards> blizzards_;

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
  MoveSimulator(std::vector<std::string> t_map, Blizzards& t_blizzards)
    : map_{std::move(t_map)}, blizzards_{t_blizzards}, blizzard_pos_{get_blizzards_pos(blizzards_)} {}

  void reset() noexcept {
    this->current_min_ = std::numeric_limits<std::size_t>::max();
    this->history_.clear();
  }

  [[nodiscard]] auto get_result() const noexcept { return this->current_min_; }

  void simulate(Coor const t_current_pos, Coor const t_exit_pos, std::size_t const t_current_time = 1) {
    using ranges::views::transform, ranges::views::filter, ranges::to_vector, ranges::find_if, ranges::actions::sort;

    if (this->blizzard_pos_.size() <= t_current_time and this->cycle_ == 0) {
      // move blizzard first, then decide where we can go
      move_blizzards(this->blizzards_);

      if (auto pos = get_blizzards_pos(this->blizzards_);
          this->blizzard_pos_.empty() or this->blizzard_pos_.front() != pos) {
        this->blizzard_pos_.emplace_back(std::move(pos));
      } else {
        this->cycle_ = t_current_time;
        fmt::println("found cycle: {}", this->cycle_);
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

void debug_blizzard(std::vector<std::string> const& t_map, Blizzards& t_blizzards, std::size_t const t_turns) {
  for (std::size_t k = 0; k < t_turns; ++k) {
    print_blizzard(t_map, get_blizzards_pos(t_blizzards), get_blizzards_dir(t_blizzards), k);
    move_blizzards(t_blizzards);
  }

  print_blizzard(t_map, get_blizzards_pos(t_blizzards), get_blizzards_dir(t_blizzards), t_turns);
}

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::views::iota;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in) | to_vector;

  Blizzards blizzards;
  auto const row_vector                        = iota(1UL, rng.front().size() - 1) | to_vector;
  auto const col_vector                        = iota(1UL, rng.size() - 1) | to_vector;
  [[maybe_unused]] auto&& [init_pos, exit_pos] = parse_map(blizzards, rng, row_vector, col_vector);

  MoveSimulator simulator{rng, blizzards};
  simulator.simulate(init_pos, exit_pos);

  fmt::println("init -> exit: {}", simulator.get_result());
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::views::iota;

  std::fstream in((INPUT_FILE));
  auto const rng = getlines(in) | to_vector;

  Blizzards blizzards;
  auto const row_vector       = iota(1UL, rng.front().size() - 1) | to_vector;
  auto const col_vector       = iota(1UL, rng.size() - 1) | to_vector;
  auto&& [init_pos, exit_pos] = parse_map(blizzards, rng, row_vector, col_vector);

  MoveSimulator simulator{rng, blizzards};
  simulator.simulate(init_pos, exit_pos);
  auto total = simulator.get_result();
  fmt::println("init -> exit: {}", total);
  simulator.reset();

  simulator.simulate(exit_pos, init_pos, total + 1);
  fmt::println("exit -> init: {}", simulator.get_result());
  total = simulator.get_result();
  simulator.reset();

  simulator.simulate(init_pos, exit_pos, total + 1);
  fmt::println("init -> exit: {}", simulator.get_result());
}

int main(int /**/, char** /**/) {
  part1();
  // part2();

  return EXIT_SUCCESS;
}
