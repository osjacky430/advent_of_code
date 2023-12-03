#include <array>
#include <cmath>
#include <cstddef>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <map>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <set>
#include <utility>
#include <vector>

struct Elf;

using Coor    = std::pair<int, int>;
using DirPred = std::pair<bool (*)(std::set<Coor> const&, Elf const&), Coor (*)(Coor)>;

struct Elf {
  bool conflicted_proposal_ = false;
  bool have_neighbor_       = false;
  DirPred predicate_        = {nullptr, nullptr};
  Coor coor_;

  [[nodiscard]] Coor get_coor() const noexcept { return this->coor_; }

  bool operator==(Elf const& t_elf) const noexcept { return this->coor_ == t_elf.coor_; }

  explicit Elf(Coor t_coor) : coor_(std::move(t_coor)) {}
};

template <char Dir>
inline constexpr auto adjacent = [](Coor const& t_to_check) {
  //    N
  //  W + E
  //    S

  auto&& [x_check, y_check] = t_to_check;
  if constexpr (Dir == 'N') {
    return std::array{Coor{x_check - 1, y_check - 1}, Coor{x_check, y_check - 1}, Coor{x_check + 1, y_check - 1}};
  } else if constexpr (Dir == 'S') {
    return std::array{Coor{x_check - 1, y_check + 1}, Coor{x_check, y_check + 1}, Coor{x_check + 1, y_check + 1}};
  } else if constexpr (Dir == 'W') {
    return std::array{Coor{x_check - 1, y_check - 1}, Coor{x_check - 1, y_check}, Coor{x_check - 1, y_check + 1}};
  } else if constexpr (Dir == 'E') {
    return std::array{Coor{x_check + 1, y_check - 1}, Coor{x_check + 1, y_check}, Coor{x_check + 1, y_check + 1}};
  }

  std::unreachable();
};

inline constexpr std::array<DirPred, 4> MOVE_ORDER = {
  DirPred{[](std::set<Coor> const& t_elves, Elf const& t_elf_to_check) {
            return ranges::none_of(adjacent<'N'>(t_elf_to_check.coor_),
                                   [&](auto&& t_coor) { return t_elves.contains(t_coor); });
          },
          [](Coor t_v) { return --t_v.second, t_v; }},  // abomination
  DirPred{[](std::set<Coor> const& t_elves, Elf const& t_elf_to_check) {
            return ranges::none_of(adjacent<'S'>(t_elf_to_check.coor_),
                                   [&](auto&& t_coor) { return t_elves.contains(t_coor); });
          },
          [](Coor t_v) { return ++t_v.second, t_v; }},
  DirPred{[](std::set<Coor> const& t_elves, Elf const& t_elf_to_check) {
            return ranges::none_of(adjacent<'W'>(t_elf_to_check.coor_),
                                   [&](auto&& t_coor) { return t_elves.contains(t_coor); });
          },
          [](Coor t_v) { return --t_v.first, t_v; }},
  DirPred{[](std::set<Coor> const& t_elves, Elf const& t_elf_to_check) {
            return ranges::none_of(adjacent<'E'>(t_elf_to_check.coor_),
                                   [&](auto&& t_coor) { return t_elves.contains(t_coor); });
          },
          [](Coor t_v) { return ++t_v.first, t_v; }},
};

void debug_print(std::vector<Elf> const& t_elves, std::size_t const t_round) {
  using ranges::minmax;

  auto&& [min_x, max_x] = minmax(t_elves, std::less<>{}, [](auto const& t_elf) { return t_elf.get_coor().first; });
  auto&& [min_y, max_y] = minmax(t_elves, std::less<>{}, [](auto const& t_elf) { return t_elf.get_coor().second; });

  auto const x_size = static_cast<std::size_t>(max_x.coor_.first - min_x.coor_.first) + 1;
  auto const y_size = static_cast<std::size_t>(max_y.coor_.second - min_y.coor_.second) + 1;

  std::vector<std::string> map(y_size, std::string(x_size, '.'));
  for (auto&& elf : t_elves) {
    auto&& [elf_x, elf_y] = elf.get_coor();
    auto const x_idx      = static_cast<std::size_t>(elf_x - min_x.coor_.first);
    auto const y_idx      = static_cast<std::size_t>(elf_y - min_y.coor_.second);

    map[y_idx][x_idx] = '#';
  }

  for (auto&& row : map) {
    fmt::println("{}", row);
  }

  fmt::println("== End of Round {} == ", t_round);
}

void move(std::vector<Elf>& t_elves, auto&& t_direction_predicate) {
  using ranges::find_if, ranges::views::transform, ranges::to, ranges::views::drop;

  auto const elf_position = t_elves | transform(&Elf::get_coor) | to<std::set>;
  std::map<Coor, Elf&> proposed_position;

  for (auto&& elf : t_elves) {
    auto const no_neighbor   = [&](auto&& t_pred) { return (*t_pred.first)(elf_position, elf); };
    auto const have_neighbor = [&](auto&& t_pred) { return not no_neighbor(t_pred); };

    elf.conflicted_proposal_ = false;
    auto const move_result   = [&]() {
      auto const begin         = ranges::begin(t_direction_predicate);
      auto const end           = ranges::end(t_direction_predicate);
      auto const first_invalid = find_if(t_direction_predicate, have_neighbor);
      if (first_invalid == end) {
        return DirPred{nullptr, nullptr};
      }

      if (first_invalid != begin) {
        return *begin;
      }

      auto const first_valid = find_if(ranges::next(first_invalid), end, no_neighbor);
      if (first_valid != end) {
        return *first_valid;
      }

      return DirPred{nullptr, nullptr};  // neighbor everywhere
    }();

    elf.predicate_          = move_result;
    auto&& [checker, mover] = move_result;
    if (checker == nullptr and mover == nullptr) {
      continue;
    }

    if (auto coor = (*mover)(elf.coor_); not proposed_position.contains(coor)) {
      proposed_position.emplace(coor, elf);
    } else {
      auto& same_proposal_elf                = proposed_position.at(coor);
      same_proposal_elf.conflicted_proposal_ = true;
      elf.conflicted_proposal_               = true;
    }
  }

  for (auto&& elf : t_elves) {
    if (auto&& [_, mover] = elf.predicate_; not elf.conflicted_proposal_ and _ != nullptr and mover != nullptr) {
      elf.coor_ = (*mover)(elf.coor_);
    }
  }
}

auto get_elves_coor_from_map(auto&& t_map) {
  std::vector<Elf> ret_val;

  ret_val.reserve(t_map.size() * t_map[0].size());
  for (std::size_t i = 0; i < t_map.size(); ++i) {
    auto& row = t_map[i];
    for (std::size_t j = 0; j < t_map[i].size(); ++j) {
      if (row[j] == '#') {
        ret_val.emplace_back(std::pair{static_cast<int>(j), static_cast<int>(i)});
      }
    }
  }

  return ret_val;
}

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::minmax, ranges::views::cycle, ranges::views::sliding,
    ranges::views::take;

  std::fstream in((INPUT_FILE));
  auto const rng = getlines(in) | to_vector;

  auto elves = get_elves_coor_from_map(rng);
  for (auto&& current_order : MOVE_ORDER | cycle | sliding(MOVE_ORDER.size()) | take(10)) {
    move(elves, current_order);
  }

  auto&& [min_x, max_x] = minmax(elves, std::less<>{}, [](auto const& t_elf) { return t_elf.get_coor().first; });
  auto&& [min_y, max_y] = minmax(elves, std::less<>{}, [](auto const& t_elf) { return t_elf.get_coor().second; });

  auto const total_tile = (max_x.coor_.first - min_x.coor_.first + 1) * (max_y.coor_.second - min_y.coor_.second + 1);
  fmt::println("empty ground tiles: {}", total_tile - static_cast<int>(elves.size()));
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::views::cycle, ranges::views::sliding, ranges::views::take;

  std::fstream in((INPUT_FILE));
  auto const rng = getlines(in) | to_vector;

  auto elves       = get_elves_coor_from_map(rng);
  auto prev_result = elves;
  for (std::size_t round = 0; auto&& current_order : MOVE_ORDER | cycle | sliding(MOVE_ORDER.size())) {
    move(elves, current_order);
    ++round;

    if (prev_result == elves) {
      fmt::println("After {} of rounds, no elves moves for the first time", round);
      break;
    }

    prev_result = elves;
  }
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
